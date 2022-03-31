/* Hawthorn heap memory allocation. This is a complete (but small) memory
 * allocator for embedded systems which have no memory allocator. Alternatively
 * you can define USE_MALLOC_HEAP to use your system's own malloc() allocator */
 
/* stack grows up from the bottom and heap grows down from the top of heap space */
#include "interpreter.h"

#ifdef DEBUG_HEAP
void ShowBigList(Hawthorn *hc)
{
    struct AllocNode *LPos;
    
    printf("Heap: bottom=0x%lx 0x%lx-0x%lx, big freelist=", (long)hc->HeapBottom, (long)&(hc->HeapMemory)[0], (long)&(hc->HeapMemory)[HEAP_SIZE]);
    for (LPos = hc->FreeListBig; LPos != NULL; LPos = LPos->NextFree)
        printf("0x%lx:%d ", (long)LPos, LPos->Size);
    
    printf("\n");
}
#endif

/* initialise the stack and heap storage */
void HeapInit(Hawthorn *hc, int StackOrHeapSize)
{
    int Count;
    int AlignOffset = 0;
    
#ifdef USE_MALLOC_STACK
    hc->HeapMemory = malloc(StackOrHeapSize);
    hc->HeapBottom = NULL;                     /* the bottom of the (downward-growing) heap */
    hc->StackFrame = NULL;                     /* the current stack frame */
    hc->HeapStackTop = NULL;                          /* the top of the stack */
#else
    hc->HeapBottom = &HeapMemory[HEAP_SIZE]; /* the bottom of the (downward-growing) heap */
    hc->StackFrame = &HeapMemory[0];         /* the current stack frame */
    hc->HeapStackTop = &HeapMemory[0];       /* the top of the stack */
#endif

    while (((unsigned long)&hc->HeapMemory[AlignOffset] & (sizeof(ALIGN_TYPE)-1)) != 0)
        AlignOffset++;
        
    hc->StackFrame = &(hc->HeapMemory)[AlignOffset];
    hc->HeapStackTop = &(hc->HeapMemory)[AlignOffset];
    *(void **)(hc->StackFrame) = NULL;
    hc->HeapBottom = &(hc->HeapMemory)[StackOrHeapSize-sizeof(ALIGN_TYPE)+AlignOffset];
    hc->FreeListBig = NULL;
    for (Count = 0; Count < FREELIST_BUCKETS; Count++)
        hc->FreeListBucket[Count] = NULL;
}

void Heahcleanup(Hawthorn *hc)
{
#ifdef USE_MALLOC_STACK
    free(hc->HeapMemory);
#endif
}

/* allocate some space on the stack, in the current stack frame
 * clears memory. can return NULL if out of stack space */
void *HeapAllocStack(Hawthorn *hc, int Size)
{
    char *NewMem = hc->HeapStackTop;
    char *NewTop = (char *)hc->HeapStackTop + MEM_ALIGN(Size);
#ifdef DEBUG_HEAP
    printf("HeapAllocStack(%ld) at 0x%lx\n", (unsigned long)MEM_ALIGN(Size), (unsigned long)hc->HeapStackTop);
#endif
    if (NewTop > (char *)hc->HeapBottom)
        return NULL;
        
    hc->HeapStackTop = (void *)NewTop;
    memset((void *)NewMem, '\0', Size);
    return NewMem;
}

/* allocate some space on the stack, in the current stack frame */
void HeapUnpopStack(Hawthorn *hc, int Size)
{
#ifdef DEBUG_HEAP
    printf("HeapUnpopStack(%ld) at 0x%lx\n", (unsigned long)MEM_ALIGN(Size), (unsigned long)hc->HeapStackTop);
#endif
    hc->HeapStackTop = (void *)((char *)hc->HeapStackTop + MEM_ALIGN(Size));
}

/* free some space at the top of the stack */
int HeapPopStack(Hawthorn *hc, void *Addr, int Size)
{
    int ToLose = MEM_ALIGN(Size);
    if (ToLose > ((char *)hc->HeapStackTop - (char *)&(hc->HeapMemory)[0]))
        return FALSE;
    
#ifdef DEBUG_HEAP
    printf("HeapPopStack(0x%lx, %ld) back to 0x%lx\n", (unsigned long)Addr, (unsigned long)MEM_ALIGN(Size), (unsigned long)hc->HeapStackTop - ToLose);
#endif
    hc->HeapStackTop = (void *)((char *)hc->HeapStackTop - ToLose);
    assert(Addr == NULL || hc->HeapStackTop == Addr);
    
    return TRUE;
}

