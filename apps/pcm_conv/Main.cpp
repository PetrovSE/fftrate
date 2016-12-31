//=============================================================================
// Includes
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "../../lib/swin.h"

#if defined(IS_WIN32)
#include <Windows.h>
#endif

#include "../../lib/types.h"
#include "../../lib/getoptw.h"
#include "../../lib/convert.h"
#include "../../lib/riffio.h"
#include "../../lib/fft.h"
#include "../../lib/str.h"
#include "../../lib/array.h"
#include "../../lib/cmdline.h"
#include "../../lib/thr.h"
#include "../../lib/clock.h"
#include "../../lib/profile.h"

#include "../pcm_common/Common.h"
#include "../pcm_common/InfoConv.h"


#define PROF_EXTERN	"fft/proftodo.inc"
#include "../../lib/profdata.h"

#define PROF_EXTERN	"convert/proftodo.inc"
#include "../../lib/profdata.h"


//=============================================================================
// Definition
//-----------------------------------------------------------------------------
#define DEFAULT_OUTPUT		"convert.wav"
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
	HELPTXT_ITEM( 1, "Usage: " SLFNAME " <input file> [output file]"								),
	HELPTXT_ITEM( 1, "               [-c number of channels]"									),
	HELPTXT_ITEM( 3, "               [-m mask] [-n on|off]"										),
	HELPTXT_ITEM( 3, "               [-I input order] [-O output order]"						),
	HELPTXT_ITEM( 1, "               [-b number of bits] [-f frequency] "						),
	HELPTXT_ITEM( 3, "               [-T trans] [-W win]"										),
	HELPTXT_ITEM( 1, "               [-v] [--info] [-h|--help|--help-all]"						),
	HELPTXT_ITEM( 2, ""																			),
	HELPTXT_ITEM( 2, "Options:"																	),
	HELPTXT_ITEM( 2, " input file    Input WAVE file"											),
	HELPTXT_ITEM( 2, " output file   Output WAVE file (by default is \"convert.wav\")"			),
	HELPTXT_ITEM( 3, ""																			),
	HELPTXT_ITEM( 3, " -c            Number of output channels:"								),
	HELPTXT_ITEM( 3, "               1 - mono"													),
	HELPTXT_ITEM( 3, "               2 - stereo"												),
	HELPTXT_ITEM( 3, "               3 - 2/1 surround"											),
	HELPTXT_ITEM( 3, "               4 - 2/2 quadro"											),
	HELPTXT_ITEM( 3, "               5 - 3/2 surround"											),
	HELPTXT_ITEM( 3, "               6 - 3/2+SW dolby"											),
	HELPTXT_ITEM( 3, " -m            Channel mask (may be conflict with '-c')"					),
	HELPTXT_ITEM( 3, ""																			),
	HELPTXT_ITEM( 3, " -n            Normalize channel matrix (by default on)"					),
	HELPTXT_ITEM( 3, " -I            Input channel order (see below)"							),
	HELPTXT_ITEM( 3, " -O            Output channel order (see below)"							),
	HELPTXT_ITEM( 3, ""																			),
	HELPTXT_ITEM( 3, "Available channels:"														),
	HELPTXT_ITEM( 3, " MS  : " SPORDER_MS														),
	HELPTXT_ITEM( 3, " ALSA: " SPORDER_ALSA														),
	HELPTXT_ITEM( 3, " OSS : " SPORDER_OSS														),
	HELPTXT_ITEM( 2, ""																			),
	HELPTXT_ITEM( 2, " -b            Number of output bits: 8, 16 .. 32, 32f or 64f"			),
	HELPTXT_ITEM( 2, " -f            Output sample rate (in Hz). Tested: from 6000 to 192000"	),
	HELPTXT_ITEM( 3, ""																			),
	HELPTXT_ITEM( 3, " -T            Transform type: " SZ_DCT ", " SZ_FFT " (default: " SZ_DCT ")"			),
	HELPTXT_ITEM( 3, " -W            Window type: " SZ_VORBIS ", " SZ_HANNING ", " SZ_RECT " (default: " SZ_VORBIS ")"	),
	HELPTXT_ITEM( 2, ""																			),
	HELPTXT_ITEM( 2, " -v            Show verbose information"									),
	HELPTXT_ITEM( 2, " --info        Show wide modules information"								),
	HELPTXT_ITEM( 2, " -h, --help    Show this message"											),
	HELPTXT_ITEM( 2, " --help-all    Show all help info"										),
HELPTXT_END


STATIC GETOPT_BEGIN( g_LongOpt )
	GETOPT_ITEM_SIMPLE(	"info"			),
	GETOPT_ITEM_SYM(	"help",		'h' ),
	GETOPT_ITEM_SIMPLE(	"help-all"		),
GETOPT_END


//=============================================================================
// Get module information
//-----------------------------------------------------------------------------
LIBINFO_FUNCTION
(
	pcm_conv,
	"WAVE PCM Converter",
	"Convert PCM format for WAVE files",
	"Copyright (c) 2009-11 PetrovSE",
	"1.0.4.0"
)

	
STATIC LIBINFO_POINTER pFnInfos[] =
{
	pcm_conv_get_info,
	convert_get_info,
	riffio_get_info,
	fft_get_info,
	profiler_get_info,
	cmdline_get_info,
};


