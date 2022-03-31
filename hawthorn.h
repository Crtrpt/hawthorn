/* Hawthorn external interface. This should be the only header you need to use if
 * you're using Hawthorn as a library. Internal details are in interpreter.h */
#ifndef Hawthorn_H
#define Hawthorn_H

/* Hawthorn version number */
#ifdef VER
#define Hawthorn_VERSION "v0.0.1 beta r" VER /* VER is the subversion version number, obtained via the Makefile */
#else
#define Hawthorn_VERSION "v0.0.1"
#endif

/* handy definitions */
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#include "interpreter.h"


#if defined(UNIX_HOST)
#include <setjmp.h>

/* this has to be a macro, otherwise errors will occur due to the stack being corrupt */
#define HawthornPlatformSetExitPoint(hc) setjmp((hc)->HawthornExitBuf)
#endif

/* parse.c */
void HawthornParse(Hawthorn *hc, const char *FileName, const char *Source, int SourceLen, int RunIt, int CleanupNow, int CleanupSource, int EnableDebugger);
void HawthornParseInteractive(Hawthorn *hc);

/* platform.c */
void HawthornCallMain(Hawthorn *hc, int argc, char **argv);
void HawthornInitialise(Hawthorn *hc, int StackSize);
void HawthornCleanup(Hawthorn *hc);
void HawthornPlatformScanFile(Hawthorn *hc, const char *FileName);

/* include.c */
void HawthornIncludeAllSystemHeaders(Hawthorn *hc);

#endif /* Hawthorn_H */
