#ifndef _MMSYS_H_
#define _MMSYS_H_


//================================================================
//  Includes
//----------------------------------------------------------------
#include "interface.h"
#include "types.h"
#include "mathmac.h"


//================================================================
//  Definitions
//----------------------------------------------------------------
#ifndef WAVE_FORMAT_PCM
#define WAVE_FORMAT_PCM					0x0001
#endif

#ifndef WAVE_FORMAT_IEEE_FLOAT
#define WAVE_FORMAT_IEEE_FLOAT			0x0003
#endif

#ifndef WAVE_FORMAT_EXTENSIBLE
#define WAVE_FORMAT_EXTENSIBLE			0xfffe
#endif


#define N_OF_MAX_CHANNELS				18


#define get_unit_type( _x )				get_hi_word( _x )
#define get_unit_size( _x )				get_low_word( _x )
#define get_unit_fmt( _x )				get_hi_byte( get_unit_type( _x ) )


#define SAMPLE_FMT_NONE					0x00
#define SAMPLE_FMT_PCM					0x01


#define SAMPLE_TYPE_NONE				make_word( SAMPLE_FMT_NONE,	0x00 )

#define SAMPLE_TYPE_INTERNAL			make_word( SAMPLE_FMT_PCM,	0x01 )
#define SAMPLE_TYPE_FIXED				make_word( SAMPLE_FMT_PCM,	0x02 )
#define SAMPLE_TYPE_FLOAT				make_word( SAMPLE_FMT_PCM,	0x03 )


#define UNIT_TYPE_NONE					make_dword( SAMPLE_TYPE_NONE, 0x0 )
#define UNIT_TYPE_INTERNAL				make_dword( SAMPLE_TYPE_INTERNAL, sizeof(DATA) )

#define UNIT_TYPE_FIXED_8				make_dword( SAMPLE_TYPE_FIXED, sizeof(BYTE) )
#define UNIT_TYPE_FIXED_16				make_dword( SAMPLE_TYPE_FIXED, sizeof(SHORT) )
#define UNIT_TYPE_FIXED_24				make_dword( SAMPLE_TYPE_FIXED, sizeof(BYTE) * 3 )
#define UNIT_TYPE_FIXED_32				make_dword( SAMPLE_TYPE_FIXED, sizeof(LONG) )

#define UNIT_TYPE_FLOAT					make_dword( SAMPLE_TYPE_FLOAT, sizeof(FLOAT) )
#define UNIT_TYPE_DOUBLE				make_dword( SAMPLE_TYPE_FLOAT, sizeof(DOUBLE) )


#ifndef MAKEFOURCC
#define MAKEFOURCC( _a, _b, _c, _d )	make_dword4( _a, _b, _c, _d )
#endif


#ifndef FCC_NULL
#define FCC_NULL						MAKEFOURCC( 0x00, 0x00, 0x00, 0x00 )	// Zero fourcc
#endif


//================================================================
//  Wave format parameters (DWORD)
//  Use for:
//		mmsys_format_is_eq
//		mmsys_format_info
//		mmsys_format_pcm_change
//----------------------------------------------------------------
//	t	- format tag
//	T	- primary tag
//
//	r	- samples rate
//
//	b	- actual bits per sample
//	B	- total bits per sample
//
//	c	- number of channels
//	m	- channel mask
//
//	s	- actual size
//	S	- full size
//----------------------------------------------------------------


//================================================================
//  Type definitions
//----------------------------------------------------------------
typedef DWORD			FOURCC;


typedef struct
{
	CONST CHAR	*szLongName;
	CONST CHAR	*szShortName;
	
	DWORD		dwMask;
	DWORD		dwDirection;
	DWORD		dwPosition;
	
} CHANNELDESC, *PCHANNELDESC;


typedef struct
{
	DWORD	pMasks[N_OF_MAX_CHANNELS];
	INT		nLen;

} CHANNELORDER, *PCHANNELORDER;


//================================================================
//  Wave format definitions
//----------------------------------------------------------------
#pragma pack( push, 2 )

#ifndef _WAVEFORMATEX_
#define _WAVEFORMATEX_

#define WAVEFORMATE_FIELDS			\
	WORD	wFormatTag;				\
	WORD	nChannels;				\
	DWORD	nSamplesPerSec;			\
	DWORD	nAvgBytesPerSec;		\
	WORD	nBlockAlign;


typedef struct
{
	WAVEFORMATE_FIELDS

} WAVEFORMAT, *PWAVEFORMAT;


typedef struct
{
	WAVEFORMATE_FIELDS
	WORD	wBitsPerSample;
	WORD	cbSize;

} WAVEFORMATEX, *PWAVEFORMATEX;

#endif // _WAVEFORMATEX_


#ifndef _WAVEFORMATEXTENSIBLE_
#define _WAVEFORMATEXTENSIBLE_

typedef struct
{
	WAVEFORMATEX	Format;
	
	union
	{
		WORD wValidBitsPerSample;
		WORD wSamplesPerBlock;
		WORD wReserved;

	} Samples;

	DWORD	dwChannelMask;
	GUID	SubFormat;

} WAVEFORMATEXTENSIBLE, *PWAVEFORMATEXTENSIBLE;

#endif // _WAVEFORMATEXTENSIBLE_

#pragma pack( pop )


//================================================================
//  Speaker position definitions
//----------------------------------------------------------------
#ifndef _SPEAKER_POSITIONS_
#define _SPEAKER_POSITIONS_

