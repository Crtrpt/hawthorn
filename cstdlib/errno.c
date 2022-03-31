/* string.h library for large systems - small embedded systems use clibrary.c instead */
#include <errno.h>
#include "../interpreter.h"

#ifndef BUILTIN_MINI_STDLIB

#ifdef EACCES
static int EACCESValue = EACCES;
#endif

#ifdef EADDRINUSE
static int EADDRINUSEValue = EADDRINUSE;
#endif

#ifdef EADDRNOTAVAIL
static int EADDRNOTAVAILValue = EADDRNOTAVAIL;
#endif

#ifdef EAFNOSUPPORT
static int EAFNOSUPPORTValue = EAFNOSUPPORT;
#endif

#ifdef EAGAIN
static int EAGAINValue = EAGAIN;
#endif

#ifdef EALREADY
static int EALREADYValue = EALREADY;
#endif

#ifdef EBADF
static int EBADFValue = EBADF;
#endif

#ifdef EBADMSG
static int EBADMSGValue = EBADMSG;
#endif

#ifdef EBUSY
static int EBUSYValue = EBUSY;
#endif

#ifdef ECANCELED
static int ECANCELEDValue = ECANCELED;
#endif

#ifdef ECHILD
static int ECHILDValue = ECHILD;
#endif

#ifdef ECONNABORTED
static int ECONNABORTEDValue = ECONNABORTED;
#endif

#ifdef ECONNREFUSED
static int ECONNREFUSEDValue = ECONNREFUSED;
#endif

#ifdef ECONNRESET
static int ECONNRESETValue = ECONNRESET;
#endif

#ifdef EDEADLK
static int EDEADLKValue = EDEADLK;
#endif

#ifdef EDESTADDRREQ
static int EDESTADDRREQValue = EDESTADDRREQ;
#endif

#ifdef EDOM
static int EDOMValue = EDOM;
#endif

#ifdef EDQUOT
static int EDQUOTValue = EDQUOT;
#endif

#ifdef EEXIST
static int EEXISTValue = EEXIST;
#endif

#ifdef EFAULT
static int EFAULTValue = EFAULT;
#endif

#ifdef EFBIG
static int EFBIGValue = EFBIG;
#endif

#ifdef EHOSTUNREACH
static int EHOSTUNREACHValue = EHOSTUNREACH;
#endif

#ifdef EIDRM
static int EIDRMValue = EIDRM;
#endif

#ifdef EILSEQ
static int EILSEQValue = EILSEQ;
#endif

#ifdef EINPROGRESS
static int EINPROGRESSValue = EINPROGRESS;
#endif

#ifdef EINTR
static int EINTRValue = EINTR;
#endif

#ifdef EINVAL
static int EINVALValue = EINVAL;
#endif

#ifdef EIO
static int EIOValue = EIO;
#endif

#ifdef EISCONN
static int EISCONNValue = EISCONN;
#endif

#ifdef EISDIR
static int EISDIRValue = EISDIR;
#endif

#ifdef ELOOP
static int ELOOPValue = ELOOP;
#endif

#ifdef EMFILE
static int EMFILEValue = EMFILE;
#endif

#ifdef EMLINK
static int EMLINKValue = EMLINK;
#endif

#ifdef EMSGSIZE
static int EMSGSIZEValue = EMSGSIZE;
#endif

#ifdef EMULTIHOP
static int EMULTIHOPValue = EMULTIHOP;
#endif

#ifdef ENAMETOOLONG
static int ENAMETOOLONGValue = ENAMETOOLONG;
#endif

#ifdef ENETDOWN
static int ENETDOWNValue = ENETDOWN;
#endif

#ifdef ENETRESET
static int ENETRESETValue = ENETRESET;
#endif

#ifdef ENETUNREACH
static int ENETUNREACHValue = ENETUNREACH;
#endif

#ifdef ENFILE
static int ENFILEValue = ENFILE;
#endif

#ifdef ENOBUFS
static int ENOBUFSValue = ENOBUFS;
#endif

