//=============================================================================
// Includes
//-----------------------------------------------------------------------------
#include <stdio.h>

#include "swin.h"
#include "types.h"
#include "array.h"
#include "getoptw.h"
#include "cmdline.h"
#include "kbhit.h"
#include "str.h"
#include "menu.h"

#if defined(IS_LINUX)

#include <stdlib.h>
#include <unistd.h>
#include <alsa/asoundlib.h>

#else

// Fake defines for Win32
#define SND_PCM_FORMAT_U8			0x0
#define SND_PCM_FORMAT_S16_LE		0x1
#define SND_PCM_FORMAT_S24_3LE		0x2
#define SND_PCM_FORMAT_S24_LE		0x3
#define SND_PCM_FORMAT_S32_LE		0x4
#define SND_PCM_FORMAT_FLOAT_LE		0x5
#define SND_PCM_FORMAT_FLOAT64_LE	0x6

#endif

#include "conf.h"


//=============================================================================
// Definitions
//-----------------------------------------------------------------------------
#define ASOUNDRC				".asoundrc"
#define ASOUNDCONF				"/etc/asound.conf"

#define MAIN_MENU_SIZE			( 20 + 1 )	// Number of items (see below) + Finalize
#define DEVICE_MENU_SIZE		6			// Space + Set number + Turn On/Off + Space + Return + Finalize

#define MAX_CARDS				min( MENU_MAX_ITEMS - MAIN_MENU_SIZE, 10 )
#define MAX_DEVICE				min( MENU_MAX_ITEMS - DEVICE_MENU_SIZE, 10 )


#define CONVERT_FFT				"fftrate"
#define CONVERT_SPEEX			"speexrate"
#define CONVERT_SPEEX_BEST		"speexrate_best"
#define CONVERT_SAMPLE			"samplerate"
#define CONVERT_SAMPLE_MED		"samplerate_medium"
#define CONVERT_SAMPLE_BEST		"samplerate_best"
#define CONVERT_SAMPLE_ORDER	"samplerate_order"
#define CONVERT_SAMPLE_LIN		"samplerate_linear"

#define FORMAT_U8				"U8"
#define FORMAT_S16_LE			"S16_LE"
#define FORMAT_S24_3LE			"S24_3LE"
#define FORMAT_S24_LE			"S24_LE"
#define FORMAT_S32_LE			"S32_LE"
#define FORMAT_FLOAT_LE			"FLOAT_LE"
#define FORMAT_FLOAT64_LE		"FLOAT64_LE"


//=============================================================================
// ROM
//-----------------------------------------------------------------------------
STATIC HELPTXT_BEGIN( g_szHelp )
	HELPTXT_ITEM( 1, "Usage: arateconf [-h|--help]"			),
	HELPTXT_ITEM( 2, ""													),
	HELPTXT_ITEM( 2, "Options:"											),
	HELPTXT_ITEM( 2, " -h, --help    Show this message"					),
HELPTXT_END


STATIC GETOPT_BEGIN( g_LongOpt )
	GETOPT_ITEM_SYM(	"help",		'h' ),
GETOPT_END


//=============================================================================
// Menu forward declaration
//-----------------------------------------------------------------------------
STATIC MENU_DECL( main_menu );
STATIC MENU_DECL( param_menu );
STATIC MENU_DECL( device_menu );
STATIC MENU_DECL( format_menu );
STATIC MENU_DECL( convert_menu );
STATIC MENU_DECL( plug_menu );
STATIC MENU_DECL( templates_menu );


//=============================================================================
// Parameters
//-----------------------------------------------------------------------------
STATIC CHAR CfgFileName[MAX_STRING] = "";

STATIC CONST CHAR *CardsName =
#if defined(IS_WIN32)
	"cards-0";
#elif defined(IS_LINUX)
	"/proc/asound/cards";
#else
	"";
#endif


//=============================================================================
//  RAM
//-----------------------------------------------------------------------------
STATIC CARDNAME Cards[MAX_CARDS];
STATIC INT nCards = 0;

STATIC CONFPARAMS Parameters[MAX_CARDS];
STATIC CONFPARAMS *pCurrParam;

STATIC CARDNAME Devices[MAX_DEVICE];
STATIC INT nDevices = 0;

STATIC BOOL bShow = FALSE;
STATIC BOOL bApply;


//=============================================================================
//  Aux functions
//-----------------------------------------------------------------------------
STATIC BOOL file_is_exist( CONST CHAR *name )
{
	FILE *fp = fopen( name, "rb" );

	if( invalid_ptr( fp ) )
		return FALSE;

	fclose( fp );
	return TRUE;
}


//=============================================================================
// 'Title' functions
//-----------------------------------------------------------------------------
STATIC VOID title_checkbox( CHAR *title, INT len, BOOL val )
{
	snprintft( title, len, " %s", check_box( val ) );
}


