//================================================================
//  Includes
//----------------------------------------------------------------
#include "../swin.h"
#include "../types.h"
#include "../array.h"
#include "../complex.h"
#include "../fft.h"
#include "../mathex.h"
#include "../profile.h"

#include "fftlib.h"

#define PROF_MAIN		"fft/proftodo.inc"
#include "../profdata.h"


//#define USE_RADIX2


//================================================================
//  ROM
//----------------------------------------------------------------
#define SQRT12		D(  7.071067811865476e-1 )


#define B1_3		D( -1.500000000000000e+0 )
#define B2_3		D(  8.660254037844387e-1 )


#define B1_5		D( -1.250000000000000e+0 )
#define B2_5		D(  5.590169943749475e-1 )
#define B3_5		D(  9.510565162951535e-1 )
#define B4_5		D(  1.538841768587627e+0 )
#define B5_5		D( -3.632712640026803e-1 )


#define B1_7		D( -1.166666666666667e+0 )
#define B2_7		D(  7.901564685254001e-1 )
#define B3_7		D(  5.585426728964774e-2 )
#define B4_7		D(  7.343022012357523e-1 )
#define B5_7		D(  4.409585518440984e-1 )
#define B6_7		D(  3.408729306239314e-1 )
#define B7_7		D( -5.339693603377252e-1 )
#define B8_7		D(  8.748422909616567e-1 )


#define B1_9		D( -5.000000000000000e-1 )
#define B2_9		D(  8.660254037844387e-1 )
#define B3_9		D(  3.420201433256689e-1 )
#define B4_9		D(  9.848077530122080e-1 )
#define B5_9		D(  6.427876096865393e-1 )
#define B6_9		D(  9.396926207859082e-1 )
#define B7_9		D(  1.736481776669304e-1 )
#define B8_9		D(  7.660444431189780e-1 )


STATIC FFT_INST g_pElementaryFFT[] = 
{
	{	9, fft_9	},
	{	8, fft_8	},
	{	7, fft_7	},
	{	5, fft_5	},
	{	4, fft_4	},
	{	3, fft_3	},
	{	2, fft_2	},
};

STATIC INT g_nOfElementaryFFT = numelems( g_pElementaryFFT );


//================================================================
//  Get module information
//----------------------------------------------------------------
LIBINFO_FUNCTION
(
	fft,
	"FFT-DCT4-MDCT",
	"FFT, DCT4 and MDCT by Cooley-Tukey, Good-Thomas and Winograd algorithms",
	"Copyright (c) 2007-10 PetrovSE",
	"2.0.0.2"
);


//================================================================
//  FFT 2
//----------------------------------------------------------------
VOID fft_2( HFFT p_inst, PCOMPLEX p_x, INT step )
{
	COMPLEX a0 = p_x[0];
	COMPLEX a1 = p_x[step];

	unref_param( p_inst );

	cmplx_add( *p_x, a0, a1 ); p_x += step;
	cmplx_sub( *p_x, a0, a1 );
}


//================================================================
//  FFT 3
//----------------------------------------------------------------
VOID fft_3( HFFT p_inst, PCOMPLEX p_x, INT step )
{
	COMPLEX a0, a1, a2;
	COMPLEX b1, b2, t;

	unref_param( p_inst );

	b1 = p_x[step];
	b2 = p_x[2 * step];

	cmplx_add( a1,   b1, b2 );
	cmplx_sub( a2,   b1, b2 );
	cmplx_add( a0, *p_x, a1 );

	cmplx_mul_real( b1, a1, B1_3 );
	cmplx_mul_real( b2, a2, B2_3 );

	cmplx_add( t, a0, b1 );

	p_x[0] = a0;
	p_x += step; cmplx_sub_conj( *p_x, t, b2 );
	p_x += step; cmplx_add_conj( *p_x, t, b2 );
}


//================================================================
//  FFT 4
//----------------------------------------------------------------
VOID fft_4( HFFT p_inst, PCOMPLEX p_x, INT step )
{
	COMPLEX a0, a1, a2, a3;
	COMPLEX t0, t1;

	COMPLEX *p0 = p_x;
	COMPLEX *p1 = &p_x[step * 2];

	unref_param( p_inst );

	cmplx_sub( a2, *p0, *p1 );
	cmplx_add( t0, *p0, *p1 );

	p0 += step; p1 += step;
	cmplx_sub( a3, *p0, *p1 );
	cmplx_add( t1, *p0, *p1 );

	cmplx_add( a0, t0, t1 );
	cmplx_sub( a1, t0, t1 );

	p_x[0]		= a0;
	p_x += step; cmplx_sub_conj( *p_x, a2, a3 );
	p_x += step; p_x[0]	= a1;
	p_x += step; cmplx_add_conj( *p_x, a2, a3 );
}


//================================================================
//  FFT 5
//----------------------------------------------------------------
VOID fft_5( HFFT p_inst, PCOMPLEX p_x, INT step )
{
	COMPLEX t0, t1, t2, t3, t4;
	COMPLEX a0, a1, a2, a3, a4, a5;
	COMPLEX b1, b2, b3, b4, b5;

	COMPLEX *p0 = &p_x[step];
	COMPLEX *p1 = &p_x[step * 4];

	unref_param( p_inst );

	cmplx_add( t0, *p0, *p1 );
	cmplx_sub( a5, *p0, *p1 );

	p0 += step; p1 -= step;
	cmplx_add( t1, *p0, *p1 );
	cmplx_sub( a4, *p1, *p0 );

	cmplx_add( a1, t0, t1 );
	cmplx_sub( a2, t0, t1 );
	cmplx_add( a3, a4, a5 );
	cmplx_add( a0, *p_x, a1 );


	cmplx_mul_real( b1, a1, B1_5 );
	cmplx_mul_real( b2, a2, B2_5 );
	cmplx_mul_real( b3, a3, B3_5 );
	cmplx_mul_real( b4, a4, B4_5 );
	cmplx_mul_real( b5, a5, B5_5 );

	
	cmplx_add( t0, a0, b1 );
	cmplx_sub( t1, b3, b4 );
	cmplx_add( t2, b3, b5 );
	
	cmplx_add( t3, t0, b2 );
	cmplx_sub( t4, t0, b2 );

	
	p_x[0] = a0;
	p_x += step; cmplx_sub_conj( *p_x, t3, t1 );
	p_x += step; cmplx_sub_conj( *p_x, t4, t2 );
	p_x += step; cmplx_add_conj( *p_x, t4, t2 );
	p_x += step; cmplx_add_conj( *p_x, t3, t1 );
}


