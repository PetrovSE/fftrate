//=============================================================================
// Includes
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdarg.h>

#include "swin.h"
#include "types.h"
#include "array.h"
#include "mathex.h"
#include "str.h"

#include "conf.h"


//=============================================================================
// Extern
//-----------------------------------------------------------------------------
EXTERN CONST CHAR *ConvertNames[];


//=============================================================================
// Type Definitions
//-----------------------------------------------------------------------------
typedef struct
{
	CONST CHAR	*name;
	DWORD		type;

	union
	{
		CONST CHAR	*s_val;
		INT			i_val;
	};

} REPL, *PREPL;


//=============================================================================
// Definitions
//-----------------------------------------------------------------------------
#define DEF_PERIOD				1024
#define DEF_BUFFER				8192


#define FIRST_ID				1024


#define LINE_6					6000
#define LINE_8					8000
#define LINE_11					11025


#define MAX_REPL				64
#define _Q( _name )				"\"" _name "\""


#define T_NEXT					T_NEXT1

#define T_NEXT1					"$1NEXT"
#define T_NEXT2					"$2NEXT"

#define T_NAME					"$NAME"
#define T_MODULE				"$MODULE"
#define T_DESC					"$DESC"
#define T_SELFNAME				"$SELFNAME"
#define T_COMMENT				"$COMMENT"
#define T_SHOW					"$SHOW"
#define T_ID					"$ID"

#define T_RATE					"$RATE"
#define T_CHANN					"$CHANNELS"
#define T_FORMAT				"$FORMAT"
#define T_VALUE					"$VALUE"

#define T_PERIOD				"$PERIOD"
#define T_BUFFER				"$BUFFER"


#define R_TYPE_NONE				0x0000
#define R_TYPE_INT				0x0001
#define R_TYPE_STR				0x0002


#define P_DMIXER				"dmixer"
#define P_DSNOOPER				"dsnooper"

#define P_PLAY					"_play"
#define P_REC					"_rec"

#define P_CONVERT				"convert"
#define P_CONVERT_PLAY			P_CONVERT P_PLAY
#define P_CONVERT_REC			P_CONVERT P_REC

#define P_SOFTVOL				"softvol"
#define P_VOL_PLAY				P_SOFTVOL P_PLAY
#define P_VOL_REC				P_SOFTVOL P_REC

#define P_EXPAND				"expand"
#define P_DUPLEX				"duplex"

#define P_DEFAULT				"!default"
#define P_PRIMARY				"primary"
#define P_CAPTURE				"capture"

#define P_PHONON				"phonon"
#define P_NORM					"norm"

#define P_NORM_COMPRESS			"_compressor"
#define P_NORM_LIMIT			"_limiter"
#define P_NORM_FORMAT			"_formatter"

#define P_LADSPA_PATH			"/usr/lib/ladspa"


#define P_VOL_NAME_PLAY			"SoftPlay"
#define P_VOL_NAME_REC			"SoftRecord"

#define P_DESC_PLAY				" for playback"
#define P_DESC_REC				" for recording"

#define P_CONV_DESC				"Sample rate converter"
#define P_CONV_DESC_PLAY		P_CONV_DESC P_DESC_PLAY
#define P_CONV_DESC_REC			P_CONV_DESC P_DESC_REC

#define P_VOL_DESC				"Soft volume controller"
#define P_VOL_DESC_PLAY			P_VOL_DESC P_DESC_PLAY
#define P_VOL_DESC_REC			P_VOL_DESC P_DESC_REC

#define P_DEF_DESC				"Default device"
#define P_PLAY_DESC				"Primary device"
#define P_CAP_DESC				"Capture device"


//=============================================================================
// RAM
//-----------------------------------------------------------------------------
STATIC FILE	*pFile	= NULL;
STATIC INT	nID		= FIRST_ID;

STATIC CHAR szDefPlayer[MAX_STRING]		= "";
STATIC CHAR szDefCapture[MAX_STRING]	= "";


//=============================================================================
// Period calculation
//-----------------------------------------------------------------------------
STATIC INT typically_rate[] =
{
//	6000		12000			24000			48000			96000			192000
	LINE_6,		2 * LINE_6,		4 * LINE_6,		8 * LINE_6,		16 * LINE_6,	32 * LINE_6,
		
//	8000		16000			32000			64000
	LINE_8,		2 * LINE_8,		4 * LINE_8,		8 * LINE_8,
		
//	11025		22050			44100			88200
	LINE_11,	2 * LINE_11,	4 * LINE_11,	8 * LINE_11,
};

