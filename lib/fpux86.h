#ifndef _FPUX86_H_
#define _FPUX86_H_


//================================================================
//  Includes
//----------------------------------------------------------------
#include <limits.h>

#include "interface.h"
#include "types.h"


//================================================================
//  Definitions
//----------------------------------------------------------------
#define FPU_EXCEPTION_MASK		0x003f
#define FPU_INVALID				0x0001
#define FPU_DENORMAL			0x0002
#define FPU_ZERODIVIDE			0x0004
#define FPU_OVERFLOW			0x0008
#define FPU_UNDERFLOW			0x0010
#define FPU_INEXACT				0x0020

#define FPU_PREC_MASK			0x0300
#define FPU_PREC_SINGLE			0x0000
#define FPU_PREC_DOUBLE			0x0200
#define FPU_PREC_EXTENDED		0x0300

#define FPU_ROUND_MASK			0x0c00
#define FPU_ROUND_NEAR			0x0000
#define FPU_ROUND_DOWN			0x0400
#define FPU_ROUND_UP			0x0800
#define FPU_ROUND_CHOP			0x0c00

#define FPU_MASK_ALL			0x1f3f
#define FPU_MOD_DEFAULT			( FPU_EXCEPTION_MASK | FPU_PREC_DOUBLE | FPU_ROUND_CHOP )

#define ATTN					"\n\t"


//================================================================
//  Function prototypes
//----------------------------------------------------------------
CDECL_BEGIN

DWORD fpux86_get_state( VOID );
VOID  fpux86_set_state( DWORD control );

VOID fpux86_modify_state( DWORD control, DWORD mask );
VOID fpux86_set_default( VOID );

DWORD fpux86_get_rounding_mode( VOID );
VOID  fpux86_set_rounding_mode( DWORD round );

CDECL_END


STATIC INLINE LONG fpux86_get_long( DATA flt )
{
	LONG retval;
    
#if defined(IS_VC)
	_asm
	{
		fld		flt
		fistp	retval
	}
#elif defined(IS_GCC)
	__asm__ __volatile__ 
	(
		"fistpl	%0"

		: "=m" (retval)
		: "t" (flt)
		: "st"
	);
#else
	retval = (LONG)flt;
#endif
	
    return retval;
}


STATIC INLINE LONG fpux86_clip_long( DATA flt )
{
	if( flt > INT32_MAX )
	{
		return INT32_MAX;
	}

	if( flt < INT32_MIN )
	{
		return INT32_MIN;
	}

	return fpux86_get_long( flt );
}

#endif // _FPUX86_H_