//================================================================
//  FFT 7
//----------------------------------------------------------------
VOID fft_7( HFFT p_inst, PCOMPLEX p_x, INT step )
{
	COMPLEX t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12;
	COMPLEX a0, a1, a2, a3, a4, a5, a6, a7, a8;
	COMPLEX b1, b2, b3, b4, b5, b6, b7, b8;

	COMPLEX *p0 = &p_x[step];
	COMPLEX *p1 = &p_x[step * 6];

	unref_param( p_inst );

	cmplx_add( t0, *p0, *p1 );
	cmplx_sub( t1, *p0, *p1 );

	p0 += step; p1 -= step;
	cmplx_add( t2, *p0, *p1 );
	cmplx_sub( t3, *p0, *p1 );

	p0 += step; p1 -= step;
	cmplx_add( t4, *p1, *p0 );
	cmplx_sub( t5, *p1, *p0 );

	cmplx_add( t6, t2, t0 );
	cmplx_add( t7, t5, t3 );
	
	cmplx_sub( a4,   t2, t0 );
	cmplx_sub( a2,   t0, t4 );
	cmplx_sub( a3,   t4, t2 );
	cmplx_sub( a7,   t5, t3 );
	cmplx_sub( a6,   t1, t5 );
	cmplx_sub( a8,   t3, t1 );
	cmplx_add( a1,   t6, t4 );
	cmplx_add( a5,   t7, t1 );
	cmplx_add( a0, *p_x, a1 );


	cmplx_mul_real( b1, a1, B1_7 );
	cmplx_mul_real( b2, a2, B2_7 );
	cmplx_mul_real( b3, a3, B3_7 );
	cmplx_mul_real( b4, a4, B4_7 );
	cmplx_mul_real( b5, a5, B5_7 );
	cmplx_mul_real( b6, a6, B6_7 );
	cmplx_mul_real( b7, a7, B7_7 );
	cmplx_mul_real( b8, a8, B8_7 );


	cmplx_add( t0, a0, b1 );
	cmplx_add( t1, b2, b3 );
	cmplx_sub( t2, b4, b3 );
	cmplx_add( t3, b2, b4 );
	cmplx_add( t4, b6, b7 );
	cmplx_sub( t5, b8, b7 );
	cmplx_add( t6, b8, b6 );
	
	cmplx_add( t7,  t0, t1 );
	cmplx_add( t8,  t0, t2 );
	cmplx_sub( t9,  t0, t3 );
	cmplx_add( t10, t4, b5 );
	cmplx_add( t11, t5, b5 );
	cmplx_sub( t12, b5, t6 );

	
	p_x[0] = a0;
	p_x += step; cmplx_sub_conj( *p_x, t7, t10 );
	p_x += step; cmplx_sub_conj( *p_x, t9, t12 );
	p_x += step; cmplx_add_conj( *p_x, t8, t11 );
	p_x += step; cmplx_sub_conj( *p_x, t8, t11 );
	p_x += step; cmplx_add_conj( *p_x, t9, t12 );
	p_x += step; cmplx_add_conj( *p_x, t7, t10 );
}


//================================================================
//  FFT 8
//----------------------------------------------------------------
VOID fft_8( HFFT p_inst, PCOMPLEX p_x, INT step )
{
	COMPLEX t0, t1, t2, t3, t4, t5, t6, t7;
	COMPLEX a0, a1, a2, a3, a4, a5, a6, a7;

	COMPLEX *p0 = p_x;
	COMPLEX *p1 = &p_x[step * 4];

	unref_param( p_inst );
	
	cmplx_add( t0, *p0, *p1 );
	cmplx_sub( a3, *p0, *p1 );

	p0 += step; p1 += step;
	cmplx_add( t1, *p0, *p1 );
	cmplx_sub( t2, *p0, *p1 );

	p0 += step; p1 += step;
	cmplx_add( t3, *p0, *p1 );
	cmplx_sub( a6, *p0, *p1 );

	p0 += step; p1 += step;
	cmplx_add( t4, *p0, *p1 );
	cmplx_sub( t5, *p0, *p1 );
	
	cmplx_add( t6, t0, t3 );
	cmplx_sub( a2, t0, t3 );
	cmplx_add( t7, t1, t4 );
	cmplx_sub( a5, t1, t4 );
	cmplx_sub( a4, t2, t5 );
	cmplx_add( a7, t2, t5 );
	cmplx_add( a0, t6, t7 );
	cmplx_sub( a1, t6, t7 );


	cmplx_mul_real( a4, a4, SQRT12 );
	cmplx_mul_real( a7, a7, SQRT12 );
	

	cmplx_add( t0, a3, a4 );
	cmplx_sub( t1, a3, a4 );
	cmplx_add( t2, a6, a7 );
	cmplx_sub( t3, a6, a7 );
	
	
	p_x[0] = a0;
	p_x += step; cmplx_sub_conj( *p_x, t0, t2 );
	p_x += step; cmplx_sub_conj( *p_x, a2, a5 );
	p_x += step; cmplx_add_conj( *p_x, t1, t3 );
	p_x += step; p_x[0]	= a1;
	p_x += step; cmplx_sub_conj( *p_x, t1, t3 );
	p_x += step; cmplx_add_conj( *p_x, a2, a5 );
	p_x += step; cmplx_add_conj( *p_x, t0, t2 );
}