STATIC VOID title_active_card( CONST MENUITEM *self, CHAR *title, INT len )
{
	CONST CARDNAME *pCard = (CONST CARDNAME *)MENU_MISC_PTR( *self );

	if( pCard == NULL || pCurrParam == NULL )
		return;
	
	snprintft
		(
			title,
			MAX_STRING, 
			"%s |  %s   |  %s     %s   |  %s    %s   | %s",
			( pCard == pCurrParam->p_card ) ? ">>>>>" : "     ",
			star_box( !flag_is_empty( pCard->dw_used ) ),
			star_box( flag_check_mask( pCard->dw_used, DEV_PLAY ) ),
			star_box( flag_check_mask( pCard->dw_default, DEV_PLAY ) ),
			star_box( flag_check_mask( pCard->dw_used, DEV_REC ) ),
			star_box( flag_check_mask( pCard->dw_default, DEV_REC ) ),
			pCard->desc
		);
}


STATIC VOID title_param( CONST MENUITEM *self, CHAR *title, INT len )
{
	if( pCurrParam == NULL )
		return;
	
	snprintft( title, len, ": %d Hz, %d ch, '%s'", pCurrParam->rate, pCurrParam->chann, pCurrParam->format.name );
}


STATIC VOID title_rate( CONST MENUITEM *self, CHAR *title, INT len )
{
	if( pCurrParam == NULL )
		return;
	
	snprintft( title, len, " = %d Hz", pCurrParam->rate );
}


STATIC VOID title_channels( CONST MENUITEM *self, CHAR *title, INT len )
{
	if( pCurrParam == NULL )
		return;
	
	snprintft( title, len, " = %d", pCurrParam->chann );
}


STATIC VOID title_format( CONST MENUITEM *self, CHAR *title, INT len )
{
	if( pCurrParam == NULL )
		return;
	
	snprintft( title, len, ": '%s'", pCurrParam->format.name );
}


STATIC VOID title_buffer_align( CONST MENUITEM *self, CHAR *title, INT len )
{
	if( pCurrParam == NULL )
		return;
	
	title_checkbox( title, len, pCurrParam->buff_resize );
}


STATIC VOID title_buffer_mul( CONST MENUITEM *self, CHAR *title, INT len )
{
	if( pCurrParam == NULL )
		return;
	
	snprintft( title, len, " = %d", pCurrParam->buff_mul );
}


STATIC VOID title_period_mul( CONST MENUITEM *self, CHAR *title, INT len )
{
	if( pCurrParam == NULL )
		return;
	
	snprintft( title, len, " = %d", pCurrParam->period_mul );
}


STATIC VOID title_ampl_play( CONST MENUITEM *self, CHAR *title, INT len )
{
	if( pCurrParam == NULL )
		return;
	
	snprintft( title, len, " = %d dB", pCurrParam->max_play_db );
}


STATIC VOID title_ampl_rec( CONST MENUITEM *self, CHAR *title, INT len )
{
	if( pCurrParam == NULL )
		return;
	
	snprintft( title, len, " = %d dB", pCurrParam->max_rec_db );
}


STATIC VOID title_convert( CONST MENUITEM *self, CHAR *title, INT len )
{
	if( pCurrParam == NULL )
		return;
	
	snprintft( title, len, ": %s", pCurrParam->convert );
}


STATIC VOID title_show( CONST MENUITEM *self, CHAR *title, INT len )
{
	snprintft( title, len, " %s", check_box( bShow ) );
}


STATIC VOID title_plug( CONST MENUITEM *self, CHAR *title, INT len )
{
	if( pCurrParam == NULL )
		return;
	
	snprintft
		(
			title,
			len,
			":\n"
			" %s Convert,  %s Expand, %s Asym\n"
			" %s Play Vol, %s Dmix\n"
			" %s Rec. Vol, %s Dsnoop\n"
			" %s Phonon,   %s Normalizator",
			check_box( pCurrParam->use_convert ),
			check_box( pCurrParam->use_expand ),
			check_box( pCurrParam->use_asym ),
			check_box( pCurrParam->use_soft_play ),
			check_box( pCurrParam->use_dmix ),
			check_box( pCurrParam->use_soft_rec ),
			check_box( pCurrParam->use_dsnoop ),
			check_box( pCurrParam->use_phonon ),
			check_box( pCurrParam->use_norm )
		);
}


STATIC VOID title_converter( CONST MENUITEM *self, CHAR *title, INT len )
{
	if( pCurrParam == NULL )
		return;
	
	title_checkbox( title, len, pCurrParam->use_convert );
}


STATIC VOID title_expand( CONST MENUITEM *self, CHAR *title, INT len )
{
	if( pCurrParam == NULL )
		return;
	
	title_checkbox( title, len, pCurrParam->use_expand );
}


STATIC VOID title_asym( CONST MENUITEM *self, CHAR *title, INT len )
{
	if( pCurrParam == NULL )
		return;
	
	title_checkbox( title, len, pCurrParam->use_asym );
}


