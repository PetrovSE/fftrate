//================================================================
//  Includes
//----------------------------------------------------------------
#include "../swin.h"
#include "../types.h"
#include "../array.h"
#include "../mathex.h"

#include "../mmsys.h"
#include "../waveio.h"

#include "waveiolib.h"


//================================================================
//  Open & Close functions
//----------------------------------------------------------------
HWAVEIO waveio_open( CONST CHAR *file_name, DWORD open_type )
{
	PRIFFIOFILE h_inst;

	arralloc_unit( h_inst );

	while( arrcheck( h_inst ) )
	{
		arrzero_unit( h_inst );

		h_inst->open_type = open_type;

		//----------------------------------------------------------------

		switch( open_type )
		{
		case WAVE_IO_READ:

			h_inst->p_file = fopen( file_name, "rb" );
			if( invalid_ptr( h_inst->p_file ) )
			{
				break;
			}

			//----------------------------------------------------------------

			if( fread( &h_inst->main_chunk, sizeof(h_inst->main_chunk), 1, h_inst->p_file ) != 1 )
			{
				break;
			}

			if
			(
				!FCCTYPE_EQ_FCC( h_inst->main_chunk.header.type, FCC_RIFF )
				||
				!FCCTYPE_EQ_FCC( h_inst->main_chunk.sub_type, FCC_WAVE )
				||
				h_inst->main_chunk.header.size < sizeof(FCCTYPE)
			)
			{
				break;
			}

			//----------------------------------------------------------------

			h_inst->rem_size = h_inst->main_chunk.header.size - sizeof(FCCTYPE);
			waveio_align( (HWAVEIO)h_inst, 1 );
			
			return (HWAVEIO)h_inst;


		case WAVE_IO_WRITE:

			h_inst->p_file = fopen( file_name, "wb" );
			if( invalid_ptr( h_inst->p_file ) )
			{
				break;
			}

			//----------------------------------------------------------------

			h_inst->main_chunk.header.type.fcc	= FCC_RIFF;
			h_inst->main_chunk.sub_type.fcc		= FCC_WAVE;
			h_inst->main_chunk.header.size		= sizeof(FCCTYPE);

			if( fwrite( &h_inst->main_chunk, sizeof(h_inst->main_chunk), 1, h_inst->p_file ) != 1 )
			{
				break;
			}

			//----------------------------------------------------------------

			waveio_align( (HWAVEIO)h_inst, 1 );
			return (HWAVEIO)h_inst;
		}

		break;
	}

	//----------------------------------------------------------------

	waveio_close( (HWAVEIO)h_inst );
	return NULL;
}


HWAVEIO waveio_close( HWAVEIO p_inst )
{
	PRIFFIOFILE h_inst = (PRIFFIOFILE)p_inst;

	if( arrcheck( h_inst ) )
	{
		if( !invalid_ptr( h_inst->p_file ) )
		{
			if( h_inst->open_type == WAVE_IO_WRITE )
			{
				waveio_flush_chunk( (HWAVEIO)h_inst );
				fseek( h_inst->p_file, 0, SEEK_SET );

				unref_result
					(
						fwrite( &h_inst->main_chunk.header, sizeof(h_inst->main_chunk.header), 1, h_inst->p_file )
					);
			}

			fclose( h_inst->p_file );
		}

		arrfree( h_inst );
	}

	return NULL;
}