//================================================================
//  FFT 9
//----------------------------------------------------------------
VOID fft_9( HFFT p_inst, PCOMPLEX p_x, INT step )
{
	COMPLEX t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11;
	COMPLEX a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11;

	COMPLEX *p0 = &p_x[step];
	COMPLEX *p1 = &p_x[step * 8];
	
	unref_param( p_inst );
	
	cmplx_add( t1, *p0,  *p1 );
	cmplx_sub( t5, *p0,  *p1 );

	p0 += step;	p1 -= step;
	cmplx_add( t2, *p0, *p1 );
	cmplx_sub( t6, *p0, *p1 );
	
	p0 += step;	p1 -= step;
	cmplx_add( t3, *p0, *p1 );
	cmplx_sub( t7, *p0, *p1 );

	p0 += step;	p1 -= step;
	cmplx_add( t4, *p0, *p1 );
	cmplx_sub( t8, *p0, *p1 );
	
	cmplx_add( t9,  t1,  t2 );
	cmplx_add( t9,  t9,  t4 );
	cmplx_sub( t10, t5,  t6 );
	cmplx_add( t10, t10, t8 );
	cmplx_add( t11, t3,  *p_x );

	cmplx_add( a6,  t5, t6 );
	cmplx_add( a7,  t6, t8 );
	cmplx_sub( a8,  t5, t8 );
	cmplx_sub( a9,  t2, t1 );
	cmplx_sub( a10, t4, t2 );
	cmplx_sub( a11, t4, t1 );


	cmplx_mul_real( a2,  t3,  B1_9 );
	cmplx_mul_real( a3,  t10, B2_9 );
	cmplx_mul_real( a4,  t9,  B1_9 );
	cmplx_mul_real( a5,  t7,  B2_9 );
	cmplx_mul_real( a6,  a6,  B3_9 );
	cmplx_mul_real( a7,  a7,  B4_9 );
	cmplx_mul_real( a8,  a8,  B5_9 );
	cmplx_mul_real( a9,  a9,  B6_9 );
	cmplx_mul_real( a10, a10, B7_9 );
	cmplx_mul_real( a11, a11, B8_9 );

	
	cmplx_add( a1,  t9, t11 );
	cmplx_add( a2,  a2, *p_x );
	cmplx_add( a4,  a4, t11 );
	
	cmplx_add( t6,  a9,  a10 );
	cmplx_add( t7,  a10, a11 );
	cmplx_sub( t8,  t7,  t6  );
	cmplx_add( t9,  a7,  a8  );
	cmplx_add( t10, a6,  a8  );
	cmplx_sub( t11, t10, t9  );
	
	cmplx_sub( t0,  a2,  t7 );
	cmplx_add( t1,  a2,  t8 );
	cmplx_add( t2,  a2,  t6 );
	cmplx_add( t3,  a5,  t9 );
	cmplx_sub( t4,  t10, a5 );
	cmplx_add( t5,  a5,  t11 );

	
	p_x[0] = a1;
	p_x += step; cmplx_sub_conj( *p_x, t0, t3 );
	p_x += step; cmplx_sub_conj( *p_x, t1, t4 );
	p_x += step; cmplx_sub_conj( *p_x, a4, a3 );
	p_x += step; cmplx_sub_conj( *p_x, t2, t5 );
	p_x += step; cmplx_add_conj( *p_x, t2, t5 );
	p_x += step; cmplx_add_conj( *p_x, a4, a3 );
	p_x += step; cmplx_add_conj( *p_x, t1, t4 );
	p_x += step; cmplx_add_conj( *p_x, t0, t3 );
}


//================================================================
//  Bitreserse functions
//----------------------------------------------------------------
STATIC PBR_INST bitreverse_index_delete( PBR_INST h_inst )
{
	if( arrcheck( h_inst ) )
	{
		arrfree( h_inst->p_index );
		arrfree( h_inst->p_itab );
	}

	arrfree( h_inst );
	return NULL;
}


STATIC PBR_INST bitreverse_index_create( INT len )
{
	BR_INST *h_inst;

	arralloc_unit( h_inst );

	while( arrcheck( h_inst ) )
	{
		INT log_len = binlog( len ) - 1;
		INT k, i, m = log_len;

		//----------------------------------------------------------------

		arrzero_unit( h_inst );

		h_inst->len   = len;
		h_inst->n_bit = one_fpp( div2( m ) );
		m = one_fpp( div2( m )  + ( m & 0x1 ) );

		//----------------------------------------------------------------

		arralloc( h_inst->p_index, len );
		if( !arrcheck( h_inst->p_index ) )
		{
			break;
		}

		arralloc( h_inst->p_itab, m );
		if( !arrcheck( h_inst->p_itab ) )
		{
			break;
		}

		//----------------------------------------------------------------

		for( i = 0 ; i < len ; i ++ )
		{
			INT j, idx, tmp;

			tmp = i;
			idx = 0;

			for( j = 0 ; j < log_len ; j ++ )
			{
				idx   = ( idx << 1 ) | ( tmp & 0x1 );
				tmp >>= 1;
			}

			h_inst->p_index[i] = idx;
		}

		//----------------------------------------------------------------

		h_inst->p_itab[0] = 0;
		h_inst->p_itab[1] = 1;

		for( k = 1 ; k < m ; k *= 2 )
		{
			for( i = 0 ; i < k ; i ++ )
			{
				h_inst->p_itab[i]		*= 2;
				h_inst->p_itab[i + k]	 = h_inst->p_itab[i] + 1;
			}
		}

		//----------------------------------------------------------------

		return h_inst;
	}
	
	return bitreverse_index_delete( h_inst );
}


STATIC INLINE VOID bitreverse_inplace( PBR_INST h_inst, PCOMPLEX p_x )
{
	INT *p_itab = h_inst->p_itab;
	INT k = h_inst->n_bit;
	INT n;

	for( n = 1 ; n < k ; n ++ )
	{
		INT start = k * p_itab[n];

		INT i = n;
		INT j = start;
		INT m;

		for( m = 1 ; m <= p_itab[n] ; m ++ )
		{
			COMPLEX t = p_x[i];

			p_x[i] = p_x[j];
			p_x[j] = t;

			i += k;
			j  = start + p_itab[m];
		}
	}
}


STATIC INLINE VOID bitreverse_download( PBR_INST h_inst, CONST COMPLEX *p_x, PCOMPLEX p_y, INT step )
{
	INT *p_index = h_inst->p_index;
	INT k, n = h_inst->len;

	for( k = 0 ; k < n ; k ++ )
	{
		p_y[k] = p_x[p_index[k] * step];
	}
}


