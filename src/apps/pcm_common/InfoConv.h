#ifndef _PCM_INFOCONV_H_
#define _PCM_INFOCONV_H_

//=============================================================================
// Definition
//-----------------------------------------------------------------------------
#define SZ_FFT				"fft"
#define SZ_DCT				"dct"

#define SZ_HANNING			"hanning"
#define SZ_VORBIS			"vorbis"
#define SZ_RECT				"rect"


//================================================================
//  Function prototypes
//----------------------------------------------------------------
VOID PrintConvMatrix( HCONVERT hConv );
VOID PrintConvInfo( HCONVERT hConv );

#endif // _PCM_INFOCONV_H_