STATIC VOID title_soft_play( CONST MENUITEM *self, CHAR *title, INT len )
{
	if( pCurrParam == NULL )
		return;
	
	title_checkbox( title, len, pCurrParam->use_soft_play );
}


STATIC VOID title_soft_rec( CONST MENUITEM *self, CHAR *title, INT len )
{
	if( pCurrParam == NULL )
		return;
	
	title_checkbox( title, len, pCurrParam->use_soft_rec );
}


STATIC VOID title_dmix( CONST MENUITEM *self, CHAR *title, INT len )
{
	if( pCurrParam == NULL )
		return;
	
	title_checkbox( title, len, pCurrParam->use_dmix );
}


STATIC VOID title_dsnoop( CONST MENUITEM *self, CHAR *title, INT len )
{
	if( pCurrParam == NULL )
		return;
	
	title_checkbox( title, len, pCurrParam->use_dsnoop );
}


STATIC VOID title_phonon( CONST MENUITEM *self, CHAR *title, INT len )
{
	if( pCurrParam == NULL )
		return;
	
	title_checkbox( title, len, pCurrParam->use_phonon );
}


STATIC VOID title_norm( CONST MENUITEM *self, CHAR *title, INT len )
{
	if( pCurrParam == NULL )
		return;
	
	title_checkbox( title, len, pCurrParam->use_norm );
}


STATIC VOID title_card( CONST CHAR *name, INT hw, CHAR *title, INT len )
{
	if( pCurrParam == NULL )
		return;
	
	snprintft( title, len, ": %s,%d", name, hw );
}


STATIC VOID title_card_out( CONST MENUITEM *self, CHAR *title, INT len )
{
	if( pCurrParam == NULL )
		return;
	
	title_card( pCurrParam->p_card->name, pCurrParam->p_card->dev_out, title, len );
}


STATIC VOID title_card_in( CONST MENUITEM *self, CHAR *title, INT len )
{
	if( pCurrParam == NULL )
		return;
	
	title_card( pCurrParam->p_card->name, pCurrParam->p_card->dev_in, title, len );
}


STATIC VOID title_cfg_file( CONST MENUITEM *self, CHAR *title, INT len )
{
	snprintft( title, len, " '%s'", CfgFileName );
}


STATIC VOID title_disable_device( CONST MENUITEM *self, CHAR *title, INT len )
{
	DWORD mask;

	if( pCurrParam == NULL )
		return;

	mask = pCurrParam->desc.b_out ? DEV_PLAY : DEV_REC;
	snprintft
		(
			title,
			len,
			"%s Turn On/Off",
			check_box( flag_check_mask( pCurrParam->p_card->dw_used, mask ) )
		);
}


//=============================================================================
// 'Do' functions
//-----------------------------------------------------------------------------
STATIC BOOL clear_screen( VOID )
{
	clrscr();
	return TRUE;
}


STATIC VOID do_exit( CONST MENUITEM *self )
{
	if( !bApply || menu_yes_no( MENU_STR_ARE_YOU_SURE ) )
		menu_stop();
}


STATIC VOID do_show( CONST MENUITEM *self )
{
	bShow	= !bShow;
	bApply	= TRUE;
}


STATIC VOID do_set_used( CONST MENUITEM *self )
{
	if( pCurrParam )
	{
		PCARDNAME p_card = pCurrParam->p_card;

		if( flag_is_empty( p_card->dw_used ) )
			flag_set( p_card->dw_used, DEV_PLAY | DEV_REC );
		else
			flag_init( p_card->dw_used );

		flag_init( p_card->dw_default );

		bApply = TRUE;
	}
}


STATIC VOID do_set_player( CONST MENUITEM *self )
{
	if( pCurrParam )
	{
		PCARDNAME p_card = pCurrParam->p_card;
		BOOL val = !flag_check_mask( p_card->dw_default, DEV_PLAY );
		INT n;

		for( n = 0 ; n < nCards ; n ++ )
			flag_reset( Parameters[n].p_card->dw_default, DEV_PLAY );

		if( val )
		{
			flag_set( p_card->dw_used, DEV_PLAY );
			flag_set( p_card->dw_default, DEV_PLAY );
		}

		bApply = TRUE;
	}
}


STATIC VOID do_set_rec( CONST MENUITEM *self )
{
	if( pCurrParam )
	{
		PCARDNAME p_card = pCurrParam->p_card;
		BOOL val = !flag_check_mask( p_card->dw_default, DEV_REC );
		INT n;
		
		for( n = 0 ; n < nCards ; n ++ )
			flag_reset( Parameters[n].p_card->dw_default, DEV_REC );
		
		if( val )
		{
			flag_set( p_card->dw_used, DEV_REC );
			flag_set( p_card->dw_default, DEV_REC );
		}

		bApply = TRUE;
	}
}


