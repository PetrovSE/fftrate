//================================================================
//  Includes
//----------------------------------------------------------------
#include <string.h>

#include "../swin.h"
#include "../types.h"
#include "../array.h"
#include "../str.h"
#include "../profile.h"

#include "../stretch.h"
#include "stretchlib.h"


//#define PROF_MAIN	"convert/proftodo.inc"
//#include "../profdata.h"


//================================================================
//  Open module
//----------------------------------------------------------------
HSTRETCH stretch_open
(
	CONST WAVEFORMATEX	*p_format,
	DATA				coef
)
{
	PSTRETCHINST h_inst;

	arralloc_unit( h_inst );

	while( arrcheck( h_inst ) )
	{
		arrzero_unit( h_inst );

		return (HSTRETCH)h_inst;
	}

	return stretch_close( (HSTRETCH)h_inst );
}


//================================================================
//  Close module
//----------------------------------------------------------------
HSTRETCH stretch_close( HSTRETCH p_inst )
{
	PSTRETCHINST h_inst = (PSTRETCHINST)p_inst;

	if( arrcheck( h_inst ) )
	{
	}

	arrfree( h_inst );
	return NULL;
}


//================================================================
//  Reset module
//----------------------------------------------------------------
BOOL stretch_reset( HSTRETCH p_inst )
{
	PSTRETCHINST h_inst = (PSTRETCHINST)p_inst;
	return TRUE;
}


//================================================================
//  Processing
//----------------------------------------------------------------
BOOL stretch_processing
(
	HSTRETCH	p_inst,
	CONST BYTE	*p_src,
	PDWORD		p_src_size,
	PBYTE		p_dst,
	PDWORD		p_dst_size
)
{
	PSTRETCHINST h_inst = (PSTRETCHINST)p_inst;
	DWORD size = min( *p_src_size, *p_dst_size );

	arrcpy( p_dst, p_src, size );

	*p_src_size = size;
	*p_dst_size = size;

	return TRUE;
}
