//================================================================
//  Includes
//----------------------------------------------------------------
#include "../swin.h"

#ifdef IS_LINUX

#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/time.h>

#include "../types.h"
#include "../array.h"
#include "../mathex.h"
#include "../thr.h"

#include "thrlib.h"
#include "thrlib_posix.h"


//================================================================
//  Sleep finction
//----------------------------------------------------------------
VOID thr_sleep( DWORD timeout_ms )
{
	TIMESPEC timeout, left;

	timeout.tv_sec	= SEC_FROM_MSEC( timeout_ms );
	timeout.tv_nsec	= NSEC_FROM_MSEC( timeout_ms );

	left.tv_sec		= 0;
	left.tv_nsec	= 0;

	while( nanosleep( &timeout, &left ) && errno == EINTR )
	{
		timeout.tv_sec	= left.tv_sec;
		timeout.tv_nsec	= left.tv_nsec;
	}
}


LONGLONG thr_time( VOID )
{
	TIMEVAL tval;
	
	gettimeofday( &tval, NULL );
	return (LONGLONG)tval.tv_sec * 1000 + (LONGLONG)tval.tv_usec / 1000;
}


VOID thr_localtime( PLOCTIME p_time )
{
	if( !invalid_ptr( p_time ) )
	{
		TIMEVAL  time_val;
		TIMEZONE time_zone;
		TIMEINFO *p_tm;
		
		gettimeofday( &time_val, &time_zone );
		p_tm = localtime( &time_val.tv_sec );
		
		p_time->year		= p_tm->tm_year + 1900;
		p_time->month		= p_tm->tm_mon  + 1;
		p_time->day			= p_tm->tm_mday;
		p_time->day_of_week = p_tm->tm_wday;
		p_time->hour		= p_tm->tm_hour;
		p_time->minute		= p_tm->tm_min;
		p_time->second		= p_tm->tm_sec;
		p_time->mseconds	= time_val.tv_usec / 1000;
	}
}


INT thr_pid( VOID )
{
	return getpid();
}


//================================================================
//  Signal finctions
//----------------------------------------------------------------
BOOL thr_signal_set( INT sig, pfn_signal_handler p_signal_handler )
{
	if( invalid_ptr( p_signal_handler ) )
	{
		return signal( sig, SIG_DFL ) != SIG_ERR;
	}
	
	return signal( sig, p_signal_handler ) != SIG_ERR;
}


//================================================================
//  Section finctions
//----------------------------------------------------------------
HSECTION thr_section_open( VOID )
{
	PSECTION h_inst;

	arralloc_unit( h_inst );
	while( arrcheck( h_inst ) )
	{
		if( non_zero( pthread_mutexattr_init( &h_inst->attr ) ) )
		{
			break;
		}
		
		pthread_mutexattr_settype( &h_inst->attr, PTHREAD_MUTEX_RECURSIVE );
		
		if( non_zero( pthread_mutex_init( &h_inst->mutex, &h_inst->attr ) ) )
		{
			break;
		}
		
		return (HSECTION)h_inst;
	}

	return thr_section_close( (HSECTION)h_inst );
}


HSECTION thr_section_close( HSECTION p_inst )
{
	PSECTION h_inst = (PSECTION )p_inst;

	if( arrcheck( h_inst ) )
	{
		pthread_mutex_destroy( &h_inst->mutex );
		pthread_mutexattr_destroy( &h_inst->attr );
		arrfree( h_inst );
	}

	return NULL;
}


VOID thr_section_enter( HSECTION p_inst )
{
	PSECTION h_inst = (PSECTION )p_inst;

	if( arrcheck( h_inst ) )
	{
		pthread_mutex_lock( &h_inst->mutex );
	}
}


VOID thr_section_leave( HSECTION p_inst )
{
	PSECTION h_inst = (PSECTION )p_inst;

	if( arrcheck( h_inst ) )
	{
		pthread_mutex_unlock( &h_inst->mutex );
	}
}


