#ifndef _MDCT_LIB_H_
#define _MDCT_LIB_H_


//================================================================
//  Struct definitions
//----------------------------------------------------------------
typedef struct
{
	INT			len;
	INT			len2;
	INT			len4;

	HDCT4		h_dct4;
	DATA		*p_temp;

} MDCT_INST, *PMDCT_INST;

#endif // _MDCT_LIB_H_
