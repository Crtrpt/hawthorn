/* Hawthorn variable storage. This provides ways of defining and accessing
 * variables */
 
#include "interpreter.h"

/* maximum size of a value to temporarily copy while we create a variable */
#define MAX_TMP_COPY_BUF 256


/* initialise the variable system */
void VariableInit(Hawthorn *hc)
{
    TableInitTable(&(hc->GlobalTable), &(hc->GlobalHashTable)[0], GLOBAL_TABLE_SIZE, TRUE);
    TableInitTable(&hc->StringLiteralTable, &hc->StringLiteralHashTable[0], STRING_LITERAL_TABLE_SIZE, TRUE);
    hc->TopStackFrame = NULL;
}

/* deallocate the contents of a variable */
void VariableFree(Hawthorn *hc, struct Value *Val)
{
    if (Val->ValOnHeap || Val->AnyValOnHeap)
    {
        /* free function bodies */
        if (Val->Typ == &hc->FunctionType && Val->Val->FuncDef.Intrinsic == NULL && Val->Val->FuncDef.Body.Pos != NULL)
            HeapFreeMem(hc, (void *)Val->Val->FuncDef.Body.Pos);

        /* free macro bodies */
        if (Val->Typ == &hc->MacroType)
            HeapFreeMem(hc, (void *)Val->Val->MacroDef.Body.Pos);

        /* free the AnyValue */
        if (Val->AnyValOnHeap)
            HeapFreeMem(hc, Val->Val);
    }

    /* free the value */
    if (Val->ValOnHeap)
        HeapFreeMem(hc, Val);
}

/* deallocate the global table and the string literal table */
void VariableTableCleanup(Hawthorn *hc, struct Table *HashTable)
{
    struct TableEntry *Entry;
    struct TableEntry *NextEntry;
    int Count;
    
    for (Count = 0; Count < HashTable->Size; Count++)
    {
        for (Entry = HashTable->HashTable[Count]; Entry != NULL; Entry = NextEntry)
        {
            NextEntry = Entry->Next;
            VariableFree(hc, Entry->p.v.Val);
                
            /* free the hash table entry */
            HeapFreeMem(hc, Entry);
        }
    }
}

void VariableCleanup(Hawthorn *hc)
{
    VariableTableCleanup(hc, &hc->GlobalTable);
    VariableTableCleanup(hc, &hc->StringLiteralTable);
}

/* allocate some memory, either on the heap or the stack and check if we've run out */
void *VariableAlloc(Hawthorn *hc, struct ParseState *Parser, int Size, int OnHeap)
{
    void *NewValue;
    
    if (OnHeap)
        NewValue = HeapAllocMem(hc, Size);
    else
        NewValue = HeapAllocStack(hc, Size);
    
    if (NewValue == NULL)
        ProgramFail(Parser, "out of memory");
    
#ifdef DEBUG_HEAP
    if (!OnHeap)
        printf("pushing %d at 0x%lx\n", Size, (unsigned long)NewValue);
#endif
        
    return NewValue;
}

/* allocate a value either on the heap or the stack using space dependent on what type we want */
struct Value *VariableAllocValueAndData(Hawthorn *hc, struct ParseState *Parser, int DataSize, int IsLValue, struct Value *LValueFrom, int OnHeap)
{
    struct Value *NewValue = VariableAlloc(hc, Parser, MEM_ALIGN(sizeof(struct Value)) + DataSize, OnHeap);
    NewValue->Val = (union AnyValue *)((char *)NewValue + MEM_ALIGN(sizeof(struct Value)));
    NewValue->ValOnHeap = OnHeap;
    NewValue->AnyValOnHeap = FALSE;
    NewValue->ValOnStack = !OnHeap;
    NewValue->IsLValue = IsLValue;
    NewValue->LValueFrom = LValueFrom;
    if (Parser) 
        NewValue->ScopeID = Parser->ScopeID;

    NewValue->OutOfScope = 0;
    
    return NewValue;
}

/* allocate a value given its type */
struct Value *VariableAllocValueFromType(Hawthorn *hc, struct ParseState *Parser, struct ValueType *Typ, int IsLValue, struct Value *LValueFrom, int OnHeap)
{
    int Size = TypeSize(Typ, Typ->ArraySize, FALSE);
    struct Value *NewValue = VariableAllocValueAndData(hc, Parser, Size, IsLValue, LValueFrom, OnHeap);
    assert(Size >= 0 || Typ == &hc->VoidType);
    NewValue->Typ = Typ;
    