//=============================================================================
// Main
//-----------------------------------------------------------------------------
INT main( INT nArg, CHAR *pszArgs[] )
{
	PWAVEFORMATEX pWaveHdr		= NULL;
	PWAVEFORMATEX pOutFormat	= NULL;

	CONST CHAR *szInName		= NULL;
	CONST CHAR *szOutName		= NULL;

	HWAVEIO	hFileIn		= NULL;
	HWAVEIO	hFileOut	= NULL;

	INT  nOfInfos	= 0;
	INT  nHelpLayer	= 1;

	DWORD dwTransform	= CONV_TRANSFORM_DCT;
	DWORD dwWindow		= CONV_WINDOW_VORBIS;

	INT  nChannels	= 0;
	INT  nBits		= 0;
	INT  Frequency	= 0;

	DWORD dwChMask	= 0;
	BOOL  bNorm		= TRUE;

	CONST CHAR *szOrderIn	= NULL;
	CONST CHAR *szOrderOut	= NULL;
	
	BOOL bFloat		= FALSE;
	BOOL bInfo		= FALSE;

	INT nLongIdx;
	INT nCnt;

	//-------------------------------------------------------------------------

	opterr = 0;

	while( ( nCnt = getopt_long( nArg, pszArgs, "c:m:n:I:O:b:f:T:W:vh", g_LongOpt, &nLongIdx ) ) != -1 )
	{
		switch( nCnt )
		{
		case 'c':
			nChannels = atoi( optarg );
			break;

		case 'm':
			dwChMask = atoi( optarg );
			break;

		case 'n':
			bNorm = strcasecmp( "on", optarg ) == 0;
			break;

		case 'I':
			szOrderIn = optarg;
			break;
			
		case 'O':
			szOrderOut = optarg;
			break;

		case 'b':
			nBits  = atoi( optarg );
			bFloat = strstr( optarg, "f" ) != NULL;
			break;

		case 'f':
			Frequency = atoi( optarg );
			break;

		case 'T':
			if( strcasecmp( SZ_FFT, optarg ) == 0 )
				dwTransform = CONV_TRANSFORM_FFT;

			if( strcasecmp( SZ_DCT, optarg ) == 0 )
				dwTransform = CONV_TRANSFORM_DCT;
			break;

		case 'W':
			if( strcasecmp( SZ_HANNING, optarg ) == 0 )
				dwWindow = CONV_WINDOW_SIN;

			if( strcasecmp( SZ_VORBIS, optarg ) == 0 )
				dwWindow = CONV_WINDOW_VORBIS;

			if( strcasecmp( SZ_RECT, optarg ) == 0 )
				dwWindow = CONV_WINDOW_NONE;
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
			case 0: // info
				nOfInfos = sizeof( pFnInfos ) / sizeof( *pFnInfos );
				break;

			case 1: // help
				nHelpLayer = max( nHelpLayer, 2 );
				break;

			case 2: // help-all
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
	{
		szOutName = (CHAR *)DEFAULT_OUTPUT;
	}

	//-------------------------------------------------------------------------

	if( szInName )
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

	profiler_add( ProfPointsFFT );
	profiler_add( ProfPointsConvert );

	profiler_reset();

	//-------------------------------------------------------------------------

	while( 1 )
	{
		DATA	tFullTime = (DATA)thr_time();
		DATA	tFileDuration;

		HCONVERT hInst = NULL;

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

		pOutFormat = mmsys_format_dublicate( pWaveHdr );
		if( pOutFormat == NULL )
		{
			printf( "Failed to format dublication!\n" );
			break;
		}

		if( Frequency )
		{
			pOutFormat = mmsys_format_pcm_change( pOutFormat, "r", Frequency );
		}

		if( nBits || bFloat )
		{
			pOutFormat = mmsys_format_pcm_change( pOutFormat, "bt", nBits, bFloat ? WAVE_FORMAT_IEEE_FLOAT : WAVE_FORMAT_PCM );
		}

		if( nChannels )
		{
			pOutFormat = mmsys_format_pcm_change( pOutFormat, "c", nChannels );
		}

		if( dwChMask )
		{
			pOutFormat = mmsys_format_pcm_change( pOutFormat, "m", dwChMask );
		}

		mmsys_format_complete( pOutFormat );

		//-------------------------------------------------------------------------

		hInst = convert_open( pWaveHdr, pOutFormat, "twnio", dwTransform, dwWindow, bNorm, szOrderIn, szOrderOut );

		if( hInst == NULL )
		{
			printf( "Failed to open converter!\n" );
			break;
		}

		//-------------------------------------------------------------------------

		convert_reset( hInst );

		//-------------------------------------------------------------------------

		printf( "Output file: %s\n", szOutName );

		hFileOut = waveio_open( szOutName, WAVE_IO_WRITE );
		if( hFileOut == NULL )
		{
			printf( "Invalid output wave file!\n" );
			break;
		}

		//-------------------------------------------------------------------------

		pWaveHdr = mmsys_format_free( pWaveHdr );
		pWaveHdr = mmsys_format_dublicate( pOutFormat );

		waveio_store_chunk( hFileOut, FCC_FMT );
		waveio_write( hFileOut, (PBYTE)pWaveHdr, mmsys_format_info( pWaveHdr, 's' ) );

		PrintWaveInfo( pWaveHdr, NULL );

		//-------------------------------------------------------------------------

		if( bInfo )
		{
			PrintConvMatrix( hInst );
			PrintConvInfo( hInst );
		}

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

			convert_processing
				(
					hInst,

					pInBuffer,
					&dwLoad,

					pOutBuffer,
					&dwSave
				);

			if( is_zero( dwLoad ) && is_zero( dwSave ) )
			{
				break;
			}

			//-------------------------------------------------------------------------

			dwRem -= dwLoad;
			arrcpy( &pInBuffer[0], &pInBuffer[dwLoad], dwRem );

			dwSaved += waveio_write( hFileOut, pOutBuffer, dwSave );
		}

		cmdline_progress_done( NULL );

		//-------------------------------------------------------------------------

		convert_close( hInst );

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
	mmsys_format_free( pOutFormat );

	waveio_close( hFileIn );
	waveio_close( hFileOut );

	return 0;
}
