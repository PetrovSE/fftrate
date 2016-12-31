#ifndef _WAVEIO_H_
#define _WAVEIO_H_


//================================================================
//  Includes
//----------------------------------------------------------------
#include "interface.h"
#include "mmsys.h"


//================================================================
//  Container chunks
//----------------------------------------------------------------
#define FCC_RIFF					MAKEFOURCC( 'R', 'I', 'F', 'F' )
#define FCC_LIST					MAKEFOURCC( 'L', 'I', 'S', 'T' )


//================================================================
//  RIFF subtypes
//----------------------------------------------------------------
#define FCC_AVI						MAKEFOURCC( 'A', 'V', 'I', ' ' )
#define FCC_WAVE					MAKEFOURCC( 'W', 'A', 'V', 'E' )
#define FCC_RMID					MAKEFOURCC( 'R', 'M', 'I', 'D' )


//================================================================
//  RIFF WAVE chunks
//----------------------------------------------------------------
#define FCC_FMT						MAKEFOURCC( 'f', 'm', 't', ' ' )
#define FCC_DATA					MAKEFOURCC( 'd', 'a', 't', 'a' )


//================================================================
//  Open file types
//----------------------------------------------------------------
enum
{
	WAVE_IO_NONE	= 0,
	WAVE_IO_READ,
	WAVE_IO_WRITE
};


//================================================================
//  Type definitions
//----------------------------------------------------------------
DECL_HANDLE( HWAVEIO );

typedef struct
{
	FOURCC		fcc_type;
	FOURCC		fcc_subtype;

	DWORD		start_id;
	BOOL		b_recursive;

} RIOFIND, *PRIOFIND;


typedef struct
{
	FOURCC		fcc_type;
	FOURCC		fcc_subtype;

	DWORD		size;
	DWORD		start_pos;
	INT			n_level;

} RIOCHUNK, *PRIOCHUNK;


//================================================================
//  Function prototypes
//----------------------------------------------------------------
CDECL_BEGIN

HWAVEIO waveio_open( CONST CHAR *file_name, DWORD open_type );
HWAVEIO waveio_close( HWAVEIO p_inst );

DWORD waveio_find_chunk( HWAVEIO p_inst, FOURCC fcc_type );
BOOL  waveio_store_chunk( HWAVEIO p_inst, FOURCC fcc_type );
BOOL  waveio_flush_chunk( HWAVEIO p_inst );

VOID  waveio_align( HWAVEIO p_inst, INT align );
DWORD waveio_position( HWAVEIO p_inst );
DWORD waveio_seek( HWAVEIO p_inst, LONG shift, DWORD seek );

DWORD waveio_read( HWAVEIO p_inst, PBYTE p_buff, DWORD buff_size );
DWORD waveio_write( HWAVEIO p_inst, CONST BYTE *p_buff, DWORD buff_size );

CDECL_END

#endif // _WAVEIO_H_
