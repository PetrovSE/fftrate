#ifndef _THRLIB_POSIX_H_
#define _THRLIB_POSIX_H_

#ifdef IS_LINUX

//================================================================
//  Type definitions
//----------------------------------------------------------------
typedef struct
{
	pthread_mutex_t		mx;
	pthread_cond_t		cv;

	BOOL				state;
	BOOL				manual_reset;

} EVENT, *PEVENT;


typedef struct
{
	pthread_t	h_thr;
	HSEMAPHORE	exit_code;
	
	pfn_thread_callback	p_call_back;
	PVOID				p_arg;
	
} THREAD, *PTHREAD;


typedef struct
{
	pthread_mutexattr_t	attr;
	pthread_mutex_t		mutex;

} SECTION, *PSECTION;

#endif // Is Linux

#endif // _THRLIB_POSIX_H_
