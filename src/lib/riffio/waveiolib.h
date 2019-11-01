#ifndef _WAVEIOLIB_H_
#define _WAVEIOLIB_H_


//================================================================
//  Includes
//----------------------------------------------------------------
#include <stdio.h>


//================================================================
//  Definitions
//----------------------------------------------------------------
#define FCCTYPE_EQ_FCC( _t, _fcc )		( (_t).fcc == (_fcc ) )
#define FCCTYPE_EQ_FCCTYPE( _t1, _t2 )	( (_t1).fcc == (_t2).fcc )

#define MAX_CHUNKS						32


//================================================================
//  Type definitions
//----------------------------------------------------------------
typedef union
{
	FOURCC		fcc;
	BYTE		txt[sizeof(FOURCC)];

} FCCTYPE, *PFCCTYPE;


typedef struct
{
	FCCTYPE	type;
	DWORD	size;

} CHUNK, *PCHUNK;


typedef struct
{
	CHUNK	header;
	FCCTYPE	sub_type;

} CHUNKEX, *PCHUNKEX;


typedef struct
{
	CHUNK	chunk;
	DWORD	file_pos;
	DWORD	rem_size;

} CHUNKIDX, *PCHUNKIDX;


typedef struct
{
	FILE		*p_file;
	DWORD		open_type;

	CHUNKEX		main_chunk;
	CHUNKIDX	chunk_index[MAX_CHUNKS];
	PCHUNKIDX	work_chunk;

	INT			chunk_counter;
	DWORD		rem_size;
	INT			align;

} RIFFIOFILE, *PRIFFIOFILE;

#endif // _WAVEIOLIB_H_