    return NewValue;
}

/* allocate a value either on the heap or the stack and copy its value. handles overlapping data */
struct Value *VariableAllocValueAndCopy(Hawthorn *hc, struct ParseState *Parser, struct Value *FromValue, int OnHeap)
{
    struct ValueType *DType = FromValue->Typ;
    struct Value *NewValue;
    char TmpBuf[MAX_TMP_COPY_BUF];
    int CopySize = TypeSizeValue(FromValue, TRUE);

    assert(CopySize <= MAX_TMP_COPY_BUF);
    memcpy((void *)&TmpBuf[0], (void *)FromValue->Val, CopySize);
    NewValue = VariableAllocValueAndData(hc, Parser, CopySize, FromValue->IsLValue, FromValue->LValueFrom, OnHeap);
    NewValue->Typ = DType;
    memcpy((void *)NewValue->Val, (void *)&TmpBuf[0], CopySize);
    
    return NewValue;
}

/* allocate a value either on the heap or the stack from an existing AnyValue and type */
struct Value *VariableAllocValueFromExistingData(struct ParseState *Parser, struct ValueType *Typ, union AnyValue *FromValue, int IsLValue, struct Value *LValueFrom)
{
    struct Value *NewValue = VariableAlloc(Parser->hc, Parser, sizeof(struct Value), FALSE);
    NewValue->Typ = Typ;
    NewValue->Val = FromValue;
    NewValue->ValOnHeap = FALSE;
    NewValue->AnyValOnHeap = FALSE;
    NewValue->ValOnStack = FALSE;
    NewValue->IsLValue = IsLValue;
    NewValue->LValueFrom = LValueFrom;
    
    return NewValue;
}

/* allocate a value either on the heap or the stack from an existing Value, sharing the value */
struct Value *VariableAllocValueShared(struct ParseState *Parser, struct Value *FromValue)
{
    return VariableAllocValueFromExistingData(Parser, FromValue->Typ, FromValue->Val, FromValue->IsLValue, FromValue->IsLValue ? FromValue : NULL);
}

/* reallocate a variable so its data has a new size */
void VariableRealloc(struct ParseState *Parser, struct Value *FromValue, int NewSize)
{
    if (FromValue->AnyValOnHeap)
        HeapFreeMem(Parser->hc, FromValue->Val);
        
    FromValue->Val = VariableAlloc(Parser->hc, Parser, NewSize, TRUE);
    FromValue->AnyValOnHeap = TRUE;
}

int VariableScopeBegin(struct ParseState * Parser, int* OldScopeID)
{
    struct TableEntry *Entry;
    struct TableEntry *NextEntry;
    Hawthorn * hc = Parser->hc;
    int Count;
    #ifdef VAR_SCOPE_DEBUG
    int FirstPrint = 0;
    #endif
    
    struct Table * HashTable = (hc->TopStackFrame == NULL) ? &(hc->GlobalTable) : &(hc->TopStackFrame)->LocalTable;

    if (Parser->ScopeID == -1) return -1;

    /* XXX dumb hash, let's hope for no collisions... */
    *OldScopeID = Parser->ScopeID;
    Parser->ScopeID = (int)(intptr_t)(Parser->SourceText) * ((int)(intptr_t)(Parser->Pos) / sizeof(char*));
    /* or maybe a more human-readable hash for debugging? */
    /* Parser->ScopeID = Parser->Line * 0x10000 + Parser->CharacterPos; */
    
    for (Count = 0; Count < HashTable->Size; Count++)
    {
        for (Entry = HashTable->HashTable[Count]; Entry != NULL; Entry = NextEntry)
        {
            NextEntry = Entry->Next;
            if (Entry->p.v.Val->ScopeID == Parser->ScopeID && Entry->p.v.Val->OutOfScope)
            {
                Entry->p.v.Val->OutOfScope = FALSE;
                Entry->p.v.Key = (char*)((intptr_t)Entry->p.v.Key & ~1);
                #ifdef VAR_SCOPE_DEBUG
                if (!FirstPrint) { PRINT_SOURCE_POS; }
                FirstPrint = 1;
                printf(">>> back into scope: %s %x %d\n", Entry->p.v.Key, Entry->p.v.Val->ScopeID, Entry->p.v.Val->Val->Integer);
                #endif
            }
        }
    }

    return Parser->ScopeID;
}

