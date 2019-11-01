#ifndef _FFT_H_
#define _FFT_H_


//================================================================
//  Includes
//----------------------------------------------------------------
#include "interface.h"
#include "complex.h"


//================================================================
//  Type definitions
//----------------------------------------------------------------
DECL_HANDLE( HFFT );


//================================================================
//  Function prototypes
//----------------------------------------------------------------
CDECL_BEGIN

LIBINFO_PROTOTYPE( fft );

VOID fft_2( HFFT p_inst, PCOMPLEX p_x, INT step );
VOID fft_3( HFFT p_inst, PCOMPLEX p_x, INT step );
VOID fft_4( HFFT p_inst, PCOMPLEX p_x, INT step );
VOID fft_5( HFFT p_inst, PCOMPLEX p_x, INT step );
VOID fft_7( HFFT p_inst, PCOMPLEX p_x, INT step );
VOID fft_8( HFFT p_inst, PCOMPLEX p_x, INT step );
VOID fft_9( HFFT p_inst, PCOMPLEX p_x, INT step );

HFFT fft_radix2_init( INT len );
HFFT fft_radix2_deinit( HFFT p_inst );
VOID fft_radix2( HFFT p_inst, PCOMPLEX p_x, INT step );

HFFT fft_splitradix_init( INT len );
HFFT fft_splitradix_deinit( HFFT p_inst );
VOID fft_splitradix( HFFT p_inst, PCOMPLEX p_x, INT step );

HFFT dft_init( INT n, INT m, BOOL b_rotate );
HFFT dft_deinit( HFFT p_inst );
VOID dft( HFFT p_inst, PCOMPLEX p_x, INT step );

HFFT fft_init( INT len );
HFFT fft_deinit( HFFT p_inst );
VOID fft( HFFT p_inst, PCOMPLEX p_x, INT step );

VOID ifft_preproc( PCOMPLEX p_x, INT len, INT step );
VOID ifft_postproc( PCOMPLEX p_x, INT len, INT step );

CDECL_END

#endif // _FFT_H_
