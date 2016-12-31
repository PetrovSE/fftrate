#ifndef _LOGFILE_H_
#define _LOGFILE_H_


//================================================================
//  Includes
//----------------------------------------------------------------
#include "interface.h"


//================================================================
//  Definitions
//----------------------------------------------------------------
#define lprintf				log_lock( _F_, _L_ ); log_unlock

#define lfailed				lprintf( "Failed!" )
#define lok					lprintf( "Ok." )


#if defined(IS_GCC)
#define _F_					__func__
#define _L_					__LINE__

#elif defined(IS_VC6)
#define _F_					__FILE__
#define _L_					__LINE__

#elif defined(IS_VC)
#define _F_					__FUNCTION__
#define _L_					__LINE__

#else
#define _F_					""
#define _L_					-1
#endif


//================================================================
//  Function prototypes
//----------------------------------------------------------------
CDECL_BEGIN

BOOL log_open( CONST CHAR *name );
VOID log_close( VOID );

VOID log_lock( CONST CHAR *func_name, INT line_numb );
VOID log_unlock( CONST CHAR *fmt, ... );

CDECL_END

#endif // _LOGFILE_H_
