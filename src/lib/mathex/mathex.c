//================================================================
//  Includes
//----------------------------------------------------------------
#include <stdio.h>
#include <string.h>

#include "../swin.h"
#include "../types.h"
#include "../array.h"
#include "../mathex.h"
#include "mathexlib.h"


//================================================================
//  ROM
//----------------------------------------------------------------
STATIC INT g_pFirstPrimes[] =
{
	2,		3,		5,		7,		11,		13,		17,		19,
	23,		29,		31,		37,		41,		43,		47,		53,
	59,		61,		67,		71,		73,		79,		83,		89,
	97,		101,	103,	107,	109,	113,	127,	131,
	137,	139,	149,	151,	157,	163,	167,	173,
	179,	181,	191,	193,	197,	199,	211,	223,
	227,	229,	233,	239,	241,	251,	257,	263,
	269,	271,	277,	281,	283,	293,	307,	311,
	313,	317,	331,	337,	347,	349,	353,	359,
	367,	373,	379,	383,	389,	397,	401,	409,
	419,	421,	431,	433,	439,	443,	449,	457,
	461,	463,	467,	479,	487,	491,	499,	503,
	509,	521,	523,	541,	547,	557,	563,	569,
	571,	577,	587,	593,	599,	601,	607,	613,
	617,	619,	631,	641,	643,	647,	653,	659,
	661,	673,	677,	683,	691,	701,	709,	719,
	727,	733,	739,	743,	751,	757,	761,	769,
	773,	787,	797,	809,	811,	821,	823,	827,
	829,	839,	853,	857,	859,	863,	877,	881,
	883,	887,	907,	911,	919,	929,	937,	941,
	947,	953,	967,	971,	977,	983,	991,	997,
	1009,	1013,	1019,	1021,	1031,	1033,	1039,	1049,
	1051,	1061,	1063,	1069,	1087,	1091,	1093,	1097,
	1103,	1109,	1117,	1123,	1129,	1151,	1153,	1163,
	1171,	1181,	1187,	1193,	1201,	1213,	1217,	1223,
};

STATIC INT g_nOfFirstPrimes = numelems( g_pFirstPrimes );


//================================================================
//  Euclid algorithms
//----------------------------------------------------------------
INT euclid( INT a, INT b )
{
	if( b > a )
	{
		INT tmp = a;

		a = b;
		b = tmp;
	}

	loopinf
	{
		INT tmp = a % b;

		if( is_zero( tmp ) )
			break;

		a = b;
		b = tmp;
	}

	return b;
}


INT euclid_ext( INT a, INT b, INT *p_x, INT *p_y )
{
	INT x2 = 1;
	INT x1 = 0;
	INT y2 = 0;
	INT y1 = 1;


	if( is_zero( a ) || is_zero( b ) )
	{
		*p_x = 0;
		*p_y = 0;
		return 0;
	}


	while( more_zero( b ) )
	{
		INT q = a / b;
		INT r = a  - q * b;

		*p_x = x2 - q * x1;
		*p_y = y2 - q * y1;

		a = b;
		b = r;

		x2 = x1;
		x1 = *p_x;
		y2 = y1;
		y1 = *p_y;
	}


	*p_x = x2;
	*p_y = y2;
	return a;
}


//================================================================
//  Others integer algorithms
//----------------------------------------------------------------
INT get_pow2( INT x )
{
	INT n;

	for( n = 1 ; is_even( x ) ; n = mul2( n ), x = div2( x ) );

	return n;
}


INT binlog( INT x )
{
	INT n;

	for( n = 0 ; x ; n ++, x = div2( x ) );

	return n;
}


INT binweight( DWORD x )
{
	INT n = 0;

	while( x )
	{
		n  += x & 0x1;
		x >>= 1;
	}

	return n;
}


INT get_prime( INT x )
{
	INT res = 1;
	INT n;

	for( n = 0 ; n < g_nOfFirstPrimes ; n ++ )
	{
		INT n_prime = g_pFirstPrimes[n];

		if( is_zero( x % n_prime ) && x != n_prime )
		{
			res = n_prime;
			break;
		}
	}

	return res;
}


//================================================================
//  Show number functions
//----------------------------------------------------------------
INT get_digits( INT x )
{
	INT n;

	x = labs( x );

	for( n = 0 ; ; )
	{
		n ++;
		x /= 10;

		if( is_zero( x ) )
			break;
	}

	return n;
}


//================================================================
//  dB function
//----------------------------------------------------------------
DATA to_db( DATA val )
{
	if( less_zero( val ) || isnan( val ) )
		return NAN;

	if( is_zero( val ) )
		return -INFINITY;

	if( isinf( val ) )
		return INFINITY;

	return (DATA)10.0 * (DATA)log10( val );
}


//================================================================
//  MSE functions
//----------------------------------------------------------------
HMSE mse_open( DWORD flag )
{
	PMSE_INST h_inst;

	arralloc_unit( h_inst );

	if( arrcheck( h_inst ) )
	{
		h_inst->flag = flag;
		return (HMSE)h_inst;
	}

	return mse_close( (HMSE)h_inst );
}


