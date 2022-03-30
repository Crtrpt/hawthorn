/* picoc main program - this varies depending on your operating system and
 * how you're using picoc */

/* include only picoc.h here - should be able to use it with only the external interfaces, no internals from interpreter.h */
#include "hawthorn.h"

/* platform-dependent code for running programs is in this file */

#if defined(UNIX_HOST)
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PICOC_STACK_SIZE (128 * 1024) /* space for the the stack */

int main(int argc, char **argv)
{

    int StackSize = getenv("STACKSIZE") ? atoi(getenv("STACKSIZE")) : PICOC_STACK_SIZE;
    Picoc pc;

    PicocInitialise(&pc, StackSize);

    PicocIncludeAllSystemHeaders(&pc);
    PicocParseInteractive(&pc);

    PicocCleanup(&pc);
    return pc.PicocExitValue;
}

#endif
