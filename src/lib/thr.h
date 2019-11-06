#ifndef _THR_H_
#define _THR_H_


//================================================================
//  Includes
//----------------------------------------------------------------
#include "interface.h"


//================================================================
//  Function prototypes
//----------------------------------------------------------------
CDECL_BEGIN

VOID thr_sleep( DWORD timeout_ms );
LONGLONG thr_time( VOID );

CDECL_END

#endif // _THR_H_
