//===================================================================================
//  Includes
//-----------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>

#include "../swin.h"
#include "../types.h"
#include "../array.h"
#include "../mathex.h"
#include "../cmdline.h"
#include "../profile.h"


//===================================================================================
//  Definitions
//-----------------------------------------------------------------------------------
#define PROF_BLANK_SYM1			'-'
#define PROF_BLANK_SYM2			'-'
#define PROF_BLANK_LEN			60


//===================================================================================
//  RAM
//-----------------------------------------------------------------------------------
STATIC PPROFPOINTINFO	g_ProfilerPointTables[PROF_MAX_TABLE];
STATIC INT				g_ProfilerNofPoints	= 0;


//===================================================================================
//  Functions
//-----------------------------------------------------------------------------------
BOOL profiler_add( PROFPOINTINFO *point_table )
{
	if
	(
		g_ProfilerNofPoints < PROF_MAX_TABLE
		&&
		!invalid_ptr( point_table )
	)
	{
		g_ProfilerPointTables[g_ProfilerNofPoints] = point_table;
		g_ProfilerNofPoints ++;

		return TRUE;
	}

	return FALSE;
}


VOID profiler_clear( VOID )
{
	g_ProfilerNofPoints = 0;
}


VOID profiler_reset( VOID )
{
	INT cnt, n;

	for( cnt = 0 ; cnt < g_ProfilerNofPoints ; cnt ++ )
	{
		for( n = 0 ; ; n ++ )
		{
			PPROFPOINTINFO info = &g_ProfilerPointTables[cnt][n];
			
			if( invalid_ptr( info->name ) )
				break;

			CPUCLOCKS_RESET( info->clocks );
			info->count = 0;
		}
	}
}


VOID profiler_print_info( FILE *fp, DATA duration_in_sec )
{
	CONST CHAR *title;
	INT cnt, n;

	//----------------------------------------------------------------

	if( g_ProfilerNofPoints == 0 )
		return;

	//----------------------------------------------------------------

	fprintfill( fp, PROF_BLANK_SYM1, PROF_BLANK_LEN );
	fprintf( fp, "\n  Profiler info:\n" );

	fprintfill( fp, PROF_BLANK_SYM2, PROF_BLANK_LEN );
	fprintf( fp, "\n" );

	//----------------------------------------------------------------

	if( more_zero( duration_in_sec ) )
	{
		duration_in_sec = D( 1.0 ) / duration_in_sec;
		title = "MCPS";
	}
	else
	{
		duration_in_sec = D( 1.0 );
		title = "MCLOCKS";
	}

	//----------------------------------------------------------------

	for( cnt = 0 ; cnt < g_ProfilerNofPoints ; cnt ++ )
	{
		if( cnt )
			fprintf( fp, "\n" );

		for( n = 0 ; ; n ++ )
		{
			PPROFPOINTINFO info = &g_ProfilerPointTables[cnt][n];
			DATA mips;

			if( invalid_ptr( info->name ) )
				break;

			if( info->count == 0 )
				continue;

			mips = (DATA)CPUCLOCKS_GETCLOCKS( info->clocks ) * duration_in_sec;

			fprintf( fp, "%-18s: %12.4lf (%d) %s\n", info->name, 1e-6 * mips, info->count, title );
		}
	}

	//----------------------------------------------------------------

	fprintfill( fp, PROF_BLANK_SYM1, PROF_BLANK_LEN );
	fprintf( fp, "\n" );
}