STATIC VOID do_test( CONST MENUITEM *self )
{
	INT n;

	printf( "Testing H/W compatibility ...\n" );

	for( n = 0 ; n < nCards ; n ++ )
	{
		CONFPARAMS *p_param	= &Parameters[n];
		PCARDNAME  p_card	= p_param->p_card;

		printf( "\nDevice: %s\n", p_card->name );

		if( flag_check_mask( p_card->dw_used, DEV_PLAY ) )
		{
			printf( "Output:\n" );
			test_parameters( p_param, TRUE );
		}
		
		if( flag_check_mask( p_card->dw_used, DEV_REC ) )
		{
			printf( "Input:\n" );
			test_parameters( p_param, FALSE );
		}

		if( flag_is_empty( p_card->dw_used ) )
			printf( "Not used\n" );
	}

	menu_press( "\n" MENU_STR_PRESS_ANY_KEY );
}


STATIC VOID do_save( CONST MENUITEM *self )
{
	INT nUsed = 0;
	INT n;

	//-----------------------------------------------------------------------------

	if( file_is_exist( CfgFileName ) )
	{
		printf( "File '%s' is already exist. ", CfgFileName );

		if( !menu_yes_no( "Overwrite " MENU_STR_YN ) )
			return;
	}

	//-----------------------------------------------------------------------------

	printf( "Saving config file...\n" );
	
	for( n = 0 ; n < nCards ; n ++ )
	{
		if( !flag_is_empty( Parameters[n].p_card->dw_used ) )
			nUsed ++;
	}

	if( config_init( CfgFileName ) )
	{
		INT nCount = 0;
		INT nDone  = 0;

		for( n = 0 ; n < nCards ; n ++ )
		{
			if( flag_is_empty( Parameters[n].p_card->dw_used ) )
				continue;

			nCount ++;

			if( config_save( &Parameters[n], nUsed == 1, bShow ) )
				nDone ++;
		}

		config_close();
		update_mixer();

		bApply = !( nCount == nDone );
		printf( "Ok.\n" );
	}
	else
		printf( "Failed!\n" );

	menu_press( "\n" MENU_STR_PRESS_ANY_KEY );
}


STATIC VOID do_delete( CONST MENUITEM *self )
{
	if( file_is_exist( CfgFileName ) )
	{
		printf( "File '%s' is exist. ", CfgFileName );
		
		if( !menu_yes_no( MENU_STR_ARE_YOU_SURE ) )
			return;
	}

	remove( CfgFileName );
}


STATIC VOID do_set_rate( CONST MENUITEM *self )
{
	if( pCurrParam == NULL )
		return;

	printf( "Sample rate (in Hz) = " );
	unref_result( scanf( "%d", &pCurrParam->rate ) );
	
	pCurrParam->rate	= correct_rate( pCurrParam->rate );
	pCurrParam->period	= calculate_period( pCurrParam->rate, pCurrParam->buff_resize );

	bApply = TRUE;
}


STATIC VOID do_set_channels( CONST MENUITEM *self )
{
	if( pCurrParam == NULL )
		return;
	
	printf( "Channels = " );
	unref_result( scanf( "%d", &pCurrParam->chann ) );
	bApply = TRUE;
}


STATIC VOID do_set_format( CONST MENUITEM *self )
{
	if( pCurrParam == NULL )
		return;

	pCurrParam->format.name = self->desc;
	pCurrParam->format.val	= MENU_MISC_IVAL( *self );

	bApply = TRUE;
}


STATIC VOID do_buffer_align( CONST MENUITEM *self )
{
	if( pCurrParam == NULL )
		return;
	
	pCurrParam->buff_resize	= !pCurrParam->buff_resize;
	pCurrParam->period		= calculate_period( pCurrParam->rate, pCurrParam->buff_resize );
	bApply = TRUE;
}


STATIC VOID do_buffer_mul( CONST MENUITEM *self )
{
	if( pCurrParam == NULL )
		return;

	printf( "Buffer multiplier = " );
	unref_result( scanf( "%d", &pCurrParam->buff_mul ) );
	
	pCurrParam->buff_mul = max( pCurrParam->buff_mul, 1 );
	pCurrParam->buff_mul = min( pCurrParam->buff_mul, 8 );

	bApply = TRUE;
}


STATIC VOID do_period_mul( CONST MENUITEM *self )
{
	if( pCurrParam == NULL )
		return;

	printf( "Period multiplier = " );
	unref_result( scanf( "%d", &pCurrParam->period_mul ) );
	
	pCurrParam->period_mul = max( pCurrParam->period_mul, 1 );
	pCurrParam->period_mul = min( pCurrParam->period_mul, 8 );

	bApply = TRUE;
}


STATIC VOID do_set_ampl_play( CONST MENUITEM *self )
{
	if( pCurrParam == NULL )
		return;
	
	printf( "Play amplification = " );
	unref_result( scanf( "%d", &pCurrParam->max_play_db ) );
	bApply = TRUE;
}


