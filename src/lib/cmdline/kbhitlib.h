#ifndef _KBHITLIB_H_
#define _KBHITLIB_H_


#if defined(IS_LINUX)

//================================================================
//  Includes
//----------------------------------------------------------------
#include <stdio.h>
#include <termios.h>
#include <unistd.h>


//================================================================
//  Definitions
//----------------------------------------------------------------
#define MAX_KEYB_CODE			32


//================================================================
//  Type definitions
//----------------------------------------------------------------
typedef struct
{
	WORD	code;
	CHAR	chmap[MAX_KEYB_CODE];

} KEYMAP;

#endif

#endif // _KBHITLIB_H_
