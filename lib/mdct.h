#ifndef _MDCT_H_
#define _MDCT_H_


//================================================================
//  Includes
//----------------------------------------------------------------
#include "interface.h"


//================================================================
//  Type definitions
//----------------------------------------------------------------
DECL_HANDLE( HMDCT );


//================================================================
//  Function prototypes
//----------------------------------------------------------------
CDECL_BEGIN

// WARNING: This MDCT generates a negative spectrum!
HMDCT mdct_init( INT len );
HMDCT mdct_deinit( HMDCT p_inst );

VOID mdct_forward( HMDCT p_inst, CONST DATA *p_x, DATA *p_y );
VOID mdct_inverse( HMDCT p_inst, CONST DATA *p_y, DATA *p_x );

CDECL_END

#endif // _MDCT_H_
