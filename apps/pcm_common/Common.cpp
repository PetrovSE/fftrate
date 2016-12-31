//=============================================================================
// Includes
//-----------------------------------------------------------------------------
#include <stdio.h>
#include "../../lib/swin.h"

#if defined(IS_WIN32)
#include <Windows.h>
#endif

#include "../../lib/types.h"
#include "../../lib/mmsys.h"

#include "Common.h"


//=============================================================================
// Static functions
//-----------------------------------------------------------------------------
STATIC CONST CHAR *FormatName( WORD wFormatTag )
{
	switch( wFormatTag )
	{
	case WAVE_FORMAT_PCM:
		return "fixed";
		
	case WAVE_FORMAT_IEEE_FLOAT:
		return "float";
	}
	
	return "unknown";
}


//=============================================================================
// Info functions
//-----------------------------------------------------------------------------
VOID PrintWaveInfo( CONST WAVEFORMATEX *pFormat, CONST CHAR *szTitle )
{
	if( pFormat == NULL )
	{
		return;
	}

	//-----------------------------------------------------------------------------

	if( szTitle )
	{
		printf( "%s\n", szTitle );
	}
	
	//-----------------------------------------------------------------------------

	printf( " Samples rate    = %d Hz\n", (INT)pFormat->nSamplesPerSec );
	printf( " Channels        = %d\n", pFormat->nChannels );
	printf
		(
			" Bits per sample = %d (actual: %d, %s)\n",
			(INT)mmsys_format_info( pFormat, 'B' ),
			(INT)mmsys_format_info( pFormat, 'b' ),
			FormatName( (INT)mmsys_format_info( pFormat, 't' ) )
		);
	printf( "\n" );
}
