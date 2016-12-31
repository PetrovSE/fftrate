//================================================================
//  Includes
//----------------------------------------------------------------
#include <string.h>

#include "../swin.h"
#include "../types.h"
#include "../array.h"
#include "../str.h"
#include "../mathex.h"
#include "../mmsys.h"


//================================================================
//  ROM
//----------------------------------------------------------------
STATIC CONST CHANNELDESC g_ChannelsName[] =
{
	{ "Front Left",				"FL",	SPEAKER_FRONT_LEFT,				SPDIR_LEFT,		SPPOS_FRONT		},
	{ "Front Right",			"FR",	SPEAKER_FRONT_RIGHT,			SPDIR_RIGHT,	SPPOS_FRONT		},
	{ "Front Center",			"FC",	SPEAKER_FRONT_CENTER,			SPDIR_CENTER,	SPPOS_FRONT		},

	{ "Low Frequency",			"LFE",	SPEAKER_LOW_FREQUENCY,			SPDIR_LFE,		SPPOS_LFE		},
	
	{ "Back Left",				"BL",	SPEAKER_BACK_LEFT,				SPDIR_LEFT,		SPPOS_BACK		},
	{ "Back Right",				"BR",	SPEAKER_BACK_RIGHT,				SPDIR_RIGHT,	SPPOS_BACK		},
	
	{ "Front Left of Center",	"RL",	SPEAKER_FRONT_LEFT_OF_CENTER,	SPDIR_LEFT,		SPPOS_FRONT_CENTER		},
	{ "Front Right of Center",	"RR",	SPEAKER_FRONT_RIGHT_OF_CENTER,	SPDIR_RIGHT,	SPPOS_FRONT_CENTER		},
	
	{ "Back Center",			"BC",	SPEAKER_BACK_CENTER,			SPDIR_CENTER,	SPPOS_BACK		},
	
	{ "Side Left",				"SL",	SPEAKER_SIDE_LEFT,				SPDIR_LEFT,		SPPOS_SIDE		},
	{ "Side Right",				"SR",	SPEAKER_SIDE_RIGHT,				SPDIR_RIGHT,	SPPOS_SIDE		},
	
	{ "Top Center",				"TC",	SPEAKER_TOP_CENTER,				SPDIR_CENTER,	SPPOS_TOP_CENTER	},
	
	{ "Top Front Left",			"TFL",	SPEAKER_TOP_FRONT_LEFT,			SPDIR_LEFT,		SPPOS_TOP_FRONT		},
	{ "Top Front Center",		"TFC",	SPEAKER_TOP_FRONT_CENTER,		SPDIR_CENTER,	SPPOS_TOP_FRONT		},
	{ "Top Front Right",		"TFR",	SPEAKER_TOP_FRONT_RIGHT,		SPDIR_RIGHT,	SPPOS_TOP_FRONT		},
	
	{ "Top Back Left",			"TBL",	SPEAKER_TOP_BACK_LEFT,			SPDIR_LEFT,		SPPOS_TOP_BACK		},
	{ "Top Back Center",		"TBC",	SPEAKER_TOP_BACK_CENTER,		SPDIR_CENTER,	SPPOS_TOP_BACK		},
	{ "Top Back Right",			"TBR",	SPEAKER_TOP_BACK_RIGHT,			SPDIR_RIGHT,	SPPOS_TOP_BACK		},
};

STATIC CONST INT g_NofChannelsName = numelems( g_ChannelsName );