STATIC VOID do_set_ampl_rec( CONST MENUITEM *self )
{
	if( pCurrParam == NULL )
		return;
	
	printf( "Record amplification = " );
	unref_result( scanf( "%d", &pCurrParam->max_rec_db ) );
	bApply = TRUE;
}


STATIC VOID do_set_convert( CONST MENUITEM *self )
{
	if( pCurrParam == NULL )
		return;
	
	pCurrParam->convert = self->desc;
	bApply = TRUE;
}


STATIC VOID do_set_converter( CONST MENUITEM *self )
{
	if( pCurrParam == NULL )
		return;
	
	pCurrParam->use_convert = !pCurrParam->use_convert;
	bApply = TRUE;
}


STATIC VOID do_set_expand( CONST MENUITEM *self )
{
	if( pCurrParam == NULL )
		return;
	
	pCurrParam->use_expand = !pCurrParam->use_expand;
	bApply = TRUE;
}


STATIC VOID do_set_asym( CONST MENUITEM *self )
{
	if( pCurrParam == NULL )
		return;
	
	pCurrParam->use_asym= !pCurrParam->use_asym;
	bApply = TRUE;
}


STATIC VOID do_set_soft_play( CONST MENUITEM *self )
{
	if( pCurrParam == NULL )
		return;
	
	pCurrParam->use_soft_play = !pCurrParam->use_soft_play;
	bApply = TRUE;
}


STATIC VOID do_set_soft_rec( CONST MENUITEM *self )
{
	if( pCurrParam == NULL )
		return;
	
	pCurrParam->use_soft_rec = !pCurrParam->use_soft_rec;
	bApply = TRUE;
}


STATIC VOID do_set_dmix( CONST MENUITEM *self )
{
	if( pCurrParam == NULL )
		return;
	
	pCurrParam->use_dmix= !pCurrParam->use_dmix;
	bApply = TRUE;
}


STATIC VOID do_set_dsnoop( CONST MENUITEM *self )
{
	if( pCurrParam == NULL )
		return;
	
	pCurrParam->use_dsnoop= !pCurrParam->use_dsnoop;
	bApply = TRUE;
}


STATIC VOID do_set_phonon( CONST MENUITEM *self )
{
	if( pCurrParam == NULL )
		return;
	
	pCurrParam->use_phonon = !pCurrParam->use_phonon;
	bApply = TRUE;
}


STATIC VOID do_set_norm( CONST MENUITEM *self )
{
	if( pCurrParam == NULL )
		return;
	
	pCurrParam->use_norm = !pCurrParam->use_norm;
	bApply = TRUE;
}


STATIC VOID do_set_default( CONST MENUITEM *self )
{
	if( pCurrParam == NULL )
		return;
	
	pCurrParam->use_convert		= TRUE;
	pCurrParam->use_expand		= FALSE;
	pCurrParam->use_asym		= TRUE;
	
	pCurrParam->use_soft_play	= FALSE;
	pCurrParam->use_soft_rec	= FALSE;
	
	pCurrParam->use_dmix		= TRUE;
	pCurrParam->use_dsnoop		= TRUE;

	pCurrParam->use_phonon		= FALSE;
	pCurrParam->use_norm		= FALSE;
	
	bApply = TRUE;
}


STATIC VOID do_set_software( CONST MENUITEM *self )
{
	if( pCurrParam == NULL )
		return;
	
	pCurrParam->use_convert		= TRUE;
	pCurrParam->use_expand		= FALSE;
	pCurrParam->use_asym		= TRUE;
	
	pCurrParam->use_soft_play	= FALSE;
	pCurrParam->use_soft_rec	= FALSE;
	
	pCurrParam->use_dmix		= TRUE;
	pCurrParam->use_dsnoop		= TRUE;

	bApply = TRUE;
}


STATIC VOID do_set_hadrware( CONST MENUITEM *self )
{
	if( pCurrParam == NULL )
		return;
	
	pCurrParam->use_convert		= FALSE;
	pCurrParam->use_expand		= FALSE;
	pCurrParam->use_asym		= FALSE;
	
	pCurrParam->use_soft_play	= FALSE;
	pCurrParam->use_soft_rec	= FALSE;
	
	pCurrParam->use_dmix		= FALSE;
	pCurrParam->use_dsnoop		= FALSE;

	bApply = TRUE;
}


STATIC VOID do_set_card( CONST MENUITEM *self )
{
	INT n = self->sym - '0';

	if( n >= 0 && n < nCards && pCurrParam )
	{
		pCurrParam = &Parameters[n];
	}
}