/* push a new stack frame on to the stack */
void HeapPushStackFrame(Hawthorn *hc)
{
#ifdef DEBUG_HEAP
    printf("Adding stack frame at 0x%lx\n", (unsigned long)hc->HeapStackTop);
#endif
    *(void **)hc->HeapStackTop = hc->StackFrame;
    hc->StackFrame = hc->HeapStackTop;
    hc->HeapStackTop = (void *)((char *)hc->HeapStackTop + MEM_ALIGN(sizeof(ALIGN_TYPE)));
}

/* pop the current stack frame, freeing all memory in the frame. can return NULL */
int HeapPopStackFrame(Hawthorn *hc)
{
    if (*(void **)hc->StackFrame != NULL)
    {
        hc->HeapStackTop = hc->StackFrame;
        hc->StackFrame = *(void **)hc->StackFrame;
#ifdef DEBUG_HEAP
        printf("Popping stack frame back to 0x%lx\n", (unsigned long)hc->HeapStackTop);
#endif
        return TRUE;
    }
    else
        return FALSE;
}

/* allocate some dynamically allocated memory. memory is cleared. can return NULL if out of memory */
void *HeapAllocMem(Hawthorn *hc, int Size)
{
#ifdef USE_MALLOC_HEAP
    return calloc(Size, 1);
#else
    struct AllocNode *NewMem = NULL;
    struct AllocNode **FreeNode;
    int AllocSize = MEM_ALIGN(Size) + MEM_ALIGN(sizeof(NewMem->Size));
    int Bucket;
    void *ReturnMem;
    
    if (Size == 0)
        return NULL;
    
    assert(Size > 0);
    
    /* make sure we have enough space for an AllocNode */
    if (AllocSize < sizeof(struct AllocNode))
        AllocSize = sizeof(struct AllocNode);
    
    Bucket = AllocSize >> 2;
    if (Bucket < FREELIST_BUCKETS && hc->FreeListBucket[Bucket] != NULL)
    { 
        /* try to allocate from a freelist bucket first */
#ifdef DEBUG_HEAP
        printf("allocating %d(%d) from bucket", Size, AllocSize);
#endif
        NewMem = hc->FreeListBucket[Bucket];
        assert((unsigned long)NewMem >= (unsigned long)&(hc->HeapMemory)[0] && (unsigned char *)NewMem - &(hc->HeapMemory)[0] < HEAP_SIZE);
        hc->FreeListBucket[Bucket] = *(struct AllocNode **)NewMem;
        assert(hc->FreeListBucket[Bucket] == NULL || ((unsigned long)hc->FreeListBucket[Bucket] >= (unsigned long)&(hc->HeapMemory)[0] && (unsigned char *)hc->FreeListBucket[Bucket] - &(hc->HeapMemory)[0] < HEAP_SIZE));
        NewMem->Size = AllocSize;
    }
    else if (hc->FreeListBig != NULL)
    { 
        /* grab the first item from the "big" freelist we can fit in */
        for (FreeNode = &hc->FreeListBig; *FreeNode != NULL && (*FreeNode)->Size < AllocSize; FreeNode = &(*FreeNode)->NextFree)
        {}
        
        if (*FreeNode != NULL)
        {
            assert((unsigned long)*FreeNode >= (unsigned long)&(hc->HeapMemory)[0] && (unsigned char *)*FreeNode - &(hc->HeapMemory)[0] < HEAP_SIZE);
            assert((*FreeNode)->Size < HEAP_SIZE && (*FreeNode)->Size > 0);
            if ((*FreeNode)->Size < AllocSize + SPLIT_MEM_THRESHOLD)
            { 
                /* close in size - reduce fragmentation by not splitting */
#ifdef DEBUG_HEAP
               printf("allocating %d(%d) from freelist, no split (%d)", Size, AllocSize, (*FreeNode)->Size);
#endif
                NewMem = *FreeNode;
                assert((unsigned long)NewMem >= (unsigned long)&(hc->HeapMemory)[0] && (unsigned char *)NewMem - &(hc->HeapMemory)[0] < HEAP_SIZE);
                *FreeNode = NewMem->NextFree;
            }
            else
            { 
                /* split this big memory chunk */
#ifdef DEBUG_HEAP
                printf("allocating %d(%d) from freelist, split chunk (%d)", Size, AllocSize, (*FreeNode)->Size);
#endif
                NewMem = (void *)((char *)*FreeNode + (*FreeNode)->Size - AllocSize);
                assert((unsigned long)NewMem >= (unsigned long)&(hc->HeapMemory)[0] && (unsigned char *)NewMem - &(hc->HeapMemory)[0] < HEAP_SIZE);
                (*FreeNode)->Size -= AllocSize;
                NewMem->Size = AllocSize;
            }
        }
    }
    
    if (NewMem == NULL)
    { 
        /* couldn't allocate from a freelist - try to increase the size of the heap area */
#ifdef DEBUG_HEAP
        printf("allocating %d(%d) at bottom of heap (0x%lx-0x%lx)", Size, AllocSize, (long)((char *)hc->HeapBottom - AllocSize), (long)HeapBottom);
#endif
        if ((char *)hc->HeapBottom - AllocSize < (char *)hc->HeapStackTop)
            return NULL;
        
        hc->HeapBottom = (void *)((char *)hc->HeapBottom - AllocSize);
        NewMem = hc->HeapBottom;
        NewMem->Size = AllocSize;
    }
    
    ReturnMem = (void *)((char *)NewMem + MEM_ALIGN(sizeof(NewMem->Size)));
    memset(ReturnMem, '\0', AllocSize - MEM_ALIGN(sizeof(NewMem->Size)));
#ifdef DEBUG_HEAP
    printf(" = %lx\n", (unsigned long)ReturnMem);
#endif
    return ReturnMem;
#endif
}