STATIC CONST INT n_rates = numelems( typically_rate );


//-----------------------------------------------------------------------------

STATIC INT calculate_ratio( INT rate, INT val, INT def )
{
	INT n = (INT)( ( D( def ) * D( rate ) ) / ( D( DEF_RATE ) * D( val ) ) + 0.5 );

	return val * max( n, 1 );
}


INT calculate_buffer( INT rate, INT period )
{
	return calculate_ratio( rate, period, DEF_BUFFER );
}


INT calculate_period( INT rate, BOOL buff_resize )
{
	INT gcd = rate;
	INT n;
	
	if( !buff_resize )
		return DEF_PERIOD;

	for( n = 0 ; n < n_rates ; n ++ )
		gcd = euclid( gcd, typically_rate[n] );
	
	return calculate_ratio( rate, rate / gcd, DEF_PERIOD );
}


INT correct_rate( INT rate )
{
	INT n;
	INT min_rate = INT32_MAX;
	INT max_rate = 0;

	for( n = 0 ; n < n_rates ; n ++ )
	{
		min_rate = min( min_rate, typically_rate[n] );
		max_rate = max( max_rate, typically_rate[n] );
	}
	
	rate = max( rate, min_rate );
	rate = min( rate, max_rate );
	
	return rate;
}


//=============================================================================
// H/W Devices
//-----------------------------------------------------------------------------
INT device_load( CONST CHAR *cards, CARDNAME *p_card, INT len )
{
	FILE *fp  = fopen( cards, "r" );
	BOOL skip = FALSE;
	INT cnt;

	if( invalid_ptr( fp ) )
		return 0;

	//----------------------------------------------------------------

	for( cnt = 0 ; cnt < len ; )
	{
		CHAR str[MAX_STRING];
		
		CHAR *start;
		CHAR *stop;
		CHAR *tail;

		//----------------------------------------------------------------

		if( fgetst( str, MAX_STRING, fp ) == NULL )
			break;

		if( skip )
		{
			skip = FALSE;
			continue;
		}

		//----------------------------------------------------------------

		start = strstr( str, "[" );
		if( invalid_ptr( start ) )
			continue;
		
		stop = strstr( start + 1, "]" );
		if( invalid_ptr( stop ) )
			continue;
		
		tail = strstr( stop + 1, ":" );
		if( invalid_ptr( tail ) )
			continue;
		
		//----------------------------------------------------------------
		
		strncpyt( p_card->desc, tail + 1, MAX_STRING );
		*stop = '\0';
		
		strncpyt( p_card->name, start + 1, MAX_STRING );
		*start = '\0';

		str_clear( str );
		str_clear( p_card->name );
		str_clear( p_card->desc );
		
		if
		(
			is_zero( strlen( p_card->name ) )
			||
			is_zero( strlen( p_card->desc ) )
			||
			!str_only_digit( str )
		)
		{
			continue;
		}

		//----------------------------------------------------------------

		p_card->dev_in	= 0;
		p_card->dev_out	= 0;

		p_card->dw_used		= DEV_PLAY | DEV_REC;
		p_card->dw_default	= is_zero( cnt ) ? DEV_PLAY | DEV_REC : 0x0;

		//----------------------------------------------------------------

		p_card	++;
		cnt		++;
		skip	= TRUE;
	}

	fclose( fp );
	return cnt;
}


VOID card_name( CONST CARDNAME *p_card, CHAR *name, INT len )
{
	snprintft( name, len, "hw:%s", p_card->name );
}


VOID device_name( CONST CARDNAME *p_card, BOOL b_out, CHAR *name, INT len )
{
	CHAR temp[MAX_STRING];
	
	card_name( p_card, temp, MAX_STRING );
	snprintft( name, len, "%s,%d", temp, b_out ? p_card->dev_out : p_card->dev_in );
}


//=============================================================================
// Config templates
//-----------------------------------------------------------------------------
#define DUMP_STR		\
	"",					\
	"",					\
	NULL,


STATIC CONST CHAR *title_section[] =
{
	"# ALSA library configuration file managed by arateconf.",
	"#",
	"# MANUAL CHANGES TO THIS FILE WILL BE OVERWRITTEN!",
	"#",
	"# Manual changes to the ALSA library configuration should be implemented",
	"# by editing the ~/.asoundrc file, not by editing this file.",
	DUMP_STR
};


