#ifndef _PROFILE_H_
#define _PROFILE_H_


//===================================================================================
//  Includes
//-----------------------------------------------------------------------------------
#include <stdio.h>
#include "clock.h"


//===================================================================================
//  Definitions
//-----------------------------------------------------------------------------------
#define PROF_MAKE_ENUM( _n )		_n ## _enum

#define PROF_MAX_TABLE				32
#define PROF_END_TABLE				{ NULL, }


//===================================================================================
//  Type definitions
//-----------------------------------------------------------------------------------
typedef struct
{
	CONST CHAR		*name;
	INT				count;

	CPUCLOCKS_INIT( clocks );

} PROFPOINTINFO, *PPROFPOINTINFO;


//===================================================================================
//  Work definitions
//-----------------------------------------------------------------------------------
#ifdef PROFILING

#define BP_START( _fn )								\
{													\
	PPROFPOINTINFO __p_info =						\
		&PROF_POINTS[PROF_MAKE_ENUM( _fn )];		\
													\
	CPUCLOCKS_START( __p_info->clocks );			\
}

#define BP_STOP( _fn )								\
{													\
	PPROFPOINTINFO __p_info =						\
		&PROF_POINTS[PROF_MAKE_ENUM( _fn )];		\
													\
	CPUCLOCKS_STOP( __p_info->clocks );				\
	__p_info->count ++;								\
}

#define FPR( _ret, _fn, _args )						\
{													\
	PPROFPOINTINFO __p_info =						\
		&PROF_POINTS[PROF_MAKE_ENUM( _fn )];		\
													\
	CPUCLOCKS_START( __p_info->clocks );			\
	##_ret _fn ##_args;								\
	CPUCLOCKS_STOP( __p_info->clocks );				\
													\
	__p_info->count ++;								\
}

//===================================================================================
//  Pure definitions
//-----------------------------------------------------------------------------------
#else // PROFILING

#define BP_START( _fn )
#define BP_STOP( _fn )

#define FPR( _ret, _fn, _args )		\
{									\
	##_ret _fn ##_args;				\
}

#endif // PROFILING

#define FP( _fn, _args )			FPR( ;, _fn, _args )

//================================================================
//  Function prototypes
//----------------------------------------------------------------
CDECL_BEGIN

BOOL profiler_add( PPROFPOINTINFO point_table );
VOID profiler_reset( VOID );
VOID profiler_print_info( FILE *fp, DATA duration_in_sec );

CDECL_END

#endif // _PROFILE_H_