STATIC VOID do_enter_device_number( CONST MENUITEM *self )
{
	CARDDESC *p_desc = (CARDDESC *)MENU_MISC_PTR( device_menu );
	
	if( !invalid_ptr( p_desc ) )
	{
		INT val;

		printf( "Device number = " );
		unref_result( scanf( "%d", &val ) );

		if( p_desc->b_out )
			p_desc->p_card->dev_out	= val;
		else
			p_desc->p_card->dev_in	= val;
		
		bApply = TRUE;
	}
}


STATIC VOID do_disable_device( CONST MENUITEM *self )
{
	CARDDESC *p_desc = (CARDDESC *)MENU_MISC_PTR( device_menu );
	
	if( !invalid_ptr( p_desc ) )
	{
		DWORD mask = p_desc->b_out ? DEV_PLAY : DEV_REC;

		flag_mod( p_desc->p_card->dw_used, !flag_check_mask( p_desc->p_card->dw_used, mask ), mask );
		flag_reset( p_desc->p_card->dw_default, mask );

		bApply = TRUE;
	}
}


STATIC VOID do_set_device_number( CONST MENUITEM *self )
{
	CARDDESC *p_desc = (CARDDESC *)MENU_MISC_PTR( device_menu );
	INT n = self->sym - '0';
	
	if( n >= 0 && n < nDevices && !invalid_ptr( p_desc ) )
	{
		if( p_desc->b_out )
			p_desc->p_card->dev_out	= Devices[n].dev_out;
		else
			p_desc->p_card->dev_in	= Devices[n].dev_out;
		
		bApply = TRUE;
	}
}


STATIC VOID do_set_hw( CONST MENUITEM *self, BOOL b_out )
{
	MENUITEM *item = device_menu.items;
	INT n;

	//-----------------------------------------------------------------------------

	if( pCurrParam == NULL )
		return;
	
	MENU_SET_TITLE( device_menu, b_out ? "Output devices" : "Input devices" );
	MENU_MISC_PTR( device_menu ) = &pCurrParam->desc;
	
	//-----------------------------------------------------------------------------

	pCurrParam->desc.p_card	= pCurrParam->p_card;
	pCurrParam->desc.b_out	= b_out;
	
	nDevices = enum_devices( pCurrParam->p_card, Devices, MAX_DEVICE, b_out );
		
	for( n = 0 ; n < nDevices ; n ++ )
	{
		menu_fill_item
			(
				item ++,
				Devices[n].desc,
				(CHAR)( n + '0' ),
				&main_menu,
				do_set_device_number,
				NULL,
				NULL
			);
	}
		  
	menu_copy_item( item ++, &MenuSeparator );

	menu_fill_item
		(
			item ++,
			"Set device number",
			'N',
			&main_menu,
			do_enter_device_number,
			NULL,
			NULL
		);
	
	menu_fill_item
		(
			item ++,
			"",
			'D',
			&main_menu,
			do_disable_device,
			title_disable_device,
			NULL
		);

	menu_copy_item( item ++, &MenuSeparator );

	menu_fill_item
		(
			item ++,
			ITEM_RET_DESC_MAIN,
			MENU_RET_SYM,
			&main_menu,
			NULL,
			NULL,
			NULL
		);

	menu_final_item( item );
}


STATIC VOID do_set_hw_in( CONST MENUITEM *self )
{
	do_set_hw( self, FALSE );
}


STATIC VOID do_set_hw_out( CONST MENUITEM *self )
{
	do_set_hw( self, TRUE );
}


//=============================================================================
// Init functions
//-----------------------------------------------------------------------------
STATIC VOID init( VOID )
{
	INT n;

	//-----------------------------------------------------------------------------
	
#if defined(IS_WIN32)
	strncpyt( CfgFileName, ASOUNDRC, MAX_STRING );
#elif defined(IS_LINUX)
	if( getuid() == 0 )
		strncpyt( CfgFileName, ASOUNDCONF, MAX_STRING );
	else
		snprintft( CfgFileName, MAX_STRING, "%s/" ASOUNDRC, getenv( "HOME" ) );
#endif

	//-----------------------------------------------------------------------------

	nCards = device_load( CardsName, Cards, MAX_CARDS );
	menu_add( &main_menu, 1, nCards );

	for( n = 0 ; n < nCards ; n ++ )
	{
		MENUMISC misc = { &Cards[n], 0 };

		Parameters[n].p_card = &Cards[n];

		menu_fill_item
			(
				&main_menu.items[n + 1],
				"",
				(CHAR)( n + '0' ),
				&main_menu,
				do_set_card,
				title_active_card,
				&misc
			);
	}

	//-----------------------------------------------------------------------------

	for( n = 0 ; n < nCards ; n ++ )
	{
		pCurrParam = &Parameters[n];

		pCurrParam->buff_resize	= TRUE;
		pCurrParam->buff_mul	= 1;
		pCurrParam->period_mul	= 1;

		pCurrParam->rate		= DEF_RATE;
		pCurrParam->chann		= DEF_CHANN;

		pCurrParam->format.name	= FORMAT_S32_LE;
		pCurrParam->format.val	= SND_PCM_FORMAT_S32_LE;

		pCurrParam->convert		= CONVERT_FFT;
		pCurrParam->period		= calculate_period( pCurrParam->rate, pCurrParam->buff_resize );

		pCurrParam->max_play_db	= 0;
		pCurrParam->max_rec_db	= 0;

		do_set_default( NULL );
	}

	//-----------------------------------------------------------------------------

	if( nCards )
		pCurrParam = &Parameters[0];
	else
		pCurrParam = NULL;
		
	bApply = FALSE;
}