STATIC CONST CHAR *device_section[] =
{
	"#=====================================================",
	"# Configuration for " T_NAME,
	"#-----------------------------------------------------",
	"",
	NULL,
};


STATIC CONST CHAR *dmixer_section[] =
{
	"# Perform dmixer",
	"pcm." T_NAME,
	"{",
	"	type			dmix",
	"	ipc_key			" T_ID,
	"	ipc_perm		0666",
	"",
	"	hint",
	"	{",
	"		show		" T_SHOW,
	"		description	" _Q( "Direct mixing of multiple audio streams" T_COMMENT ),
	"	}",
	"",
	"	slave",
	"	{",
	"		pcm		" _Q( T_NEXT ),
	"",
	"		rate		" T_RATE,
	"		channels	" T_CHANN,
	"		format		" T_FORMAT,
	"",
	"		period_size	" T_PERIOD,
	"		buffer_size	" T_BUFFER,
	"	}",
	"}",
	DUMP_STR
};


STATIC CONST CHAR *dsnooper_section[] =
{
	"# Perform dsnooper",
	"pcm." T_NAME,
	"{",
	"	type			dsnoop",
	"	ipc_key			" T_ID,
	"	ipc_perm		0666",
	"",
	"	hint",
	"	{",
	"		show		" T_SHOW,
	"		description	" _Q( "Recording from the same device for several applications simultaneously" T_COMMENT ),
	"	}",
	"",
	"	slave",
	"	{",
	"		pcm		" _Q( T_NEXT ),
	"",
	"		rate		" T_RATE,
	"		format		" T_FORMAT,
	"",
	"		period_size	" T_PERIOD,
	"		buffer_size	" T_BUFFER,
	"	}",
	"}",
	DUMP_STR
};


STATIC CONST CHAR *softvol_section[] =
{
	"# Perform softvol",
	"pcm." T_NAME,
	"{",
	"	type			softvol",
	"	slave.pcm		" _Q( T_NEXT ),
	"",
	"	hint",
	"	{",
	"		show		" T_SHOW,
	"		description	" _Q( T_DESC T_COMMENT ),
	"	}",
	"",
	"	control",
	"	{",
	"		name	" _Q( T_SELFNAME ),
	"		card	" _Q( T_MODULE ),
	"		device	" T_ID,
	"	}",
	"",
	"	max_dB		" T_VALUE ".0",
	"	resolution	32",
	"}",
	DUMP_STR
};


STATIC CONST CHAR *expand_section[] =
{
	"# Perform expand",
	"pcm." T_NAME,
	"{",
	"	type			plug",
	"	route_policy		" _Q( "duplicate" ),
	"	slave.pcm		" _Q( T_NEXT ),
	"",
	"	hint",
	"	{",
	"		show		" T_SHOW,
	"		description	" _Q( "Channels expander (mono to stereo, stereo to dolby, etc.)" T_COMMENT ),
	"	}",
	"}",
	DUMP_STR
};


STATIC CONST CHAR *asym_section[] =
{
	"# Perform duplex",
	"pcm." T_NAME,
	"{",
	"	type			asym",
	"	playback.pcm		" _Q( T_NEXT1 ),
	"	capture.pcm		" _Q( T_NEXT2 ),
	"",
	"	hint",
	"	{",
	"		show		" T_SHOW,
	"		description	" _Q( "Full duplex for simultaneous playback and recording" T_COMMENT ),
	"	}",
	"}",
	DUMP_STR
};


STATIC CONST CHAR *convert_section[] =
{
	"# Perform convert",
	"pcm." T_NAME,
	"{",
	"	type			rate",
	"	converter		" T_MODULE,
	"",
	"	hint",
	"	{",
	"		show		" T_SHOW,
	"		description	" _Q( T_DESC T_COMMENT ),
	"	}",
	""
	"",
	"	slave",
	"	{",
	"		pcm	" _Q( T_NEXT ),
	"		rate	" T_RATE,
	"		format	" T_FORMAT,
	"	}",
	"}",
	DUMP_STR
};