STATIC INLINE VOID bitreverse_upload( PBR_INST h_inst, CONST COMPLEX *p_x, PCOMPLEX p_y, INT step )
{
	INT *p_index = h_inst->p_index;
	INT k, n = h_inst->len;

	for( k = 0 ; k < n ; k ++ )
	{
		p_y[p_index[k] * step] = p_x[k];
	}
}



//================================================================
//  FFT by the Cooley-Tukey algorithm
//----------------------------------------------------------------
HFFT fft_radix2_init( INT len )
{
	PFFTRADIX2_INST h_inst;

	arralloc_unit( h_inst );

	while( arrcheck( h_inst ) )
	{
		INT i;

		//---------------------------------------------------------------------------

		arrzero_unit( h_inst );

		//---------------------------------------------------------------------------
		
		if( !is_pow2( len ) )
		{
			break;
		}

		//---------------------------------------------------------------------------

		h_inst->len		= len;
		h_inst->cos_len	= len / 2;

		//---------------------------------------------------------------------------

		arralloc( h_inst->p_table, h_inst->cos_len );
		if( !arrcheck( h_inst->p_table ) )
		{
			break;	
		}

		h_inst->h_index = bitreverse_index_create( h_inst->len );
		if( !arrcheck( h_inst->h_index ) )
		{
			break;	
		}
		
		arralloc( h_inst->p_temp, h_inst->len );
		if( !arrcheck( h_inst->p_temp ) )
		{
			break;	
		}

		//---------------------------------------------------------------------------
		
		for( i = 0 ; i < h_inst->cos_len ; i ++ )
		{
			DATA x = (DATA)( 2.0 * M_PI * (DATA)i / (DATA)h_inst->len  );
			cmplx_make( h_inst->p_table[i], (DATA)cos( x ), -(DATA)sin( x ) );
		}

		//---------------------------------------------------------------------------

		return (HFFT)h_inst;
	}

	//---------------------------------------------------------------------------

	return fft_radix2_deinit( (HFFT)h_inst );
}


HFFT fft_radix2_deinit( HFFT p_inst )
{
	PFFTRADIX2_INST h_inst = (PFFTRADIX2_INST )p_inst;

	if( arrcheck( h_inst ) )
	{
		arrfree( h_inst->p_table );
		arrfree( h_inst->p_temp );

		bitreverse_index_delete( h_inst->h_index );
	}

	arrfree( h_inst );
	return NULL;
}


VOID fft_radix2( HFFT p_inst, PCOMPLEX p_x, INT step )
{
	PFFTRADIX2_INST h_inst = (PFFTRADIX2_INST )p_inst;
	PCOMPLEX p_work = p_x;

	INT k, n = h_inst->len;
	INT pos;
	

	//================================================================
	//  Bit reverse
	//----------------------------------------------------------------
	if( step == 1 )
	{
		bitreverse_inplace( h_inst->h_index, p_work );
	}
	else
	{
		p_work = h_inst->p_temp;
		bitreverse_download( h_inst->h_index, p_x, p_work, step );
	}


	//================================================================
	//  Zero butterfly
	//----------------------------------------------------------------
	for( k = 0 ; k < n ; k += 2 )
	{
		COMPLEX d = p_work[k + 1];

		cmplx_sub( p_work[k + 1], p_work[k + 0], d );
		cmplx_add( p_work[k + 0], p_work[k + 0], d );
	}


	//================================================================
	//  Main cycle
	//----------------------------------------------------------------
	pos = n / 2;
	k   = 2;

	do
	{
		CONST COMPLEX *p_tbl = h_inst->p_table;
		INT i;
		
		pos /= 2;

		for( i = 0 ; i < k ; i ++ )
		{
			PCOMPLEX p_dij = &p_work[i];
			PCOMPLEX p_dii = &p_dij[k];

			INT base;
			
			for( base = 0 ; base < pos ; base ++ )
			{
				COMPLEX d;

				cmplx_mul( d, *p_dii, *p_tbl );
				cmplx_sub( *p_dii, *p_dij, d );
				cmplx_add( *p_dij, *p_dij, d );

				p_dij += 2 * k;
				p_dii += 2 * k;
			}

			p_tbl += pos;
		}

		k *= 2;

	} while( k < n );


	//================================================================
	//  Upload data
	//----------------------------------------------------------------
	if( p_work != p_x )
	{
		for( k = 0 ; k < n ; k ++ )
		{
			( *p_x ) = ( *p_work ++ );
			p_x += step;
		}
	}
}



//================================================================
//  Split-Radix FFT
//----------------------------------------------------------------
HFFT fft_splitradix_init( INT len )
{
	PFFTSPLITRADIX_INST h_inst;

	arralloc_unit( h_inst );

	while( arrcheck( h_inst ) )
	{
		INT i;

		//---------------------------------------------------------------------------

		arrzero_unit( h_inst );

		h_inst->len = len;

		//---------------------------------------------------------------------------

		if( !is_pow2( len ) || len < 4 )
		{
			break;
		}

		//---------------------------------------------------------------------------

		h_inst->h_index = bitreverse_index_create( h_inst->len );

		arralloc( h_inst->p_table, h_inst->len );
		if( !arrcheck( h_inst->p_table ) )
		{
			break;	
		}

		arralloc( h_inst->p_temp, h_inst->len );
		if( !arrcheck( h_inst->p_temp ) )
		{
			break;	
		}

		//---------------------------------------------------------------------------

		for( i = 0 ; i < h_inst->len ; i ++ )
		{
			DATA x = (DATA)( -2.0 * M_PI * (DATA)i / (DATA)h_inst->len );
			cmplx_make( h_inst->p_table[i], (DATA)cos( x ), -(DATA)sin( x ) );
		}

		//---------------------------------------------------------------------------

		return (HFFT)h_inst;
	}

	//---------------------------------------------------------------------------

	return fft_splitradix_deinit( (HFFT)h_inst );
}