//================================================================
//  Chunk functions
//----------------------------------------------------------------
DWORD waveio_find_chunk( HWAVEIO p_inst, FOURCC fcc_type )
{
	PRIFFIOFILE h_inst = (PRIFFIOFILE)p_inst;

	while
	(
		arrcheck( h_inst )
		&&
		!invalid_ptr( h_inst->p_file )
		&&
		h_inst->open_type == WAVE_IO_READ
	)
	{
		INT cnt;

		h_inst->work_chunk = NULL;

		//----------------------------------------------------------------

		for( cnt = 0 ; cnt < h_inst->chunk_counter ; cnt ++ )
		{
			PCHUNKIDX p_chunk = &h_inst->chunk_index[cnt];

			if( FCCTYPE_EQ_FCC( p_chunk->chunk.type, fcc_type ) )
			{
				fseek( h_inst->p_file, p_chunk->file_pos, SEEK_SET );

				p_chunk->rem_size	= p_chunk->chunk.size;
				h_inst->work_chunk	= p_chunk;

				return p_chunk->chunk.size;
			}
		}

		//----------------------------------------------------------------

		if( h_inst->chunk_counter )
		{
			PCHUNKIDX p_chunk = &h_inst->chunk_index[h_inst->chunk_counter - 1];

			if( fseek( h_inst->p_file, p_chunk->file_pos + p_chunk->chunk.size, SEEK_SET ) )
			{
				break;
			}
		}

		//----------------------------------------------------------------

		while( h_inst->chunk_counter < MAX_CHUNKS )
		{
			PCHUNKIDX p_chunk = &h_inst->chunk_index[h_inst->chunk_counter ++];

			if
			(
				h_inst->rem_size < sizeof(p_chunk->chunk)
				||
				fread( &p_chunk->chunk, sizeof(p_chunk->chunk), 1, h_inst->p_file ) != 1
			)
			{
				break;
			}

			//----------------------------------------------------------------

			h_inst->rem_size -= sizeof(p_chunk->chunk);
			p_chunk->file_pos = ftell( h_inst->p_file );
			p_chunk->rem_size = p_chunk->chunk.size;

			//----------------------------------------------------------------

			if( FCCTYPE_EQ_FCC( p_chunk->chunk.type, fcc_type ) )
			{
				h_inst->work_chunk = p_chunk;
				return p_chunk->chunk.size;
			}

			//----------------------------------------------------------------

			if( fseek( h_inst->p_file, p_chunk->chunk.size, SEEK_CUR ) )
			{
				break;
			}

			h_inst->rem_size -= min( h_inst->rem_size, p_chunk->chunk.size );
		}

		break;
	}

	return INVALID_SIZE;
}


BOOL waveio_store_chunk( HWAVEIO p_inst, FOURCC fcc_type )
{
	PRIFFIOFILE h_inst = (PRIFFIOFILE)p_inst;

	while
	(
		arrcheck( h_inst )
		&&
		!invalid_ptr( h_inst->p_file )
		&&
		h_inst->open_type == WAVE_IO_WRITE
	)
	{
		INT cnt;

		waveio_flush_chunk( (HWAVEIO)h_inst );

		//----------------------------------------------------------------

		for( cnt = 0 ; cnt < h_inst->chunk_counter ; cnt ++ )
		{
			PCHUNKIDX p_chunk = &h_inst->chunk_index[cnt];

			if( FCCTYPE_EQ_FCC( p_chunk->chunk.type, fcc_type ) )
			{
				if( fseek( h_inst->p_file, p_chunk->file_pos, SEEK_SET ) )
				{
					break;
				}

				h_inst->work_chunk				= p_chunk;
				h_inst->work_chunk->rem_size	= h_inst->work_chunk->chunk.size;

				return TRUE;
			}
		}

		//----------------------------------------------------------------

		if( h_inst->chunk_counter )
		{
			PCHUNKIDX p_chunk = &h_inst->chunk_index[h_inst->chunk_counter - 1];

			if( fseek( h_inst->p_file, p_chunk->file_pos + p_chunk->chunk.size, SEEK_SET ) )
			{
				break;
			}
		}

		//----------------------------------------------------------------

		if( h_inst->chunk_counter < MAX_CHUNKS )
		{
			h_inst->work_chunk = &h_inst->chunk_index[h_inst->chunk_counter ++];

			h_inst->work_chunk->chunk.type.fcc	 = fcc_type;
			h_inst->main_chunk.header.size		+= sizeof(h_inst->work_chunk->chunk);

			if( fwrite( &h_inst->work_chunk->chunk, sizeof(h_inst->work_chunk->chunk), 1, h_inst->p_file ) != 1 )
			{
				break;
			}

			h_inst->work_chunk->file_pos = ftell( h_inst->p_file );

			return TRUE;
		}

		break;
	}

	return FALSE;
}


BOOL waveio_flush_chunk( HWAVEIO p_inst )
{
	PRIFFIOFILE h_inst = (PRIFFIOFILE)p_inst;

	while
	(
		arrcheck( h_inst )
		&&
		!invalid_ptr( h_inst->p_file )
		&&
		h_inst->open_type == WAVE_IO_WRITE
	)
	{
		if( invalid_ptr( h_inst->work_chunk ) )
		{
			return TRUE;
		}

		if( fseek( h_inst->p_file, h_inst->work_chunk->file_pos - sizeof(h_inst->work_chunk->chunk), SEEK_SET ) )
		{
			break;
		}

		if( fwrite( &h_inst->work_chunk->chunk, sizeof(h_inst->work_chunk->chunk), 1, h_inst->p_file ) != 1 )
		{
			break;
		}

		h_inst->work_chunk = NULL;

		return TRUE;
	}

	return FALSE;
}