#define SPEAKER_FRONT_LEFT				0x00001
#define SPEAKER_FRONT_RIGHT				0x00002
#define SPEAKER_FRONT_CENTER			0x00004
#define SPEAKER_LOW_FREQUENCY			0x00008
#define SPEAKER_BACK_LEFT				0x00010
#define SPEAKER_BACK_RIGHT				0x00020
#define SPEAKER_FRONT_LEFT_OF_CENTER	0x00040
#define SPEAKER_FRONT_RIGHT_OF_CENTER	0x00080
#define SPEAKER_BACK_CENTER				0x00100
#define SPEAKER_SIDE_LEFT				0x00200
#define SPEAKER_SIDE_RIGHT				0x00400
#define SPEAKER_TOP_CENTER				0x00800
#define SPEAKER_TOP_FRONT_LEFT			0x01000
#define SPEAKER_TOP_FRONT_CENTER		0x02000
#define SPEAKER_TOP_FRONT_RIGHT			0x04000
#define SPEAKER_TOP_BACK_LEFT			0x08000
#define SPEAKER_TOP_BACK_CENTER			0x10000
#define SPEAKER_TOP_BACK_RIGHT			0x20000

// Used to specify that any possible permutation of speaker configurations
#define SPEAKER_ALL						0x80000000

#endif // _SPEAKER_POSITIONS_


//================================================================
//  Speaker order
//----------------------------------------------------------------
#define SPORDER_DEFAULT				NULL
#define SPORDER_MS					"FL,FR,FC,LFE,BL,BR,RL,RR,BC,SL,SR,TC,TFL,TFC,TFR,TBL,TBC,TBR"
#define SPORDER_ALSA				"FL,FR,BL,BR,FC,LFE,RL,RR"
#define SPORDER_OSS					"FL,FR,FC,LFE,BL,BR,RL,RR"


//================================================================
//  Speaker direction
//----------------------------------------------------------------
#define SPDIR_LEFT					0x01
#define SPDIR_RIGHT					0x02
#define SPDIR_CENTER				0x04
#define SPDIR_LFE					0x08


//================================================================
//  Speaker positions
//----------------------------------------------------------------
#define SPPOS_FRONT					0x0001
#define SPPOS_LFE					0x0002
#define SPPOS_BACK					0x0004
#define SPPOS_FRONT_CENTER			0x0008
#define SPPOS_SIDE					0x0010
#define SPPOS_TOP_CENTER			0x0020
#define SPPOS_TOP_FRONT				0x0040
#define SPPOS_TOP_BACK				0x0080


//================================================================
//  Extented wave format definitions
//----------------------------------------------------------------
#if !defined(EXTRACT_WAVEFORMATEX_ID)
#define EXTRACT_WAVEFORMATEX_ID( _Guid )				(WORD)( (_Guid).Data1 )
#endif

#if !defined(INIT_WAVEFORMATEX_GUID)
#define INIT_WAVEFORMATEX_GUID( _Guid, _wFormatTag )	\
{														\
	(_Guid).Data1		= (WORD)(_wFormatTag);			\
	(_Guid).Data2		= 0x0000;						\
	(_Guid).Data3		= 0x0010;						\
	(_Guid).Data4[0]	= 0x80;							\
	(_Guid).Data4[1]	= 0x00;							\
	(_Guid).Data4[2]	= 0x00;							\
	(_Guid).Data4[3]	= 0xaa;							\
	(_Guid).Data4[4]	= 0x00;							\
	(_Guid).Data4[5]	= 0x38;							\
	(_Guid).Data4[6]	= 0x9b;							\
	(_Guid).Data4[7]	= 0x71;							\
}
#endif


//================================================================
//  Function prototypes
//----------------------------------------------------------------
CDECL_BEGIN

PCHANNELORDER mmsys_channel_order_make( CONST CHAR *str_desc );
PCHANNELORDER mmsys_channel_order_free( PCHANNELORDER p_order );

BOOL  mmsys_channel_order_is_same( CONST CHANNELORDER *p_order1, CONST CHANNELORDER *p_order2, DWORD mask );
DWORD mmsys_channel_order_mask( CONST CHANNELORDER *p_order );
CONST CHANNELDESC *mmsys_channel_desc( DWORD mask, CONST CHANNELORDER *p_order, INT chann_num );

FOURCC mmsys_str_to_fcc( CONST CHAR *str );
DWORD  mmsys_make_chmask( INT chann );

#define mmsys_format_is_pcm( _f )		( get_unit_fmt( mmsys_format_unit_type( _f ) ) == SAMPLE_FMT_PCM )

BOOL  mmsys_format_is_eq( CONST WAVEFORMATEX *p_format1, CONST WAVEFORMATEX *p_format2, CONST CHAR *mask );

DWORD mmsys_format_info( CONST WAVEFORMATEX *p_format, CHAR type );
DWORD mmsys_format_unit_type( CONST WAVEFORMATEX *p_format );

PWAVEFORMATEX mmsys_format_alloc( DWORD size );
PWAVEFORMATEX mmsys_format_free( PWAVEFORMATEX p_format );
PWAVEFORMATEX mmsys_format_dublicate( CONST WAVEFORMATEX *p_format );
VOID mmsys_format_complete( WAVEFORMATEX *p_format );

PWAVEFORMATEX mmsys_format_pcm_change( PWAVEFORMATEX p_format, CONST CHAR *types, ... );
PWAVEFORMATEX mmsys_format_pcm_fill
(
	WORD	format_tag,
	WORD	channels,
	DWORD	samples_per_sec,
	WORD	bits_per_sample,
	DWORD	channel_mask
);

CDECL_END

#endif // _MMSYS_H_
