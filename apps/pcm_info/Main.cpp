//=============================================================================
// Includes
//-----------------------------------------------------------------------------
#include <stdio.h>
#include "../../lib/swin.h"

#if defined(IS_WIN32)
#include <Windows.h>
#endif

#include "../../lib/types.h"
#include "../../lib/array.h"
#include "../../lib/getoptw.h"
#include "../../lib/riffio.h"
#include "../../lib/cmdline.h"
#include "../../lib/str.h"

#include "../pcm_common/Common.h"


//=============================================================================
// ROM
//-----------------------------------------------------------------------------
STATIC HELPTXT_BEGIN( g_szHelp )
	HELPTXT_ITEM( 1, "Usage: " SLFNAME " <file> [--info]" ),
HELPTXT_END


STATIC GETOPT_BEGIN( g_LongOpt )
	GETOPT_ITEM_SIMPLE( "info" ),
GETOPT_END


//=============================================================================
// Get module information
//-----------------------------------------------------------------------------
LIBINFO_FUNCTION
(
	pcm_info,
	"WAVE Info",
	"Get information abount WAVE PCM file",
	"Copyright (c) 2011 PetrovSE",
	"1.0.0.4"
)


STATIC LIBINFO_POINTER pFnInfos[] =
{
	pcm_info_get_info,
	riffio_get_info,
	cmdline_get_info,
};


//=============================================================================
// Main
//-----------------------------------------------------------------------------
INT main( INT nArg, CHAR *pszArgs[] )
{
	CONST CHAR *szFileName	= NULL;
	PWAVEFORMATEX	pFormat	= NULL;
	HWAVEIO			hFile	= NULL;

	INT	nOfInfos	= 0;
	INT	nHelpLayer	= 1;

	INT nLongIdx;
	INT nCnt;

	opterr = 0;

	//-----------------------------------------------------------------------------

	while( ( nCnt = getopt_long( nArg, pszArgs, "", g_LongOpt, &nLongIdx ) ) != -1 )
	{
		switch( nCnt )
		{
		case 0:
			switch( nLongIdx )
			{
			case 0: // info
				nOfInfos = sizeof( pFnInfos ) / sizeof( *pFnInfos );
				break;
			}
			break;
		}
	}

	//-----------------------------------------------------------------------------

	if( optind < nArg )
	{
		szFileName = pszArgs[optind];
	}

	//-------------------------------------------------------------------------

	if( szFileName )
	{
		nOfInfos	= max( nOfInfos, 1 );
		nHelpLayer	= 0;
	}

	cmdline_print_infos( pFnInfos, nOfInfos );
	cmdline_print_help( g_szHelp, pszArgs[0], nHelpLayer );

	if( nHelpLayer )
	{
		return 1;
	}

	//-------------------------------------------------------------------------

	while( 1 )
	{
		CHAR  szNofSamples[MAX_STRING];
		DATA  tFileDuration;
		INT   nFileDuration;
		
		DWORD dwMask;
		DWORD dwSize;

		//-------------------------------------------------------------------------

		printf( "Opening file \"%s\" ... ", szFileName );

		hFile = waveio_open( szFileName, WAVE_IO_READ );
		if( hFile == NULL )
		{
			printf( "Failed!\n" );
			break;
		}

		//-------------------------------------------------------------------------

		dwSize	= waveio_find_chunk( hFile, FCC_FMT );
		pFormat	= mmsys_format_alloc( dwSize );

		if( pFormat == NULL )
		{
			printf( "No format!\n" );
			break;
		}

		if( waveio_read( hFile, (PBYTE)pFormat, dwSize ) != dwSize )
		{
			printf( "I/O file error!\n" );
			break;
		}

		//-------------------------------------------------------------------------

		dwSize	= waveio_find_chunk( hFile, FCC_DATA );
		
		//-------------------------------------------------------------------------

		if( !mmsys_format_is_pcm( pFormat ) )
		{
			printf( "Invalid input format!\n" );
			break;
		}

		mmsys_format_complete( pFormat );
		printf( "Ok.\n\n" );

		//-------------------------------------------------------------------------

		PrintWaveInfo( pFormat, "Parameters:" );

		printf
			(
				"Extensible format: %s\n\n",
				bool_box( mmsys_format_info( pFormat, 'T' ) == WAVE_FORMAT_EXTENSIBLE, BOOL_BOX_ONOFF )
			);

		//-------------------------------------------------------------------------

		dwMask = mmsys_format_info( pFormat, 'm' );

		printf( "Channel mask: %06x hex, %d dec\n", (INT)dwMask, (INT)dwMask );
		printf( "Channels    : " );

		for( nCnt = 0 ; ; nCnt ++ )
		{
			CONST CHANNELDESC *pInfo = mmsys_channel_desc( dwMask, NULL, nCnt );
			if( pInfo == NULL )
			{
				break;
			}

			if( nCnt )
			{
				printf( ", " );
			}

			printf( "%s", pInfo->szShortName );
		}
		printf( "\n\n" );

		//-------------------------------------------------------------------------

		number_to_str( dwSize / pFormat->nBlockAlign, szNofSamples, MAX_STRING );
		
		tFileDuration = (DATA)dwSize / pFormat->nAvgBytesPerSec;
		nFileDuration = (INT)tFileDuration / 60;

		printf( "Samples : %s\n", szNofSamples ); 
		printf
			(
				"Duration: %d min %f sec\n",
				nFileDuration,
				tFileDuration - D( 60.0 ) * nFileDuration
			); 

		//-------------------------------------------------------------------------

		printf( "\n" );
		break;
	}

	//-------------------------------------------------------------------------

	waveio_close( hFile );
	mmsys_format_free( pFormat );

	return 0;
}
