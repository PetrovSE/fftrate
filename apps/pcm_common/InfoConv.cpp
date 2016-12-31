//=============================================================================
// Includes
//-----------------------------------------------------------------------------
#include <stdio.h>
#include "../../lib/swin.h"

#if defined(IS_WIN32)
#include <Windows.h>
#endif

#include "../../lib/types.h"
#include "../../lib/convert.h"
#include "../../lib/cmdline.h"
#include "../../lib/str.h"
#include "../../lib/mmsys.h"

#include "InfoConv.h"


//=============================================================================
// Info functions
//-----------------------------------------------------------------------------
VOID PrintConvMatrix( HCONVERT hConv )
{
	CONVSTAT ConvStat;

	if( hConv == NULL )
	{
		return;
	}

	//-----------------------------------------------------------------------------

	convert_stat_mode( hConv, CONV_STAT_GETMATRIX, TRUE );
	convert_get_stat( hConv, &ConvStat );
	convert_stat_mode( hConv, CONV_STAT_GETMATRIX, FALSE );
	
	//-----------------------------------------------------------------------------

	if( flag_check_mask( ConvStat.dwModeDone, CONV_STAT_GETMATRIX ) )
	{
		INT nChannOut = binweight( ConvStat.TransformMatrix.dwChannMaskOut );
		INT nChannIn  = binweight( ConvStat.TransformMatrix.dwChannMaskIn );

		INT i, j;

		if( ConvStat.TransformMatrix.bEmptyMatrix )
		{
			return;
		}

		//-----------------------------------------------------------------------------

		fprintfill( stdout, ' ', 8 );

		for( i = 0 ; i < nChannOut ; i ++ )
		{
			CONST CHANNELDESC *pDesc =
				mmsys_channel_desc
					(
						ConvStat.TransformMatrix.dwChannMaskOut,
						ConvStat.TransformMatrix.pOrderOut,
						i
					);

			printf( "%-8s", pDesc ? pDesc->szShortName : "" );
		}

		printf( "\n" );

		//-----------------------------------------------------------------------------
	
		for( i = 0 ; i < nChannIn ; i ++ )
		{
			CONST CHANNELDESC *pDesc =
				mmsys_channel_desc
					(
						ConvStat.TransformMatrix.dwChannMaskIn,
						ConvStat.TransformMatrix.pOrderIn,
						i
					);
			
			printf( "%-8s", pDesc ? pDesc->szShortName : "" );

			//-----------------------------------------------------------------------------

			for( j = 0 ; j < nChannOut ; j ++ )
			{
				if( CHANN_IS_USED( ConvStat.TransformMatrix.ppMixer[i][j] ) )
				{
					printf( "%5.3f", ConvStat.TransformMatrix.ppMixer[i][j] );
				}
				else
				{
					fprintfill( stdout, '-', 5 );
				}

				fprintfill( stdout, ' ', 3 );
			}

			printf( "\n" );		
		}

		printf( "\n" );
	}
}


VOID PrintConvInfo( HCONVERT hConv )
{
	CONVSTAT ConvStat;
	
	if( hConv == NULL )
	{
		return;
	}

	//-----------------------------------------------------------------------------

	convert_stat_mode( hConv, CONV_STAT_GETINFO, TRUE );
	convert_get_stat( hConv, &ConvStat );
	
	//-----------------------------------------------------------------------------

	if( flag_check_mask( ConvStat.dwModeDone, CONV_STAT_GETINFO ) )
	{
		PCONVNFO pInfo = &ConvStat.Info;
		
		printf
			(
				"Windows : \"%s\" (%d => %d)\n",
				pInfo->szWindowName,
				pInfo->nWindowLenIn,
				pInfo->nWindowLenOut
			);
			
		printf( "Delay   : %d ms\n", pInfo->nDelay );
		printf( "Sync.   : %s\n", onoff_box( pInfo->bSync ) );
		printf( "Trans.  : \"%s\"\n", pInfo->szTransformName );
		printf( "\n" );
	}
}
