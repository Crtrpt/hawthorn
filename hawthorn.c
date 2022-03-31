/* Hawthorn main program - this varies depending on your operating system and
 * how you're using Hawthorn */
 
/* include only Hawthorn.h here - should be able to use it with only the external interfaces, no internals from interpreter.h */
#include "hawthorn.h"

/* platform-dependent code for running programs is in this file */

#if defined(UNIX_HOST)
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define Hawthorn_STACK_SIZE (128*1024)              /* space for the the stack */

int main(int argc, char **argv)
{
    int ParamCount = 1;
    int DontRunMain = FALSE;
    int StackSize = getenv("STACKSIZE") ? atoi(getenv("STACKSIZE")) : Hawthorn_STACK_SIZE;
    Hawthorn hc;
    
    HawthornInitialise(&hc, StackSize);

    if (argc < 2)
    {
        printf("Format: hawthorn <csource1.c>... [- <arg1>...]    : run a program (calls main() to start it)\n"
               "        hawthorn -s <csource1.c>... [- <arg1>...] : script mode - runs the program without calling main()\n"
               "        Hawthorn -i                               : interactive mode\n\n"
               "======================================================================================================\n");
        HawthornIncludeAllSystemHeaders(&hc);
        HawthornParseInteractive(&hc);
    }
    else {
        if (strcmp(argv[ParamCount], "-s") == 0 || strcmp(argv[ParamCount], "-m") == 0)
        {
            DontRunMain = TRUE;
            HawthornIncludeAllSystemHeaders(&hc);
            ParamCount++;
        }
        else
        {
            if (HawthornPlatformSetExitPoint(&hc))
            {
                HawthornCleanup(&hc);
                return hc.HawthornExitValue;
            }

            for (; ParamCount < argc && strcmp(argv[ParamCount], "-") != 0; ParamCount++)
                HawthornPlatformScanFile(&hc, argv[ParamCount]);

            if (!DontRunMain)
                HawthornCallMain(&hc, argc - ParamCount, &argv[ParamCount]);
        }

    }
    
  
    

    
    HawthornCleanup(&hc);
    return hc.HawthornExitValue;
}

#endif
