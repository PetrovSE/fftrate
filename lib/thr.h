#ifndef _THR_H_
#define _THR_H_


//================================================================
//  Includes
//----------------------------------------------------------------
#include "interface.h"


//================================================================
//  Definitions
//----------------------------------------------------------------
enum
{
	THR_SM_CHECK = 0,
	THR_SM_SET,
	THR_SM_ADD,
	THR_SM_AND,
	THR_SM_OR,
	THR_SM_XOR
};


#if defined(IS_WIN32)
#define SIGHUP						1		// Hangup (POSIX)
#define SIGINT						2		// Interrupt (ANSI)
#define SIGQUIT						3		// Quit (POSIX)
#define SIGILL						4		// Illegal instruction (ANSI)
#define SIGTRAP						5		// Trace trap (POSIX)
#define SIGABRT						6		// Abort (ANSI)
#define SIGBUS						7		// BUS error (4.2 BSD)
#define SIGFPE						8		// Floating-point exception (ANSI)
#define SIGKILL						9		// Kill, unblockable (POSIX)
#define SIGUSR1						10		// User-defined signal 1 (POSIX)
#define SIGSEGV						11		// Segmentation violation (ANSI)
#define SIGUSR2						12		// User-defined signal 2 (POSIX)
#define SIGPIPE						13		// Broken pipe (POSIX)
#define SIGALRM						14		// Alarm clock (POSIX)
#define SIGTERM						15		// Termination (ANSI)
#define SIGSTKFLT					16		// Stack fault
#define SIGCLD						SIGCHLD	// Same as SIGCHLD (System V)
#define SIGCHLD						17		// Child status has changed (POSIX)
#define SIGCONT						18		// Continue (POSIX)
#define SIGSTOP						19		// Stop, unblockable (POSIX)
#define SIGTSTP						20		// Keyboard stop (POSIX)
#define SIGTTIN						21		// Background read from tty (POSIX)
#define SIGTTOU						22		// Background write to tty (POSIX)
#define SIGURG						23		// Urgent condition on socket (4.2 BSD)
#define SIGXCPU						24		// CPU limit exceeded (4.2 BSD)
#define SIGXFSZ						25		// File size limit exceeded (4.2 BSD)
#define SIGVTALRM					26		// Virtual alarm clock (4.2 BSD)
#define SIGPROF						27		// Profiling alarm clock (4.2 BSD)
#define SIGWINCH					28		// Window size change (4.3 BSD, Sun)
#define SIGPOLL						SIGIO	// Pollable event occurred (System V)
#define SIGIO						29		// I/O now possible (4.2 BSD)
#define SIGPWR						30		// Power failure restart (System V)
#define SIGSYS						31		// Bad system call
#define SIGUNUSED					31

#elif defined(IS_LINUX)
#include <signal.h>

#endif



//================================================================
//  Type definitions
//----------------------------------------------------------------
DECL_HANDLE( HSECTION );
DECL_HANDLE( HSEMAPHORE );
DECL_HANDLE( HTEVENT );
DECL_HANDLE( HTHREAD );
DECL_HANDLE( HPIPE );
DECL_HANDLE( HSYNCBUFF );

typedef VOID (*pfn_signal_handler)( INT );
typedef DWORD (*pfn_thread_callback)( PVOID );

typedef struct
{
	LONG	year;			// Year
	INT		month;			// Month: 1 - 12
	INT		day_of_week;	// Day of week: 0 - 6
	INT		day;			// Day: 1 - 31
	INT		hour;			// Hours: 0 - 23
	INT		minute;			// Minutes:	0 - 59
	INT		second;			// Seconds: 0 - 59
	INT		mseconds;		// Milliseconds: 0 - 999
	
} LOCTIME, *PLOCTIME;


//================================================================
//  Function prototypes
//----------------------------------------------------------------
CDECL_BEGIN

LIBINFO_PROTOTYPE( thr );