//================================================================
//  Channel order make & free
//----------------------------------------------------------------
PCHANNELORDER mmsys_channel_order_make( CONST CHAR *str_desc )
{
	PCHANNELORDER p_order = NULL;

	loopinf
	{
		if( invalid_ptr( str_desc ) )
		{
			break;
		}

		//----------------------------------------------------------------

		arralloc_unit( p_order );
		if( !arrcheck( p_order ) )
		{
			break;
		}

		//----------------------------------------------------------------

		for( p_order->nLen = 0 ; *str_desc ; str_desc ++ )
		{
			INT cnt;

			for( cnt = 0 ; cnt < g_NofChannelsName ; cnt ++ )
			{
				CONST CHANNELDESC *p_info = &g_ChannelsName[cnt];

				CHAR test[MAX_NAME];
				INT  len = (INT)strlen( p_info->szShortName );

				if( (INT)strlen( str_desc ) < len )
				{
					continue;
				}

				strncpyt( test, str_desc, min( len + 1, MAX_NAME ) );

				//----------------------------------------------------------------

				if( strcasecmp( test, p_info->szShortName ) == 0 )
				{
					if( p_order->nLen < N_OF_MAX_CHANNELS )
					{
						p_order->pMasks[p_order->nLen ++] = p_info->dwMask;
						str_desc += len - 1;
						break;
					}

					p_order = mmsys_channel_order_free( p_order );
					break;
				}
			}
		}

		break;
	}

	return p_order;
}


PCHANNELORDER mmsys_channel_order_free( PCHANNELORDER p_order )
{
	arrfree( p_order );
	return NULL;
}


//================================================================
//  Channel description functions
//----------------------------------------------------------------
STATIC CONST CHANNELDESC *mmsys_local_order_desc( CONST CHANNELORDER *p_order, INT pos )
{
	CONST CHANNELDESC *p_info = NULL;

	if( invalid_ptr( p_order ) )
	{
		if( pos < g_NofChannelsName )
		{
			p_info = &g_ChannelsName[pos];
		}
	}
	else
	{
		if( pos < p_order->nLen )
		{
			INT cnt;

			for( cnt = 0 ; cnt < g_NofChannelsName ; cnt ++ )
			{
				if( g_ChannelsName[cnt].dwMask == p_order->pMasks[pos] )
				{
					p_info = &g_ChannelsName[cnt];
					break;
				}
			}
		}
	}

	return p_info;
}


STATIC DWORD mmsys_local_get_mask( DWORD mask, CONST CHANNELORDER *p_order, INT pos )
{
	CONST CHANNELDESC *p_info = mmsys_channel_desc( mask, p_order, pos );
	return invalid_ptr( p_info ) ? 0x0 : p_info->dwMask;
}


BOOL mmsys_channel_order_is_same( CONST CHANNELORDER *p_order1, CONST CHANNELORDER *p_order2, DWORD mask )
{
	INT cnt;

	for( cnt = 0 ; ; cnt ++ )
	{
		DWORD mask1 = mmsys_local_get_mask( mask, p_order1, cnt );
		DWORD mask2 = mmsys_local_get_mask( mask, p_order2, cnt );

		if( mask1 != mask2 )
		{
			return FALSE;
		}

		if( is_zero( mask1 ) )
		{
			break;
		}
	}

	return TRUE;
}


DWORD mmsys_channel_order_mask( CONST CHANNELORDER *p_order )
{
	DWORD mask = 0x0;
	INT cnt;

	for( cnt = 0 ; ; cnt ++ )
	{
		CONST CHANNELDESC *p_info = mmsys_local_order_desc( p_order, cnt );

		if( invalid_ptr( p_info ) )
		{
			break;
		}

		mask |= p_info->dwMask;
	}

	return mask;
}


CONST CHANNELDESC *mmsys_channel_desc( DWORD mask, CONST CHANNELORDER *p_order, INT chann_num )
{
	INT cnt;

	//----------------------------------------------------------------

	for( cnt = 0 ; ; cnt ++ )
	{
		CONST CHANNELDESC *p_info = mmsys_local_order_desc( p_order, cnt );

		if( invalid_ptr( p_info ) )
		{
			break;
		}

		//----------------------------------------------------------------

		if( flag_check_mask( mask, p_info->dwMask ) )
		{
			if( chann_num == 0 )
			{
				return p_info;
			}

			chann_num --;
		}
	}

	//----------------------------------------------------------------

	return NULL;
}


