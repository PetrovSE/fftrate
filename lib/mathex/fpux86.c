//================================================================
//  Includes
//----------------------------------------------------------------
#include "../swin.h"
#include "../types.h"
#include "../fpux86.h"


//================================================================
//  Functions
//----------------------------------------------------------------
DWORD fpux86_get_state( VOID )
{
	DWORD control = 0x0;
	
#if defined(IS_VC)
	__asm fnstcw control;
#elif defined(IS_GCC)
	__asm__ __volatile__ ( "fnstcw %0" : "=m" (control) );
#endif
	
    return control;
}


VOID fpux86_set_state( DWORD control )
{
#if defined(IS_VC)
	__asm fldcw control;
#elif defined(IS_GCC)
	__asm__ __volatile__ ( "fldcw %0" : : "m" (control) );
#endif
}


VOID fpux86_modify_state( DWORD control, DWORD mask )
{
	DWORD old_control = fpux86_get_state();
	DWORD new_control = ( ( old_control & ( ~mask ) ) | ( control & mask ) );
	
	fpux86_set_state( new_control );
}


VOID fpux86_set_default( VOID )
{
	fpux86_modify_state( FPU_MOD_DEFAULT, FPU_MASK_ALL );
}


DWORD fpux86_get_rounding_mode( VOID )
{
	return fpux86_get_state() & FPU_ROUND_MASK;
}


VOID fpux86_set_rounding_mode( DWORD round )
{
	fpux86_modify_state( round, FPU_ROUND_MASK );
}
