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
#include "../../lib/str.h"
#include "../../lib/getoptw.h"
#include "../../lib/convert.h"
#include "../../lib/riffio.h"
#include "../../lib/fft.h"
#include "../../lib/cmdline.h"
#include "../../lib/mathex.h"

#include "../pcm_common/Common.h"


//=============================================================================
// Definition
//-----------------------------------------------------------------------------
#define BUFFER_LEN				65536L
#define BUFFER_INP				( BUFFER_LEN * sizeof(BYTE) )
#define BUFFER_OUT				( BUFFER_LEN * sizeof(DATA) )


//=============================================================================
// RAM
//-----------------------------------------------------------------------------
STATIC BYTE		g_BufferIn[2][BUFFER_INP];
STATIC BYTE		g_BufferOut[2][BUFFER_OUT];
STATIC BYTE		g_BufferResid[BUFFER_INP];


//=============================================================================
// ROM
//-----------------------------------------------------------------------------
STATIC HELPTXT_BEGIN( g_szHelp )
	HELPTXT_ITEM( 1, "Usage: " SLFNAME " <file #1> <file #2> [<resid>] [-b count] [-e count]"			),
	HELPTXT_ITEM( 1, "               [--info] [-h|--help]"											),
	HELPTXT_ITEM( 2, ""																				),
	HELPTXT_ITEM( 2, "Options:"																		),
	HELPTXT_ITEM( 2, " file #1       First WAVE file"												),
	HELPTXT_ITEM( 2, " file #2       Second WAVE file"												),
	HELPTXT_ITEM( 2, " resid         Residual WAVE file"											),
	HELPTXT_ITEM( 2, ""																				),
	HELPTXT_ITEM( 2, " -b            Fragment begining (in samples)"								),
	HELPTXT_ITEM( 2, " -e            Fragment end (in samples)"										),
	HELPTXT_ITEM( 2, ""																				),
	HELPTXT_ITEM( 2, " --info        Show wide modules information"									),
	HELPTXT_ITEM( 2, " -h, --help    Show this message"												),
HELPTXT_END


STATIC GETOPT_BEGIN( g_LongOpt )
	GETOPT_ITEM_SIMPLE(	"info"			),
	GETOPT_ITEM_SYM(	"help",		'h' ),
GETOPT_END


//=============================================================================
// Get module information
//-----------------------------------------------------------------------------
LIBINFO_FUNCTION
(
	pcm_mse,
	"MSE calculation",
	"Mean Square Error calculation for WAVE file",
	"Copyright (c) 2009-12 PetrovSE",
	"1.0.4.0"
)


STATIC LIBINFO_POINTER pFnInfos[] =
{
	pcm_mse_get_info,
	convert_get_info,
	mathex_get_info,
	riffio_get_info,
	cmdline_get_info,
};


//=============================================================================
// Local types
//-----------------------------------------------------------------------------
typedef union
{
	DATA	dataVal;
	
	DOUBLE	doubleVal;
	FLOAT	floatVal;
	
	LONG	longVal;
	SHORT	shortVal;
	BYTE	byteVal;
	
} SIMPLETYPE;


typedef struct
{
	CONST CHAR		*szNames;
	PWAVEFORMATEX	pFormat;
	HWAVEIO			hFiles;
	HCONVERT		hConv;
	
	BYTE	*pBufferIn;
	BYTE	*pBufferOut;
	
	DWORD	dwRemIn;
	DWORD	dwRemOut;
	
} WAVEFILE;


