#ifndef _THRLIB_H_
#define _THRLIB_H_


//================================================================
//  Definitions
//----------------------------------------------------------------
#define THR_SYNCBUFF_LOCK_READ			0x1
#define THR_SYNCBUFF_LOCK_WRITE			0x2
#define THR_SYNCBUFF_DISCONNECT			0x4


//================================================================
//  Type definitions
//----------------------------------------------------------------
typedef struct
{
	HSECTION	h_section;
	INT			counter;

} SEMAPHORE, *PSEMAPHORE;


typedef struct
{
	HSECTION	h_section;
	HTEVENT		h_event;

	BYTE		*buffer;
	DWORD		size;

	DWORD		read;
	DWORD		write;
	DWORD		flag;

} SYNCBUFF, *PSYNCBUFF;

#endif // _THRLIB_H_
