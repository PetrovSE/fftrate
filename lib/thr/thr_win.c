//================================================================
//  Includes
//----------------------------------------------------------------
#include "../swin.h"

#ifdef IS_WIN32
#include <Windows.h>

#include "../array.h"
#include "../thr.h"

#include "thrlib.h"
#include "thrlib_win.h"


//================================================================
//  RAM
//----------------------------------------------------------------
STATIC pfn_signal_handler g_signal_int	= NULL;


//================================================================
//  Sleep finction
//----------------------------------------------------------------
VOID thr_sleep( DWORD timeout_ms )
{
	Sleep( timeout_ms );
}


LONGLONG thr_time( VOID )
{
	return GetTickCount();
}


VOID thr_localtime( PLOCTIME p_time )
{
	if( !invalid_ptr( p_time ) )
	{
		SYSTEMTIME ltime;
		
		GetLocalTime( &ltime );
		
		p_time->year		= ltime.wYear;
		p_time->month		= ltime.wMonth;
		p_time->day			= ltime.wDay;
		p_time->day_of_week = ltime.wDayOfWeek;
		p_time->hour		= ltime.wHour;
		p_time->minute		= ltime.wMinute;
		p_time->second		= ltime.wSecond;
		p_time->mseconds	= ltime.wMilliseconds;
	}
}


INT thr_pid( VOID )
{
	return (INT)GetCurrentProcessId();
}


//================================================================
//  Signal finctions
//----------------------------------------------------------------
STATIC BOOL WINAPI thr_local_signal_hook( DWORD type )
{
	switch( type )
	{
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		if( !invalid_ptr( g_signal_int ) )
		{
			g_signal_int( 0 );
			return TRUE;
		}

		break;
	}
	
	return FALSE;
}


BOOL thr_signal_set( INT sig, pfn_signal_handler p_signal_handler )
{
	BOOL b_set = TRUE;

	//----------------------------------------------------------------

	SetConsoleCtrlHandler( thr_local_signal_hook, FALSE );

	//----------------------------------------------------------------

	switch( sig )
	{
	case SIGINT:
	case SIGTERM:
		g_signal_int = p_signal_handler;
		break;

	default:
		b_set = FALSE;
	}

	//----------------------------------------------------------------

	if( invalid_ptr( g_signal_int ) )
	{
		return b_set;
	}

	b_set &= non_zero( SetConsoleCtrlHandler( thr_local_signal_hook, TRUE ) );
	return b_set;
}


//================================================================
//  Section finctions
//----------------------------------------------------------------
HSECTION thr_section_open( VOID )
{
	CRITICAL_SECTION *h_inst;

	arralloc_unit( h_inst );

	if( arrcheck( h_inst ) )
	{
		InitializeCriticalSection( h_inst );
		return (HSECTION)h_inst;
	}

	return NULL;
}


HSECTION thr_section_close( HSECTION p_inst )
{
	CRITICAL_SECTION *h_inst = (CRITICAL_SECTION *)p_inst;

	if( arrcheck( h_inst ) )
	{
		DeleteCriticalSection( h_inst );
		arrfree( h_inst );
	}

	return NULL;
}


VOID thr_section_enter( HSECTION p_inst )
{
	CRITICAL_SECTION *h_inst = (CRITICAL_SECTION *)p_inst;

	if( arrcheck( h_inst ) )
	{
		EnterCriticalSection( h_inst );
	}
}


VOID thr_section_leave( HSECTION p_inst )
{
	CRITICAL_SECTION * h_inst = (CRITICAL_SECTION *)p_inst;

	if( arrcheck( h_inst ) )
	{
		LeaveCriticalSection( h_inst );
	}
}


//================================================================
//  Event finctions
//----------------------------------------------------------------
HTEVENT thr_event_open( BOOL b_manual_reset, BOOL b_init_state )
{
	return CreateEvent( NULL, b_manual_reset, b_init_state, NULL );
}


HTEVENT thr_event_close( HTEVENT p_inst )
{
	HANDLE h_event = (HANDLE)p_inst;

	if( arrcheck( h_event ) )
	{
		CloseHandle( h_event );
	}

	return NULL;
}


VOID thr_event_set( HTEVENT p_inst )
{
	HANDLE h_event = (HANDLE)p_inst;

	if( arrcheck( h_event ) )
	{
		SetEvent( h_event );
	}
}


VOID thr_event_reset( HTEVENT p_inst )
{
	HANDLE h_event = (HANDLE)p_inst;

	if( arrcheck( h_event ) )
	{
		ResetEvent( h_event );
	}
}