//=============================================================================
// Common
//-----------------------------------------------------------------------------
static void print_value( HCONVERT hConv, BOOL bFloatData, WORD nBits, COMBOTYPE uVal )
{
	SIMPLETYPE inVal[N_OF_MAX_CHANNELS];
	SIMPLETYPE outVal[N_OF_MAX_CHANNELS];
	
	DWORD dwIn;

	arrzero( inVal, N_OF_MAX_CHANNELS );
	arrzero( outVal, N_OF_MAX_CHANNELS );
	
	//-----------------------------------------------------------------------------

	if( bFloatData )
	{
		inVal[0].dataVal = uVal.data;
		dwIn = sizeof(DATA);
	}
	else
	{
		inVal[0].longVal = uVal.long_int;
		dwIn = sizeof(LONG);
	}

	dwIn *= N_OF_MAX_CHANNELS;

	//-----------------------------------------------------------------------------
	
	if( hConv )
	{
		DWORD dwOut = sizeof(outVal);
		
		convert_reset( hConv );
		convert_processing( hConv, (CONST BYTE *)inVal, &dwIn, (PBYTE )outVal, &dwOut );
	}
	
	//-----------------------------------------------------------------------------
	
	if( bFloatData )
	{
		switch( nBits )
		{
		case 32:
			printf( "%g", outVal[0].floatVal );
			break;
			
		case 64:
			printf( "%g", outVal[0].doubleVal );
			break;
		}
	}
	else
	{
		switch( nBits )
		{
		case 8:
			printf( "%d", (INT)outVal[0].byteVal );
			break;
			
		case 16:
			printf( "%d", (INT)outVal[0].shortVal );
			break;
			
		default:
			printf( "%d", (INT)outVal[0].longVal );
			break;
		}
	}
}


static void print_diff( HCONVERT hConv, HMSE hMSE, BOOL bFloatData, WORD nBits, BOOL bFirst, DWORD nStart )
{
	DWORD nPos = mse_get_cnt( hMSE, bFirst ? MSE_CNT_DIFF_FIRST : MSE_CNT_DIFF_MAX );

	if( nPos == INVALID_SIZE )
	{
		printf( "none\n" );
	}
	else
	{
		print_value
			(
				hConv,
				bFloatData,
				nBits, 
				mse_get_val( hMSE, bFirst ? MSE_VAL_DIFF_FIRST : MSE_VAL_DIFF_MAX )
			);

		printf
			(
				" on position %d (%d in fragment)\n",
				(INT)( nPos + nStart ),
				(INT)nPos
			);
	}
}


//=============================================================================
// Wave file functions
//-----------------------------------------------------------------------------
static void wavefile_reset( WAVEFILE *pFile, BYTE *pBuffIn, BYTE *pBuffOut )
{
	pFile->szNames	= NULL;
	pFile->pFormat	= NULL;
	pFile->hFiles	= NULL;
	pFile->hConv	= NULL;
	
	pFile->dwRemIn	= 0;
	pFile->dwRemOut	= 0;
	
	pFile->pBufferIn	= pBuffIn;
	pFile->pBufferOut	= pBuffOut;
}


static void wavefile_free( WAVEFILE *pFile )
{
	waveio_close( pFile->hFiles );
	convert_close( pFile->hConv );
	mmsys_format_free( pFile->pFormat );
}


static BOOL wavefile_open( WAVEFILE *pFile )
{
	DWORD dwSize;

	//-------------------------------------------------------------------------

	printf( "Opening file \'%s\' ... ", pFile->szNames );
	
	pFile->hFiles = waveio_open( pFile->szNames, WAVE_IO_READ );
	if( pFile->hFiles == NULL )
	{
		printf( "Failed!\n" );
		return FALSE;
	}
	
	//-------------------------------------------------------------------------
	
	dwSize = waveio_find_chunk( pFile->hFiles, FCC_FMT );
	
	pFile->pFormat = mmsys_format_alloc( dwSize );
	if( pFile->pFormat == NULL )
	{
		printf( "No format!\n" );
		return FALSE;
	}
	
	if( waveio_read( pFile->hFiles, (PBYTE)pFile->pFormat, dwSize ) != dwSize )
	{
		printf( "I/O file error!\n" );
		return FALSE;
	}
	
	if( !mmsys_format_is_pcm( pFile->pFormat ) )
	{
		printf( "Invalid input format!\n" );
		return FALSE;
	}
	
	mmsys_format_complete( pFile->pFormat );
	PrintWaveInfo( pFile->pFormat, "Ok." );
	
	return TRUE;
}


