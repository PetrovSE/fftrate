//=============================================================================
// Includes
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "swin.h"

#if defined(IS_WIN32)
#include <Windows.h>
#endif

#include "types.h"
#include "getoptw.h"
#include "stretch.h"
#include "waveio.h"
#include "fft.h"
#include "str.h"
#include "array.h"
#include "cmdline.h"
#include "thr.h"
#include "clock.h"
#include "profile.h"

#include "../pcm_common/Common.h"

#define PROF_EXTERN	"fft/proftodo.inc"
#include "profdata.h"


//=============================================================================
// Definition
//-----------------------------------------------------------------------------
#define DEFAULT_OUTPUT		"stretch.wav"
#define BUFFER_SIZE			65636L


//=============================================================================
// RAM
//-----------------------------------------------------------------------------
STATIC BYTE pInBuffer[BUFFER_SIZE];
STATIC BYTE pOutBuffer[BUFFER_SIZE];


//=============================================================================
// ROM
//-----------------------------------------------------------------------------
STATIC HELPTXT_BEGIN( g_szHelp )
	HELPTXT_ITEM( 1, "Usage: " SLFNAME " <input file> [output file] [-s stretch]"				),
	HELPTXT_ITEM( 2, ""																			),
	HELPTXT_ITEM( 2, "Options:"																	),
	HELPTXT_ITEM( 2, " input file    Input WAVE file"											),
	HELPTXT_ITEM( 2, " output file   Output WAVE file (by default is \"stretch.wav\")"			),
	HELPTXT_ITEM( 3, ""																			),
	HELPTXT_ITEM( 3, " -s            Stretch coef"												),
	HELPTXT_ITEM( 3, ""																			),
	HELPTXT_ITEM( 2, " -v            Show verbose information"									),
	HELPTXT_ITEM( 2, " -h, --help    Show this message"											),
	HELPTXT_ITEM( 2, " --help-all    Show all help info"										),
HELPTXT_END


STATIC GETOPT_BEGIN( g_LongOpt )
	GETOPT_ITEM_SYM(	"help",		'h' ),
	GETOPT_ITEM_SIMPLE(	"help-all"		),
GETOPT_END


