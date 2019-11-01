//================================================================
//  Includes
//----------------------------------------------------------------
#include "../swin.h"

#if defined(IS_WIN32)
#include <Windows.h>
#include <tchar.h>
#endif

#if defined(IS_GCC)
#include <stdio.h>
#include <time.h>
#endif

#include "../types.h"
#include "../array.h"
#include "../str.h"
#include "../clock.h"



//================================================================
//  Get module information
//----------------------------------------------------------------
LIBINFO_FUNCTION
(
	profiler,
	"Profiler",
	"Function profiling",
	"Copyright (c) 2009-10 PetrovSE",
	"1.0.2.2"
);


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#if defined(IS_PC)
#define CLOCK_DEF
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//================================================================
//  Functions
//----------------------------------------------------------------
#if defined(IS_LINUX)
STATIC LONGLONG clock_local_parse_cpuinfo_cur_freq( VOID )
{
	LONGLONG result = 0;
	FILE *fp = fopen( "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq", "r" );

	if( invalid_ptr( fp ) )
		return 0;

	loopinf
	{
		CHAR str[MAX_STRING];
		CHAR *ptr = fgetst( str, MAX_STRING, fp );

		if( invalid_ptr( ptr ) )
			break;

		result = (LONGLONG)( atof( ptr ) * 1e3 + 0.5 );
		if( result > 0 )
			break;

		result = 0;
	};

	fclose( fp );
	return result;
}


STATIC LONGLONG clock_local_parse_cpuinfo( VOID )
{
	LONGLONG result = 0;
	FILE *fp = fopen( "/proc/cpuinfo", "r" );

	if( invalid_ptr( fp ) )
		return 0;

	loopinf
	{
		CHAR str[MAX_STRING];
		CHAR *ptr = fgetst( str, MAX_STRING, fp );

		if( invalid_ptr( ptr ) )
			break;

		ptr = strstr( ptr, "MHz" );

		if( invalid_ptr( ptr ) )
			continue;

		ptr = strstr( ptr, ":" );
		if( invalid_ptr( ptr ) )
			continue;

		result = (LONGLONG)( atof( ptr + 1 ) * 1e6 + 0.5 );
		if( result > 0 )
			break;

		result = 0;
	};

	fclose( fp );
	return result;
}
#endif


LONGLONG clock_get_cpu_counter( VOID )
{
#if defined(IS_VC)
	LARGE_INTEGER result;
	
	__asm
	{
		rdtsc
		mov result.LowPart,  eax
		mov result.HighPart, edx
	}
	
	return result.QuadPart;
	
#elif defined(IS_GCC)
	LONGLONG result;

	__asm__ __volatile__( "rdtsc" : "=A"( result ) );
	
	return result;
#else
	return 0;
#endif
}

LONGLONG clock_get_cpu_freq( VOID )
{
#if defined(IS_WIN32)
	HKEY h_key;

	if
	(
		RegOpenKey
			(
				HKEY_LOCAL_MACHINE,
				_T( "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0" ),
				&h_key
			) == ERROR_SUCCESS
	)
	{
		DWORD n_val;
		DWORD n_size = sizeof( n_val );
		DWORD n_type = REG_DWORD;

		if
		(
			RegQueryValueEx
				(
					h_key,
					_T( "~MHz" ),
					0,
					&n_type,
					(LPBYTE )&n_val,
					&n_size
				) != ERROR_SUCCESS
		)
		{
			n_val = 1;
		}

		RegCloseKey( h_key );

		return (LONGLONG)n_val * (LONGLONG)1000000L;
	}

	return 1;
	
#elif defined(IS_LINUX)
	LONGLONG result = 0;
	
	loopinf
	{
		result = clock_local_parse_cpuinfo_cur_freq();
		if( result )
			break;
		
		result = clock_local_parse_cpuinfo();
		if( result )
			break;
		
		return 1;
	}
	
	return result;
#else
	return 1;
#endif
}

#endif // PC



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#if defined(IS_ARM) && defined(IS_VC)
#define CLOCK_DEF
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//================================================================
//  Includes
//----------------------------------------------------------------
#include <Windows.h>


//================================================================
//  Definitions
//----------------------------------------------------------------
#define TICKS_PER_SEC			1000L
#define CLOCKS_PER_TICK			(LONGLONG)( 312e6 / TICKS_PER_SEC + 0.5 )


//================================================================
//  Functions
//----------------------------------------------------------------
LONGLONG clock_get_cpu_counter( VOID )
{
	return CLOCKS_PER_TICK * (LONGLONG)GetTickCount();
}


LONGLONG clock_get_cpu_freq( VOID )
{
	return CLOCKS_PER_TICK * (LONGLONG)TICKS_PER_SEC;
}

#endif // VisualStudio on ARM


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifndef CLOCK_DEF // All platform
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//================================================================
//  Includes
//----------------------------------------------------------------
#include <time.h>


//================================================================
//  Functions
//----------------------------------------------------------------
LONGLONG clock_get_cpu_counter( VOID )
{
	return (LONGLONG)clock();
}

LONGLONG clock_get_cpu_freq( VOID )
{
	return (LONGLONG)CLOCKS_PER_SEC;
}

#endif
