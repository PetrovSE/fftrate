#ifndef _DCT4_H_
#define _DCT4_H_


//================================================================
//  Includes
//----------------------------------------------------------------
#include "interface.h"


//================================================================
//  Type definitions
//----------------------------------------------------------------
DECL_HANDLE( HDCT4 );


//================================================================
//  Function prototypes
//----------------------------------------------------------------
CDECL_BEGIN

HDCT4 dct4_init( INT len );
HDCT4 dct4_deinit( HDCT4 p_inst );

VOID dct4( HDCT4 p_inst, CONST DATA *p_x, DATA *p_y );

CDECL_END

#endif // _DCT4_H_
