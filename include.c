/* Hawthorn include system - can emulate system includes from built-in libraries
 * or it can include and parse files if the system has files */

#include "hawthorn.h"
#include "interpreter.h"

#ifndef NO_HASH_INCLUDE


/* initialise the built-in include libraries */
void IncludeInit(Hawthorn *hc)
{
#ifndef BUILTIN_MINI_STDLIB
    IncludeRegister(hc, "ctype.h", NULL, &StdCtypeFunctions[0], NULL);
    IncludeRegister(hc, "errno.h", &StdErrnoSetupFunc, NULL, NULL);
# ifndef NO_FP
    IncludeRegister(hc, "math.h", &MathSetupFunc, &MathFunctions[0], NULL);
# endif
    IncludeRegister(hc, "stdbool.h", &StdboolSetupFunc, NULL, StdboolDefs);
    IncludeRegister(hc, "stdio.h", &StdioSetupFunc, &StdioFunctions[0], StdioDefs);
    IncludeRegister(hc, "stdlib.h", &StdlibSetupFunc, &StdlibFunctions[0], NULL);
    IncludeRegister(hc, "string.h", &StringSetupFunc, &StringFunctions[0], NULL);
    IncludeRegister(hc, "time.h", &StdTimeSetupFunc, &StdTimeFunctions[0], StdTimeDefs);
# ifndef WIN32
    IncludeRegister(hc, "unistd.h", &UnistdSetupFunc, &UnistdFunctions[0], UnistdDefs);
# endif
#endif
}

/* clean up space used by the include system */
void IncludeCleanup(Hawthorn *hc)
{
    struct IncludeLibrary *ThisInclude = hc->IncludeLibList;
    struct IncludeLibrary *NextInclude;
    
    while (ThisInclude != NULL)
    {
        NextInclude = ThisInclude->NextLib;
        HeapFreeMem(hc, ThisInclude);
        ThisInclude = NextInclude;
    }

    hc->IncludeLibList = NULL;
}

/* register a new build-in include file */
void IncludeRegister(Hawthorn *hc, const char *IncludeName, void (*SetupFunction)(Hawthorn *hc), struct LibraryFunction *FuncList, const char *SetuhcSource)
{
    struct IncludeLibrary *NewLib = HeapAllocMem(hc, sizeof(struct IncludeLibrary));
    NewLib->IncludeName = TableStrRegister(hc, IncludeName);
    NewLib->SetupFunction = SetupFunction;
    NewLib->FuncList = FuncList;
    NewLib->SetuhcSource = SetuhcSource;
    NewLib->NextLib = hc->IncludeLibList;
    hc->IncludeLibList = NewLib;
}

/* include all of the system headers */
void HawthornIncludeAllSystemHeaders(Hawthorn *hc)
{
    struct IncludeLibrary *ThisInclude = hc->IncludeLibList;
    
    for (; ThisInclude != NULL; ThisInclude = ThisInclude->NextLib)
        IncludeFile(hc, ThisInclude->IncludeName);
}

/* include one of a number of predefined libraries, or perhaps an actual file */
void IncludeFile(Hawthorn *hc, char *FileName)
{
    struct IncludeLibrary *LInclude;
    
    /* scan for the include file name to see if it's in our list of predefined includes */
    for (LInclude = hc->IncludeLibList; LInclude != NULL; LInclude = LInclude->NextLib)
    {
        if (strcmp(LInclude->IncludeName, FileName) == 0)
        {
            /* found it - protect against multiple inclusion */
            if (!VariableDefined(hc, FileName))
            {
                VariableDefine(hc, NULL, FileName, NULL, &hc->VoidType, FALSE);
                
                /* run an extra startup function if there is one */
                if (LInclude->SetupFunction != NULL)
                    (*LInclude->SetupFunction)(hc);
                
                /* parse the setup C source code - may define types etc. */
                if (LInclude->SetuhcSource != NULL)
                    HawthornParse(hc, FileName, LInclude->SetuhcSource, strlen(LInclude->SetuhcSource), TRUE, TRUE, FALSE, FALSE);
                
                /* set up the library functions */
                if (LInclude->FuncList != NULL)
                    LibraryAdd(hc, &hc->GlobalTable, FileName, LInclude->FuncList);
            }
            
            return;
        }
    }
    
    /* not a predefined file, read a real file */
    HawthornPlatformScanFile(hc, FileName);
}

#endif /* NO_HASH_INCLUDE */