STATIC CONST CHAR *phonon_section[] =
{
	"# Phonon virtual device (for KDE)",
	"pcm." T_NAME,
	"{",
	"	type		plug",
	"	slave.pcm	" _Q( T_NEXT ),
	"",
	"	hint",
	"	{",
	"		show		on",
	"		description	" _Q( "ALSA Phonon virtual device" T_COMMENT ),
	"	}",
	"}",
	DUMP_STR
};


STATIC CONST CHAR *norm_section[] =
{
	"# Audio normalizator over LADPSA",
	"pcm." T_NAME,
	"{",
	"	type			plug",
	"	slave.pcm		" _Q( T_NAME P_NORM_COMPRESS ),
	"	hint.description	" _Q( "LADPSA normalizator: entry point" T_COMMENT ),
	"}",
	"",
	"pcm." T_NAME P_NORM_COMPRESS,
	"{",
	"	type			ladspa",
	"	path			" _Q( P_LADSPA_PATH ),
	"	slave.pcm		" _Q( T_NAME P_NORM_LIMIT ),
	"",
	"	hint",
	"	{",
	"		show		" T_SHOW,
	"		description	" _Q( "LADPSA normalizator: level controller" T_COMMENT ),
	"	}",
	"",
	"	plugins",
	"	[{",
	"		label		sc1",
	"		    #		Description: Attack time (ms)   Release time (ms)   Threshold (dB)   Ratio   Knee radius   Makeup gain (dB)",
	"		input		{ controls [ 30                 500                 0                6.5     6             20           ] }",
	"	}]",
	"}",
	"",
	"pcm." T_NAME P_NORM_LIMIT,
	"{",
	"	type			ladspa",
	"	path			" _Q( P_LADSPA_PATH ),
	"	slave.pcm		" _Q( T_NAME P_NORM_FORMAT ),
	"",
	"	hint",
	"	{",
	"		show		" T_SHOW,
	"		description	" _Q( "LADPSA normalizator: clamper" T_COMMENT ),
	"	}",
	"",
	"	playback_plugins",
	"	[{",
	"		label		fastLookaheadLimiter",
	"		    #		Description: Gain (dB)   Limit (dB)   Release time (s)",
	"		input		{ controls [ 0           0            0.8          ] }",
	"	}]",
	"}",
	"",
	"pcm." T_NAME P_NORM_FORMAT,
	"{",
	"	type			lfloat",
	"",
	"	hint",
	"	{",
	"		show		" T_SHOW,
	"		description	" _Q( "LADPSA normalizator: float to int converter" T_COMMENT ),
	"	}",
	"",
	"	slave",
	"	{",
	"		pcm	" _Q( T_NEXT ),
	"		format	" T_FORMAT,
	"	}",
	"}",
	DUMP_STR
};


STATIC CONST CHAR *plug_section[] =
{
	"# Perform plug device",
	"pcm." T_NAME,
	"{",
	"	type			plug",
	"	slave.pcm		" _Q( T_NEXT ),
	"	hint.description	" _Q( T_DESC T_COMMENT ),
	"}",
	DUMP_STR
};


//=============================================================================
// Device name functions
//-----------------------------------------------------------------------------
STATIC BOOL device_is_invalid( CONST CHAR *name )
{
	return strlen( name ) == 0;
}


STATIC BOOL device_is_equal( CONST CHAR *name1, CONST CHAR *name2 )
{
	return strcmp( name1, name2 ) == 0;
}


STATIC VOID device_clear( CHAR *name )
{
	if( !invalid_ptr( name ) )
		*name = '\0';
}