//=============================================================================
// Menu declaration
//-----------------------------------------------------------------------------
STATIC MENU_FULL( main_menu, "Main menu", clear_screen, 0 )
	ITEM_SIMPLE
		(
			"\n    Curr. | Used | Play (def) | Rec (def) | Available cards"
			"\n----------|------|------------|-----------|-----------------------------------",
			MENU_NULL_SYM
		),
	ITEM_SEPARATOR,
	ITEM_DO(			"Toggle used flag",							'U',	do_set_used		),
	ITEM_DO(			"Set this device as default player",		'P',	do_set_player	),
	ITEM_DO(			"Set this device as default recorder",		'R',	do_set_rec		),
	ITEM_SEPARATOR,
	ITEM_FULL(			"Output device",	'O',	&device_menu,	do_set_hw_out,	title_card_out,	MENU_EMPTY_MISC	),
	ITEM_FULL(			"Input device ",	'I',	&device_menu,	do_set_hw_in,	title_card_in,	MENU_EMPTY_MISC	),
	ITEM_SEPARATOR,
	ITEM_NEXT_TITLE(	"Format",			'F',	&param_menu,	title_param	),
	ITEM_NEXT_TITLE(	"Converter",		'C',	&convert_menu,	title_convert	),
	ITEM_SEPARATOR,
	ITEM_DO_TITLE(		"Show all plugins",	'A',	do_show,		title_show		),
	ITEM_NEXT_TITLE(	"Plug-ins",			'M',	&plug_menu,		title_plug		),
	ITEM_SEPARATOR,
	ITEM_DO(			"Test",				'T',	do_test ),
	ITEM_DO_TITLE(		"Save to",			'S',	do_save,		title_cfg_file	),
	ITEM_DO_TITLE(		"Delete",			'X',	do_delete,		title_cfg_file	),
	ITEM_SEPARATOR,
	ITEM_DO(			"Exit",		KEY_ESC,		do_exit ),
MENU_END


STATIC MENU_FULL( param_menu, "Sound parameters", clear_screen, 0 )
	ITEM_DO_TITLE(		"Sample rate",		'S',	do_set_rate,		title_rate		),
	ITEM_DO_TITLE(		"Channels   ",		'C', 	do_set_channels,	title_channels	),
	ITEM_NEXT_TITLE(	"Format",			'F',	&format_menu,		title_format	),
	ITEM_SEPARATOR,
	ITEM_DO_TITLE(		"Alignment buffer and period",	'A',	do_buffer_align,		title_buffer_align	),
	ITEM_DO_TITLE(		"Set buffer multiplier",		'B',	do_buffer_mul,			title_buffer_mul	),
	ITEM_DO_TITLE(		"Set period multiplier",		'M',	do_period_mul,			title_period_mul	),
	ITEM_SEPARATOR,
	ITEM_DO_TITLE(		"Play ampl.",		'P',	do_set_ampl_play,	title_ampl_play	),
	ITEM_DO_TITLE(		"Rec. ampl.",		'R',	do_set_ampl_rec,	title_ampl_rec	),
	ITEM_SEPARATOR,
	ITEM_RETURN_TO_MAIN( &main_menu ),
MENU_END


STATIC MENU_BEGIN( format_menu, "Available sample formats" )
	ITEM_FULL(	FORMAT_U8,			'0',	&param_menu,	do_set_format,	NULL, MENU_MAKE_MISC( NULL, SND_PCM_FORMAT_U8			)	),
	ITEM_FULL(	FORMAT_S16_LE,		'1',	&param_menu,	do_set_format,	NULL, MENU_MAKE_MISC( NULL, SND_PCM_FORMAT_S16_LE		)	),
	ITEM_FULL(	FORMAT_S24_3LE,		'2',	&param_menu,	do_set_format,	NULL, MENU_MAKE_MISC( NULL, SND_PCM_FORMAT_S24_3LE		)	),
	ITEM_FULL(	FORMAT_S24_LE,		'3',	&param_menu,	do_set_format,	NULL, MENU_MAKE_MISC( NULL, SND_PCM_FORMAT_S24_LE		)	),
	ITEM_FULL(	FORMAT_S32_LE,		'4',	&param_menu,	do_set_format,	NULL, MENU_MAKE_MISC( NULL, SND_PCM_FORMAT_S32_LE		)	),
	ITEM_FULL(	FORMAT_FLOAT_LE,	'5',	&param_menu,	do_set_format,	NULL, MENU_MAKE_MISC( NULL, SND_PCM_FORMAT_FLOAT_LE		)	),
	ITEM_FULL(	FORMAT_FLOAT64_LE,	'6',	&param_menu,	do_set_format,	NULL, MENU_MAKE_MISC( NULL, SND_PCM_FORMAT_FLOAT64_LE	)	),
	ITEM_SEPARATOR,
	ITEM_RETURN_TO_MAIN( &param_menu ),
