//================================================================
//  Includes
//----------------------------------------------------------------
#include "../swin.h"
#include "../types.h"
#include "../array.h"
#include "../mathex.h"
#include "../thr.h"
#include "thrlib.h"


//================================================================
//  Get module information
//----------------------------------------------------------------
LIBINFO_FUNCTION
(
	thr,
	"Threads",
	"Multithreads interface & events",
	"Copyright (c) 2009-11 PetrovSE",
	"1.0.6.2"
);


//================================================================
//  Semaphore functions
//----------------------------------------------------------------
HSEMAPHORE thr_semaphore_open( VOID )
{
	PSEMAPHORE h_inst;
	
	arralloc_unit( h_inst );
	
	while( arrcheck( h_inst ) )
	{
		arrzero_unit( h_inst );
		
		//----------------------------------------------------------------
		
		h_inst->h_section = thr_section_open();
		if( invalid_ptr( h_inst->h_section ) )
			break;
		
		//----------------------------------------------------------------
		
		thr_semaphore_reset( (HSEMAPHORE)h_inst );
		
		return (HSEMAPHORE)h_inst;
	}
	
	//----------------------------------------------------------------
	
	return thr_semaphore_close( (HSEMAPHORE)h_inst );
}


HSEMAPHORE thr_semaphore_close( HSEMAPHORE p_inst )
{
	PSEMAPHORE h_inst = (PSEMAPHORE)p_inst;
	
	if( arrcheck( h_inst ) )
	{
		thr_section_close( h_inst->h_section );
		arrfree( h_inst );
	}

	return NULL;
}


INT thr_semaphore_update( HSEMAPHORE p_inst, DWORD mode, INT val )
{
	PSEMAPHORE h_inst = (PSEMAPHORE)p_inst;
	
	if( arrcheck( h_inst ) )
	{
		INT ret = 0;
		
		thr_section_enter( h_inst->h_section );

		switch( mode )
		{
		case THR_SM_CHECK:
			ret = h_inst->counter;
			break;

		case THR_SM_SET:
			ret = h_inst->counter = val;
			break;
			
		case THR_SM_ADD:
			ret = ( h_inst->counter += val );
			break;
			
		case THR_SM_AND:
			ret = ( h_inst->counter &= val );
			break;
			
		case THR_SM_OR:
			ret = ( h_inst->counter |= val );
			break;
			
		case THR_SM_XOR:
			ret = ( h_inst->counter ^= val );
			break;
		}
		
		thr_section_leave( h_inst->h_section );
		
		return ret;
	}
	
	return 0;
}


//================================================================
//  Sync buffer functions
//----------------------------------------------------------------
HSYNCBUFF thr_syncbuff_open( DWORD size )
{
	PSYNCBUFF h_inst;
	
	arralloc_unit( h_inst );
	
	while( arrcheck( h_inst ) )
	{
		arrzero_unit( h_inst );
		
		//----------------------------------------------------------------

		h_inst->h_section = thr_section_open();
		if( invalid_ptr( h_inst->h_section ) )
			break;

		//----------------------------------------------------------------

		h_inst->h_event = thr_event_open( TRUE, FALSE );
		if( invalid_ptr( h_inst->h_event ) )
			break;

		//----------------------------------------------------------------

		arralloc( h_inst->buffer, size );
		if( !arrcheck( h_inst->buffer ) )
			break;

		h_inst->size = size;

		//----------------------------------------------------------------

		if( !thr_syncbuff_connect( (HSYNCBUFF)h_inst ) )
			break;
		
		return (HSYNCBUFF)h_inst;
	}
	
	return thr_syncbuff_close( (HSYNCBUFF)h_inst );
}


HSYNCBUFF thr_syncbuff_close( HSYNCBUFF p_inst )
{
	PSYNCBUFF h_inst = (PSYNCBUFF )p_inst;

	if( arrcheck( h_inst ) )
	{
		thr_syncbuff_disconnect( (HSYNCBUFF)h_inst );

		thr_section_close( h_inst->h_section );
		thr_event_close( h_inst->h_event );

		arrfree( h_inst->buffer );
		arrfree( h_inst );
	}

	return NULL;
}


BOOL thr_syncbuff_connect( HSYNCBUFF p_inst )
{
	PSYNCBUFF h_inst = (PSYNCBUFF )p_inst;
	
	while( arrcheck( h_inst ) )
	{
		thr_syncbuff_disconnect( (HSYNCBUFF)h_inst );
		thr_event_reset( h_inst->h_event );

		h_inst->read	= 0;
		h_inst->write	= 0;
		flag_init( h_inst->flag );

		return TRUE;
	}

	return FALSE;
}