//================================================================
//  Event finctions
//----------------------------------------------------------------
HTEVENT thr_event_open( BOOL b_manual_reset, BOOL b_init_state )
{
	PEVENT h_inst;

	arralloc_unit( h_inst );
	if( arrcheck( h_inst ) )
	{
		pthread_mutex_init( &h_inst->mx, NULL );
		pthread_cond_init( &h_inst->cv, NULL );

		h_inst->state			= b_init_state;
		h_inst->manual_reset	= b_manual_reset;

		return (HTEVENT)h_inst;
	}

	return thr_event_close( (HTEVENT)h_inst );
}


HTEVENT thr_event_close( HTEVENT p_inst )
{
	PEVENT h_inst = (PEVENT)p_inst;

	if( arrcheck( h_inst ) )
	{
		pthread_mutex_destroy( &h_inst->mx );
		pthread_cond_destroy( &h_inst->cv );
		arrfree( h_inst );
	}

	return NULL;
}


VOID thr_event_set( HTEVENT p_inst )
{
	PEVENT h_inst = (PEVENT)p_inst;

	if( arrcheck( h_inst ) )
	{
		pthread_mutex_lock( &h_inst->mx );
		h_inst->state = TRUE;
		pthread_cond_broadcast( &h_inst->cv );
		pthread_mutex_unlock( &h_inst->mx );
	}
}


VOID thr_event_reset( HTEVENT p_inst )
{
	PEVENT h_inst = (PEVENT)p_inst;

	if( arrcheck( h_inst ) )
	{
		pthread_mutex_lock( &h_inst->mx );
		h_inst->state = FALSE;
		pthread_mutex_unlock( &h_inst->mx );
	}
}


VOID thr_event_wait( HTEVENT p_inst, DWORD timeout_ms )
{
	PEVENT h_inst = (PEVENT)p_inst;

	if( arrcheck( h_inst ) )
	{
		pthread_mutex_lock( &h_inst->mx );
		while( !h_inst->state )
		{
			pthread_cond_wait( &h_inst->cv, &h_inst->mx );
		}

		if( !h_inst->manual_reset )
		{
			h_inst->state = FALSE;
		}
		pthread_mutex_unlock( &h_inst->mx );
	}
}


//================================================================
//  Thread finctions
//----------------------------------------------------------------
STATIC PVOID thr_thread_callback( PVOID p_arg )
{
	PTHREAD h_inst = (PTHREAD)p_arg;
	
	if( invalid_ptr( h_inst->p_call_back ) )
	{
		thr_semaphore_set( h_inst->exit_code, 0x0 );
	}
	else
	{
		thr_semaphore_set( h_inst->exit_code, h_inst->p_call_back( h_inst->p_arg ) );
	}
	
	return 0x0;
}


HTHREAD thr_thread_open( pfn_thread_callback p_callback_function, PVOID p_arg )
{
	PTHREAD h_inst = NULL;

	arralloc_unit( h_inst );
	while( arrcheck( h_inst ) )
	{
		INT ret;

		arrzero_unit( h_inst );
	
		//----------------------------------------------------------------
		
		h_inst->exit_code = thr_semaphore_open();
		if( !arrcheck( h_inst->exit_code ) )
		{
			break;
		}
		
		thr_semaphore_set( h_inst->exit_code, THR_IS_ACTIVE );
		
		//----------------------------------------------------------------
		
		h_inst->p_call_back	= p_callback_function;
		h_inst->p_arg		= p_arg;
		
		ret = pthread_create( &h_inst->h_thr, NULL, thr_thread_callback, h_inst );
		if( non_zero( ret ) || is_zero( h_inst ) )
		{
			break;
		}
		
		//----------------------------------------------------------------
		
		return (HTHREAD)h_inst;
	}

	return thr_thread_close( (HTHREAD)h_inst );
}


HTHREAD thr_thread_close( HTHREAD p_inst )
{
	PTHREAD h_inst = (PTHREAD)p_inst;
	
	if( arrcheck( h_inst ) )
	{
		if( non_zero( h_inst->h_thr ) )
		{
			pthread_join( h_inst->h_thr, NULL );
		}

		if( arrcheck( h_inst->exit_code ) )
		{
			h_inst->exit_code = thr_semaphore_close( h_inst->exit_code );
		}
		
		arrfree( h_inst );
	}
	
	return NULL;
}