//=============================================================================
// Config functions
//-----------------------------------------------------------------------------
STATIC BOOL save_text( FILE *fp, CONST CHAR *data[], ... )
{
	BOOL b_empty_next = FALSE;
	REPL repl[MAX_REPL];
	INT  repl_n;

	//-----------------------------------------------------------------------------

	if( invalid_ptr( data ) )
		return FALSE;
	
	//-----------------------------------------------------------------------------

	{
		va_list vl;
		va_start( vl, data );

		for( repl_n = 0 ; repl_n < MAX_REPL ; repl_n ++ )
		{
			PREPL curr = &repl[repl_n];
			BOOL b_next;

			curr->name = va_arg( vl, CONST CHAR * );

			if( invalid_ptr( curr->name ) )
				break;

			//-----------------------------------------------------------------------------

			if
			(
				strcasecmp( curr->name, T_ID )		== 0
				||
				strcasecmp( curr->name, T_RATE )	== 0
				||
				strcasecmp( curr->name, T_CHANN )	== 0
				||
				strcasecmp( curr->name, T_PERIOD )	== 0
				||
				strcasecmp( curr->name, T_BUFFER )	== 0
				||
				strcasecmp( curr->name, T_VALUE )	== 0
			)
			{
				curr->type	= R_TYPE_INT;
				curr->i_val	= va_arg( vl, INT );
				continue;
			}

			b_next = 
				strcasecmp( curr->name, T_NEXT1 ) == 0
				||
				strcasecmp( curr->name, T_NEXT2 ) == 0;

			if
			(
				strcasecmp( curr->name, T_NAME )		== 0
				||
				strcasecmp( curr->name, T_MODULE )		== 0
				||
				strcasecmp( curr->name, T_DESC )		== 0
				||
				strcasecmp( curr->name, T_SELFNAME )	== 0
				||
				strcasecmp( curr->name, T_COMMENT )		== 0
				||
				strcasecmp( curr->name, T_SHOW )		== 0
				||
				strcasecmp( curr->name, T_FORMAT )		== 0
				||
				b_next
			)
			{
				curr->type	= R_TYPE_STR;
				curr->s_val	= va_arg( vl, CONST CHAR * );

				if( b_next && device_is_invalid( curr->s_val ) )
					b_empty_next = TRUE;

				continue;
			}

			break;
		}


		va_end( vl );
	}

	if( b_empty_next )
		return FALSE;

	//-----------------------------------------------------------------------------

	while( !invalid_ptr( *data ) )
	{
		CHAR str[MAX_STRING];
		INT n;

		strncpyt( str, *data ++, MAX_STRING );
		
		for( n = 0 ; n < repl_n ; n ++ )
		{
			PREPL curr = &repl[n];

			switch( curr->type )
			{
			case R_TYPE_INT:
				{
					CHAR temp[MAX_NAME];

					snprintft( temp, MAX_NAME, "%d", curr->i_val );
					str_replace( str, MAX_STRING, curr->name, temp );
				}
				break;

			case R_TYPE_STR:
				str_replace( str, MAX_STRING, curr->name, curr->s_val );
				break;
			}
		}
			
		fprintf( fp, "%s\n", str );
	}

	//-----------------------------------------------------------------------------

	return TRUE;
}


STATIC BOOL save_vol
(
	FILE				*fp,
	CONST CHAR			*name,
	CONST CHAR			*next,
	CONST CHAR			*desc,
	CONST CHAR			*selfname,
	CONST CHAR			*card,
	CONST CHAR			*comment,
	CONST CHAR			*show,
	INT					n_dev,
	INT					val
)
{
	return save_text
		(
			fp, softvol_section,
			T_NAME,		name,
			T_NEXT,		next,
			T_DESC,		desc,
			T_SELFNAME,	selfname,
			T_MODULE,	card,
			T_COMMENT,	comment,
			T_SHOW,		show,
			T_ID,		n_dev,
			T_VALUE,	val,
			NULL
		);
}


STATIC BOOL save_convert
(
	FILE				*fp,
	CONST CHAR			*name,
	CONST CHAR			*next,
	CONST CHAR			*desc,
	CONST CHAR			*comment,
	CONST CHAR			*show,
	CONST CONFPARAMS	*param
)
{
	return save_text
		(
			fp, convert_section,
			T_NAME,		name,
			T_COMMENT,	comment,
			T_SHOW,		show,
			T_NEXT,		next,
			T_DESC,		desc,
			T_MODULE,	param->convert,
			T_RATE,		param->rate,
			T_FORMAT,	param->format.name,
			NULL
		);
}


STATIC BOOL save_plug
(
	FILE				*fp,
	CONST CHAR			*name,
	CONST CHAR			*next,
	CONST CHAR			*comment,
	CONST CHAR			*desc
)
{
	return save_text
		(
			fp, plug_section,
			T_NAME,		name,
			T_NEXT,		next,
			T_COMMENT,	comment,
			T_DESC,		desc,
			NULL
		);
}


STATIC VOID save_plugs
(
	FILE				*fp,
	CONST CHAR			*dmix,
	CONST CHAR			*dsnoop,
	CONST CHAR			*def,
	CONST CHAR			*cap,
	CONST CHAR			*comment
)
{
	if( device_is_equal( dmix, dsnoop ) )
	{
		save_plug( fp, def, dmix, comment, P_DEF_DESC );
		return;
	}

	save_plug( fp, cap, dsnoop, comment, P_CAP_DESC );
	save_plug( fp, def, dmix, comment, P_PLAY_DESC );
}