//================================================================
//  Common functions
//----------------------------------------------------------------
FOURCC mmsys_str_to_fcc( CONST CHAR *str )
{
	if( invalid_ptr( str ) || strlen( str ) < 4 )
	{
		return FCC_NULL;
	}
	
	return MAKEFOURCC( str[0], str[1], str[2], str[3] );
}


DWORD mmsys_make_chmask( INT chann )
{
	switch( chann )
	{
	case 1:
		// Mono
		return SPEAKER_FRONT_CENTER;
		
	case 2:
		// Stereo
		return SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
		
	case 3:
		// 2.1
		return SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_LOW_FREQUENCY;
		
	case 4:
		// 4.0
		return 
			SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT
			|
			SPEAKER_BACK_LEFT  | SPEAKER_BACK_RIGHT;
		
	case 5:
		// 5.0
		return 
			SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER
			|
			SPEAKER_BACK_LEFT  | SPEAKER_BACK_RIGHT;
		
	case 6:
		// 5.1
		return 
			SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER
			|
			SPEAKER_BACK_LEFT  | SPEAKER_BACK_RIGHT
			|
			SPEAKER_LOW_FREQUENCY;
		
	case 7:
		// 7.0
		return 
			SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER
			|
			SPEAKER_BACK_LEFT  | SPEAKER_BACK_RIGHT
			|
			SPEAKER_FRONT_LEFT_OF_CENTER  | SPEAKER_FRONT_RIGHT_OF_CENTER;
		
	case 8:
		// 7.1
		return 
			SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER
			|
			SPEAKER_BACK_LEFT  | SPEAKER_BACK_RIGHT
			|
			SPEAKER_FRONT_LEFT_OF_CENTER  | SPEAKER_FRONT_RIGHT_OF_CENTER
			|
			SPEAKER_LOW_FREQUENCY;
	}

	// Unknown channel map
	return one_fpp( chann ) - 1;
}


//================================================================
//  Wave format functions
//----------------------------------------------------------------
BOOL mmsys_format_is_eq( CONST WAVEFORMATEX *p_format1, CONST WAVEFORMATEX *p_format2, CONST CHAR *mask )
{
	if( arrcheck( p_format1 ) && arrcheck( p_format2 ) && !invalid_ptr( mask ) )
	{
		for( ; *mask ; )
		{
			CHAR type = *mask ++;

			if
			(
				mmsys_format_info( p_format1, type )
				!=
				mmsys_format_info( p_format2, type )
			)
			{
				return FALSE;
			}
		}

		return TRUE;
	}

	return FALSE;
}


DWORD mmsys_format_info( CONST WAVEFORMATEX *p_format, CHAR type )
{
	PWAVEFORMATEXTENSIBLE p_format_ex = (PWAVEFORMATEXTENSIBLE)p_format;
	BOOL b_extensible = FALSE;

	//----------------------------------------------------------------

	if( !arrcheck( p_format_ex ) )
	{
		return 0x0;
	}

	//----------------------------------------------------------------

	b_extensible = ( p_format->wFormatTag == WAVE_FORMAT_EXTENSIBLE );

	switch( type )
	{
	case 't':
		if( b_extensible )
		{
			return EXTRACT_WAVEFORMATEX_ID( p_format_ex->SubFormat );
		}

	case 'T':
		return p_format->wFormatTag;


	case 'r':
		return p_format->nSamplesPerSec;


	case 'b':
		if( b_extensible && p_format_ex->Samples.wValidBitsPerSample )
		{
			return p_format_ex->Samples.wValidBitsPerSample;
		}

	case 'B':
		return p_format->wBitsPerSample;


	case 'c':
		return p_format->nChannels;

	case 'm':
		return b_extensible ? p_format_ex->dwChannelMask : mmsys_make_chmask( p_format_ex->Format.nChannels );

	case 's':
		if( b_extensible || !mmsys_format_is_pcm( p_format ) )
		{
			return p_format->cbSize + sizeof(WAVEFORMATEX);
		}

		return sizeof(WAVEFORMAT) + sizeof(WORD);

	case 'S':
		return p_format->cbSize + sizeof(WAVEFORMATEX);
	}

	return 0x0;
}