VOID thr_sleep( DWORD timeout_ms );
LONGLONG thr_time( VOID );
VOID thr_localtime( PLOCTIME p_time );
INT thr_pid( VOID );

HSECTION thr_section_open( VOID );
HSECTION thr_section_close( HSECTION p_inst );

VOID thr_section_enter( HSECTION p_inst );
VOID thr_section_leave( HSECTION p_inst );


HSEMAPHORE thr_semaphore_open( VOID );
HSEMAPHORE thr_semaphore_close( HSEMAPHORE p_inst );

INT thr_semaphore_update( HSEMAPHORE p_inst, DWORD mode, INT val );


HTEVENT thr_event_open( BOOL b_manual_reset, BOOL b_init_state );
HTEVENT thr_event_close( HTEVENT p_inst );


BOOL thr_signal_set( INT sig, pfn_signal_handler p_signal_handler );
#define thr_signal_reset( _s )		thr_signal_set( _s, NULL )


VOID thr_event_set( HTEVENT p_inst );
VOID thr_event_reset( HTEVENT p_inst );
VOID thr_event_wait( HTEVENT p_inst, DWORD timeout_ms );


HTHREAD thr_thread_open( pfn_thread_callback p_callback_function, PVOID p_arg );
HTHREAD thr_thread_close( HTHREAD p_inst );
VOID thr_thread_terminate( HTHREAD p_inst );
VOID thr_thread_wait( HTHREAD p_inst, DWORD timeout_ms );
BOOL thr_thread_exit_code( HTHREAD p_inst, DWORD *code );


HPIPE thr_pipe_open( VOID );
HPIPE thr_pipe_close( HPIPE p_inst );

BOOL  thr_pipe_connect( HPIPE p_inst );
BOOL  thr_pipe_disconnect( HPIPE p_inst );

DWORD thr_pipe_write( HPIPE p_inst, CONST BYTE *p_frame, DWORD size );
DWORD thr_pipe_read( HPIPE p_inst, BYTE *p_frame, DWORD size );


HSYNCBUFF thr_syncbuff_open( DWORD size );
HSYNCBUFF thr_syncbuff_close( HSYNCBUFF p_inst );

BOOL thr_syncbuff_connect( HSYNCBUFF p_inst );
BOOL thr_syncbuff_disconnect( HSYNCBUFF p_inst );

CONST BYTE *thr_syncbuff_lock_read( HSYNCBUFF p_inst, DWORD *p_size, BOOL wait );
BOOL thr_syncbuff_unlock_read( HSYNCBUFF p_inst, DWORD size );

PBYTE thr_syncbuff_lock_write( HSYNCBUFF p_inst, DWORD *p_size, BOOL wait );
BOOL  thr_syncbuff_unlock_write( HSYNCBUFF p_inst, DWORD size );

CDECL_END


//================================================================
//  Macros definitions
//----------------------------------------------------------------
#define thr_semaphore_check( _inst )		thr_semaphore_update( _inst, THR_SM_CHECK, 0 )
#define thr_semaphore_mask( _inst, _v )		thr_semaphore_and( _inst, _v )

#define thr_semaphore_set( _inst, _v )		thr_semaphore_update( _inst, THR_SM_SET, _v )
#define thr_semaphore_reset( _inst )		thr_semaphore_update( _inst, THR_SM_SET, 0 )

#define thr_semaphore_add( _inst, _v )		thr_semaphore_update( _inst, THR_SM_ADD, _v )
#define thr_semaphore_and( _inst, _v )		thr_semaphore_update( _inst, THR_SM_AND, _v )
#define thr_semaphore_or( _inst, _v )		thr_semaphore_update( _inst, THR_SM_OR, _v )
#define thr_semaphore_xor( _inst, _v )		thr_semaphore_update( _inst, THR_SM_XOR, _v )

#define thr_thread_is_active( _inst )		( arrcheck( _inst ) && !thr_thread_exit_code( _inst, NULL ) )

#endif // _THR_H_