//-----------------------------------------------------------------------------

STATIC VOID plug_name( CHAR *name, CONST CHAR *prefix, CONST CHAR *suff )
{
	if( strlen( suff ) )
		snprintft( name, MAX_STRING, "%s_%s", prefix, suff );
	else
		strncpyt( name, prefix, MAX_STRING );
}


STATIC VOID plug_next( CHAR *name, CONST CHAR *next )
{
	strncpyt( name, next, MAX_STRING );
}

//-----------------------------------------------------------------------------

BOOL config_init( CONST CHAR *name )
{
	device_clear( szDefPlayer );
	device_clear( szDefCapture );

	nID		= FIRST_ID;
	pFile	= fopen( name, "w" );

	if( invalid_ptr( pFile ) )
		return FALSE;

	save_text( pFile, title_section, NULL );
	return TRUE;
}


VOID config_close( VOID )
{
	if( !invalid_ptr( pFile ) )
	{
		if( !device_is_invalid( szDefPlayer ) || !device_is_invalid( szDefCapture ) )
		{
			save_text( pFile, device_section, T_NAME, "default audio device", NULL );
			save_plugs( pFile, szDefPlayer, szDefCapture, P_DEFAULT, P_CAPTURE, "" );
		}

		fclose( pFile );
	}

	pFile = NULL;
}