BOOL thr_syncbuff_disconnect( HSYNCBUFF p_inst )
{
	PSYNCBUFF h_inst = (PSYNCBUFF )p_inst;
	
	while( arrcheck( h_inst ) )
	{
		thr_section_enter( h_inst->h_section );
		thr_event_set( h_inst->h_event );

		h_inst->read	= h_inst->size;
		h_inst->write	= h_inst->size;
		flag_set( h_inst->flag, THR_SYNCBUFF_DISCONNECT );

		thr_section_leave( h_inst->h_section );

		return TRUE;
	}

	return FALSE;
}


STATIC VOID thr_syncbuff_local_update( PSYNCBUFF h_inst )
{
	if( flag_is_empty( h_inst->flag ) )
	{
		h_inst->write -= h_inst->read;
		arrcpy( h_inst->buffer, &h_inst->buffer[h_inst->read], h_inst->write );
		h_inst->read = 0;

		if
		(
			( h_inst->read < h_inst->write )
			||
			( h_inst->write < h_inst->size )
		)
		{
			thr_event_set( h_inst->h_event );
		}
	}
}


CONST BYTE *thr_syncbuff_lock_read( HSYNCBUFF p_inst, DWORD *p_size, BOOL wait )
{
	PSYNCBUFF h_inst = (PSYNCBUFF )p_inst;
	
	while
	(
		arrcheck( h_inst )
		&&
		!invalid_ptr( p_size )
	)
	{
		CONST BYTE *p_ret = NULL;

		//----------------------------------------------------------------

		loopinf
		{
			BOOL loop = FALSE;

			thr_section_enter( h_inst->h_section );

			p_ret	= &h_inst->buffer[h_inst->read];
			*p_size	= h_inst->write - h_inst->read;

			//----------------------------------------------------------------

			if( flag_check_mask( h_inst->flag, THR_SYNCBUFF_DISCONNECT ) )
				wait = FALSE;

			if( *p_size == 0 )
				loop = wait;

			//----------------------------------------------------------------

			if( loop )
				thr_event_reset( h_inst->h_event );
			else
				flag_set( h_inst->flag, THR_SYNCBUFF_LOCK_READ );

			thr_section_leave( h_inst->h_section );

			//----------------------------------------------------------------

			if( loop )
			{
				thr_event_wait( h_inst->h_event, INFINITE_TIMEOUT );
				continue;
			}

			break;
		}

		//----------------------------------------------------------------

		return p_ret;
	}

	return NULL;
}


BOOL thr_syncbuff_unlock_read( HSYNCBUFF p_inst, DWORD size )
{
	PSYNCBUFF h_inst = (PSYNCBUFF )p_inst;
	BOOL ret = FALSE;
	
	if( arrcheck( h_inst ) )
	{
		thr_section_enter( h_inst->h_section );

		if( h_inst->write >= ( h_inst->read + size ) )
		{
			h_inst->read += size;
			ret = TRUE;
		}

		flag_reset( h_inst->flag, THR_SYNCBUFF_LOCK_READ );
		thr_syncbuff_local_update( h_inst );

		thr_section_leave( h_inst->h_section );
	}

	return ret;
}


PBYTE thr_syncbuff_lock_write( HSYNCBUFF p_inst, DWORD *p_size, BOOL wait )
{
	PSYNCBUFF h_inst = (PSYNCBUFF )p_inst;
	
	while
	(
		arrcheck( h_inst )
		&&
		!invalid_ptr( p_size )
	)
	{
		PBYTE p_ret;

		//----------------------------------------------------------------

		loopinf
		{
			BOOL loop = FALSE;

			thr_section_enter( h_inst->h_section );
		
			p_ret	= &h_inst->buffer[h_inst->write];
			*p_size	= h_inst->size - h_inst->write;
	
			//----------------------------------------------------------------

			if( flag_check_mask( h_inst->flag, THR_SYNCBUFF_DISCONNECT ) )
				wait = FALSE;

			if( *p_size == 0 )
				loop = wait;

			//----------------------------------------------------------------

			if( loop )
				thr_event_reset( h_inst->h_event );
			else
				flag_set( h_inst->flag, THR_SYNCBUFF_LOCK_WRITE );

			thr_section_leave( h_inst->h_section );

			//----------------------------------------------------------------

			if( loop )
			{
				thr_event_wait( h_inst->h_event, INFINITE_TIMEOUT );
				continue;
			}

			break;
		}

		//----------------------------------------------------------------

		return p_ret;
	}

	return NULL;
}


BOOL thr_syncbuff_unlock_write( HSYNCBUFF p_inst, DWORD size )
{
	PSYNCBUFF h_inst = (PSYNCBUFF )p_inst;
	BOOL ret = FALSE;
	
	if( arrcheck( h_inst ) )
	{
		thr_section_enter( h_inst->h_section );

		if( h_inst->size >= ( h_inst->write + size ) )
		{
			h_inst->write += size;
			ret = TRUE;
		}

		flag_reset( h_inst->flag, THR_SYNCBUFF_LOCK_WRITE );
		thr_syncbuff_local_update( h_inst );

		thr_section_leave( h_inst->h_section );
	}

	return ret;
}