static DWORD wavefile_init( WAVEFILE *pFile, PWAVEFORMATEX pOutFormat )
{
	DWORD dwSize;

	pFile->hConv = convert_open( pFile->pFormat, pOutFormat, NULL );
	if( pFile->hConv == NULL )
	{
		printf( "Converter failed!\n" );
		return INVALID_SIZE;
	}
	
	//-------------------------------------------------------------------------
	
	dwSize = waveio_find_chunk( pFile->hFiles, FCC_DATA );
	if( dwSize == INVALID_SIZE )
	{
		printf( "No data!\n" );
		return INVALID_SIZE;
	}
	
	return dwSize / pFile->pFormat->nBlockAlign;
}


static BOOL wavefile_seek( WAVEFILE *pFile, DWORD dwSeek )
{
	dwSeek *= pFile->pFormat->nBlockAlign;
	return waveio_seek( pFile->hFiles, dwSeek, SEEK_CUR ) == dwSeek;
}


static void wavefile_load( WAVEFILE *pFile )
{
	DWORD dwOut;
	DWORD dwInp;
	
	pFile->dwRemIn +=
		waveio_read
			(
				pFile->hFiles,
				&pFile->pBufferIn[pFile->dwRemIn],
				BUFFER_INP - pFile->dwRemIn
			);
			
	dwInp = pFile->dwRemIn;
	dwOut = BUFFER_OUT - pFile->dwRemOut;

	convert_processing
		(
			pFile->hConv,
			pFile->pBufferIn,
			&dwInp,
			&pFile->pBufferOut[pFile->dwRemOut],
			&dwOut
		);
				
	pFile->dwRemIn  -= dwInp;
	pFile->dwRemOut += dwOut;

	arrcpy( &pFile->pBufferIn[0], &pFile->pBufferIn[dwInp], pFile->dwRemIn );
}


static void wavefile_flush( WAVEFILE *pFile, DWORD dwDone )
{
	pFile->dwRemOut -= dwDone;
	arrcpy( pFile->pBufferOut, &pFile->pBufferOut[dwDone], pFile->dwRemOut );
}


