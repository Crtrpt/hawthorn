/* Hawthorn interactive debugger */

#ifndef NO_DEBUGGER

#include "interpreter.h"

#define BREAKPOINT_HASH(p) ( ((unsigned long)(p)->FileName) ^ (((p)->Line << 16) | ((p)->CharacterPos << 16)) )

/* initialise the debugger by clearing the breakpoint table */
void DebugInit(Hawthorn *hc)
{
    TableInitTable(&hc->BreakpointTable, &hc->BreakpointHashTable[0], BREAKPOINT_TABLE_SIZE, TRUE);
    hc->BreakpointCount = 0;
}

/* free the contents of the breakpoint table */
void DebugCleanup(Hawthorn *hc)
{
    struct TableEntry *Entry;
    struct TableEntry *NextEntry;
    int Count;
    
    for (Count = 0; Count < hc->BreakpointTable.Size; Count++)
    {
        for (Entry = hc->BreakpointHashTable[Count]; Entry != NULL; Entry = NextEntry)
        {
            NextEntry = Entry->Next;
            HeapFreeMem(hc, Entry);
        }
    }
}

/* search the table for a breakpoint */
static struct TableEntry *DebugTableSearchBreakpoint(struct ParseState *Parser, int *AddAt)
{
    struct TableEntry *Entry;
    Hawthorn *hc = Parser->hc;
    int HashValue = BREAKPOINT_HASH(Parser) % hc->BreakpointTable.Size;
    
    for (Entry = hc->BreakpointHashTable[HashValue]; Entry != NULL; Entry = Entry->Next)
    {
        if (Entry->p.b.FileName == Parser->FileName && Entry->p.b.Line == Parser->Line && Entry->p.b.CharacterPos == Parser->CharacterPos)
            return Entry;   /* found */
    }
    
    *AddAt = HashValue;    /* didn't find it in the chain */
    return NULL;
}

/* set a breakpoint in the table */
void DebugSetBreakpoint(struct ParseState *Parser)
{
    int AddAt;
    struct TableEntry *FoundEntry = DebugTableSearchBreakpoint(Parser, &AddAt);
    Hawthorn *hc = Parser->hc;
    
    if (FoundEntry == NULL)
    {   
        /* add it to the table */
        struct TableEntry *NewEntry = HeapAllocMem(hc, sizeof(struct TableEntry));
        if (NewEntry == NULL)
            ProgramFailNoParser(hc, "out of memory");
            
        NewEntry->p.b.FileName = Parser->FileName;
        NewEntry->p.b.Line = Parser->Line;
        NewEntry->p.b.CharacterPos = Parser->CharacterPos;
        NewEntry->Next = hc->BreakpointHashTable[AddAt];
        hc->BreakpointHashTable[AddAt] = NewEntry;
        hc->BreakpointCount++;
    }
}

/* delete a breakpoint from the hash table */
int DebugClearBreakpoint(struct ParseState *Parser)
{
    struct TableEntry **EntryPtr;
    Hawthorn *hc = Parser->hc;
    int HashValue = BREAKPOINT_HASH(Parser) % hc->BreakpointTable.Size;
    
    for (EntryPtr = &hc->BreakpointHashTable[HashValue]; *EntryPtr != NULL; EntryPtr = &(*EntryPtr)->Next)
    {
        struct TableEntry *DeleteEntry = *EntryPtr;
        if (DeleteEntry->p.b.FileName == Parser->FileName && DeleteEntry->p.b.Line == Parser->Line && DeleteEntry->p.b.CharacterPos == Parser->CharacterPos)
        {
            *EntryPtr = DeleteEntry->Next;
            HeapFreeMem(hc, DeleteEntry);
            hc->BreakpointCount--;

            return TRUE;
        }
    }

    return FALSE;
}

/* before we run a statement, check if there's anything we have to do with the debugger here */
void DebugCheckStatement(struct ParseState *Parser)
{
    int DoBreak = FALSE;
    int AddAt;
    Hawthorn *hc = Parser->hc;
    
    /* has the user manually pressed break? */
    if (hc->DebugManualBreak)
    {
        PlatformPrintf(hc->CStdOut, "break\n");
        DoBreak = TRUE;
        hc->DebugManualBreak = FALSE;
    }
    
    /* is this a breakpoint location? */
    if (Parser->hc->BreakpointCount != 0 && DebugTableSearchBreakpoint(Parser, &AddAt) != NULL)
        DoBreak = TRUE;
    
    /* handle a break */
    if (DoBreak)
    {
        PlatformPrintf(hc->CStdOut, "Handling a break\n");
        HawthornParseInteractiveNoStartPrompt(hc, FALSE);
    }
}

void DebugStep()
{
}
#endif /* !NO_DEBUGGER */