/* free some dynamically allocated memory */
void HeapFreeMem(Hawthorn *hc, void *Mem)
{
#ifdef USE_MALLOC_HEAP
    free(Mem);
#else
    struct AllocNode *MemNode = (struct AllocNode *)((char *)Mem - MEM_ALIGN(sizeof(MemNode->Size)));
    int Bucket = MemNode->Size >> 2;
    
#ifdef DEBUG_HEAP
    printf("HeapFreeMem(0x%lx)\n", (unsigned long)Mem);
#endif
    assert((unsigned long)Mem >= (unsigned long)&(hc->HeapMemory)[0] && (unsigned char *)Mem - &(hc->HeapMemory)[0] < HEAP_SIZE);
    assert(MemNode->Size < HEAP_SIZE && MemNode->Size > 0);
    if (Mem == NULL)
        return;
    
    if ((void *)MemNode == hc->HeapBottom)
    { 
        /* pop it off the bottom of the heap, reducing the heap size */
#ifdef DEBUG_HEAP
        printf("freeing %d from bottom of heap\n", MemNode->Size);
#endif
        hc->HeapBottom = (void *)((char *)hc->HeapBottom + MemNode->Size);
#ifdef DEBUG_HEAP
        ShowBigList(hc);
#endif
    }
    else if (Bucket < FREELIST_BUCKETS)
    { 
        /* we can fit it in a bucket */
#ifdef DEBUG_HEAP
        printf("freeing %d to bucket\n", MemNode->Size);
#endif
        assert(hc->FreeListBucket[Bucket] == NULL || ((unsigned long)hc->FreeListBucket[Bucket] >= (unsigned long)&(hc->HeapMemory)[0] && (unsigned char *)FreeListBucket[Bucket] - &HeapMemory[0] < HEAP_SIZE));
        *(struct AllocNode **)MemNode = hc->FreeListBucket[Bucket];
        hc->FreeListBucket[Bucket] = (struct AllocNode *)MemNode;
    }
    else
    { 
        /* put it in the big memory freelist */
#ifdef DEBUG_HEAP
        printf("freeing %lx:%d to freelist\n", (unsigned long)Mem, MemNode->Size);
#endif
        assert(hc->FreeListBig == NULL || ((unsigned long)hc->FreeListBig >= (unsigned long)&(hc->HeapMemory)[0] && (unsigned char *)hc->FreeListBig - &(hc->HeapMemory)[0] < HEAP_SIZE));
        MemNode->NextFree = hc->FreeListBig;
        FreeListBig = MemNode;
#ifdef DEBUG_HEAP
        ShowBigList(hc);
#endif
    }
#endif
}