#ifdef ENODATA
static int ENODATAValue = ENODATA;
#endif

#ifdef ENODEV
static int ENODEVValue = ENODEV;
#endif

#ifdef ENOENT
static int ENOENTValue = ENOENT;
#endif

#ifdef ENOEXEC
static int ENOEXECValue = ENOEXEC;
#endif

#ifdef ENOLCK
static int ENOLCKValue = ENOLCK;
#endif

#ifdef ENOLINK
static int ENOLINKValue = ENOLINK;
#endif

#ifdef ENOMEM
static int ENOMEMValue = ENOMEM;
#endif

#ifdef ENOMSG
static int ENOMSGValue = ENOMSG;
#endif

#ifdef ENOPROTOOPT
static int ENOPROTOOPTValue = ENOPROTOOPT;
#endif

#ifdef ENOShc
static int ENOShcValue = ENOShc;
#endif

#ifdef ENOSR
static int ENOSRValue = ENOSR;
#endif

#ifdef ENOSTR
static int ENOSTRValue = ENOSTR;
#endif

#ifdef ENOSYS
static int ENOSYSValue = ENOSYS;
#endif

#ifdef ENOTCONN
static int ENOTCONNValue = ENOTCONN;
#endif

#ifdef ENOTDIR
static int ENOTDIRValue = ENOTDIR;
#endif

#ifdef ENOTEMPTY
static int ENOTEMPTYValue = ENOTEMPTY;
#endif

#ifdef ENOTRECOVERABLE
static int ENOTRECOVERABLEValue = ENOTRECOVERABLE;
#endif

#ifdef ENOTSOCK
static int ENOTSOCKValue = ENOTSOCK;
#endif

#ifdef ENOTSUP
static int ENOTSUPValue = ENOTSUP;
#endif

#ifdef ENOTTY
static int ENOTTYValue = ENOTTY;
#endif

#ifdef ENXIO
static int ENXIOValue = ENXIO;
#endif

#ifdef EOPNOTSUPP
static int EOPNOTSUPPValue = EOPNOTSUPP;
#endif

#ifdef EOVERFLOW
static int EOVERFLOWValue = EOVERFLOW;
#endif

#ifdef EOWNERDEAD
static int EOWNERDEADValue = EOWNERDEAD;
#endif

#ifdef EPERM
static int EPERMValue = EPERM;
#endif

#ifdef EPIPE
static int EPIPEValue = EPIPE;
#endif

#ifdef EPROTO
static int EPROTOValue = EPROTO;
#endif

#ifdef EPROTONOSUPPORT
static int EPROTONOSUPPORTValue = EPROTONOSUPPORT;
#endif

#ifdef EPROTOTYPE
static int EPROTOTYPEValue = EPROTOTYPE;
#endif

#ifdef ERANGE
static int ERANGEValue = ERANGE;
#endif

#ifdef EROFS
static int EROFSValue = EROFS;
#endif

#ifdef ESPIPE
static int ESPIPEValue = ESPIPE;
#endif

#ifdef ESRCH
static int ESRCHValue = ESRCH;
#endif

#ifdef ESTALE
static int ESTALEValue = ESTALE;
#endif

#ifdef ETIME
static int ETIMEValue = ETIME;
#endif

#ifdef ETIMEDOUT
static int ETIMEDOUTValue = ETIMEDOUT;
#endif

#ifdef ETXTBSY
static int ETXTBSYValue = ETXTBSY;
#endif

#ifdef EWOULDBLOCK
static int EWOULDBLOCKValue = EWOULDBLOCK;
#endif

#ifdef EXDEV
static int EXDEVValue = EXDEV;
#endif