void VariableScopeEnd(struct ParseState * Parser, int ScopeID, int PrevScopeID)
{
    struct TableEntry *Entry;
    struct TableEntry *NextEntry;
    Hawthorn * hc = Parser->hc;
    int Count;
    #ifdef VAR_SCOPE_DEBUG
    int FirstPrint = 0;
    #endif

    struct Table * HashTable = (hc->TopStackFrame == NULL) ? &(hc->GlobalTable) : &(hc->TopStackFrame)->LocalTable;

    if (ScopeID == -1) return;

    for (Count = 0; Count < HashTable->Size; Count++)
    {
        for (Entry = HashTable->HashTable[Count]; Entry != NULL; Entry = NextEntry)
        {
            NextEntry = Entry->Next;
            if (Entry->p.v.Val->ScopeID == ScopeID && !Entry->p.v.Val->OutOfScope)
            {
                #ifdef VAR_SCOPE_DEBUG
                if (!FirstPrint) { PRINT_SOURCE_POS; }
                FirstPrint = 1;
                printf(">>> out of scope: %s %x %d\n", Entry->p.v.Key, Entry->p.v.Val->ScopeID, Entry->p.v.Val->Val->Integer);
                #endif
                Entry->p.v.Val->OutOfScope = TRUE;
                Entry->p.v.Key = (char*)((intptr_t)Entry->p.v.Key | 1); /* alter the key so it won't be found by normal searches */
            }
        }
    }

    Parser->ScopeID = PrevScopeID;
}

int VariableDefinedAndOutOfScope(Hawthorn * hc, const char* Ident)
{
    struct TableEntry *Entry;
    int Count;

    struct Table * HashTable = (hc->TopStackFrame == NULL) ? &(hc->GlobalTable) : &(hc->TopStackFrame)->LocalTable;
    for (Count = 0; Count < HashTable->Size; Count++)
    {
        for (Entry = HashTable->HashTable[Count]; Entry != NULL; Entry = Entry->Next)
        {
            if (Entry->p.v.Val->OutOfScope && (char*)((intptr_t)Entry->p.v.Key & ~1) == Ident)
                return TRUE;
        }
    }
    return FALSE;
}

/* define a variable. Ident must be registered */
struct Value *VariableDefine(Hawthorn *hc, struct ParseState *Parser, char *Ident, struct Value *InitValue, struct ValueType *Typ, int MakeWritable)
{
    struct Value * AssignValue;
    struct Table * currentTable = (hc->TopStackFrame == NULL) ? &(hc->GlobalTable) : &(hc->TopStackFrame)->LocalTable;
    
    int ScopeID = Parser ? Parser->ScopeID : -1;
#ifdef VAR_SCOPE_DEBUG
    if (Parser) fprintf(stderr, "def %s %x (%s:%d:%d)\n", Ident, ScopeID, Parser->FileName, Parser->Line, Parser->CharacterPos);
#endif
    
    if (InitValue != NULL)
        AssignValue = VariableAllocValueAndCopy(hc, Parser, InitValue, hc->TopStackFrame == NULL);
    else
        AssignValue = VariableAllocValueFromType(hc, Parser, Typ, MakeWritable, NULL, hc->TopStackFrame == NULL);
    
    AssignValue->IsLValue = MakeWritable;
    AssignValue->ScopeID = ScopeID;
    AssignValue->OutOfScope = FALSE;

    if (!TableSet(hc, currentTable, Ident, AssignValue, Parser ? ((char *)Parser->FileName) : NULL, Parser ? Parser->Line : 0, Parser ? Parser->CharacterPos : 0))
        ProgramFail(Parser, "'%s' is already defined", Ident);
    
    return AssignValue;
}

/* define a variable. Ident must be registered. If it's a redefinition from the same declaration don't throw an error */
struct Value *VariableDefineButIgnoreIdentical(struct ParseState *Parser, char *Ident, struct ValueType *Typ, int IsStatic, int *FirstVisit)
{
    Hawthorn *hc = Parser->hc;
    struct Value *ExistingValue;
    const char *DeclFileName;
    int DeclLine;
    int DeclColumn;
    
