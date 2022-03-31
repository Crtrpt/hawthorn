#include "../hawthorn.h"
#include "../interpreter.h"



/* mark where to end the program for platforms which require this */
jmp_buf HawthornExitBuf;

#ifndef NO_DEBUGGER
#include <signal.h>

Hawthorn *break_hc = NULL;

static void BreakHandler(int Signal)
{
    break_hc->DebugManualBreak = TRUE;
}

void PlatformInit(Hawthorn *hc)
{
    /* capture the break signal and pass it to the debugger */
    break_hc = hc;
    signal(SIGINT, BreakHandler);
}
#else
void PlatformInit(Hawthorn *hc)
{
}
#endif

void PlatformCleanup(Hawthorn *hc)
{
}

/* get a line of interactive input */
char *PlatformGetLine(char *Buf, int MaxLen, const char *Prompt)
{
    if (Prompt != NULL)
        printf("%s", Prompt);
        
    fflush(stdout);
    return fgets(Buf, MaxLen, stdin);
}

/* get a character of interactive input */
int PlatformGetCharacter()
{
    fflush(stdout);
    return getchar();
}

/* write a character to the console */
void PlatformPutc(unsigned char OutCh, union OutputStreamInfo *Stream)
{
    putchar(OutCh);
}

/* read a file into memory */
char *PlatformReadFile(Hawthorn *hc, const char *FileName)
{
    struct stat FileInfo;
    char *ReadText;
    FILE *InFile;
    int BytesRead;
    char *p;
    
    if (stat(FileName, &FileInfo))
        ProgramFailNoParser(hc, "can't read file %s\n", FileName);
    
    ReadText = malloc(FileInfo.st_size + 1);
    if (ReadText == NULL)
        ProgramFailNoParser(hc, "out of memory\n");
        
    InFile = fopen(FileName, "r");
    if (InFile == NULL)
        ProgramFailNoParser(hc, "can't read file %s\n", FileName);
    
    BytesRead = fread(ReadText, 1, FileInfo.st_size, InFile);
    if (BytesRead == 0)
        ProgramFailNoParser(hc, "can't read file %s\n", FileName);

    ReadText[BytesRead] = '\0';
    fclose(InFile);
    
    if ((ReadText[0] == '#') && (ReadText[1] == '!'))
    {
        for (p = ReadText; (*p != '\r') && (*p != '\n'); ++p)
        {
            *p = ' ';
        }
    }
    
    return ReadText;    
}

/* read and scan a file for definitions */
void HawthornPlatformScanFile(Hawthorn *hc, const char *FileName)
{
    char *SourceStr = PlatformReadFile(hc, FileName);

    /* ignore "#!/path/to/Hawthorn" .. by replacing the "#!" with "//" */
    if (SourceStr != NULL && SourceStr[0] == '#' && SourceStr[1] == '!') 
    { 
        SourceStr[0] = '/'; 
        SourceStr[1] = '/'; 
    }

    HawthornParse(hc, FileName, SourceStr, strlen(SourceStr), TRUE, FALSE, TRUE, TRUE);
}

/* exit the program */
void PlatformExit(Hawthorn *hc, int RetVal)
{
    hc->HawthornExitValue = RetVal;
    longjmp(hc->HawthornExitBuf, 1);
}