HFFT fft_splitradix_deinit( HFFT p_inst )
{
	PFFTSPLITRADIX_INST h_inst = (PFFTSPLITRADIX_INST )p_inst;

	if( arrcheck( h_inst ) )
	{
		arrfree( h_inst->p_table );
		arrfree( h_inst->p_temp );

		bitreverse_index_delete( h_inst->h_index );
	}

	arrfree( h_inst );
	return NULL;
}


VOID fft_splitradix( HFFT p_inst, PCOMPLEX p_x, INT step )
{
	PFFTSPLITRADIX_INST h_inst = (PFFTSPLITRADIX_INST )p_inst;
	PCOMPLEX p_work = p_x;
	PCOMPLEX p_last;

	INT n = h_inst->len;
	INT n2, st, id;


	//================================================================
	//  Dowdload data
	//----------------------------------------------------------------
	if( step != 1 )
	{
		PCOMPLEX ptr = p_x;

		p_work = h_inst->p_temp;

		for( id = 0 ; id < n ; id ++ )
		{
			p_work[id] = *ptr;
			ptr += step;
		}
	}
	
	p_last = &p_work[n];


	//================================================================
	//  Main cycle
	//----------------------------------------------------------------
	for( st = 1, n2 = n ; n2 > 2 ; n2 /= 2, st *= 2 )
	{
		PCOMPLEX p_x0;
		INT n4 = n2 / 4;
		INT n8 = n4 / 2;
		INT j;


		//================================================================
		//  Zero butterfly
		//----------------------------------------------------------------
		p_x0 = p_work;
		id   = 2 * n2;

		do
		{
			PCOMPLEX p_x1 = &p_x0[n4];
			PCOMPLEX p_x2 = &p_x1[n4];
			PCOMPLEX p_x3 = &p_x2[n4];

			do
			{
				COMPLEX b1, b2;

				cmplx_sub(    b1, *p_x0, *p_x2 );
				cmplx_add( *p_x0, *p_x0, *p_x2 );

				cmplx_sub(    b2, *p_x1, *p_x3 );
				cmplx_add( *p_x1, *p_x1, *p_x3 );

				cmplx_sub_conj( *p_x2, b1, b2 );
				cmplx_add_conj( *p_x3, b1, b2 );

				p_x0 += id;
				p_x1 += id;
				p_x2 += id;
				p_x3 += id;

			} while( p_x0 < p_last );

			p_x0 = &p_work[2 * id - n2];
			id  *= 4;

		} while( p_x0 < p_last );


		if( n4 <= 1 )
		{
			continue;
		}


		//================================================================
		//  Primitive butterfly
		//----------------------------------------------------------------
		p_x0 = &p_work[n8];
		id   = 2 * n2;

		do
		{
			PCOMPLEX p_x1 = &p_x0[n4];
			PCOMPLEX p_x2 = &p_x1[n4];
			PCOMPLEX p_x3 = &p_x2[n4];

			do
			{
				COMPLEX b1, b2, b3;

				cmplx_sub(    b1, *p_x2, *p_x0 );
				cmplx_add( *p_x0, *p_x0, *p_x2 );

				cmplx_sub(    b2, *p_x3, *p_x1 );
				cmplx_add( *p_x1, *p_x1, *p_x3 );

				cmplx_add_conj( b3, b2, b1 );
				cmplx_mul_real( b3, b3, SQRT12 );
				cmplx_add_conj( *p_x2, b3, b3 );

				cmplx_add_conj( b3, b1, b2 );
				cmplx_mul_real( b3, b3, SQRT12 );
				cmplx_add_conj( *p_x3, b3, b3 );

				p_x0 += id;
				p_x1 += id;
				p_x2 += id;
				p_x3 += id;

			} while( p_x0 < p_last );

			p_x0 = &p_work[2 * id - n2 + n8];
			id  *= 4;

		} while( p_x0 < ( p_last - 1 ) );


		//================================================================
		//  Full butterfly
		//----------------------------------------------------------------
		for( j = 1 ; j < n8 ; j ++ )
		{
			INT jn = n4 - 2 * j;

			p_x0 = &p_work[j];
			id   = 2 * n2;

			do
			{
				PCOMPLEX p_x1 = &p_x0[n4];
				PCOMPLEX p_x2 = &p_x1[n4];
				PCOMPLEX p_x3 = &p_x2[n4];


				do
				{
					COMPLEX b1, b2, b3;
					INT ofs = (INT)( p_x0 - p_work ) * st;

					CONST COMPLEX *m1 = &h_inst->p_table[ofs & ( n - 1 )];
					CONST COMPLEX *m3 = &h_inst->p_table[( 3 * ofs ) & ( n - 1 )];


					cmplx_sub( b1, *p_x0, *p_x2 );
					cmplx_sub( b2, *p_x3, *p_x1 );

					cmplx_add( *p_x0, *p_x0, *p_x2 );
					cmplx_add( *p_x1, *p_x1, *p_x3 );

					cmplx_add_conj( b3, b1, b2 );
					cmplx_sub_conj( b1, b1, b2 );

					cmplx_mul_conj( *p_x2, b3, *m1 );
					cmplx_mul_conj( *p_x3, b1, *m3 );

					p_x0 += jn;
					p_x1 += jn;
					p_x2 += jn;
					p_x3 += jn;


					cmplx_sub( b1, *p_x0, *p_x2 );
					cmplx_sub( b2, *p_x3, *p_x1 );

					cmplx_add( *p_x0, *p_x0, *p_x2 );
					cmplx_add( *p_x1, *p_x1, *p_x3 );

					cmplx_sub_conj( b3, b2, b1 );
					cmplx_add_conj( b2, b2, b1 );

					cmplx_mul( *p_x2, b3, *m1 );
					cmplx_mul( *p_x3, b2, *m3 );

					p_x0 += ( id - jn );
					p_x1 += ( id - jn );
					p_x2 += ( id - jn );
					p_x3 += ( id - jn );

				} while( p_x0 < p_last );


				p_x0 = &p_work[2 * id - n2 + j];
				id  *= 4;

			} while( p_x0 < p_last );
		}
	}


	//================================================================
	//  Final calculation
	//----------------------------------------------------------------
	{
		PCOMPLEX p_x0 = p_work;
	
		id = 4;

		do
		{
			PCOMPLEX p_x1 = &p_x0[1];

			do
			{
				COMPLEX t = *p_x0;

				cmplx_add( *p_x0, t, *p_x1 );
				cmplx_sub( *p_x1, t, *p_x1 );

				p_x0 += id;
				p_x1 += id;

			} while( p_x0 < p_last );

			p_x0 = &p_work[2 * ( id - 1 )];
			id  *= 4;

		} while( p_x0 < p_last );
	}


	//================================================================
	//  Bit reverse
	//----------------------------------------------------------------
	if( p_work == p_x )
	{
		bitreverse_inplace( h_inst->h_index, p_work );
	}
	else
	{
		bitreverse_upload( h_inst->h_index, p_work, p_x, step );
	}
}