BOOL config_save( CONST CONFPARAMS *param, BOOL b_single, BOOL b_show )
{
	CHAR p_dmix[MAX_STRING]		= "";
	CHAR p_dsnoop[MAX_STRING]	= "";
	CHAR p_comment[MAX_STRING]	= "";
	CHAR p_temp[MAX_STRING];

	CONST CHAR *p_name	= param->p_card->name;
	CONST CHAR *p_suff	= p_name;
	CONST CHAR *p_show	= b_show ? "on" : "off";

	//-----------------------------------------------------------------------------

	if( invalid_ptr( pFile ) )
		return FALSE;

	//-----------------------------------------------------------------------------

	if( b_single )
		p_suff = "";
	else
		snprintft( p_comment, MAX_STRING, " (%s)", p_name );

	if( flag_check_mask( param->p_card->dw_used, DEV_PLAY ) )
		device_name( param->p_card, TRUE, p_dmix, MAX_STRING );

	if( flag_check_mask( param->p_card->dw_used, DEV_REC ) )
		device_name( param->p_card, FALSE, p_dsnoop, MAX_STRING );

	//-----------------------------------------------------------------------------

	save_text( pFile, device_section, T_NAME, p_name, NULL );

	//-----------------------------------------------------------------------------

	if( param->use_dmix )
	{
		plug_name( p_temp, P_DMIXER, p_suff );

		if
		(
			save_text
			(
				pFile, dmixer_section,
				T_NAME,		p_temp,
				T_NEXT,		p_dmix,
				T_COMMENT,	p_comment,
				T_ID,		nID ++,
				T_RATE,		param->rate,
				T_CHANN,	param->chann,
				T_FORMAT,	param->format.name,
				T_PERIOD,	param->period * param->period_mul,
				T_BUFFER,	calculate_buffer( param->rate, param->period ) * param->buff_mul,
				T_SHOW,		p_show,
				NULL
			)
		)
		{
			plug_next( p_dmix, p_temp );
		}
	}

	//-----------------------------------------------------------------------------

	if( param->use_dsnoop )
	{
		plug_name( p_temp, P_DSNOOPER, p_suff );

		if
		(
			save_text
			(
				pFile, dsnooper_section,
				T_NAME,		p_temp,
				T_NEXT,		p_dsnoop,
				T_COMMENT,	p_comment,
				T_ID,		nID ++,
				T_RATE,		param->rate,
				T_FORMAT,	param->format.name,
				T_PERIOD,	param->period * param->period_mul,
				T_BUFFER,	calculate_buffer( param->rate, param->period ) * param->buff_mul,
				T_SHOW,		p_show,
				NULL
			)
		)
		{
			plug_next( p_dsnoop, p_temp );
		}
	}

	//-----------------------------------------------------------------------------
	
	if( param->use_soft_play )
	{
		plug_name( p_temp, P_VOL_PLAY, p_suff );

		if
		(
			save_vol
			(
				pFile,
				p_temp,
				p_dmix,
				P_VOL_DESC_PLAY, 
				P_VOL_NAME_PLAY,
				p_name,
				p_comment, 
				p_show,
				param->p_card->dev_out, 
				param->max_play_db
			)
		)
		{
			plug_next( p_dmix, p_temp );
		}
	}

	if( param->use_soft_rec )
	{
		plug_name( p_temp, P_VOL_REC, p_suff );

		if
		(
			save_vol
			(
				pFile, 
				p_temp, 
				p_dsnoop,
				P_VOL_DESC_REC,
				P_VOL_NAME_REC,
				p_name,
				p_comment,
				p_show,
				param->p_card->dev_in,
				param->max_play_db
			)
		)
		{
			plug_next( p_dsnoop, p_temp );
		}
	}

	//-----------------------------------------------------------------------------

	if( param->use_expand )
	{
		plug_name( p_temp, P_EXPAND, p_suff );
		
		if( save_text( pFile, expand_section, T_NAME, p_temp, T_COMMENT, p_comment, T_NEXT, p_dmix, T_SHOW, p_show, NULL ) )
			plug_next( p_dmix, p_temp );
	}
	
	//-----------------------------------------------------------------------------
	
	if( param->use_asym )
	{
		plug_name( p_temp, P_DUPLEX, p_suff );
		
		if( save_text( pFile, asym_section, T_NAME, p_temp, T_COMMENT, p_comment, T_NEXT1, p_dmix, T_NEXT2, p_dsnoop, T_SHOW, p_show, NULL ) )
		{
			plug_next( p_dmix,   p_temp );
			plug_next( p_dsnoop, p_temp );
		}
	}

	//-----------------------------------------------------------------------------

	if( param->use_convert )
	{
		if( device_is_equal( p_dmix, p_dsnoop ) )
		{
			plug_name( p_temp, P_CONVERT, p_suff );
			
			if( save_convert( pFile, p_temp, p_dmix, P_CONV_DESC, p_comment, p_show, param ) )
			{
				plug_next( p_dmix,   p_temp );
				plug_next( p_dsnoop, p_temp );
			}
		}
		else
		{
			plug_name( p_temp, P_CONVERT_PLAY, p_suff );
			
			if( save_convert( pFile, p_temp, p_dmix, P_CONV_DESC_PLAY, p_comment, p_show, param ) )
				plug_next( p_dmix, p_temp );
			
			plug_name( p_temp, P_CONVERT_REC, p_suff );
			
			if( save_convert( pFile, p_temp, p_dsnoop, P_CONV_DESC_REC, p_comment, p_show, param ) )
				plug_next( p_dsnoop, p_temp );
		}
	}

	//-----------------------------------------------------------------------------

	if( param->use_phonon )
	{
		plug_name( p_temp, P_PHONON, p_suff );
		save_text( pFile, phonon_section, T_NAME, p_temp, T_COMMENT, p_comment, T_NEXT, p_dmix, NULL );
	}
	
	if( param->use_norm )
	{
		plug_name( p_temp, P_NORM, p_suff );
		save_text( pFile, norm_section, T_NAME, p_temp, T_COMMENT, p_comment, T_NEXT, p_dmix, T_FORMAT, param->format.name, T_SHOW, p_show, NULL );
	}

	//-----------------------------------------------------------------------------
	
	if( flag_check_mask( param->p_card->dw_default, DEV_PLAY ) )
	{
		strncpyt( szDefPlayer, p_dmix, MAX_STRING );
		device_clear( p_dmix );
	}
		
	if( flag_check_mask( param->p_card->dw_default, DEV_REC ) )
	{
		strncpyt( szDefCapture, p_dsnoop, MAX_STRING );
		device_clear( p_dsnoop );
	}

	//-----------------------------------------------------------------------------

	{
		CHAR p_def[MAX_STRING];
		CHAR p_cap[MAX_STRING];

		plug_name( p_def, P_PRIMARY, p_suff );
		plug_name( p_cap, P_CAPTURE, p_suff );

		save_plugs( pFile, p_dmix, p_dsnoop, p_def, p_cap, p_comment );
	}

	//-----------------------------------------------------------------------------

	return TRUE;
}