//=============================================================================
// Main
//-----------------------------------------------------------------------------
INT main( INT nArg, CHAR *pszArgs[] )
{
	PWAVEFORMATEX pGlobalFormat = NULL;
	HMSE hMSE[N_OF_MAX_CHANNELS];

	WAVEFILE InputFiles[2];
	WAVEFILE ResidFile;

	BOOL bFloatData = FALSE;
	WORD nBits		= 0;

	INT		nChann	= 0;
	DWORD	dwChannMask;

	DWORD	dwStart	= 0;
	DWORD	dwStop	= 0;

	INT	nOfInfos	= 0;
	INT	nHelpLayer	= 1;

	INT nLongIdx;
	INT nCnt;

	//-------------------------------------------------------------------------

	for( nCnt = 0 ; nCnt < 2 ; nCnt ++ )
	{
		wavefile_reset( &InputFiles[nCnt], g_BufferIn[nCnt], g_BufferOut[nCnt] );
	}
	
	wavefile_reset( &ResidFile, g_BufferOut[0], g_BufferResid );

	//-------------------------------------------------------------------------

	opterr = 0;
	
	while( ( nCnt = getopt_long( nArg, pszArgs, "b:e:h", g_LongOpt, &nLongIdx ) ) != -1 )
	{
		switch( nCnt )
		{
		case 'b':
			dwStart	= atoi( optarg );
			break;

		case 'e':
			dwStop	= atoi( optarg );
			break;

		case 'h':
			nHelpLayer = max( nHelpLayer, 2 );
			break;

		case 0:
			switch( nLongIdx )
			{
			case 0: // info
				nOfInfos	= sizeof( pFnInfos ) / sizeof( *pFnInfos );
				break;
				
			case 1: // help
				nHelpLayer = max( nHelpLayer, 2 );
				break;
			}
			break;
		}
	}

	//-------------------------------------------------------------------------

	for( nCnt = 0 ; optind < nArg && nCnt < 2 ; nCnt ++, optind ++ )
	{
		InputFiles[nCnt].szNames = pszArgs[optind];
	}

	for( ; optind < nArg ; optind ++ )
	{
		if( ResidFile.szNames == NULL )
		{
			ResidFile.szNames = pszArgs[optind];
		}

		break;
	}

	//-------------------------------------------------------------------------

	if( InputFiles[0].szNames && InputFiles[1].szNames )
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
		DWORD dwDataSize[2];

		DWORD dwStartPos = 0;
		DWORD dwSize;
		DWORD dwProc;

		//-------------------------------------------------------------------------

		for( nCnt = 0 ; nCnt < 2 ; nCnt ++ )
		{
			WAVEFILE *pFile = &InputFiles[nCnt];

			if( !wavefile_open( pFile ) )
			{
				break;
			}

			//-------------------------------------------------------------------------

			if( nCnt == 0 )
			{
				pGlobalFormat		= mmsys_format_dublicate( pFile->pFormat );
				ResidFile.pFormat	= mmsys_format_dublicate( pFile->pFormat );
			}
			else
			{
				if
				(
					!mmsys_format_is_eq
					(
						ResidFile.pFormat,
						pFile->pFormat,
						"rc"
					)
				)
				{
					printf( "Different file format!\n" );
					break;
				}
			}

			//-------------------------------------------------------------------------

			if( mmsys_format_info( pFile->pFormat, 't' ) == WAVE_FORMAT_IEEE_FLOAT )
			{
				bFloatData |= TRUE;
			}

			nBits = max( nBits, (WORD)mmsys_format_info( pFile->pFormat, 'b' ) );
		}

		if( nCnt < 2 )
		{
			break;
		}

		//-------------------------------------------------------------------------

		{
			WORD wDataBits;
			WORD wFormatTag;

			if( bFloatData )
			{
				wDataBits  = sizeof(DATA);
				wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
			}
			else
			{
				wDataBits  = sizeof(LONG);
				wFormatTag = WAVE_FORMAT_PCM;
			}

			pGlobalFormat		= mmsys_format_pcm_change( pGlobalFormat, "bt", wDataBits * 8, wFormatTag );
			ResidFile.pFormat	= mmsys_format_pcm_change( ResidFile.pFormat, "bt", nBits, wFormatTag );
		}

		if( pGlobalFormat == NULL || ResidFile.pFormat == NULL )
		{
			break;
		}

		//-------------------------------------------------------------------------

		for( nCnt = 0 ; nCnt < 2 ; nCnt ++ )
		{
			dwDataSize[nCnt] = wavefile_init( &InputFiles[nCnt], pGlobalFormat );
			if( dwDataSize[nCnt] == INVALID_SIZE )
			{
				break;
			}
		}

		if( nCnt < 2 )
		{
			break;
		}

		//-------------------------------------------------------------------------

		if( ResidFile.szNames )
		{
			printf( "Creating residual file \'%s\' ... ", ResidFile.szNames );

			//-------------------------------------------------------------------------

			ResidFile.hFiles = waveio_open( ResidFile.szNames, WAVE_IO_WRITE );
			if( ResidFile.hFiles )
			{
				waveio_store_chunk( ResidFile.hFiles, FCC_FMT );
				waveio_write
					(
						ResidFile.hFiles,
						(PBYTE)ResidFile.pFormat,
						mmsys_format_info( ResidFile.pFormat, 's' )
					);

				waveio_store_chunk( ResidFile.hFiles, FCC_DATA );
				printf( "Ok.\n" );
			}
			else
			{
				printf( "Failed!\n" );
			}

			PrintWaveInfo( ResidFile.pFormat, NULL );
		}

		//-------------------------------------------------------------------------

		PrintWaveInfo( pGlobalFormat, "Processing format:" );

		//-------------------------------------------------------------------------
		
		ResidFile.hConv = convert_open( pGlobalFormat, ResidFile.pFormat, NULL );
		if( ResidFile.hConv == NULL )
		{
			printf( "Residual convert error!\n" );
		}

		//-------------------------------------------------------------------------

		nChann = pGlobalFormat->nChannels;

		//-------------------------------------------------------------------------

		for( nCnt = 0 ; nCnt < nChann ; nCnt ++ )
		{
			hMSE[nCnt] = mse_open( MSE_FLAG_DEFAULT | MSE_FLAG_DIFF );
			if( hMSE[nCnt] == NULL )
			{
				break;
			}

			mse_reset( hMSE[nCnt] );
		}

		if( nCnt != nChann )
		{
			printf( "MSE module error!\n" );
			break;
		}

		//-------------------------------------------------------------------------

		dwSize = min( dwDataSize[0], dwDataSize[1] );

		if( dwDataSize[0] != dwDataSize[1] )
		{
			nCnt = ( dwDataSize[0] > dwDataSize[1] ) ? 0 : 1;
			printf
				(
					"File \'%s\' is longer than \'%s\'\n",
					InputFiles[nCnt].szNames,
					InputFiles[1 - nCnt].szNames
				);
		}

		//-------------------------------------------------------------------------

		if( dwStop )
		{
			dwStop = min( dwStop, dwSize );
		}
		else
		{
			dwStop = dwSize;
		}

		dwStart = min( dwStart, dwStop );
		dwSize	= ( dwStop - dwStart ) * pGlobalFormat->nBlockAlign;

		//-------------------------------------------------------------------------

		for( nCnt = 0 ; nCnt < 2 ; nCnt ++ )
		{
			if( !wavefile_seek( &InputFiles[nCnt], dwStart ) )
			{
				break;
			}
		}

		if( nCnt < 2 )
		{
			printf( "Processing error!\n" );
			break;
		}

		//-------------------------------------------------------------------------

		printf( "\n" );
		cmdline_progress_init( PROGRESS_FILL_PROGR );
		
		//-------------------------------------------------------------------------

		for( dwProc = 0 ; ; )
		{
			DWORD dwDonePos;

			for( nCnt = 0 ; nCnt < 2 ; nCnt ++ )
			{
				wavefile_load( &InputFiles[nCnt] );
			}

			dwDonePos = min( InputFiles[0].dwRemOut, InputFiles[1].dwRemOut );
			if( dwDonePos == 0 )
			{
				break;
			}

			//-------------------------------------------------------------------------

			for( nCnt = 0 ; nCnt < nChann ; nCnt ++ )
			{
				if( bFloatData )
				{
					DATA *pData0 = (DATA *)&InputFiles[0].pBufferOut[dwStartPos] + nCnt;
					DATA *pData1 = (DATA *)&InputFiles[1].pBufferOut[dwStartPos] + nCnt;

					mse_data
						(
							hMSE[nCnt],
							pData0,
							pData1,
							pData0,
							( dwDonePos - dwStartPos ) / sizeof(DATA),
							nChann
						);
				}
				else
				{
					LONG *pData0 = (LONG *)&InputFiles[0].pBufferOut[dwStartPos] + nCnt;
					LONG *pData1 = (LONG *)&InputFiles[1].pBufferOut[dwStartPos] + nCnt;
						
					mse_long
						(
							hMSE[nCnt],
							pData0,
							pData1,
							pData0,
							( dwDonePos - dwStartPos ) / sizeof(LONG),
							nChann
						);
				}
			}

			dwStartPos = dwDonePos;

			//-------------------------------------------------------------------------

			if( ResidFile.hFiles && ResidFile.hConv )
			{
				DWORD dwSave = BUFFER_INP;

				convert_processing
					(
						ResidFile.hConv,
						ResidFile.pBufferIn,
						&dwDonePos,
						ResidFile.pBufferOut,
						&dwSave
					);

				waveio_write( ResidFile.hFiles, ResidFile.pBufferOut, dwSave );
			}

			//-------------------------------------------------------------------------

			for( nCnt = 0 ; nCnt < 2 ; nCnt ++ )
			{
				wavefile_flush( &InputFiles[nCnt], dwDonePos );
			}

			//-------------------------------------------------------------------------

			dwStartPos	-= dwDonePos;
			dwProc		+= dwDonePos;

			cmdline_progress_show( dwProc, dwSize, NULL );
		}

		cmdline_progress_done( NULL );

		//-------------------------------------------------------------------------

		printf( "\n" );
		if( dwSize != dwProc )
		{
			printf( "Warning: data is corrupted!\n" );
		}

		printf
			(
				"Fragment form %d to %d (%d samples)\n\n",
				(INT)dwStart,
				(INT)dwStop,
				(INT)mse_get_cnt( hMSE[0], MSE_CNT_COUNTER )
			);

		//-------------------------------------------------------------------------
		
		dwChannMask	= mmsys_format_info( pGlobalFormat, 'm' );

		for( nCnt = 0 ; nCnt < nChann ; nCnt ++ )
		{
			CONST CHANNELDESC *pChannId = mmsys_channel_desc( dwChannMask, NULL, nCnt );
			CHAR szTemp[MAX_STRING];

			COMBOTYPE dMSE = mse_get_val( hMSE[nCnt], MSE_VAL_ERROR );
			COMBOTYPE dMin = mse_get_val( hMSE[nCnt], MSE_VAL_RANGE_MIN );
			COMBOTYPE dMax = mse_get_val( hMSE[nCnt], MSE_VAL_RANGE_MAX );

			DWORD dwCounter		= mse_get_cnt( hMSE[nCnt], MSE_CNT_COUNTER );
			DWORD dwTotalDiff	= mse_get_cnt( hMSE[nCnt], MSE_CNT_DIFF_TOTAL );


			if( pChannId )
			{
				snprintft( szTemp, MAX_STRING, "%s", pChannId->szLongName );
			}
			else
			{
				snprintft( szTemp, MAX_STRING, "Channel #%d", nCnt );
			}

			strncatt( szTemp, ":", MAX_STRING );
			printf( "%s\n", szTemp );
			fprintfill( stdout, '~', (INT)strlen( szTemp ) );
			printf( "\n" );


			printf( " Dynamic range: from " );
			print_value( ResidFile.hConv, bFloatData, nBits, dMin );
			printf( " to " );
			print_value( ResidFile.hConv, bFloatData, nBits, dMax );
			printf( "\n" );

			printf( " MSE = %.10f\n", dMSE.data );
			printf( " SNR = %g dB\n", -to_db( dMSE.data ) );

			printf( " First diff.: " );
			print_diff( ResidFile.hConv, hMSE[nCnt], bFloatData, nBits, TRUE, dwStart );

			printf( " Max diff.  : " );
			print_diff( ResidFile.hConv, hMSE[nCnt], bFloatData, nBits, FALSE, dwStart );
				
			printf
				(
					" Number of different samples: %d ( %.1f%% )\n\n",
					(INT)dwTotalDiff,
					dwCounter ? (DATA)100.0 * (DATA)dwTotalDiff / (DATA)dwCounter : 0
				);
		}

		//-------------------------------------------------------------------------

		break;
	}

	//-------------------------------------------------------------------------

	mmsys_format_free( pGlobalFormat );

	for( nCnt = 0 ; nCnt < nChann ; nCnt ++ )
	{
		mse_close( hMSE[nCnt] );
	}

	for( nCnt = 0 ; nCnt < 2 ; nCnt ++ )
	{
		wavefile_free( &InputFiles[nCnt] );
	}

	wavefile_free( &ResidFile );

	return 0;
}