DWORD mmsys_format_unit_type( CONST WAVEFORMATEX *p_format )
{
	WORD bits = (WORD)mmsys_format_info( p_format, 'b' );

	switch( mmsys_format_info( p_format, 't' ) )
	{
	case WAVE_FORMAT_PCM:
		
		if( bits == 8 )
		{
			return UNIT_TYPE_FIXED_8;
		}
		
		if( bits == 16 )
		{
			return UNIT_TYPE_FIXED_16;
		}
		
		if( bits < 16 )
		{
			break;
		}
		
		if( bits <= 24 )
		{
			return UNIT_TYPE_FIXED_24;
		}
		
		if( bits <= 32 )
		{
			return UNIT_TYPE_FIXED_32;
		}
		
		break;
		
		//----------------------------------------------------------------
		
	case WAVE_FORMAT_IEEE_FLOAT:
		
		if( bits == 32 )
		{
			return UNIT_TYPE_FLOAT;
		}
		
		if( bits == 64 )
		{
			return UNIT_TYPE_DOUBLE;
		}
		
		break;
	}
	
	//----------------------------------------------------------------
	
	return UNIT_TYPE_NONE;
}


PWAVEFORMATEX mmsys_format_alloc( DWORD size )
{
	PWAVEFORMATEX p_format = NULL;

	//----------------------------------------------------------------

	if( size == INVALID_SIZE )
	{
		return NULL;
	}

	//----------------------------------------------------------------

	size = max( size, sizeof(WAVEFORMATEX) );

	arralloc_bytes( p_format, size );
	if( !arrcheck( p_format ) )
	{
		return NULL;
	}

	//----------------------------------------------------------------

	arrzero_bytes( p_format, size );

	p_format->wBitsPerSample	= 16;
	p_format->cbSize			= (WORD)( size - sizeof(WAVEFORMATEX) );

	return p_format;
}


PWAVEFORMATEX mmsys_format_free( PWAVEFORMATEX p_format )
{
	arrfree( p_format );
	return NULL;
}


PWAVEFORMATEX mmsys_format_dublicate( CONST WAVEFORMATEX *p_format )
{
	PWAVEFORMATEX p_dup = NULL;

	while( !invalid_ptr( p_format ) )
	{
		DWORD size = mmsys_format_info( p_format, 'S' );

		if( is_zero( size ) )
		{
			break;
		}

		//----------------------------------------------------------------

		arralloc_bytes( p_dup, size );
			
		if( !arrcheck( p_dup ) )
		{
			break;
		}
			
		//----------------------------------------------------------------

		arrcpy_bytes( p_dup, p_format, size );
		mmsys_format_complete( p_dup );

		break;
	}

	return p_dup;
}


VOID mmsys_format_complete( WAVEFORMATEX *p_format )
{
	DWORD unit_type = mmsys_format_unit_type( p_format );
	
	//----------------------------------------------------------------
	
	switch( get_unit_fmt( unit_type ) )
	{
	case SAMPLE_FMT_PCM:
		p_format->nBlockAlign		= (WORD)( get_unit_size( unit_type ) * p_format->nChannels );
		p_format->nAvgBytesPerSec	= (DWORD)p_format->nBlockAlign * p_format->nSamplesPerSec;
		break;
	}
}


