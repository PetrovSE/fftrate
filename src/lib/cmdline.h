#ifndef _CMDHELP_H_
#define _CMDHELP_H_


//================================================================
//  Includes
//----------------------------------------------------------------
#include "interface.h"


//================================================================
//  Definitions
//----------------------------------------------------------------
#define HELPTXT_BEGIN( _name )			CONST HELPTXT _name[] = {
#define HELPTXT_END						{ 0, NULL } };
#define HELPTXT_ITEM( _l, _t )			{ _l, _t, }


#define PROGRESS_FILL_PROGR				"|| "
#define PROGRESS_FILL_SLIDER			"-#-"

#define SLFNAME							"#SELF_NAME"


//================================================================
//  Type definitions
//----------------------------------------------------------------
typedef struct
{
	INT			layer;
	CONST CHAR	*text;

} HELPTXT, *PHELPTXT;


//================================================================
//  Function prototypes
//----------------------------------------------------------------
CDECL_BEGIN

LIBINFO_PROTOTYPE( cmdline );

VOID printf_null( CONST CHAR *fmt, ... );

#ifdef DEBUGGING
#define dbg_printf				printf
#else
#define dbg_printf				printf_null
#endif

VOID fprintfill( FILE *fp, CHAR ch, INT len );
VOID clrscr( VOID );

VOID number_to_str( LONGLONG val, CHAR *str, INT len );

VOID cmdline_print_infos( CONST LIBINFO_POINTER *p_fn_infos, INT number );
VOID cmdline_print_help( CONST HELPTXT *p_help, CONST CHAR *self, INT n_layer );

VOID cmdline_progress_init( CONST CHAR *fills );
VOID cmdline_progress_show( DWORD proc, DWORD size, CONST CHAR *title );
VOID cmdline_progress_done( CONST CHAR *title );

CDECL_END

#endif // _CMDHELP_H_