VOID thr_thread_terminate( HTHREAD p_inst )
{
	PTHREAD h_inst = (PTHREAD)p_inst;

	if( !thr_thread_is_active( p_inst ) )
	{
		return;
	}

	if( arrcheck( h_inst ) )
	{
		if( non_zero( h_inst->h_thr ) )
		{
			pthread_cancel( h_inst->h_thr );
		}
		
		thr_semaphore_set( h_inst->exit_code, 0x0 );
	}
}


VOID thr_thread_wait( HTHREAD p_inst, DWORD timeout_ms )
{
	while( timeout_ms )
	{
		DWORD tm = min( timeout_ms, 100 );
			
		if( !thr_thread_is_active( p_inst ) )
		{
			break;
		}
			
		thr_sleep( tm );
		if( timeout_ms != INFINITE_TIMEOUT )
		{
			timeout_ms -= tm;
		}
	}
}


BOOL thr_thread_exit_code( HTHREAD p_inst, DWORD *code )
{
	PTHREAD h_inst  = (PTHREAD)p_inst;
	DWORD exit_code = THR_IS_ACTIVE;
	
	if( arrcheck( h_inst ) )
	{
		exit_code = thr_semaphore_check( h_inst->exit_code );
		
		if( !invalid_ptr( code ) )
		{
			*code = exit_code;
		}
	}
	
	return exit_code != THR_IS_ACTIVE;
}


//================================================================
//  Pipe finctions
//----------------------------------------------------------------
HPIPE thr_pipe_open( VOID )
{
	INT *h_inst = NULL;

	arralloc( h_inst, 2 );

	while( arrcheck( h_inst ) )
	{
		arrzero( h_inst, 2 );

		if( !thr_pipe_connect( (HPIPE)h_inst ) )
		{
			break;
		}

		return (HPIPE)h_inst;
	}

	return thr_pipe_close( (HPIPE)h_inst );
}


HPIPE thr_pipe_close( HPIPE p_inst )
{
	INT *h_inst = (INT *)p_inst;

	if( arrcheck( h_inst ) )
	{
		thr_pipe_disconnect( (HPIPE)h_inst );
		arrfree( h_inst );
	}

	return NULL;
}


BOOL thr_pipe_connect( HPIPE p_inst )
{
	INT *h_inst = (INT *)p_inst;
	
	thr_pipe_disconnect( (HPIPE)h_inst );

	while( arrcheck( h_inst ) )
	{
		if( less_zero( pipe( h_inst ) ) )
		{
			break;
		}

		return TRUE;
	}
	
	return FALSE;
}


BOOL thr_pipe_disconnect( HPIPE p_inst )
{
	INT *h_inst = (INT *)p_inst;
	
	while( arrcheck( h_inst ) )
	{
		if( non_zero( h_inst[0] ) )
		{
			close( h_inst[0] );
		}

		if( non_zero( h_inst[1] ) )
		{
			close( h_inst[1] );
		}

		arrzero( h_inst, 2 );
		
		return TRUE;
	}
	
	return FALSE;
}


DWORD thr_pipe_write( HPIPE p_inst, CONST BYTE *p_frame, DWORD size )
{
	INT *h_inst = (INT *)p_inst;

	while( arrcheck( h_inst ) )
	{
		INT ret = write( h_inst[1], p_frame, size );
		
		if( less_zero( ret ) )
		{
			break;
		}
		
		return (DWORD)ret;
	}

	return INVALID_SIZE;
}


DWORD thr_pipe_read( HPIPE p_inst, BYTE *p_frame, DWORD size )
{
	INT *h_inst = (INT *)p_inst;

	while( arrcheck( h_inst ) )
	{
		INT ret = read( h_inst[0], p_frame, size );
		
		if( less_zero( ret ) )
		{
			break;
		}
		
		return (DWORD)ret;
	}

	return INVALID_SIZE;
}

#endif // Is Linux
