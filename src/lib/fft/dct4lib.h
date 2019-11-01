#ifndef _DCT4_LIB_H_
#define _DCT4_LIB_H_


//================================================================
//  Struct definitions
//----------------------------------------------------------------
typedef struct
{
	INT			len;
	INT			len2;

	HFFT		h_fft;

	PCOMPLEX	pK1;
	PCOMPLEX	pK2;
	PCOMPLEX	p_temp;

} DCT4_INST, *PDCT4_INST;

#endif // _DCT4_LIB_H_