//================================================================
//  DFT
//----------------------------------------------------------------
HFFT dft_init( INT n, INT m, BOOL b_rotate )
{
	PDFT_INST h_inst;

	arralloc_unit( h_inst );

	while( arrcheck( h_inst ) )
	{
		INT i;
		INT len = b_rotate ? n * m : n;
	
		//---------------------------------------------------------------------------

		arrzero_unit( h_inst );

		h_inst->n			= n;
		h_inst->m			= m;
		h_inst->b_rotate	= b_rotate;

		//---------------------------------------------------------------------------

		arralloc( h_inst->p_table, h_inst->n - 1 );
		if( !arrcheck( h_inst->p_table ) )
		{
			break;
		}

		arrzero( h_inst->p_table, h_inst->n - 1 );

		if( !b_rotate )
		{
			arralloc( h_inst->p_temp, len );
			if( !arrcheck( h_inst->p_temp ) )
			{
				break;
			}
		}

		//---------------------------------------------------------------------------

		for( i = 0 ; i < h_inst->n - 1 ; i ++ )
		{
			INT j;
			
			arralloc( h_inst->p_table[i], h_inst->m - 1 );
			if( !arrcheck( h_inst->p_table[i] ) )
			{
				i = -1;
				break;
			}

			for( j = 0 ; j < h_inst->m - 1 ; j ++ )
			{
				DATA x = (DATA)( 2.0 * M_PI * (DATA)( i + 1 ) * (DATA)( j + 1 ) / (DATA)len );
				cmplx_make( h_inst->p_table[i][j], (DATA)cos( x ), -(DATA)sin( x ) );
			}
		}

		//---------------------------------------------------------------------------

		if( less_zero( i ) )
		{
			break;
		}

		return (HFFT)h_inst;
	}

	//---------------------------------------------------------------------------

	return dft_deinit( (HFFT)h_inst );
}


HFFT dft_deinit( HFFT p_inst )
{
	PDFT_INST h_inst = (PDFT_INST )p_inst;

	if( arrcheck( h_inst ) )
	{
		if( arrcheck( h_inst->p_table ) )
		{
			INT n;

			for( n = 0 ; n < h_inst->n - 1 ; n ++ )
			{
				arrfree( h_inst->p_table[n] );
			}
		}

		arrfree( h_inst->p_table );
		arrfree( h_inst->p_temp );
	}

	arrfree( h_inst );
	return NULL;
}


VOID dft( HFFT p_inst, PCOMPLEX p_x, INT step )
{
	PDFT_INST h_inst = (PDFT_INST )p_inst;

	//---------------------------------------------------------------------------

	if( h_inst->b_rotate )
	{
		PCOMPLEX *p_table = h_inst->p_table;
		INT len = h_inst->n - 1;
		INT cnt = h_inst->m;

		p_x += cnt * step;
		
		for( ; len ; len -- )
		{
			PCOMPLEX p_tbl = *p_table ++;
			INT m;
			
			p_x += step;
			
			for( m = cnt - 1 ; m ; m -- )
			{
				cmplx_mul( *p_x, *p_x, *p_tbl );

				p_x   += step;
				p_tbl ++;
			}
		}
	}
	else
	{
		PCOMPLEX *p_table = h_inst->p_table;
		PCOMPLEX  p_temp  = h_inst->p_temp;

		INT len = h_inst->n;
		INT cnt = h_inst->m;
		INT n;

		//---------------------------------------------------------------------------

		for( n = 0 ; n < len ; n ++ )
		{
			p_temp[n] = p_x[n * step];
		}

		//---------------------------------------------------------------------------

		for( n = 0 ; n < len ; n ++ )
		{
			COMPLEX res;
		
			if( is_zero( n ) )
			{
				INT m;

				cmplx_zero( res );
			
				for( m = 0 ; m < cnt ; m ++ )
				{
					cmplx_add( res, res, p_temp[m] );
				}
			}
			else
			{
				PCOMPLEX p_tbl  = *p_table ++;
				PCOMPLEX p_data = &p_temp[1];
				INT m;

				res = p_temp[0];
			
				for( m = cnt - 1 ; m ; m -- )
				{
					COMPLEX tmp;
				
					cmplx_mul( tmp, *p_data, *p_tbl );
					cmplx_add( res, res, tmp );

					p_tbl  ++;
					p_data ++;
				}
			}
			
			*p_x = res;
			p_x += step;
		}
	}
}


//================================================================
//  Index functions
//----------------------------------------------------------------
STATIC INLINE VOID move_by_index
(
	PCOMPLEX	p_dst,
	PCOMPLEX	p_src,
	INT			*p_index,
	INT			len
)
{
	if( arrcheck( p_index ) )
	{
		for( ; len ; len -- )
		{
			p_dst[*p_index ++] = *p_src ++;
		}
		
		return;
	}
	
	//---------------------------------------------------------------------------
	
	arrcpy( p_dst, p_src, len );
}


STATIC INLINE VOID load_by_index
(
	PCOMPLEX	p_dst,
	PCOMPLEX	p_src,
	INT			*p_index,
	INT			len,
	INT			step
)
{
	if( arrcheck( p_index ) )
	{
		for( ; len ; len -- )
		{
			p_dst[*p_index ++] = *p_src;
			p_src += step;
		}
			
		return;
	}

	//---------------------------------------------------------------------------

	for( ; len ; len -- )
	{
		( *p_dst ++ ) = *p_src;
		p_src += step;
	}
}