PWAVEFORMATEX mmsys_format_pcm_change( PWAVEFORMATEX p_format, CONST CHAR *types, ... )
{
	while( arrcheck( p_format ) && !invalid_ptr( types ) )
	{
		PWAVEFORMATEX p_result;

		WORD  format_tag	= (WORD)mmsys_format_info( p_format, 't' );
		WORD  n_chann		= (WORD)mmsys_format_info( p_format, 'c' );
		DWORD sampling		= mmsys_format_info( p_format, 'r' );
		WORD  bits			= (WORD)mmsys_format_info( p_format, 'b' );
		DWORD chann_mask	= mmsys_format_info( p_format, 'm' );

		WORD  n_chann_old		= n_chann;
		DWORD chann_mask_old	= chann_mask;

		//----------------------------------------------------------------

		parse_arg_start( types, "wt wc dr wb dm" )
			&format_tag,
			&n_chann,
			&sampling,
			&bits,
			&chann_mask
		parse_arg_stop
		
		//----------------------------------------------------------------

		{
			BOOL new_chann	= ( n_chann_old != n_chann );
			BOOL new_mask	= ( chann_mask_old != chann_mask );

			if( new_chann ^ new_mask )
			{
				if( new_chann )
				{
					chann_mask = 0x0;
				}

				if( new_mask )
				{
					n_chann = 0;
				}
			}
		}

		//----------------------------------------------------------------

		p_result = mmsys_format_pcm_fill
			(
				format_tag,
				n_chann,
				sampling,
				bits,
				chann_mask
			);

		mmsys_format_free( p_format );
		return p_result;
	}

	return NULL;
}


PWAVEFORMATEX mmsys_format_pcm_fill
(
	WORD	format_tag,
	WORD	channels,
	DWORD	samples_per_sec,
	WORD	bits_per_sample,
	DWORD	channel_mask
)
{
	PWAVEFORMATEX p_format = NULL;

	//----------------------------------------------------------------

	loopinf
	{
		BOOL b_extensible;
		DWORD size;

		//----------------------------------------------------------------

		if
		(
			( is_zero( channels ) && is_zero( channel_mask ) )
			||
			is_zero( samples_per_sec )
			||
			is_zero( bits_per_sample )
		)
		{
			break;
		}

		//----------------------------------------------------------------

		if( is_zero( channels ) )
		{
			channels = binweight( channel_mask );
		}
		
		if
		(
			channel_mask == mmsys_make_chmask( channels )
			||
			channels != binweight( channel_mask )
		)
		{
			channel_mask = 0x0;
		}

		//----------------------------------------------------------------

		b_extensible =
			( channels > 2 )
			||
			( bits_per_sample % 8 )
			||
			non_zero( channel_mask )
			||
			( format_tag == WAVE_FORMAT_EXTENSIBLE );

		if( b_extensible )
		{
			size = sizeof(WAVEFORMATEXTENSIBLE);
		}
		else
		{
			size = sizeof(WAVEFORMATEX);
		}

		//----------------------------------------------------------------

		p_format = mmsys_format_alloc( size );

		if( !arrcheck( p_format ) )
		{
			break;
		}

		//----------------------------------------------------------------

		p_format->wFormatTag		= b_extensible ? WAVE_FORMAT_EXTENSIBLE : format_tag;
		p_format->nChannels			= channels;
		p_format->nSamplesPerSec	= samples_per_sec;
		p_format->wBitsPerSample	= alignment( bits_per_sample, 8 );

		if( b_extensible )
		{
			PWAVEFORMATEXTENSIBLE p_format_ex = (PWAVEFORMATEXTENSIBLE)p_format;
			INIT_WAVEFORMATEX_GUID( p_format_ex->SubFormat, format_tag );
			
			p_format_ex->Samples.wValidBitsPerSample	= bits_per_sample;
			p_format_ex->dwChannelMask					= is_zero( channel_mask ) ? mmsys_make_chmask( channels ) : channel_mask;
		}

		//----------------------------------------------------------------

		if( !mmsys_format_is_pcm( p_format ) )
		{
			break;
		}

		//----------------------------------------------------------------

		mmsys_format_complete( p_format );
		return p_format;
	}

	//----------------------------------------------------------------

	return mmsys_format_free( p_format );
}
