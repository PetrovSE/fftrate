//================================================================
//  Includes
//----------------------------------------------------------------
#include "../swin.h"
#include "../types.h"
#include "../array.h"
#include "../complex.h"
#include "../fft.h"
#include "../dct4.h"
#include "../mathex.h"
#include "../profile.h"

#include "dct4lib.h"

#define PROF_SLAVE		"fft/proftodo.inc"
#include "../profdata.h"


//================================================================
//  Open & close functions
//----------------------------------------------------------------
HDCT4 dct4_init( INT len )
{
	PDCT4_INST h_inst;
	
	arralloc_unit( h_inst );
	
	while( arrcheck( h_inst ) )
	{
		DATA coef;
		INT n;

		arrzero_unit( h_inst );

		//----------------------------------------------------------------

		if( len % 2 )
			break;

		h_inst->len		= len;
		h_inst->len2	= len / 2;

		//----------------------------------------------------------------

		arralloc( h_inst->pK1, h_inst->len2 );
		arralloc( h_inst->pK2, h_inst->len2 );
		arralloc( h_inst->p_temp, h_inst->len2 );

		if
		(
			!arrcheck( h_inst->pK1 ) 
			||
			!arrcheck( h_inst->pK2 )
			||
			!arrcheck( h_inst->p_temp )
		)
		{
			break;
		}

		//----------------------------------------------------------------

		h_inst->h_fft = fft_init( h_inst->len2 );
		if( invalid_ptr( h_inst->h_fft ) )
			break;

		//----------------------------------------------------------------

		coef = (DATA)sqrt( D( 2.0 ) / D( h_inst->len ) );

		for( n = 0 ; n < h_inst->len2 ; n ++ )
		{
			DATA x = -M_PI * ( D( n ) + D( 0.25 ) ) / D( h_inst->len );

			cmplx_make( h_inst->pK1[n], (DATA)cos( x ), (DATA)sin( x ) );
			cmplx_mul_real( h_inst->pK1[n], h_inst->pK1[n], coef );

			x = -M_PI * D( n ) / D( h_inst->len );

			cmplx_make( h_inst->pK2[n], (DATA)cos( x ), (DATA)sin( x ) );
		}

		//----------------------------------------------------------------

		return (HDCT4)h_inst;
	}

	return NULL;
}


HDCT4 dct4_deinit( HDCT4 p_inst )
{
	PDCT4_INST h_inst = (PDCT4_INST )p_inst;

	if( arrcheck( h_inst ) )
	{
		arrfree( h_inst->pK1 );
		arrfree( h_inst->pK2 );
		arrfree( h_inst->p_temp );

		if( !invalid_ptr( h_inst->h_fft ) )
			fft_deinit( h_inst->h_fft );
	}

	arrfree( h_inst );
	return NULL;
}


//================================================================
//  Transform function
//----------------------------------------------------------------
VOID dct4( HDCT4 p_inst, CONST DATA *p_x, DATA *p_y )
{
	PDCT4_INST h_inst = (PDCT4_INST )p_inst;
	INT n;

	//----------------------------------------------------------------
	
	BP_START( DCT4_PRE_PROC );
	
	{
		CONST COMPLEX	*ptrK	= h_inst->pK1;
		CONST DATA		*p_t	= &p_x[h_inst->len - 1];
		COMPLEX			*ptr	= h_inst->p_temp;

		for( n = h_inst->len2 ; n ; n --, ptr ++, ptrK ++ )
		{
			COMPLEX temp;
			cmplx_make( temp, *p_x, *p_t );
			cmplx_mul( *ptr, temp, *ptrK );

			p_x += 2;
			p_t -= 2;
		}
	}

	BP_STOP( DCT4_PRE_PROC );
	
	//----------------------------------------------------------------

	fft( h_inst->h_fft, h_inst->p_temp, 1 );

	//----------------------------------------------------------------
	
	BP_START( DCT4_POST_PROC );

	{
		CONST COMPLEX *ptrK = h_inst->pK2;
		COMPLEX	*ptr = h_inst->p_temp;
		DATA	*p_t = &p_y[h_inst->len - 1];

		for( n = h_inst->len2 ; n ; n --, ptr ++, ptrK ++ )
		{
			COMPLEX temp;
			cmplx_mul( temp, *ptr, *ptrK );

			*p_y =  temp.re;
			*p_t = -temp.im;

			p_y += 2;
			p_t -= 2;
		}
	}

	BP_STOP( DCT4_POST_PROC );
}