VOID thr_event_wait( HTEVENT p_inst, DWORD timeout_ms )
{
	HANDLE h_event = (HANDLE)p_inst;

	if( arrcheck( h_event ) )
	{
		WaitForSingleObject( h_event, timeout_ms );
	}
}


//================================================================
//  Thread finctions
//----------------------------------------------------------------
HTHREAD thr_thread_open( pfn_thread_callback p_callback_function, PVOID p_arg )
{
	DWORD  process_id;
	HANDLE h_inst = CreateThread
		(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE )p_callback_function,
			p_arg,
			0x0,
			&process_id
		);

	return (HTHREAD)h_inst;
}


HTHREAD thr_thread_close( HTHREAD p_inst )
{
	thr_thread_wait( p_inst, INFINITE_TIMEOUT );

	if( arrcheck( p_inst ) )
	{
		CloseHandle( (HANDLE)p_inst );
	}

	return NULL;
}


VOID thr_thread_terminate( HTHREAD p_inst )
{
	if( !thr_thread_is_active( p_inst ) )
	{
		return;
	}

	if( arrcheck( p_inst ) )
	{
		TerminateThread( (HANDLE)p_inst, 0x0 );
	}
}


VOID thr_thread_wait( HTHREAD p_inst, DWORD timeout_ms )
{
	if( !thr_thread_is_active( p_inst ) )
	{
		return;
	}

	if( arrcheck( p_inst ) )
	{
		WaitForSingleObject( (HANDLE)p_inst, timeout_ms );
	}
}


BOOL thr_thread_exit_code( HTHREAD p_inst, DWORD *code )
{
	BOOL ret = FALSE;

	if( arrcheck( p_inst ) )
	{
		DWORD exit_code = 0x0;
		
		if( GetExitCodeThread( (HANDLE)p_inst, &exit_code ) )
		{
			if( exit_code == STILL_ACTIVE )
			{
				exit_code = THR_IS_ACTIVE;
			}
			else
			{
				ret = TRUE;
			}
		}

		if( !invalid_ptr( code ) )
		{
			*code = exit_code;
		}
	}

	return ret;
}


//================================================================
//  Pipe finctions
//----------------------------------------------------------------
HPIPE thr_pipe_open( VOID )
{
	PPIPE h_inst;
	
	arralloc_unit( h_inst );
	
	while( arrcheck( h_inst ) )
	{
		arrzero_unit( h_inst );

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
	PPIPE h_inst = (PPIPE )p_inst;
	
	if( arrcheck( h_inst ) )
	{
		thr_pipe_disconnect( (HPIPE)h_inst );
		arrfree( h_inst );
	}

	return NULL;
}


BOOL thr_pipe_connect( HPIPE p_inst )
{
	PPIPE h_inst = (PPIPE )p_inst;
	
	thr_pipe_disconnect( (HPIPE)h_inst );

	while( arrcheck( h_inst ) )
	{
		if( !CreatePipe( &h_inst->pipe_read, &h_inst->pipe_write, NULL, 0 ) )
		{
			break;
		}
		
		return TRUE;
	}
	
	return FALSE;
}


BOOL thr_pipe_disconnect( HPIPE p_inst )
{
	PPIPE h_inst = (PPIPE )p_inst;
	
	while( arrcheck( h_inst ) )
	{
		if( !invalid_ptr( h_inst->pipe_write ) )
		{
			CloseHandle( h_inst->pipe_write );
		}

		if( !invalid_ptr( h_inst->pipe_read ) )
		{
			CloseHandle( h_inst->pipe_read );
		}

		h_inst->pipe_read	= NULL;
		h_inst->pipe_write	= NULL;

		return TRUE;
	}

	return FALSE;
}


DWORD thr_pipe_write( HPIPE p_inst, CONST BYTE *p_frame, DWORD size )
{
	PPIPE h_inst = (PPIPE )p_inst;
	
	while( arrcheck( h_inst ) )
	{
		DWORD written = 0;

		if( !WriteFile( h_inst->pipe_write, p_frame, size, &written, NULL ) )
		{
			break;
		}

		return written;
	}

	return INVALID_SIZE;
}


DWORD thr_pipe_read( HPIPE p_inst, BYTE *p_frame, DWORD size )
{
	PPIPE h_inst = (PPIPE )p_inst;
	
	while( arrcheck( h_inst ) )
	{
		DWORD read = 0;
		
		if( !ReadFile( h_inst->pipe_read, p_frame, size, &read, NULL ) )
		{
			break;
		}
		
		return read;
	}
	
	return INVALID_SIZE;
}

#endif // Is Win32
