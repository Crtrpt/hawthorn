/* string.h library for large systems - small embedded systems use clibrary.c instead */
#include "../interpreter.h"

#ifndef BUILTIN_MINI_STDLIB

static int trueValue = 1;
static int falseValue = 0;


/* structure definitions */
const char StdboolDefs[] = "typedef int bool;";

/* creates various system-dependent definitions */
void StdboolSetupFunc(Hawthorn *hc)
{
    /* defines */
    VariableDefinePlatformVar(hc, NULL, "true", &hc->IntType, (union AnyValue *)&trueValue, FALSE);
    VariableDefinePlatformVar(hc, NULL, "false", &hc->IntType, (union AnyValue *)&falseValue, FALSE);
    VariableDefinePlatformVar(hc, NULL, "__bool_true_false_are_defined", &hc->IntType, (union AnyValue *)&trueValue, FALSE);
}

#endif /* !BUILTIN_MINI_STDLIB */