    /* is the type a forward declaration? */
    if (TypeIsForwardDeclared(Parser, Typ))
        ProgramFail(Parser, "type '%t' isn't defined", Typ);

    if (IsStatic)
    {
        char MangledName[LINEBUFFER_MAX];
        char *MNPos = &MangledName[0];
        char *MNEnd = &MangledName[LINEBUFFER_MAX-1];
        const char *RegisteredMangledName;
        
        /* make the mangled static name (avoiding using sprintf() to minimise library impact) */
        memset((void *)&MangledName, '\0', sizeof(MangledName));
        *MNPos++ = '/';
        strncpy(MNPos, (char *)Parser->FileName, MNEnd - MNPos);
        MNPos += strlen(MNPos);
        
        if (hc->TopStackFrame != NULL)
        {
            /* we're inside a function */
            if (MNEnd - MNPos > 0) *MNPos++ = '/';
            strncpy(MNPos, (char *)hc->TopStackFrame->FuncName, MNEnd - MNPos);
            MNPos += strlen(MNPos);
        }
            
        if (MNEnd - MNPos > 0) *MNPos++ = '/';
        strncpy(MNPos, Ident, MNEnd - MNPos);
        RegisteredMangledName = TableStrRegister(hc, MangledName);
        
        /* is this static already defined? */
        if (!TableGet(&hc->GlobalTable, RegisteredMangledName, &ExistingValue, &DeclFileName, &DeclLine, &DeclColumn))
        {
            /* define the mangled-named static variable store in the global scope */
            ExistingValue = VariableAllocValueFromType(Parser->hc, Parser, Typ, TRUE, NULL, TRUE);
            TableSet(hc, &hc->GlobalTable, (char *)RegisteredMangledName, ExistingValue, (char *)Parser->FileName, Parser->Line, Parser->CharacterPos);
            *FirstVisit = TRUE;
        }

        /* static variable exists in the global scope - now make a mirroring variable in our own scope with the short name */
        VariableDefinePlatformVar(Parser->hc, Parser, Ident, ExistingValue->Typ, ExistingValue->Val, TRUE);
        return ExistingValue;
    }
    else
    {
        if (Parser->Line != 0 && TableGet((hc->TopStackFrame == NULL) ? &hc->GlobalTable : &hc->TopStackFrame->LocalTable, Ident, &ExistingValue, &DeclFileName, &DeclLine, &DeclColumn)
                && DeclFileName == Parser->FileName && DeclLine == Parser->Line && DeclColumn == Parser->CharacterPos)
            return ExistingValue;
        else
            return VariableDefine(Parser->hc, Parser, Ident, NULL, Typ, TRUE);
    }
}

/* check if a variable with a given name is defined. Ident must be registered */
int VariableDefined(Hawthorn *hc, const char *Ident)
{
    struct Value *FoundValue;
    
    if (hc->TopStackFrame == NULL || !TableGet(&hc->TopStackFrame->LocalTable, Ident, &FoundValue, NULL, NULL, NULL))
    {
        if (!TableGet(&hc->GlobalTable, Ident, &FoundValue, NULL, NULL, NULL))
            return FALSE;
    }

    return TRUE;
}

/* get the value of a variable. must be defined. Ident must be registered */
void VariableGet(Hawthorn *hc, struct ParseState *Parser, const char *Ident, struct Value **LVal)
{
    if (hc->TopStackFrame == NULL || !TableGet(&hc->TopStackFrame->LocalTable, Ident, LVal, NULL, NULL, NULL))
    {
        if (!TableGet(&hc->GlobalTable, Ident, LVal, NULL, NULL, NULL))
        {
            if (VariableDefinedAndOutOfScope(hc, Ident))
                ProgramFail(Parser, "'%s' is out of scope", Ident);
            else
                ProgramFail(Parser, "'%s' is undefined", Ident);
        }
    }
}

