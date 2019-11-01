#ifndef _CONVERT_H_
#define _CONVERT_H_


//================================================================
//  Includes
//----------------------------------------------------------------
#include "interface.h"
#include "mathex.h"
#include "mmsys.h"


//================================================================
//  Definitions
//----------------------------------------------------------------
#define CONV_STAT_ALL				0xffff
#define CONV_STAT_GETMATRIX			0x0001
#define CONV_STAT_GETPEAKS			0x0002
#define CONV_STAT_GETINFO			0x0004

enum
{
	CONV_WINDOW_NONE	= 0,
	CONV_WINDOW_SIN,
	CONV_WINDOW_VORBIS
};

enum
{
	CONV_TRANSFORM_NONE	= 0,
	CONV_TRANSFORM_FFT,
	CONV_TRANSFORM_DCT
};

#define CHANN_IS_USED( _v )			more_zero( _v )


//================================================================
//  Convert parameters
//  Use for:
//		convert_open
//----------------------------------------------------------------
//  t   - (DWORD)transform type (see CONV_TRANSFORM_, default: CONV_TRANSFORM_DCT)
//  w   - (DWORD)window type (see CONV_WINDOW_, default: CONV_WINDOW_VORBIS)
//
//  s   - (BOOL) sync (default: TRUE)
//  n   - (BOOL) normalize channel matrix  (default: TRUE)
//
//  i   - (CONST CHAR *) input channel order  (see SPORDER_ in "mmsys.h")
//  o   - (CONST CHAR *) output channel order (see SPORDER_ in "mmsys.h")
//----------------------------------------------------------------


//================================================================
//  Type definitions
//----------------------------------------------------------------
DECL_HANDLE( HCONVERT );

typedef struct
{
	DWORD			dwChannMask;
	PCHANNELORDER	pOrder;
	
	DATA			pValues[N_OF_MAX_CHANNELS];

} SAMPLEPEAKS, *PSAMPLEPEAKS;


typedef struct
{
	DWORD			dwChannMaskIn;
	DWORD			dwChannMaskOut;

	PCHANNELORDER	pOrderIn;
	PCHANNELORDER	pOrderOut;

	BOOL			bEmptyMatrix;

	// mixer: [in chann][out chann]
	DATA			ppMixer[N_OF_MAX_CHANNELS][N_OF_MAX_CHANNELS];

} CHANNELMIXMATRIX, *PCHANNELMIXMATRIX;


typedef struct
{
	CONST CHAR *szTransformName;
	CONST CHAR *szWindowName;

	INT		nWindowLenIn;
	INT		nWindowLenOut;

	BOOL	bSync;
	INT		nDelay;

} CONVNFO, *PCONVNFO;


typedef struct
{
	CHANNELMIXMATRIX	TransformMatrix;

	SAMPLEPEAKS			InputPeaks;
	SAMPLEPEAKS			OutputPeaks;

	CONVNFO				Info;
	DWORD				dwModeDone;

} CONVSTAT, *PCONVSTAT;


//================================================================
//  Function prototypes
//----------------------------------------------------------------
CDECL_BEGIN

LIBINFO_PROTOTYPE( convert );

HCONVERT convert_open
(
	CONST WAVEFORMATEX	*p_src_format,
	CONST WAVEFORMATEX	*p_dst_format,
	CONST CHAR			*types,
	...
);
HCONVERT convert_close( HCONVERT p_inst );

BOOL convert_reset( HCONVERT p_inst );
BOOL convert_processing
(
	HCONVERT	p_inst,
	CONST BYTE	*p_src,
	PDWORD		p_src_size,
	PBYTE		p_dst,
	PDWORD		p_dst_size
);

BOOL convert_stat_mode( HCONVERT p_inst, DWORD mode, BOOL b_set );
BOOL convert_get_stat( HCONVERT p_inst, PCONVSTAT p_stat );

CDECL_END

#endif // _CONVERT_H_
