#ifndef _STRETCH_H_
#define _STRETCH_H_


//================================================================
//  Includes
//----------------------------------------------------------------
#include "interface.h"
#include "mathex.h"
#include "mmsys.h"


//================================================================
//  Type definitions
//----------------------------------------------------------------
DECL_HANDLE( HSTRETCH );

//================================================================
//  Function prototypes
//----------------------------------------------------------------
CDECL_BEGIN

HSTRETCH stretch_open
(
	CONST WAVEFORMATEX	*p_format,
	DATA				coef
);
HSTRETCH stretch_close( HSTRETCH p_inst );

BOOL stretch_reset( HSTRETCH p_inst );
BOOL stretch_processing
(
	HSTRETCH	p_inst,
	CONST BYTE	*p_src,
	PDWORD		p_src_size,
	PBYTE		p_dst,
	PDWORD		p_dst_size
);

CDECL_END

#endif // _STRETCH_H_
