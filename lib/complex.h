#ifndef _COMPLEX_H_
#define _COMPLEX_H_


//================================================================
//  Includes
//----------------------------------------------------------------
#include <math.h>
#include "types.h"


//================================================================
//  Type definitions
//----------------------------------------------------------------
typedef struct
{
	DATA	re;
	DATA	im;

} COMPLEX, *PCOMPLEX;


typedef struct
{
	COMPLEX	cmplx;
	DATA	data;
	LONG	long_int;

} COMBOTYPE, *PCOMBOTYPE;


//================================================================
//  Macros definitions
//----------------------------------------------------------------
#define cmplx_make( _x, _re, _im )		\
{										\
	(_x).re = _re;						\
	(_x).im = _im;						\
}

#define cmplx_make_real( _x, _re )		cmplx_make( _x, _re, 0 )
#define cmplx_zero( _x )				cmplx_make( _x, 0, 0 )

#define cmplx_abs2( _x )				( (_x).re * (_x).re + (_x).im * (_x).im )
#define cmplx_abs( _x )					sqrt( cmplx_abs2( _x, _y ) )


#define cmplx_conj( _r, _x )			\
{										\
	cmplx_make							\
		(								\
			_r,							\
			(_x).re,					\
			-(_x).im					\
		);								\
}


#define cmplx_add( _r, _x, _y )			\
{										\
	cmplx_make							\
		(								\
			_r,							\
			(_x).re + (_y).re,			\
			(_x).im + (_y).im			\
		);								\
}


#define cmplx_add_conj( _r, _x, _y )	\
{										\
	cmplx_make							\
		(								\
			_r,							\
			(_x).re - (_y).im,			\
			(_x).im + (_y).re			\
		);								\
}


#define cmplx_sub( _r, _x, _y )			\
{										\
	cmplx_make							\
		(								\
			_r,							\
			(_x).re - (_y).re,			\
			(_x).im - (_y).im			\
		);								\
}


#define cmplx_sub_conj( _r, _x, _y )	\
{										\
	cmplx_make							\
		(								\
			_r,							\
			(_x).re + (_y).im,			\
			(_x).im - (_y).re			\
		);								\
}


#define cmplx_mul( _r, _x, _y )			\
{										\
	DATA __re =							\
		(_x).re * (_y).re				\
		-								\
		(_x).im * (_y).im;				\
	DATA __im =							\
		(_x).re * (_y).im				\
		+								\
		(_x).im * (_y).re;				\
										\
	cmplx_make( _r, __re, __im );		\
}


#define cmplx_mul_conj( _r, _x, _y )	\
{										\
	DATA __re =							\
		(_x).re * (_y).re				\
		+								\
		(_x).im * (_y).im;				\
	DATA __im =							\
		(_x).im * (_y).re				\
		-								\
		(_x).re * (_y).im;				\
										\
	cmplx_make( _r, __re, __im );		\
}


#define cmplx_mul_real( _r, _x, _rl )	\
{										\
	cmplx_make							\
		(								\
			_r,							\
			(_x).re * (_rl),			\
			(_x).im * (_rl)				\
		);								\
}


#define cmplx_min( _r, _x, _y )			\
{										\
	cmplx_make							\
		(								\
			_r,							\
			min( (_x).re, (_y).re ),	\
			min( (_x).im, (_y).im )		\
		);								\
}


#define cmplx_min_real( _r, _x, _rl )	\
{										\
	cmplx_make							\
		(								\
			_r,							\
			min( (_x).re, _rl ),		\
			(_x).im						\
		);								\
}


#define cmplx_min_imag( _r, _x, _il )	\
{										\
	cmplx_make							\
		(								\
			_r,							\
			(_x).re,					\
			min( (_x).im, _il )			\
		);								\
}


#define cmplx_max( _r, _x, _y )			\
{										\
	cmplx_make							\
		(								\
			_r,							\
			max( (_x).re, (_y).re ),	\
			max( (_x).im, (_y).im )		\
		);								\
}


#define cmplx_max_real( _r, _x, _rl )	\
{										\
	cmplx_make							\
		(								\
			_r,							\
			max( (_x).re, _rl ),		\
			(_x).im						\
		);								\
}


#define cmplx_max_imag( _r, _x, _il )	\
{										\
	cmplx_make							\
		(								\
			_r,							\
			(_x).re,					\
			max( (_x).im, _il )			\
		);								\
}

#endif // _COMPLEX_H_
