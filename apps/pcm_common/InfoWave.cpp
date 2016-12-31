//=============================================================================
// Includes
//-----------------------------------------------------------------------------
#include <stdio.h>
#include "../../src/swin.h"

#if defined(IS_WIN32)
#include <Windows.h>
#endif

#include "../../src/types.h"
#include "../../src/wavedev.h"
#include "../../src/riffio.h"
#include "../../src/cmdline.h"
#include "../../src/str.h"

#include "InfoWave.h"


//=============================================================================
// Info functions
//-----------------------------------------------------------------------------
VOID PrintWaveSystemEnum( VOID )
{
	INT nCnt;
	INT nMaxLen = 0;
	
	printf( "Wave systems:\n" );
		
	for( nCnt = 0 ; ; nCnt ++ )
	{
		CONST PLUGINFO *pInfo = wavedev_system_enum( nCnt );
			
		if( pInfo == NULL )
		{
			break;
		}
		
		nMaxLen = max( nMaxLen, (INT)strlen( pInfo->system_name ) );
	}
	
	//----------------------------------------------------------------------------
	
	for( nCnt = 0 ; ; nCnt ++ )
	{
		CONST PLUGINFO *pInfo = wavedev_system_enum( nCnt );
			
		if( pInfo == NULL )
		{
			break;
		}
			
		printf( " %s", pInfo->system_name );
		fprintfill( stdout, ' ', nMaxLen - (INT)strlen( pInfo->system_name ) );
		printf( " - \"%s\"\n", pInfo->system_desc );
	}
	
	printf( "\n" );
}


VOID PrintWaveSystemInfo( CONST PLUGINFO *pSysInfo )
{
	if( pSysInfo )
	{
		printf( "Wave system [%s]: \"%s\"\n", pSysInfo->system_name, pSysInfo->system_desc );
	}
}


VOID PrintWaveDevEnum( CONST CHAR *szTitle, CONST PLUGINFO *pSysInfo, DWORD dwMode )
{
	INT nCnt;
	
	if( pSysInfo == NULL )
	{
		return;
	}
	
	if( szTitle == NULL )
	{
		szTitle = "Wave devices";
	}
	
	//----------------------------------------------------------------------------
	
	printf( "%s [%s]:\n", szTitle, pSysInfo->system_name );

	for( nCnt = 0 ; ; nCnt ++ )
	{
		DEVINFO Info;

		if( !wavedev_device_enum( &Info, nCnt, dwMode ) )
		{
			break;
		}
		
		printf( " %s", Info.id );
		
		//----------------------------------------------------------------------------
		
		if( strlen( Info.desc ) )
		{
			CHAR desc[MAX_STRING];

			strncpyt( desc, "\n", MAX_STRING );
			strncatt( desc, Info.desc, MAX_STRING );
			str_replace( desc, MAX_STRING, "\n", "\n    " );
			
			printf( "%s", desc );
		}
		
		printf( "\n" );
	}
	
	printf( "\n" );
}


VOID PrintWaveDevInfo( CONST CHAR *szTitle, HWAVEDEV hWaveDev, BOOL bExclusive )
{
	if( szTitle == NULL )
	{
		szTitle = "Wave device";
	}
	
	//----------------------------------------------------------------------------
	
	printf( "%s ", szTitle );
		
	if( hWaveDev == NULL )
	{
		printf( "failed!\n" );
	}
	else
	{
		DEVINFO Info;
			
		if( wavedev_get_device_name( hWaveDev, &Info ) )
		{
			printf( "[%s]: \"%s\" %c\n", Info.id, Info.desc, bExclusive ? '!' : ' ' );
		}
		else
		{
			printf( ": unknown\n" );
		}
	}
}
