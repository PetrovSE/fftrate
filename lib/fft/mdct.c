//================================================================
//  Includes
//----------------------------------------------------------------
#include "../swin.h"
#include "../types.h"
#include "../array.h"
#include "../complex.h"
#include "../dct4.h"
#include "../mdct.h"
#include "../mathex.h"
#include "../profile.h"

#include "mdctlib.h"

#define PROF_SLAVE		"fft/proftodo.inc"
#include "../profdata.h"


//================================================================
//  Open & close functions
//----------------------------------------------------------------
HMDCT mdct_init( INT len )
{
	PMDCT_INST h_inst;
	
	arralloc_unit( h_inst );
	
	while( arrcheck( h_inst ) )
	{
		arrzero_unit( h_inst );

		//----------------------------------------------------------------

		if( len % 4 )
		{
			break;
		}

		h_inst->len		= len;
		h_inst->len2	= len / 2;
		h_inst->len4	= len / 4;

		//----------------------------------------------------------------

		arralloc( h_inst->p_temp, h_inst->len2 );
		if( !arrcheck( h_inst->p_temp ) )
		{
			break;
		}

		//----------------------------------------------------------------

		h_inst->h_dct4 = dct4_init( h_inst->len2 );
		if( invalid_ptr( h_inst->h_dct4 ) )
		{
			break;
		}

		//----------------------------------------------------------------

		return (HMDCT)h_inst;
	}

	return NULL;
}


HMDCT mdct_deinit( HMDCT p_inst )
{
	PMDCT_INST h_inst = (PMDCT_INST )p_inst;

	if( arrcheck( h_inst ) )
	{
		arrfree( h_inst->p_temp );

		if( !invalid_ptr( h_inst->h_dct4 ) )
		{
			dct4_deinit( h_inst->h_dct4 );
		}
	}

	arrfree( h_inst );
	return NULL;
}


//================================================================
//  Transform functions
//----------------------------------------------------------------
VOID mdct_forward( HMDCT p_inst, CONST DATA *p_x, DATA *p_y )
{
	PMDCT_INST h_inst = (PMDCT_INST )p_inst;
	CONST DATA *ptr1  = &p_x[h_inst->len4 * 3];
	CONST DATA *ptr2  = ptr1 - 1;
	DATA *ptr = h_inst->p_temp;
	INT n;

	//----------------------------------------------------------------

	BP_START( MDCT_PRE_PROC );

	for( n = h_inst->len4 ; n ; n -- )
	{
		( *ptr ++ ) = ( *ptr2 -- ) + ( *ptr1 ++ );
	}

	for( ptr1 = p_x, n = h_inst->len4 ; n ; n -- )
	{
		( *ptr ++ ) = ( *ptr2 -- ) - ( *ptr1 ++ );
	}

	BP_STOP( MDCT_PRE_PROC );

	//----------------------------------------------------------------

	dct4( h_inst->h_dct4, h_inst->p_temp, p_y );
}


VOID mdct_inverse( HMDCT p_inst, CONST DATA *p_y, DATA *p_x )
{
	PMDCT_INST h_inst = (PMDCT_INST )p_inst;
	DATA *ptr = &h_inst->p_temp[h_inst->len4];
	INT n;

	//----------------------------------------------------------------

	dct4( h_inst->h_dct4, p_y, h_inst->p_temp );

	//----------------------------------------------------------------
	
	BP_START( MDCT_POST_PROC );

	for( n = h_inst->len4 ; n ; n -- )
	{
		( *p_x ++ ) = -( *ptr ++ );
	}

	for( ptr --, n = h_inst->len4 ; n ; n -- )
	{
		( *p_x ++ ) = ( *ptr -- );
	}

	for( n = h_inst->len4 ; n ; n -- )
	{
		( *p_x ++ ) = ( *ptr -- );
	}

	for( ptr ++, n = h_inst->len4 ; n ; n -- )
	{
		( *p_x ++ ) = ( *ptr ++ );
	}

	BP_STOP( MDCT_POST_PROC );
}