/* define a global variable shared with a platform global. Ident will be registered */
void VariableDefinePlatformVar(Hawthorn *hc, struct ParseState *Parser, char *Ident, struct ValueType *Typ, union AnyValue *FromValue, int IsWritable)
{
    struct Value *SomeValue = VariableAllocValueAndData(hc, NULL, 0, IsWritable, NULL, TRUE);
    SomeValue->Typ = Typ;
    SomeValue->Val = FromValue;
    
    if (!TableSet(hc, (hc->TopStackFrame == NULL) ? &hc->GlobalTable : &hc->TopStackFrame->LocalTable, TableStrRegister(hc, Ident), SomeValue, Parser ? Parser->FileName : NULL, Parser ? Parser->Line : 0, Parser ? Parser->CharacterPos : 0))
        ProgramFail(Parser, "'%s' is already defined", Ident);
}

/* free and/or pop the top value off the stack. Var must be the top value on the stack! */
void VariableStackPop(struct ParseState *Parser, struct Value *Var)
{
    int Success;
    
#ifdef DEBUG_HEAP
    if (Var->ValOnStack)
        printf("popping %ld at 0x%lx\n", (unsigned long)(sizeof(struct Value) + TypeSizeValue(Var, FALSE)), (unsigned long)Var);
#endif
        
    if (Var->ValOnHeap)
    { 
        if (Var->Val != NULL)
            HeapFreeMem(Parser->hc, Var->Val);
            
        Success = HeapPopStack(Parser->hc, Var, sizeof(struct Value));                       /* free from heap */
    }
    else if (Var->ValOnStack)
        Success = HeapPopStack(Parser->hc, Var, sizeof(struct Value) + TypeSizeValue(Var, FALSE));  /* free from stack */
    else
        Success = HeapPopStack(Parser->hc, Var, sizeof(struct Value));                       /* value isn't our problem */
        
    if (!Success)
        ProgramFail(Parser, "stack underrun");
}

/* add a stack frame when doing a function call */
void VariableStackFrameAdd(struct ParseState *Parser, const char *FuncName, int NumParams)
{
    struct StackFrame *NewFrame;
    
    HeapPushStackFrame(Parser->hc);
    NewFrame = HeapAllocStack(Parser->hc, sizeof(struct StackFrame) + sizeof(struct Value *) * NumParams);
    if (NewFrame == NULL)
        ProgramFail(Parser, "out of memory");
        
    ParserCopy(&NewFrame->ReturnParser, Parser);
    NewFrame->FuncName = FuncName;
    NewFrame->Parameter = (NumParams > 0) ? ((void *)((char *)NewFrame + sizeof(struct StackFrame))) : NULL;
    TableInitTable(&NewFrame->LocalTable, &NewFrame->LocalHashTable[0], LOCAL_TABLE_SIZE, FALSE);
    NewFrame->PreviousStackFrame = Parser->hc->TopStackFrame;
    Parser->hc->TopStackFrame = NewFrame;
}

/* remove a stack frame */
void VariableStackFramePop(struct ParseState *Parser)
{
    if (Parser->hc->TopStackFrame == NULL)
        ProgramFail(Parser, "stack is empty - can't go back");
        
    ParserCopy(Parser, &Parser->hc->TopStackFrame->ReturnParser);
    Parser->hc->TopStackFrame = Parser->hc->TopStackFrame->PreviousStackFrame;
    HeapPopStackFrame(Parser->hc);
}

/* get a string literal. assumes that Ident is already registered. NULL if not found */
struct Value *VariableStringLiteralGet(Hawthorn *hc, char *Ident)
{
    struct Value *LVal = NULL;

    if (TableGet(&hc->StringLiteralTable, Ident, &LVal, NULL, NULL, NULL))
        return LVal;
    else
        return NULL;
}

/* define a string literal. assumes that Ident is already registered */
void VariableStringLiteralDefine(Hawthorn *hc, char *Ident, struct Value *Val)
{
    TableSet(hc, &hc->StringLiteralTable, Ident, Val, NULL, 0, 0);
}

/* check a pointer for validity and dereference it for use */
void *VariableDereferencePointer(struct ParseState *Parser, struct Value *PointerValue, struct Value **DerefVal, int *DerefOffset, struct ValueType **DerefType, int *DerefIsLValue)
{
    if (DerefVal != NULL)
        *DerefVal = NULL;
        
    if (DerefType != NULL)
        *DerefType = PointerValue->Typ->FromType;
        
    if (DerefOffset != NULL)
        *DerefOffset = 0;
        
    if (DerefIsLValue != NULL)
        *DerefIsLValue = TRUE;

    return PointerValue->Val->Pointer;
}