STATIC INLINE VOID save_by_index
(
	PCOMPLEX	p_dst,
	PCOMPLEX	p_src,
	INT			*p_index,
	INT			len,
	INT			step
)
{
	if( arrcheck( p_index ) )
	{
		for( ; len ; len -- )
		{
			p_dst[( *p_index ++ ) * step] = *p_src ++;
		}
				
		return;
	}
			
	//---------------------------------------------------------------------------
		
	for( ; len ; len -- )
	{
		( *p_dst ) = *p_src ++;
		p_dst += step;
	}
}


STATIC BOOL index_is_linear( INT *p_index, INT len )
{
	INT i;

	for( i = 0 ; i < len ; i ++ )
	{
		if( i != p_index[i] )
		{
			return FALSE;
		}
	}

	return TRUE;
}


STATIC BOOL make_gt_input_index( INT n, INT m, INT **pp_index )
{
	INT *p_index;
	INT len = n * m;
	INT i, k;

	//---------------------------------------------------------------------------

	arralloc( p_index, len );
	if( !arrcheck( p_index ) )
	{
		return FALSE;
	}

	//---------------------------------------------------------------------------

	for( i = k = 0 ; i < n ; i ++ )
	{
		INT j;

		for( j = 0 ; j < m ; j ++ )
		{
			p_index[( i * m + j * n ) % len] = k ++;
		}
	}

	//---------------------------------------------------------------------------

	if( index_is_linear( p_index, len ) )
	{
		arrfree( p_index );
		p_index = NULL;
	}

	*pp_index = p_index;

	return TRUE;
}


STATIC BOOL make_gt_output_index( INT n, INT m, INT **pp_index )
{
	INT *p_index;
	INT len = n * m;
	INT nn, mm;
	INT i, k;

	//---------------------------------------------------------------------------

	arralloc( p_index, len );
	if( !arrcheck( p_index ) )
	{
		return FALSE;
	}

	//---------------------------------------------------------------------------

	euclid_ext( n, m, &nn, &mm );
	
	for( i = k = 0 ; i < n ; i ++ )
	{
		INT j;
		
		for( j = 0 ; j < m ; j ++ )
		{
			p_index[k ++] = mod( i * m * mm + j * n * nn, len );
		}
	}

	//---------------------------------------------------------------------------

	if( index_is_linear( p_index, len ) )
	{
		arrfree( p_index );
		p_index = NULL;
	}
	
	*pp_index = p_index;

	return TRUE;
}


STATIC BOOL make_ct_input_index( INT n, INT m, INT **p_index )
{
	unref_param( n );
	unref_param( m );

	*p_index = NULL;

	return TRUE;
}


STATIC BOOL make_ct_output_index( INT n, INT m, INT **pp_index )
{
	INT *p_index;
	INT len = n * m;
	INT i, k;

	//---------------------------------------------------------------------------

	arralloc( p_index, len );
	if( !arrcheck( p_index ) )
	{
		return FALSE;
	}

	//---------------------------------------------------------------------------

	for( i = k = 0 ; i < m ; i ++ )
	{
		INT j;

		for( j = i ; j < len ; j += m )
		{
			p_index[j] = k ++;
		}
	}

	//---------------------------------------------------------------------------

	if( index_is_linear( p_index, len ) )
	{
		arrfree( p_index );
		p_index = NULL;
	}
	
	*pp_index = p_index;
	
	return TRUE;
}


//================================================================
//  FFT
//----------------------------------------------------------------
STATIC INT get_length_divisor( INT len )
{
	INT n_prime = get_prime( len );
	INT n_pow2  = get_pow2( len );
	INT n_elem  = 1;
	INT i;

	//---------------------------------------------------------------------------

	for( i = 0 ; i < g_nOfElementaryFFT ; i ++ )
	{
		INT elem_len = g_pElementaryFFT[i].len;

		if( is_zero( len % elem_len ) )
		{
			n_elem = elem_len;
			break;
		}
	}

	//---------------------------------------------------------------------------

	if( n_pow2 > n_elem )
	{
		return n_pow2;
	}

	if( n_elem > 1 )
	{
		return n_elem;
	}

	return n_prime == 1 ? len : n_prime;
}


STATIC BOOL get_fft_by_length( INT len, PFFT_INST h_inst )
{
	INT i;

	//---------------------------------------------------------------------------

	arrzero_unit( h_inst );

	//---------------------------------------------------------------------------

	if( len == 1 )
	{
		h_inst->len = 1;
		return TRUE;
	}

	//---------------------------------------------------------------------------

	for( i = 0 ; i < g_nOfElementaryFFT ; i ++ )
	{
		if( len == g_pElementaryFFT[i].len )
		{
			h_inst->len		= len;
			h_inst->fn_proc	= g_pElementaryFFT[i].fn_proc;
			return TRUE;
		}
	}

	//---------------------------------------------------------------------------

	h_inst->len = len;
	
	if( is_pow2( len ) )
	{
	#ifdef USE_RADIX2
		h_inst->fn_proc		= fft_radix2;
		h_inst->fn_deinit	= fft_radix2_deinit;
		h_inst->h_inst		= fft_radix2_init( h_inst->len );
	#else
		h_inst->fn_proc		= fft_splitradix;
		h_inst->fn_deinit	= fft_splitradix_deinit;
		h_inst->h_inst		= fft_splitradix_init( h_inst->len );
	#endif
	}
	else
	{
		h_inst->fn_proc		= dft;
		h_inst->fn_deinit	= dft_deinit;
		h_inst->h_inst		= dft_init( h_inst->len, h_inst->len, FALSE );
	}
	
	return arrcheck( h_inst->h_inst );
}


STATIC BOOL search_fft( INT len, PFFT_INST h_inst, BOOL b_first )
{
	INT n_divisor = get_length_divisor( len );

	//---------------------------------------------------------------------------

	if( b_first || n_divisor == len )
	{
		return get_fft_by_length( n_divisor, h_inst );
	}

	//---------------------------------------------------------------------------

	h_inst->len			= len;
	h_inst->fn_proc		= fft;
	h_inst->fn_deinit	= fft_deinit;
	h_inst->h_inst		= fft_init( h_inst->len );

	return arrcheck( h_inst->h_inst );
}


