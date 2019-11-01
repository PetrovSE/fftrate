//================================================================
//  Includes
//----------------------------------------------------------------
#include <stdio.h>

#include "../swin.h"
#include "../types.h"
#include "../array.h"
#include "../mathex.h"
#include "../str.h"
#include "../cmdline.h"


//================================================================
//  Get module information
//----------------------------------------------------------------
LIBINFO_FUNCTION
(
	cmdline,
	"CmdLine",
	"Command line support functions",
	"Copyright (c) 2007-16 PetrovSE",
	"1.0.4.8"
);


//================================================================
//  Definitions
//----------------------------------------------------------------
#define ADD_SYMB( _p, _c )			( * --(_p) ) = (_c);
#define PROGRESS_LEN				50


//================================================================
//  RAM
//----------------------------------------------------------------
STATIC INT		g_progress_state			= -1;
STATIC CHAR		g_prev_title[MAX_STRING]	= "";

STATIC CHAR		g_fill_prev;
STATIC CHAR		g_fill_curr;
STATIC CHAR		g_fill_next;


//================================================================
//  Debug functions
//----------------------------------------------------------------
VOID printf_null( CONST CHAR *fmt, ... )
{
	unref_param( fmt );
}


//================================================================
//  Print functions
//----------------------------------------------------------------
VOID fprintfill( FILE *fp, CHAR ch, INT len )
{
	for( ; len > 0 ; len -- )
		fputc( ch, fp );
}


//================================================================
//  Screen functions
//----------------------------------------------------------------
VOID clrscr( VOID )
{
#if defined(IS_LINUX)
	unref_result( system( "clear" ) );
#elif defined(IS_WIN32)
	system( "cls" );
#endif
}


//================================================================
//  Digit funstion
//----------------------------------------------------------------
VOID number_to_str( LONGLONG val, CHAR *str, INT len )
{
	CHAR text[MAX_STRING] = { 0 };
	CHAR *last = &text[MAX_STRING - 1];

	INT sign = 0;
	INT cnt;

	//----------------------------------------------------------------

	if( less_zero( val ) )
	{
		val  = -val;
		sign = 1;
	}

	//----------------------------------------------------------------

	for( cnt = 0 ; ; cnt ++ )
	{
		CHAR ch = (CHAR)( val % 10 + '0' );

		if( is_zero( val ) && cnt )
			break;

		ADD_SYMB( last, ch );
		val /= 10;

		if( val && ( cnt % 3 ) == 2 )
			ADD_SYMB( last, '.' );
	}

	//----------------------------------------------------------------

	if( sign )
		ADD_SYMB( last, '-' );

	//----------------------------------------------------------------

	strncpyt( str, last, len );
}


//================================================================
//  Print help
//----------------------------------------------------------------
STATIC VOID cmdline_local_print_info( CONST CHAR *title, CONST CHAR *info )
{
	if( !invalid_ptr( info ) )
		fprintf( stderr, "%s: %s\n", title, info );
}


VOID cmdline_print_infos( CONST LIBINFO_POINTER *p_fn_infos, INT number )
{
	if( is_zero( number ) )
		return;

	fprintf( stderr, "=================================================================\n" );
	fprintf( stderr, "Module Info:\n\n" );

	if( !invalid_ptr( p_fn_infos ) )
	{
		INT cnt;

		for( cnt = 0 ; cnt < number ; cnt ++ )
		{
			if( cnt )
				fprintf( stderr, "\n" );

			cmdline_local_print_info( "Short name", p_fn_infos[cnt]( LIBINFO_SHORTNAME ) );
			cmdline_local_print_info( "Long name ", p_fn_infos[cnt]( LIBINFO_LONGNAME ) );
			cmdline_local_print_info( "Copyright ", p_fn_infos[cnt]( LIBINFO_COPYRIGHT ) );
			cmdline_local_print_info( "Version   ", p_fn_infos[cnt]( LIBINFO_VERSION ) );
			cmdline_local_print_info( "Build     ", p_fn_infos[cnt]( LIBINFO_BUILD ) );
		}
	}

	fprintf( stderr, "=================================================================\n\n" );
}


VOID cmdline_print_help( CONST HELPTXT *p_help, CONST CHAR *self, INT n_layer )
{
	INT n_count;

	for( n_count = 0 ; ; n_count ++ )
	{
		CONST HELPTXT *ptr = &p_help[n_count];
		CHAR *text;

		if( invalid_ptr( ptr->text ) )
			break;

		if( n_layer < ptr->layer )
			continue;

		if( self )
		{
			INT len = ( (INT)strlen( ptr->text ) + 1 ) * 2;
			arralloc( text, len );
			if( !arrcheck( text ) )
				continue;

			strncpyt( text, ptr->text, len );
			str_replace( text, len, SLFNAME, get_filename( self ) );
		}
		else
			text = (CHAR *)ptr->text;

		fprintf( stderr, "%s\n", text );

		if( text != ptr->text )
			arrfree( text );
	}
}


//================================================================
//  Progress function
//----------------------------------------------------------------
STATIC VOID cmdline_progress_fill( INT done )
{
	fputs( "\r[", stderr );

	done = max( done, 0 );
	done = min( done, PROGRESS_LEN );

	fprintfill( stderr, g_fill_prev, done );
	fputc( g_fill_curr, stderr );
	fprintfill( stderr, g_fill_next, PROGRESS_LEN - done );

	fputs( "] ", stderr );
}


VOID cmdline_progress_init( CONST CHAR *fills )
{
	g_progress_state = -1;
	strncpyt( g_prev_title, "", MAX_STRING );

	g_fill_prev	= fills[0];
	g_fill_curr	= fills[1];
	g_fill_next	= fills[2];

	cmdline_progress_fill( 0 );
}


VOID cmdline_progress_show( DWORD proc, DWORD size, CONST CHAR *title )
{
	DATA percent = (DATA)proc / (DATA)size;
	INT  state   = (INT)( PROGRESS_LEN * percent + 0.5 );

	if
	(
		g_progress_state != state
		||
		( !invalid_ptr( title ) && strncmp( g_prev_title, title, MAX_STRING ) )
	)
	{
		g_progress_state = state;
		cmdline_progress_fill( state );

		if( !invalid_ptr( title ) )
		{
			strncpyt( g_prev_title, title, MAX_STRING );
			fprintf( stderr, "%s", title );
		}
		else
			fprintf( stderr, "%5.1f %%", 100.0f * percent );

		fflush( stderr );
	}
}


VOID cmdline_progress_done( CONST CHAR *title )
{
	CONST CHAR *text = "Ok.";

	if( !invalid_ptr( title ) )
		text = title;

	fprintf( stderr, "\n%s\n", text );
}