HMSE mse_close( HMSE p_inst )
{
	PMSE_INST h_inst = (PMSE_INST )p_inst;

	if( arrcheck( h_inst ) )
		arrfree( h_inst );
	
	return NULL;
}


VOID mse_reset( HMSE p_inst )
{
	PMSE_INST h_inst = (PMSE_INST )p_inst;

	if( arrcheck( h_inst ) )
	{
		PMSEERR_INST	p_err	= &h_inst->err_inst;
		PMSERANGE_INST	p_range	= &h_inst->range_inst;
		PMSEDIFF_INST	p_diff	= &h_inst->diff_inst;

		h_inst->count = 0;

		//----------------------------------------------------------------

		p_err->diff	= D( 0.0 );
		p_err->gain	= D( 0.0 );

		//----------------------------------------------------------------

		cmplx_make( p_range->vMin.cmplx,  INFINITY,  INFINITY );
		cmplx_make( p_range->vMax.cmplx, -INFINITY, -INFINITY );

		p_range->vMin.data =  INFINITY;
		p_range->vMax.data = -INFINITY;

		p_range->vMin.long_int = INT32_MAX;
		p_range->vMax.long_int = INT32_MIN;

		//----------------------------------------------------------------
		
		arrzero_unit( p_diff );

		p_diff->first_pos	= INVALID_SIZE;
		p_diff->max_pos		= INVALID_SIZE;
	}
}


VOID mse_complex( HMSE p_inst, CONST COMPLEX *p_x, CONST COMPLEX *p_y, COMPLEX *p_r, INT len, INT step )
{
	PMSE_INST h_inst = (PMSE_INST )p_inst;

	if( arrcheck( h_inst ) )
	{
		PMSEERR_INST	p_err	= &h_inst->err_inst;
		PMSERANGE_INST	p_range	= &h_inst->range_inst;
		PMSEDIFF_INST	p_diff	= &h_inst->diff_inst;
		INT i;
		
		//----------------------------------------------------------------
		
		for( i = 0 ; i < len ; i += step )
		{
			COMPLEX diff;
			
			cmplx_sub( diff, *p_x, *p_y );

			//----------------------------------------------------------------
			
			if( flag_check_mask( h_inst->flag, MSE_FLAG_ERROR ) )
			{
				p_err->diff += cmplx_abs2( diff );
				p_err->gain += cmplx_abs2( *p_x );
			}
			
			//----------------------------------------------------------------
			
			if( flag_check_mask( h_inst->flag, MSE_FLAG_RANGE ) )
			{
				cmplx_min( p_range->vMin.cmplx, p_range->vMin.cmplx, *p_x );
				cmplx_max( p_range->vMax.cmplx, p_range->vMax.cmplx, *p_x );
			}
			
			//----------------------------------------------------------------
			
			if( flag_check_mask( h_inst->flag, MSE_FLAG_DIFF ) )
			{
				DATA fdiff = cmplx_abs2( diff );

				if( !invalid_ptr( p_r ) )
				{
					( *p_r ) = diff;
					p_r += step;
				}

				//----------------------------------------------------------------
				
				if( fdiff > D( 0 ) )
				{
					p_diff->total_pos ++;
					
					if( p_diff->first_pos == INVALID_SIZE )
					{
						p_diff->vFirst.cmplx	= diff;
						p_diff->first_pos		= h_inst->count;
					}
				}
				
				if( cmplx_abs2( p_diff->vMax.cmplx ) < fdiff )
				{
					p_diff->vMax.cmplx	= diff;
					p_diff->max_pos		= h_inst->count;
				}
			}
			
			//----------------------------------------------------------------
			
			p_x += step;
			p_y += step;
			h_inst->count ++;
		}

	}
}


VOID mse_data( HMSE p_inst, CONST DATA *p_x, CONST DATA *p_y, DATA *p_r, INT len, INT step )
{
	PMSE_INST h_inst = (PMSE_INST )p_inst;

	if( arrcheck( h_inst ) )
	{
		PMSEERR_INST	p_err	= &h_inst->err_inst;
		PMSERANGE_INST	p_range	= &h_inst->range_inst;
		PMSEDIFF_INST	p_diff	= &h_inst->diff_inst;
		INT i;

		//----------------------------------------------------------------

		for( i = 0 ; i < len ; i += step )
		{
			DATA diff = ( *p_x ) - ( *p_y );

			//----------------------------------------------------------------

			if( flag_check_mask( h_inst->flag, MSE_FLAG_ERROR ) )
			{
				p_err->diff += sqr( diff );
				p_err->gain += sqr( D( *p_x ) );
			}

			//----------------------------------------------------------------

			if( flag_check_mask( h_inst->flag, MSE_FLAG_RANGE ) )
			{
				p_range->vMin.data = min( p_range->vMin.data, *p_x );
				p_range->vMax.data = max( p_range->vMax.data, *p_x );
			}
			
			//----------------------------------------------------------------

			if( flag_check_mask( h_inst->flag, MSE_FLAG_DIFF ) )
			{
				if( !invalid_ptr( p_r ) )
				{
					( *p_r ) = diff;
					p_r += step;
				}

				//----------------------------------------------------------------

				diff = (DATA)fabs( diff );
				
				if( diff > D( 0 ) )
				{
					p_diff->total_pos ++;
					
					if( p_diff->first_pos == INVALID_SIZE )
					{
						p_diff->vFirst.data	= diff;
						p_diff->first_pos	= h_inst->count;
					}
				}
				
				if( p_diff->vMax.data < diff )
				{
					p_diff->vMax.data	= diff;
					p_diff->max_pos		= h_inst->count;
				}
			}

			//----------------------------------------------------------------

			p_x += step;
			p_y += step;
			h_inst->count ++;
		}
	}
}