STATIC VOID subfft_deinit( PFFT_INST p_inst )
{
	if( !invalid_ptr( p_inst->fn_deinit ) && arrcheck( p_inst->h_inst ) )
	{
		p_inst->fn_deinit( p_inst->h_inst );
	}
}


HFFT fft_init( INT len )
{
	PNODEFFT_INST h_inst;

	arralloc_unit( h_inst );

	while( arrcheck( h_inst ) )
	{
		INT  n, m;

		arrzero_unit( h_inst );

		//---------------------------------------------------------------------------

		h_inst->len = len;

		arralloc( h_inst->p_temp, h_inst->len );
		if( !arrcheck( h_inst->p_temp ) )
		{
			break;
		}

		//---------------------------------------------------------------------------

		if( !search_fft( h_inst->len, &h_inst->fft_lines, TRUE ) )
		{
			break;
		}

		if( !search_fft( h_inst->len / h_inst->fft_lines.len, &h_inst->fft_rows, FALSE ) )
		{
			break;
		}

		//---------------------------------------------------------------------------

		n = h_inst->fft_lines.len;
		m = h_inst->fft_rows.len;

		//---------------------------------------------------------------------------

		if( euclid( n, m ) == 1 )
		{
			if
			(
				!make_gt_input_index( n, m, &h_inst->p_index_in )
				||
				!make_gt_output_index( n, m, &h_inst->p_index_out )
			)
			{
				break;
			}
		}
		else
		{
			if
			(
				!make_ct_input_index( n, m, &h_inst->p_index_in )
				||
				!make_ct_output_index( n, m, &h_inst->p_index_out )
			)
			{
				break;
			}

			h_inst->p_rotate_inst = (PDFT_INST)dft_init( n, m, TRUE );
			if( !arrcheck( h_inst->p_rotate_inst ) )
			{
				break;
			}
		}

		h_inst->b_no_index =
			!arrcheck( h_inst->p_index_in )
			&&
			!arrcheck( h_inst->p_index_out );

		//---------------------------------------------------------------------------

		return (HFFT)h_inst;
	}

	//---------------------------------------------------------------------------

	return fft_deinit( (HFFT)h_inst );
}


HFFT fft_deinit( HFFT p_inst )
{
	PNODEFFT_INST h_inst = (PNODEFFT_INST)p_inst;

	if( arrcheck( h_inst ) )
	{
		subfft_deinit( &h_inst->fft_lines );
		subfft_deinit( &h_inst->fft_rows );

		if( !invalid_ptr( h_inst->p_rotate_inst ) )
		{
			dft_deinit( (HFFT)h_inst->p_rotate_inst );
		}

		arrfree( h_inst->p_index_in );
		arrfree( h_inst->p_index_out );
		arrfree( h_inst->p_temp );
	}

	arrfree( h_inst );
	return NULL;
}


VOID fft( HFFT p_inst, PCOMPLEX p_x, INT step )
{
	PNODEFFT_INST h_inst = (PNODEFFT_INST)p_inst;
	PCOMPLEX p_temp = h_inst->p_temp;
	PCOMPLEX p_work = p_x;

	//---------------------------------------------------------------------------

	BP_START( FFT_INP_INDEX );

	if( !h_inst->b_no_index )
	{
		p_work = p_temp;

		if( step == 1 )
		{
			move_by_index
				(
					p_work,
					p_x,
					h_inst->p_index_in,
					h_inst->len
				);
		}
		else
		{
			load_by_index
				(
					p_work,
					p_x,
					h_inst->p_index_in,
					h_inst->len,
					step
				);
		}
	}

	BP_STOP( FFT_INP_INDEX );

	//---------------------------------------------------------------------------

	if( !invalid_ptr( h_inst->fft_lines.fn_proc ) )
	{
		PCOMPLEX ptr = p_work;
		INT len = h_inst->fft_rows.len;
		INT i;

		for( i = 0 ; i < len ; i ++ )
		{
			h_inst->fft_lines.fn_proc( h_inst->fft_lines.h_inst, ptr ++, len );
		}
	}

	//---------------------------------------------------------------------------

	BP_START( FFT_ROTATE );

	if( h_inst->p_rotate_inst )
	{
		dft( (HFFT)h_inst->p_rotate_inst, p_work, 1 );
	}

	BP_STOP( FFT_ROTATE );

	//---------------------------------------------------------------------------

	if( !invalid_ptr( h_inst->fft_rows.fn_proc ) )
	{
		PCOMPLEX ptr = p_work;
		INT len  = h_inst->fft_lines.len;
		INT step = h_inst->fft_rows.len;
		INT i;

		for( i = 0 ; i < len ; i ++ )
		{
			h_inst->fft_rows.fn_proc( h_inst->fft_rows.h_inst, ptr, 1 );
			ptr += step;
		}
	}

	//---------------------------------------------------------------------------

	BP_START( FFT_OUT_INDEX );

	if( p_work != p_x )
	{
		if( step == 1 )
		{
			move_by_index
				(
					p_x,
					p_work,
					h_inst->p_index_out,
					h_inst->len
				);
		}
		else
		{
			save_by_index
				(
					p_x,
					p_work,
					h_inst->p_index_out,
					h_inst->len,
					step
				);
		}
	}

	BP_STOP( FFT_OUT_INDEX );
}


//================================================================
//  Inverse FFT Processing
//----------------------------------------------------------------
VOID ifft_preproc( PCOMPLEX p_x, INT len, INT step )
{
	for( ; len ; len -- )
	{
		cmplx_conj( *p_x, *p_x );
		p_x += step;
	}
}


VOID ifft_postproc( PCOMPLEX p_x, INT len, INT step )
{
	DATA fc = (DATA)1.0 / (DATA)len;
	
	for( ; len ; len -- )
	{
		cmplx_conj( *p_x, *p_x );
		cmplx_mul_real( *p_x, *p_x, fc );

		p_x += step;
	}
}
