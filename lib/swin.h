#ifndef _SWIN_H_
#define _SWIN_H_


//================================================================
//  Includes
//----------------------------------------------------------------
#include <stdlib.h>
#include "mathmac.h"


//================================================================
//  Definition
//----------------------------------------------------------------
#define INVALID_SIZE		(DWORD)( -1 )
#define INFINITE_TIMEOUT	(DWORD)( -1 )
#define THR_IS_ACTIVE		(DWORD)( -1 )

#define MAX_STRING			1024
#define MAX_NAME			64


#if defined(DEBUG) || defined(_DEBUG)
#define DEBUGGING
#endif


#define STATIC				static
#define EXTERN				extern

#ifndef CONST
#define CONST				const
#endif

#ifndef TRUE
#define TRUE				0x1
#endif

#ifndef FALSE
#define FALSE				0x0
#endif


#define STRUCT_TERMINATE	{ 0 }


//================================================================
//  Compiler detection
//----------------------------------------------------------------
#if defined(_MSC_VER)
#define IS_VC
#endif

#if _MSC_VER == 1200
#define IS_VC6
#endif

#if defined(__GNUC__)
#define IS_GCC
#endif

		
//================================================================
//  Platform detection
//----------------------------------------------------------------
#if defined(_M_IX86) || defined(__i386__)
#define IS_PC_32
#elif defined(_M_IA64) || defined(_M_X64) || defined(__x86_64__) || defined(__ia64__)
#define IS_PC_64
#endif

#if defined(IS_PC_32) || defined(IS_PC_64)
#define IS_PC
#endif

#if defined(_ARM_)
#define IS_ARM
#endif


//================================================================
//  System detection
//----------------------------------------------------------------
#if defined(WIN32)
#define IS_WIN32
#endif

#if defined(linux)
#define IS_LINUX
#endif


//================================================================
//  Common definitions
//----------------------------------------------------------------
#define unref_result( _p )		if( _p );
#define unref_param( _p )		\
{								\
	(_p) = (_p);				\
}

#define loopinf					while( 1 )

#endif // _SWIN_H_
