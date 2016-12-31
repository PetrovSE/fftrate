#ifndef _TYPESS_H_
#define _TYPESS_H_


//================================================================
//  Definitions
//----------------------------------------------------------------
#define HIGH_PRECISION


//================================================================
//  Macros definitions
//----------------------------------------------------------------
#define TIME_NANOSEC				( (LONGLONG)1000000000 )
#define TIME_MICROSEC				1000000
#define TIME_MSEC					1000

#define SEC_FROM_MSEC( _ms )		( (_ms) / TIME_MSEC )
#define MCSEC_FROM_MSEC( _ms )		( ( (_ms) % TIME_MSEC ) * TIME_MSEC )
#define NSEC_FROM_MSEC( _ms )		( ( (_ms) % TIME_MSEC ) * TIME_MICROSEC )


//================================================================
//  Compiler depends definitions
//----------------------------------------------------------------

// GNU GCC compiler
#if defined(IS_GCC)
#define __STDC_LIMIT_MACROS
#include <stdint.h>

#ifndef INLINE
#define INLINE				inline
#endif

typedef struct timeval		TIMEVAL;
typedef struct timezone		TIMEZONE;
typedef struct tm			TIMEINFO;
typedef struct timespec		TIMESPEC;
typedef struct termios		TERMIOS;
typedef struct stat			STATD;

// MS Visual Studio compiler
#elif defined(IS_VC)

typedef char				int8_t;
typedef unsigned __int8		uint8_t;

typedef __int16				int16_t;
typedef unsigned __int16	uint16_t;

typedef __int32				int32_t;
typedef unsigned __int32	uint32_t;

typedef __int64				int64_t;
typedef unsigned __int64	uint64_t;

#define INT16_MAX			( (__int32)_I16_MAX )
#define INT16_MIN			( (__int32)_I16_MIN )

#define INT32_MAX			( (__int32)_I32_MAX )
#define INT32_MIN			( (__int32)_I32_MIN )

#define INLINE				__forceinline

// Unknown compiler
#else

typedef char				int8_t;
typedef unsigned char		uint8_t;

typedef short				int16_t;
typedef unsigned short		uint16_t;

typedef long				int32_t;
typedef unsigned long		uint32_t;

typedef int					int64_t;
typedef unsigned int		uint64_t;

#define INT16_MAX			SHRT_MAX
#define INT16_MIN			SHRT_MIN

#define INT32_MAX			LONG_MAX
#define INT32_MIN			LONG_MIN

#define INLINE
#endif



//================================================================
//  Type definitions
//----------------------------------------------------------------
#if !defined(_WINDEF_) && !defined(_WINDEF_H)

typedef char			CHAR;
typedef int8_t			SBYTE;
typedef uint8_t			BYTE,	*PBYTE;

typedef int16_t			SHORT;
typedef uint16_t		WORD,	*PWORD;

typedef int32_t			LONG;
typedef uint32_t		DWORD,	*PDWORD;

typedef int64_t			LONGLONG;
typedef uint64_t		ULONGLONG, *PULONGLONG;

typedef int				INT;
typedef unsigned int	UINT;
typedef int				BOOL;

typedef long			SLONG;
typedef unsigned long	ULONG;

typedef double			DOUBLE;
typedef float			FLOAT;

#define VOID			void
typedef VOID *			PVOID;

#endif

//----------------------------------------------------------------

#ifdef HIGH_PRECISION
typedef double			DATA;
#else
typedef float			DATA;
#endif

typedef union
{
	BYTE	dump[sizeof(DATA)];
	DATA	data;
	
} DATADUMP, *PDATADUMP;

#define D( _x )			(DATA)( _x )

//----------------------------------------------------------------

#if defined(IS_LINUX)
typedef union
{
	struct
	{
		DWORD	LowPart;
		LONG	HighPart;
	};
	
	struct
	{
		DWORD	LowPart;
		LONG	HighPart;
	} u;
	
	LONGLONG QuadPart;
	
} LARGE_INTEGER, *PLARGE_INTEGER;
#endif


//================================================================
//  GUID definitions
//----------------------------------------------------------------
#ifndef GUID_DEFINED
#define GUID_DEFINED

typedef struct
{
	DWORD	Data1;
	WORD	Data2;
	WORD	Data3;
	BYTE	Data4[8];
	
} GUID, *PGUID;

#endif // GUID_DEFINED


//================================================================
//  DirectX definitions
//----------------------------------------------------------------
#if defined(IS_WIN32)

#define DX_VTBL_T( _type, _fn, _inst )		( (##_type)( _inst ) )->lpVtbl->##_fn( (##_type)_inst
#define DX_VTBL( _fn, _inst )				( _inst )->lpVtbl->##_fn( _inst

#define DX_RELEASE( _inst )					DX_VTBL( Release, _inst ) )
#define DX_INITDDS( dds )					{ arrzero_unit( &dds ); dds.dwSize = sizeof(dds); }

#endif // IS_WIN32

#endif // _TYPESS_H_
