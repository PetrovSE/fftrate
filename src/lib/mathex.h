#ifndef _MATHEX_H_
#define _MATHEX_H_


//================================================================
//  Includes
//----------------------------------------------------------------
#include <stdlib.h>
#include <limits.h>
#include <math.h>

#include "types.h"
#include "interface.h"
#include "complex.h"
#include "mathmac.h"


//================================================================
//  MSE definitions
//----------------------------------------------------------------
#define MSE_FLAG_ERROR					0x0001
#define MSE_FLAG_RANGE					0x0002
#define MSE_FLAG_DIFF					0x0004

#define MSE_FLAG_DEFAULT				( MSE_FLAG_ERROR | MSE_FLAG_RANGE )


#define MSE_CNT_COUNTER					0x0000

#define MSE_CNT_DIFF_TOTAL				0x0100
#define MSE_CNT_DIFF_FIRST				0x0101
#define MSE_CNT_DIFF_MAX				0x0102


#define MSE_VAL_ERROR					0x0000

#define MSE_VAL_RANGE_MIN				0x0100
#define MSE_VAL_RANGE_MAX				0x0101

#define MSE_VAL_DIFF_FIRST				0x0200
#define MSE_VAL_DIFF_MAX				0x0201


//================================================================
//  Constant
//----------------------------------------------------------------
#define DATA_EPS						D( 1e-30 )
#define LOG_TO_LOG2						D( 1.442695040888963e+0 )

#ifndef M_PI
#define M_PI							D( 3.141592653589793e+0 )
#endif


//================================================================
//  Type definitions
//----------------------------------------------------------------
DECL_HANDLE( HMSE );


//================================================================
//  Function prototypes
//----------------------------------------------------------------
CDECL_BEGIN

LIBINFO_PROTOTYPE( mathex );


INT euclid( INT a, INT b );
INT euclid_ext( INT a, INT b, INT *x, INT *y );

INT get_pow2( INT x );
INT binlog( INT x );
INT binweight( DWORD x );
INT get_prime( INT x );

INT get_digits( INT x );
DATA to_db( DATA val );


//================================================================
//  MSE functions
//----------------------------------------------------------------
HMSE mse_open( DWORD flag );
HMSE mse_close( HMSE p_inst );
VOID mse_reset( HMSE p_inst );

VOID mse_complex( HMSE p_inst, CONST COMPLEX *p_x, CONST COMPLEX *p_y, COMPLEX *p_r, INT len, INT step );
VOID mse_data( HMSE p_inst, CONST DATA *p_x, CONST DATA *p_y, DATA *p_r, INT len, INT step );
VOID mse_long( HMSE p_inst, CONST LONG *p_x, CONST LONG *p_y, LONG *p_r, INT len, INT step );

DWORD mse_get_cnt( HMSE p_inst, DWORD flag );
COMBOTYPE mse_get_val( HMSE p_inst, DWORD flag );


//================================================================
//  Float functions
//----------------------------------------------------------------

#ifdef HIGH_PRECISION
	#define __builtin_infd		__builtin_inf
	#define __builtin_nand		__builtin_nan
#else
	#define __builtin_infd		__builtin_inff
	#define __builtin_nand		__builtin_nanf
#endif

#ifndef INFINITY
#define INFINITY		__builtin_infd()
#endif

#ifndef NAN
#define NAN				__builtin_nand( "" )
#endif


#if defined(IS_VC)

DOUBLE __builtin_inf( VOID );
FLOAT  __builtin_inff( VOID );

DOUBLE __builtin_nan( CONST CHAR *str );
FLOAT  __builtin_nanf( CONST CHAR *str );

BOOL isnan( DATA x );
BOOL isinf( DATA x );
BOOL isfinite( DATA x );

#endif // Vidual Studio

CDECL_END

#endif // _MATHEX_H_
