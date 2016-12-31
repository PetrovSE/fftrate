#ifndef _THRLIB_WIN_H_
#define _THRLIB_WIN_H_

#ifdef IS_WIN32

//================================================================
//  Type definitions
//----------------------------------------------------------------
typedef struct
{
	HANDLE	pipe_read;
	HANDLE	pipe_write;

} PIPE, *PPIPE;

#endif // Is Win32

#endif // _THRLIB_WIN_H_