MENU_END


STATIC MENU_BEGIN( convert_menu, "Available converters" )
	ITEM_NEXT_DO(	CONVERT_FFT,			'0',	&main_menu,		do_set_convert	),
	ITEM_NEXT_DO(	CONVERT_SPEEX,			'1',	&main_menu,		do_set_convert	),
	ITEM_NEXT_DO(	CONVERT_SPEEX_BEST,		'2',	&main_menu,		do_set_convert	),
	ITEM_NEXT_DO(	CONVERT_SAMPLE,			'3',	&main_menu,		do_set_convert	),
	ITEM_NEXT_DO(	CONVERT_SAMPLE_MED,		'4',	&main_menu,		do_set_convert	),
	ITEM_NEXT_DO(	CONVERT_SAMPLE_BEST,	'5',	&main_menu,		do_set_convert	),
	ITEM_NEXT_DO(	CONVERT_SAMPLE_ORDER,	'6',	&main_menu,		do_set_convert	),
	ITEM_NEXT_DO(	CONVERT_SAMPLE_LIN,		'7',	&main_menu,		do_set_convert	),
	ITEM_SEPARATOR,
	ITEM_RETURN_TO_MAIN( &main_menu ),
MENU_END


STATIC MENU_FULL( plug_menu, "Plugins", clear_screen, 0 )
	ITEM_NEXT(		"Templates",		'T',	&templates_menu ),
	ITEM_SEPARATOR,
	ITEM_DO_TITLE(	"Convert",			'C',	do_set_converter,		title_converter	),
	ITEM_DO_TITLE(	"Expand ",			'E',	do_set_expand,			title_expand	),
	ITEM_DO_TITLE(	"Asymmetrical",		'A',	do_set_asym,			title_asym	),
	ITEM_SEPARATOR,
	ITEM_DO_TITLE(	"Soft play volume",	'P',	do_set_soft_play,		title_soft_play	),
	ITEM_DO_TITLE(	"Soft rec. volume",	'R', 	do_set_soft_rec,		title_soft_rec	),
	ITEM_SEPARATOR,
	ITEM_DO_TITLE(	"DMix  ",			'M', 	do_set_dmix,			title_dmix		),
	ITEM_DO_TITLE(	"DSnoop",			'S', 	do_set_dsnoop,			title_dsnoop	),
	ITEM_SEPARATOR,
	ITEM_DO_TITLE(	"Phonon",			'K', 	do_set_phonon,			title_phonon	),
	ITEM_DO_TITLE(	"Normalizator",		'N', 	do_set_norm,			title_norm	),
	ITEM_SEPARATOR,
	ITEM_RETURN_TO_MAIN( &main_menu ),
MENU_END


STATIC MENU_BEGIN( templates_menu, "Plugin templates" )
	ITEM_NEXT_DO(	"Default settings",	'D',	&plug_menu,	do_set_default	),
	ITEM_NEXT_DO(	"Software card",	'S',	&plug_menu,	do_set_software	),
	ITEM_NEXT_DO(	"Hardware card",	'H',	&plug_menu,	do_set_hadrware	),
	ITEM_SEPARATOR,
	ITEM_RETURN_TO_PARENT( &plug_menu ),
MENU_END


//=============================================================================
// Main
//-----------------------------------------------------------------------------
INT main( INT nargs, CHAR *argv[] )
{
	INT n_help_layer	= 0;
	INT long_idx;
	INT cnt;

	//-------------------------------------------------------------------------

	arrzero( Cards, MAX_CARDS );
	arrzero( Parameters, MAX_CARDS );
	arrzero( Devices, MAX_DEVICE );
	
	//-------------------------------------------------------------------------

	opterr = 0;
	
	while( ( cnt = getopt_long( nargs, argv, "h", g_LongOpt, &long_idx ) ) != -1 )
	{
		switch( cnt )
		{
		case 'h':
			n_help_layer	= max( n_help_layer, 2 );
			break;

		case 0:
			switch( long_idx )
			{
			case 0: // help
				n_help_layer	= max( n_help_layer, 2 );
				break;
			}
			break;
		}
	}
	
	//-------------------------------------------------------------------------
	
	cmdline_print_help( g_szHelp, argv[0], n_help_layer );
	
	if( n_help_layer )
		return 1;

	//-------------------------------------------------------------------------

	init();
	menu_parse( &main_menu );

	return 0;
}
