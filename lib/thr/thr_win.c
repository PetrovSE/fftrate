//================================================================
//  Includes
//----------------------------------------------------------------
#include "../swin.h"

#ifdef IS_WIN32
#include <Windows.h>

#include "../array.h"
#include "../thr.h"


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

#endif // Is Win32
