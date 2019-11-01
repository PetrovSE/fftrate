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

#endif // Is Linux