VOID mse_long( HMSE p_inst, CONST LONG *p_x, CONST LONG *p_y, LONG *p_r, INT len, INT step )
{
	PMSE_INST h_inst = (PMSE_INST )p_inst;

	if( arrcheck( h_inst ) )
	{
		PMSEERR_INST	p_err	= &h_inst->err_inst;
		PMSERANGE_INST	p_range = &h_inst->range_inst;
		PMSEDIFF_INST	p_diff	= &h_inst->diff_inst;
		INT i;
		
		//----------------------------------------------------------------
		
		for( i = 0 ; i < len ; i += step )
		{
			LONG diff = ( *p_x ) - ( *p_y );
			
			//----------------------------------------------------------------
			
			if( flag_check_mask( h_inst->flag, MSE_FLAG_ERROR ) )
			{
				p_err->diff += sqr( D( diff ) );
				p_err->gain += sqr( D( *p_x ) );
			}
			
			//----------------------------------------------------------------
			
			if( flag_check_mask( h_inst->flag, MSE_FLAG_RANGE ) )
			{
				p_range->vMin.long_int = min( p_range->vMin.long_int, *p_x );
				p_range->vMax.long_int = max( p_range->vMax.long_int, *p_x );
			}
			
			//----------------------------------------------------------------
			
			if( flag_check_mask( h_inst->flag, MSE_FLAG_DIFF ) )
			{
				if( !invalid_ptr( p_r ) )
				{
					( *p_r ) = diff;
					p_r += step;
				}
				
				//----------------------------------------------------------------
				
				diff = labs( diff );
				
				if( diff > 0 )
				{
					p_diff->total_pos ++;
					
					if( p_diff->first_pos == INVALID_SIZE )
					{
						p_diff->vFirst.long_int	= diff;
						p_diff->first_pos		= h_inst->count;
					}
				}
				
				if( p_diff->vMax.long_int < diff )
				{
					p_diff->vMax.long_int	= diff;
					p_diff->max_pos			= h_inst->count;
				}
			}
			
			//----------------------------------------------------------------
			
			p_x += step;
			p_y += step;
			h_inst->count ++;
		}
	}
}

//----------------------------------------------------------------

DWORD mse_get_cnt( HMSE p_inst, DWORD flag )
{
	PMSE_INST h_inst = (PMSE_INST )p_inst;

	if( arrcheck( h_inst ) )
	{
		PMSEDIFF_INST p_diff = &h_inst->diff_inst;

		switch( flag )
		{
		case MSE_CNT_COUNTER:
			return h_inst->count;

		case MSE_CNT_DIFF_TOTAL:
			return p_diff->total_pos;

		case MSE_CNT_DIFF_FIRST:
			return p_diff->first_pos;

		case MSE_CNT_DIFF_MAX:
			return p_diff->max_pos;
		}
	}

	return 0;
}


COMBOTYPE mse_get_val( HMSE p_inst, DWORD flag )
{
	PMSE_INST h_inst = (PMSE_INST )p_inst;

	if( arrcheck( h_inst ) )
	{
		PMSEERR_INST	p_err	= &h_inst->err_inst;
		PMSERANGE_INST	p_range	= &h_inst->range_inst;
		PMSEDIFF_INST	p_diff	= &h_inst->diff_inst;

		//----------------------------------------------------------------

		switch( flag )
		{
		case MSE_VAL_ERROR:
			{
				COMBOTYPE ret;

				loopinf
				{
					if( more_zero( p_err->gain ) )
					{
						ret.data = p_err->diff / p_err->gain;
						break;
					}

					if( more_zero( p_err->diff ) )
					{
						ret.data = INFINITY;
						break;
					}

					ret.data = NAN;
					break;
				}

				//----------------------------------------------------------------

				cmplx_make_real( ret.cmplx, ret.data );
				ret.long_int = (LONG)ret.data;

				return ret;
			}

		case MSE_VAL_RANGE_MIN:
			return p_range->vMin;

		case MSE_VAL_RANGE_MAX:
			return p_range->vMax;

		case MSE_VAL_DIFF_FIRST:
			return p_diff->vFirst;

		case MSE_VAL_DIFF_MAX:
			return p_diff->vMax;
		}
	}

	//---------------------------------------------------------------

	{
		COMBOTYPE res;
		arrzero_unit( &res );
		return res;
	}
}
