//================================================================
//  Includes
//----------------------------------------------------------------
#include <stdio.h>
#include <stdarg.h>

#include "../swin.h"
#include "../types.h"
#include "../array.h"
#include "../str.h"
#include "../thr.h"
#include "../logfile.h"

#include "logfilelib.h"


//================================================================
//  RAM
//----------------------------------------------------------------
STATIC PLOGFILE	g_log_inst = NULL;


//================================================================
//  Open & Close functions
//----------------------------------------------------------------
BOOL log_open( CONST CHAR *name )
{
	if( arrcheck( g_log_inst ) )
	{
		return TRUE;
	}

	//----------------------------------------------------------------

	arralloc_unit( g_log_inst );

	while( arrcheck( g_log_inst ) )
	{
		CHAR old_name[MAX_STRING];

		arrzero_unit( g_log_inst );

		//----------------------------------------------------------------
		
		g_log_inst->h_section = thr_section_open();
		if( !arrcheck( g_log_inst->h_section ) )
		{
			break;
		}

		//----------------------------------------------------------------

		snprintft( old_name, MAX_STRING, "%s.old", name );
		remove( old_name );
		rename( name, old_name );

		g_log_inst->fp = fopen( name, "w" );
		if( invalid_ptr( g_log_inst->fp ) )
		{
			break;
		}

		//----------------------------------------------------------------

		g_log_inst->dump	= LOG_DUMP;
		g_log_inst->delay	= 0;

		//----------------------------------------------------------------

		return TRUE;
	}

	log_close();
	return FALSE;
}


VOID log_close( VOID )
{
	if( arrcheck( g_log_inst ) )
	{
		thr_section_close( g_log_inst->h_section );

		if( !invalid_ptr( g_log_inst->fp ) )
		{
			fclose( g_log_inst->fp );
		}
	}

	arrfree( g_log_inst );
}


//================================================================
//  Logging function
//----------------------------------------------------------------
VOID log_lock( CONST CHAR *func_name, INT line_numb )
{
	if( arrcheck( g_log_inst ) )
	{
		LOCTIME ltime;
		INT n, len;

		CONST CHAR *ptr = func_name;
		CHAR name[MAX_STRING] = "";
		CHAR dump[MAX_STRING] = "";

		
		//----------------------------------------------------------------

		loopinf
		{
			CONST CHAR *tmp = strstr( ptr, "\\" );

			if( invalid_ptr( tmp ) )
			{
				tmp = strstr( ptr, "/" );
			}

			if( invalid_ptr( tmp ) )
			{
				break;
			}

			ptr = tmp + 1;
		}

		//----------------------------------------------------------------

		if( line_numb < 0 )
		{
			strncpyt( name, ptr, MAX_STRING );
		}
		else
		{
			snprintft( name, MAX_STRING, "%s, %4d", ptr, line_numb );
		}

		//----------------------------------------------------------------

		len = (INT)strlen( name );
		n   = g_log_inst->dump - len;

		if( n < 0 )
		{
			g_log_inst->dump += alignment( -n, LOG_DUMP_STEP );
			g_log_inst->delay = 0;
		}
		
		if( n > LOG_DUMP_STEP )
		{
			g_log_inst->delay ++;

			if( g_log_inst->delay > LOG_DUMP_DELAY )
			{
				g_log_inst->dump -= LOG_DUMP_STEP;
				g_log_inst->delay = 0;
			}
		}
		else
		{
			g_log_inst->delay = 0;
		}

		//----------------------------------------------------------------

		len = g_log_inst->dump - len;
		len = max( len, 0 );
		len = min( len, MAX_STRING - 1 );

		for( n = 0 ; n < len ; n ++ )
		{
			dump[n] = ' ';
		}

		dump[n] = STR_TERM;

		//----------------------------------------------------------------

		thr_section_enter( g_log_inst->h_section );
		thr_localtime( &ltime );

		snprintft
			(
				g_log_inst->prefix,
				MAX_STRING,
				"%02d/%02d/%d  %02d:%02d:%02d.%03d  [%s%s]  ",
				
				ltime.day,
				ltime.month,
				ltime.year,
				
				ltime.hour,
				ltime.minute,
				ltime.second,
				ltime.mseconds,

				dump,
				name
			);
	}
}


VOID log_unlock( CONST CHAR *fmt, ... )
{
	if( arrcheck( g_log_inst ) )
	{
		va_list vl;
		va_start( vl, fmt );
		
		//----------------------------------------------------------------

		fputs( g_log_inst->prefix, g_log_inst->fp );
		vfprintf( g_log_inst->fp, fmt, vl );
		
		fputc( '\n', g_log_inst->fp );
		fflush( g_log_inst->fp );

		//----------------------------------------------------------------

		thr_section_leave( g_log_inst->h_section );
		va_end( vl );
	}
}