//================================================================
//  Seek & position functions
//----------------------------------------------------------------
VOID waveio_align( HWAVEIO p_inst, INT align )
{
	PRIFFIOFILE h_inst = (PRIFFIOFILE)p_inst;
	
	if( arrcheck( h_inst ) )
	{
		h_inst->align = align;
	}
}


DWORD waveio_position( HWAVEIO p_inst )
{
	PRIFFIOFILE h_inst = (PRIFFIOFILE)p_inst;
	
	while
	(
		arrcheck( h_inst )
		&&
		!invalid_ptr( h_inst->p_file )
		&&
		!invalid_ptr( h_inst->work_chunk )
	)
	{
		switch( h_inst->open_type )
		{
		case WAVE_IO_READ:
			return
				h_inst->work_chunk->chunk.size
				-
				h_inst->work_chunk->rem_size;

		case WAVE_IO_WRITE:
			return h_inst->work_chunk->chunk.size;
		}

		break;
	}

	return INVALID_SIZE;
}


DWORD waveio_seek( HWAVEIO p_inst, LONG shift, DWORD seek )
{
	PRIFFIOFILE h_inst = (PRIFFIOFILE)p_inst;

	while
	(
		arrcheck( h_inst )
		&&
		!invalid_ptr( h_inst->p_file )
		&&
		!invalid_ptr( h_inst->work_chunk )
		&&
		h_inst->open_type == WAVE_IO_READ
	)
	{
		DWORD start	= h_inst->work_chunk->file_pos;
		DWORD stop	= start + h_inst->work_chunk->chunk.size;
		DWORD pos, rem;

		//----------------------------------------------------------------

		switch( seek )
		{
		case SEEK_SET:
			pos = start;
			break;

		case SEEK_CUR:
			pos = stop - h_inst->work_chunk->rem_size;
			break;

		case SEEK_END:
			pos = stop;
			break;

		default:
			return FALSE;
		}

		//----------------------------------------------------------------

		if( shift < 0 )
		{
			shift = -shift;
			shift = min( pos - start, (DWORD)shift );
			shift = -shift;
		}

		if( shift > 0 )
		{
			shift = min( stop - pos, (DWORD)shift );
		}

		pos = alignment( pos - start + shift, h_inst->align ) + start;
		rem = stop - pos;

		//----------------------------------------------------------------

		h_inst->rem_size += rem;
		h_inst->rem_size -= h_inst->work_chunk->rem_size;

		h_inst->work_chunk->rem_size = rem;

		//----------------------------------------------------------------

		fseek( h_inst->p_file, pos, SEEK_SET );

		return (DWORD)( pos - start );
	}

	return INVALID_SIZE;
}


//================================================================
//  Read & Write functions
//----------------------------------------------------------------
DWORD waveio_read( HWAVEIO p_inst, PBYTE p_buff, DWORD buff_size )
{
	PRIFFIOFILE h_inst = (PRIFFIOFILE)p_inst;

	while
	(
		arrcheck( h_inst )
		&&
		!invalid_ptr( h_inst->p_file )
		&&
		!invalid_ptr( h_inst->work_chunk )
		&&
		h_inst->open_type == WAVE_IO_READ
	)
	{
		buff_size = min( h_inst->rem_size, buff_size );
		buff_size = min( h_inst->work_chunk->rem_size, buff_size );

		if( more_zero( buff_size ) )
		{
			buff_size = (DWORD)fread( p_buff, sizeof(BYTE), buff_size, h_inst->p_file );
		}

		h_inst->rem_size				-= buff_size;
		h_inst->work_chunk->rem_size	-= buff_size;

		return buff_size;
	}

	return 0;
}


DWORD waveio_write( HWAVEIO p_inst, CONST BYTE *p_buff, DWORD buff_size )
{
	PRIFFIOFILE h_inst = (PRIFFIOFILE)p_inst;

	while
	(
		arrcheck( h_inst )
		&&
		!invalid_ptr( h_inst->p_file )
		&&
		!invalid_ptr( h_inst->work_chunk )
		&&
		h_inst->open_type == WAVE_IO_WRITE
	)
	{
		if( h_inst->work_chunk->rem_size )
		{
			buff_size = min( buff_size, h_inst->work_chunk->rem_size );
		}

		buff_size = (DWORD)fwrite( p_buff, sizeof(BYTE), buff_size, h_inst->p_file );

		h_inst->main_chunk.header.size	+= buff_size;
		h_inst->work_chunk->chunk.size	+= buff_size;

		return buff_size;
	}

	return 0;
}