//=============================================================================
// Main
//-----------------------------------------------------------------------------
INT main( INT nArg, CHAR *pszArgs[] )
{
	PWAVEFORMATEX pWaveHdr		= NULL;

	CONST CHAR *szInName		= NULL;
	CONST CHAR *szOutName		= NULL;

	HWAVEIO	hFileIn		= NULL;
	HWAVEIO	hFileOut	= NULL;

	DATA fStretchCoef = 1.0;
	INT  nHelpLayer	= 1;

	BOOL bInfo		= FALSE;

	INT nLongIdx;
	INT nCnt;

	//-------------------------------------------------------------------------

	opterr = 0;

	while( ( nCnt = getopt_long( nArg, pszArgs, "s:vh", g_LongOpt, &nLongIdx ) ) != -1 )
	{
		switch( nCnt )
		{
		case 's':
			fStretchCoef = atof( optarg );
			break;

		case 'v':
			bInfo = TRUE;
			break;

		case 'h':
			nHelpLayer = max( nHelpLayer, 2 );
			break;

		case 0:
			switch( nLongIdx )
			{
			case 0: // help
				nHelpLayer = max( nHelpLayer, 2 );
				break;

			case 1: // help-all
				nHelpLayer = max( nHelpLayer, 3 );
				break;
			}
			break;
		}
	}

	//-------------------------------------------------------------------------

	for( ; optind < nArg ; optind ++ )
	{
		if( szInName == NULL )
		{
			szInName = pszArgs[optind];
			continue;
		}

		if( szOutName == NULL )
		{
			szOutName = pszArgs[optind];
			continue;
		}

		break;
	}

	if( szOutName == NULL )
		szOutName = (CHAR *)DEFAULT_OUTPUT;

	//-------------------------------------------------------------------------

	if( szInName )
		nHelpLayer = 0;

	cmdline_print_help( g_szHelp, pszArgs[0], nHelpLayer );

	if( nHelpLayer )
		return 1;

	//-------------------------------------------------------------------------

	profiler_add( ProfPointsFFT );

	profiler_reset();

	//-------------------------------------------------------------------------

	while( 1 )
	{
		DATA	tFullTime = (DATA)thr_time();
		DATA	tFileDuration;

		HSTRETCH hInst = NULL;

		DWORD dwSize;
		DWORD dwRem;

		DWORD dwProc;
		DWORD dwSaved;
		DWORD dwSrcAvgBytes;

		//-------------------------------------------------------------------------

		printf( "Input file : %s\n", szInName );

		hFileIn = waveio_open( szInName, WAVE_IO_READ );
		if( hFileIn == NULL )
		{
			printf( "Invalid input wave file!\n" );
			break;
		}

		//-------------------------------------------------------------------------

		dwSize   = waveio_find_chunk( hFileIn, FCC_FMT );
		pWaveHdr = mmsys_format_alloc( dwSize );

		if( pWaveHdr == NULL )
		{
			printf( "No format!\n" );
			break;
		}

		if( waveio_read( hFileIn, (PBYTE)pWaveHdr, dwSize ) != dwSize )
		{
			printf( "I/O file error!\n" );
			break;
		}

		if( !mmsys_format_is_pcm( pWaveHdr ) )
		{
			printf( "Invalid input format!\n" );
			break;
		}

		mmsys_format_complete( pWaveHdr );

		//-------------------------------------------------------------------------

		dwSrcAvgBytes = pWaveHdr->nAvgBytesPerSec;

		PrintWaveInfo( pWaveHdr, NULL );

		dwSize = waveio_find_chunk( hFileIn, FCC_DATA );
		if( dwSize == INVALID_SIZE )
		{
			printf( "No data!\n" );
			break;
		}

		//-------------------------------------------------------------------------

		hInst = stretch_open( pWaveHdr, fStretchCoef );

		if( hInst == NULL )
		{
			printf( "Failed to open converter!\n" );
			break;
		}

		//-------------------------------------------------------------------------

		stretch_reset( hInst );

		//-------------------------------------------------------------------------

		printf( "Output file: %s\n", szOutName );

		hFileOut = waveio_open( szOutName, WAVE_IO_WRITE );
		if( hFileOut == NULL )
		{
			printf( "Invalid output wave file!\n" );
			break;
		}

		//-------------------------------------------------------------------------

		waveio_store_chunk( hFileOut, FCC_FMT );
		waveio_write( hFileOut, (PBYTE)pWaveHdr, mmsys_format_info( pWaveHdr, 's' ) );

		PrintWaveInfo( pWaveHdr, NULL );

		//-------------------------------------------------------------------------

		dwRem   = 0;
		dwProc  = 0;
		dwSaved = 0;

		waveio_store_chunk( hFileOut, FCC_DATA );
		
		printf( "\n" );
		cmdline_progress_init( PROGRESS_FILL_PROGR );

		while( 1 )
		{
			DWORD dwLoad = waveio_read( hFileIn, &pInBuffer[dwRem], BUFFER_SIZE - dwRem );
			DWORD dwSave;

			dwRem  += dwLoad;
			dwProc += dwLoad;

			cmdline_progress_show( dwProc, dwSize, NULL );

			//-------------------------------------------------------------------------

			dwLoad = dwRem;
			dwSave = BUFFER_SIZE;

			stretch_processing
				(
					hInst,

					pInBuffer,
					&dwLoad,

					pOutBuffer,
					&dwSave
				);

			if( is_zero( dwLoad ) && is_zero( dwSave ) )
				break;

			//-------------------------------------------------------------------------

			dwRem -= dwLoad;
			arrcpy( &pInBuffer[0], &pInBuffer[dwLoad], dwRem );

			dwSaved += waveio_write( hFileOut, pOutBuffer, dwSave );
		}

		cmdline_progress_done( NULL );

		//-------------------------------------------------------------------------

		stretch_close( hInst );

		//-------------------------------------------------------------------------

		tFileDuration	= (DATA)dwSize / (DATA)dwSrcAvgBytes;
		tFullTime		= ( (DATA)thr_time() - tFullTime ) / D( TIME_MSEC );


		printf( "\nFile duration  : %f sec\n", tFileDuration );
		printf
			(
				"Processing time: %f sec (%5.2f%% of real time)\n",
				(DOUBLE)tFullTime,
				(DOUBLE)100.0 * (DOUBLE)tFullTime / tFileDuration
			);

		profiler_print_info( stderr, (DATA)tFileDuration );
		break;
	}

	//-------------------------------------------------------------------------

	mmsys_format_free( pWaveHdr );

	waveio_close( hFileIn );
	waveio_close( hFileOut );

	return 0;
}
