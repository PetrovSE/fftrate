#ifndef _FFTLIB_H_
#define _FFTLIB_H_


//================================================================
//  Includes
//----------------------------------------------------------------
#include "../fft.h"


//================================================================
//  Type definitions
//----------------------------------------------------------------
typedef VOID (*pfn_fft)( HFFT, PCOMPLEX, INT );
typedef HFFT (*pfn_fft_deinit)( HFFT );

typedef struct 
{
	INT		*p_index;
	INT		*p_itab;

	INT		len;
	INT		n_bit;

} BR_INST, *PBR_INST;


typedef struct
{
	INT				len;
	pfn_fft			fn_proc;
	pfn_fft_deinit	fn_deinit;
	HFFT			h_inst;

} FFT_INST, *PFFT_INST;


typedef struct
{
	INT			len;
	INT			cos_len;

	PBR_INST	h_index;
	PCOMPLEX	p_table;
	PCOMPLEX	p_temp;
	
} FFTRADIX2_INST, *PFFTRADIX2_INST;


typedef struct
{
	INT			len;

	PBR_INST	h_index;
	COMPLEX		*p_table;
	COMPLEX		*p_temp;

} FFTSPLITRADIX_INST, *PFFTSPLITRADIX_INST;


typedef struct
{
	INT			n, m;
	BOOL		b_rotate;

	PCOMPLEX	*p_table;
	PCOMPLEX	p_temp;
	
} DFT_INST, *PDFT_INST;


typedef struct
{
	INT			len;

	FFT_INST	fft_lines;
	FFT_INST	fft_rows;

	PDFT_INST	p_rotate_inst;
	PCOMPLEX	p_temp;

	INT			*p_index_in;
	INT			*p_index_out;
	BOOL		b_no_index;
	
} NODEFFT_INST, *PNODEFFT_INST;

#endif // _FFTLIB_H_