/* creates various system-dependent definitions */
void StdErrnoSetupFunc(Hawthorn *hc)
{
    /* defines */
#ifdef EACCES
    VariableDefinePlatformVar(hc, NULL, "EACCES", &hc->IntType, (union AnyValue *)&EACCESValue, FALSE);
#endif

#ifdef EADDRINUSE
    VariableDefinePlatformVar(hc, NULL, "EADDRINUSE", &hc->IntType, (union AnyValue *)&EADDRINUSEValue, FALSE);
#endif

#ifdef EADDRNOTAVAIL
    VariableDefinePlatformVar(hc, NULL, "EADDRNOTAVAIL", &hc->IntType, (union AnyValue *)&EADDRNOTAVAILValue, FALSE);
#endif

#ifdef EAFNOSUPPORT
    VariableDefinePlatformVar(hc, NULL, "EAFNOSUPPORT", &hc->IntType, (union AnyValue *)&EAFNOSUPPORTValue, FALSE);
#endif

#ifdef EAGAIN
    VariableDefinePlatformVar(hc, NULL, "EAGAIN", &hc->IntType, (union AnyValue *)&EAGAINValue, FALSE);
#endif

#ifdef EALREADY
    VariableDefinePlatformVar(hc, NULL, "EALREADY", &hc->IntType, (union AnyValue *)&EALREADYValue, FALSE);
#endif

#ifdef EBADF
    VariableDefinePlatformVar(hc, NULL, "EBADF", &hc->IntType, (union AnyValue *)&EBADFValue, FALSE);
#endif

#ifdef EBADMSG
    VariableDefinePlatformVar(hc, NULL, "EBADMSG", &hc->IntType, (union AnyValue *)&EBADMSGValue, FALSE);
#endif

#ifdef EBUSY
    VariableDefinePlatformVar(hc, NULL, "EBUSY", &hc->IntType, (union AnyValue *)&EBUSYValue, FALSE);
#endif

#ifdef ECANCELED
    VariableDefinePlatformVar(hc, NULL, "ECANCELED", &hc->IntType, (union AnyValue *)&ECANCELEDValue, FALSE);
#endif

#ifdef ECHILD
    VariableDefinePlatformVar(hc, NULL, "ECHILD", &hc->IntType, (union AnyValue *)&ECHILDValue, FALSE);
#endif

#ifdef ECONNABORTED
    VariableDefinePlatformVar(hc, NULL, "ECONNABORTED", &hc->IntType, (union AnyValue *)&ECONNABORTEDValue, FALSE);
#endif

#ifdef ECONNREFUSED
    VariableDefinePlatformVar(hc, NULL, "ECONNREFUSED", &hc->IntType, (union AnyValue *)&ECONNREFUSEDValue, FALSE);
#endif

#ifdef ECONNRESET
    VariableDefinePlatformVar(hc, NULL, "ECONNRESET", &hc->IntType, (union AnyValue *)&ECONNRESETValue, FALSE);
#endif

#ifdef EDEADLK
    VariableDefinePlatformVar(hc, NULL, "EDEADLK", &hc->IntType, (union AnyValue *)&EDEADLKValue, FALSE);
#endif

#ifdef EDESTADDRREQ
    VariableDefinePlatformVar(hc, NULL, "EDESTADDRREQ", &hc->IntType, (union AnyValue *)&EDESTADDRREQValue, FALSE);
#endif

#ifdef EDOM
    VariableDefinePlatformVar(hc, NULL, "EDOM", &hc->IntType, (union AnyValue *)&EDOMValue, FALSE);
#endif

#ifdef EDQUOT
    VariableDefinePlatformVar(hc, NULL, "EDQUOT", &hc->IntType, (union AnyValue *)&EDQUOTValue, FALSE);
#endif

#ifdef EEXIST
    VariableDefinePlatformVar(hc, NULL, "EEXIST", &hc->IntType, (union AnyValue *)&EEXISTValue, FALSE);
#endif

#ifdef EFAULT
    VariableDefinePlatformVar(hc, NULL, "EFAULT", &hc->IntType, (union AnyValue *)&EFAULTValue, FALSE);
#endif

#ifdef EFBIG
    VariableDefinePlatformVar(hc, NULL, "EFBIG", &hc->IntType, (union AnyValue *)&EFBIGValue, FALSE);
#endif

#ifdef EHOSTUNREACH
    VariableDefinePlatformVar(hc, NULL, "EHOSTUNREACH", &hc->IntType, (union AnyValue *)&EHOSTUNREACHValue, FALSE);
#endif

#ifdef EIDRM
    VariableDefinePlatformVar(hc, NULL, "EIDRM", &hc->IntType, (union AnyValue *)&EIDRMValue, FALSE);
#endif

#ifdef EILSEQ
    VariableDefinePlatformVar(hc, NULL, "EILSEQ", &hc->IntType, (union AnyValue *)&EILSEQValue, FALSE);
#endif

#ifdef EINPROGRESS
    VariableDefinePlatformVar(hc, NULL, "EINPROGRESS", &hc->IntType, (union AnyValue *)&EINPROGRESSValue, FALSE);
#endif

#ifdef EINTR
    VariableDefinePlatformVar(hc, NULL, "EINTR", &hc->IntType, (union AnyValue *)&EINTRValue, FALSE);
#endif

#ifdef EINVAL
    VariableDefinePlatformVar(hc, NULL, "EINVAL", &hc->IntType, (union AnyValue *)&EINVALValue, FALSE);
#endif

#ifdef EIO
    VariableDefinePlatformVar(hc, NULL, "EIO", &hc->IntType, (union AnyValue *)&EIOValue, FALSE);
#endif

#ifdef EISCONN
    VariableDefinePlatformVar(hc, NULL, "EISCONN", &hc->IntType, (union AnyValue *)&EISCONNValue, FALSE);
#endif

#ifdef EISDIR
    VariableDefinePlatformVar(hc, NULL, "EISDIR", &hc->IntType, (union AnyValue *)&EISDIRValue, FALSE);
#endif

#ifdef ELOOP
    VariableDefinePlatformVar(hc, NULL, "ELOOP", &hc->IntType, (union AnyValue *)&ELOOPValue, FALSE);
#endif

#ifdef EMFILE
    VariableDefinePlatformVar(hc, NULL, "EMFILE", &hc->IntType, (union AnyValue *)&EMFILEValue, FALSE);
#endif

#ifdef EMLINK
    VariableDefinePlatformVar(hc, NULL, "EMLINK", &hc->IntType, (union AnyValue *)&EMLINKValue, FALSE);
#endif

#ifdef EMSGSIZE
    VariableDefinePlatformVar(hc, NULL, "EMSGSIZE", &hc->IntType, (union AnyValue *)&EMSGSIZEValue, FALSE);
#endif

#ifdef EMULTIHOP
    VariableDefinePlatformVar(hc, NULL, "EMULTIHOP", &hc->IntType, (union AnyValue *)&EMULTIHOPValue, FALSE);
#endif

#ifdef ENAMETOOLONG
    VariableDefinePlatformVar(hc, NULL, "ENAMETOOLONG", &hc->IntType, (union AnyValue *)&ENAMETOOLONGValue, FALSE);
#endif

#ifdef ENETDOWN
    VariableDefinePlatformVar(hc, NULL, "ENETDOWN", &hc->IntType, (union AnyValue *)&ENETDOWNValue, FALSE);
#endif

#ifdef ENETRESET
    VariableDefinePlatformVar(hc, NULL, "ENETRESET", &hc->IntType, (union AnyValue *)&ENETRESETValue, FALSE);
#endif

#ifdef ENETUNREACH
    VariableDefinePlatformVar(hc, NULL, "ENETUNREACH", &hc->IntType, (union AnyValue *)&ENETUNREACHValue, FALSE);
#endif

#ifdef ENFILE
    VariableDefinePlatformVar(hc, NULL, "ENFILE", &hc->IntType, (union AnyValue *)&ENFILEValue, FALSE);
#endif

#ifdef ENOBUFS
    VariableDefinePlatformVar(hc, NULL, "ENOBUFS", &hc->IntType, (union AnyValue *)&ENOBUFSValue, FALSE);
#endif

#ifdef ENODATA
    VariableDefinePlatformVar(hc, NULL, "ENODATA", &hc->IntType, (union AnyValue *)&ENODATAValue, FALSE);
#endif

#ifdef ENODEV
    VariableDefinePlatformVar(hc, NULL, "ENODEV", &hc->IntType, (union AnyValue *)&ENODEVValue, FALSE);
#endif

#ifdef ENOENT
    VariableDefinePlatformVar(hc, NULL, "ENOENT", &hc->IntType, (union AnyValue *)&ENOENTValue, FALSE);
#endif

#ifdef ENOEXEC
    VariableDefinePlatformVar(hc, NULL, "ENOEXEC", &hc->IntType, (union AnyValue *)&ENOEXECValue, FALSE);
#endif

#ifdef ENOLCK
    VariableDefinePlatformVar(hc, NULL, "ENOLCK", &hc->IntType, (union AnyValue *)&ENOLCKValue, FALSE);
#endif

#ifdef ENOLINK
    VariableDefinePlatformVar(hc, NULL, "ENOLINK", &hc->IntType, (union AnyValue *)&ENOLINKValue, FALSE);
#endif

#ifdef ENOMEM
    VariableDefinePlatformVar(hc, NULL, "ENOMEM", &hc->IntType, (union AnyValue *)&ENOMEMValue, FALSE);
#endif

#ifdef ENOMSG
    VariableDefinePlatformVar(hc, NULL, "ENOMSG", &hc->IntType, (union AnyValue *)&ENOMSGValue, FALSE);
#endif

#ifdef ENOPROTOOPT
    VariableDefinePlatformVar(hc, NULL, "ENOPROTOOPT", &hc->IntType, (union AnyValue *)&ENOPROTOOPTValue, FALSE);
#endif

#ifdef ENOShc
    VariableDefinePlatformVar(hc, NULL, "ENOShc", &hc->IntType, (union AnyValue *)&ENOShcValue, FALSE);
#endif

#ifdef ENOSR
    VariableDefinePlatformVar(hc, NULL, "ENOSR", &hc->IntType, (union AnyValue *)&ENOSRValue, FALSE);
#endif

#ifdef ENOSTR
    VariableDefinePlatformVar(hc, NULL, "ENOSTR", &hc->IntType, (union AnyValue *)&ENOSTRValue, FALSE);
#endif

#ifdef ENOSYS
    VariableDefinePlatformVar(hc, NULL, "ENOSYS", &hc->IntType, (union AnyValue *)&ENOSYSValue, FALSE);
#endif

#ifdef ENOTCONN
    VariableDefinePlatformVar(hc, NULL, "ENOTCONN", &hc->IntType, (union AnyValue *)&ENOTCONNValue, FALSE);
#endif

#ifdef ENOTDIR
    VariableDefinePlatformVar(hc, NULL, "ENOTDIR", &hc->IntType, (union AnyValue *)&ENOTDIRValue, FALSE);
#endif

#ifdef ENOTEMPTY
    VariableDefinePlatformVar(hc, NULL, "ENOTEMPTY", &hc->IntType, (union AnyValue *)&ENOTEMPTYValue, FALSE);
#endif

#ifdef ENOTRECOVERABLE
    VariableDefinePlatformVar(hc, NULL, "ENOTRECOVERABLE", &hc->IntType, (union AnyValue *)&ENOTRECOVERABLEValue, FALSE);
#endif

#ifdef ENOTSOCK
    VariableDefinePlatformVar(hc, NULL, "ENOTSOCK", &hc->IntType, (union AnyValue *)&ENOTSOCKValue, FALSE);
#endif

#ifdef ENOTSUP
    VariableDefinePlatformVar(hc, NULL, "ENOTSUP", &hc->IntType, (union AnyValue *)&ENOTSUPValue, FALSE);
#endif

#ifdef ENOTTY
    VariableDefinePlatformVar(hc, NULL, "ENOTTY", &hc->IntType, (union AnyValue *)&ENOTTYValue, FALSE);
#endif

#ifdef ENXIO
    VariableDefinePlatformVar(hc, NULL, "ENXIO", &hc->IntType, (union AnyValue *)&ENXIOValue, FALSE);
#endif

#ifdef EOPNOTSUPP
    VariableDefinePlatformVar(hc, NULL, "EOPNOTSUPP", &hc->IntType, (union AnyValue *)&EOPNOTSUPPValue, FALSE);
#endif

#ifdef EOVERFLOW
    VariableDefinePlatformVar(hc, NULL, "EOVERFLOW", &hc->IntType, (union AnyValue *)&EOVERFLOWValue, FALSE);
#endif

#ifdef EOWNERDEAD
    VariableDefinePlatformVar(hc, NULL, "EOWNERDEAD", &hc->IntType, (union AnyValue *)&EOWNERDEADValue, FALSE);
#endif

#ifdef EPERM
    VariableDefinePlatformVar(hc, NULL, "EPERM", &hc->IntType, (union AnyValue *)&EPERMValue, FALSE);
#endif

#ifdef EPIPE
    VariableDefinePlatformVar(hc, NULL, "EPIPE", &hc->IntType, (union AnyValue *)&EPIPEValue, FALSE);
#endif

#ifdef EPROTO
    VariableDefinePlatformVar(hc, NULL, "EPROTO", &hc->IntType, (union AnyValue *)&EPROTOValue, FALSE);
#endif

#ifdef EPROTONOSUPPORT
    VariableDefinePlatformVar(hc, NULL, "EPROTONOSUPPORT", &hc->IntType, (union AnyValue *)&EPROTONOSUPPORTValue, FALSE);
#endif

#ifdef EPROTOTYPE
    VariableDefinePlatformVar(hc, NULL, "EPROTOTYPE", &hc->IntType, (union AnyValue *)&EPROTOTYPEValue, FALSE);
#endif

#ifdef ERANGE
    VariableDefinePlatformVar(hc, NULL, "ERANGE", &hc->IntType, (union AnyValue *)&ERANGEValue, FALSE);
#endif

#ifdef EROFS
    VariableDefinePlatformVar(hc, NULL, "EROFS", &hc->IntType, (union AnyValue *)&EROFSValue, FALSE);
#endif

#ifdef ESPIPE
    VariableDefinePlatformVar(hc, NULL, "ESPIPE", &hc->IntType, (union AnyValue *)&ESPIPEValue, FALSE);
#endif

#ifdef ESRCH
    VariableDefinePlatformVar(hc, NULL, "ESRCH", &hc->IntType, (union AnyValue *)&ESRCHValue, FALSE);
#endif

#ifdef ESTALE
    VariableDefinePlatformVar(hc, NULL, "ESTALE", &hc->IntType, (union AnyValue *)&ESTALEValue, FALSE);
#endif

#ifdef ETIME
    VariableDefinePlatformVar(hc, NULL, "ETIME", &hc->IntType, (union AnyValue *)&ETIMEValue, FALSE);
#endif

#ifdef ETIMEDOUT
    VariableDefinePlatformVar(hc, NULL, "ETIMEDOUT", &hc->IntType, (union AnyValue *)&ETIMEDOUTValue, FALSE);
#endif

#ifdef ETXTBSY
    VariableDefinePlatformVar(hc, NULL, "ETXTBSY", &hc->IntType, (union AnyValue *)&ETXTBSYValue, FALSE);
#endif

#ifdef EWOULDBLOCK
    VariableDefinePlatformVar(hc, NULL, "EWOULDBLOCK", &hc->IntType, (union AnyValue *)&EWOULDBLOCKValue, FALSE);
#endif

#ifdef EXDEV
    VariableDefinePlatformVar(hc, NULL, "EXDEV", &hc->IntType, (union AnyValue *)&EXDEVValue, FALSE);
#endif

    VariableDefinePlatformVar(hc, NULL, "errno", &hc->IntType, (union AnyValue *)&errno, TRUE);
}

#endif /* !BUILTIN_MINI_STDLIB */
