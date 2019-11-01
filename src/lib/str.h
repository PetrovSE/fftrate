#ifndef _STR_H_
#define _STR_H_


//================================================================
//  Includes
//----------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "interface.h"


//================================================================
//  Definitions
//----------------------------------------------------------------
enum
{
	BOOL_BOX_ONOFF = 0,
	BOOL_BOX_CHECK,
	BOOL_BOX_STAR
};

#define STR( _s )				(CHAR *)(_s)


//================================================================
//  Macros definitions
//----------------------------------------------------------------
#if defined(IS_LINUX)
#define FILE_SYS_SEP		'/'

#elif defined(IS_WIN32)
#define FILE_SYS_SEP		'\\'

#define strcasecmp			_stricmp
#define snprintf			_snprintf
#define vsnprintf			_vsnprintf

#define strupr				_strupr
#define strlwr				_strlwr

#else
#define FILE_SYS_SEP		'\\'
#define strcasecmp			strcmp
#endif


//================================================================
//  Command line for parse_arg: "NA NA NA"
//  where is:
//
//  N - type descriptor:
//     d - DWORD <- DWORD
//     w - WORD  <- DWORD
//
//     b - BOOL     <- BOOL
//     L - LONGLONG <- LONGLONG
//
//     p - PVOID <- PVOID
//
//  A - char ID
//----------------------------------------------------------------


//================================================================
//  Function prototypes
//----------------------------------------------------------------
CDECL_BEGIN

VOID str_clear_left( CHAR *str );
VOID str_clear_rght( CHAR *str );
VOID str_clear( CHAR *str );
VOID str_clear_comment( CHAR *str, CONST CHAR *comm );

BOOL str_only_digit( CONST CHAR *str );
BOOL str_replace( CHAR *str, INT len, CONST CHAR *templ1, CONST CHAR *templ2 );

INT symcasecmp( CHAR sym1, CHAR sym2 );

#ifdef IS_WIN32
CONST CHAR *strcasestr( CONST CHAR *str1, CONST CHAR *str2 );
#endif

CONST CHAR *str_fix_null( CONST CHAR *str );
CONST CHAR *bool_box( BOOL val, DWORD type );


#define STR_TERM					'\0'

#define is_digit( _c )				( (_c) >= '0' && (_c) <= '9' )
#define is_space( _c )				( (_c) == ' ' || (_c) == '\t' || (_c) == '\r' || (_c) == '\n' )
#define is_term( _c )				( (_c) == STR_TERM )

#define onoff_box( _v )				bool_box( _v, BOOL_BOX_ONOFF )
#define check_box( _v )				bool_box( _v, BOOL_BOX_CHECK )
#define star_box( _v )				bool_box( _v, BOOL_BOX_STAR )


VOID snprintft( CHAR *str, INT len, CONST CHAR *format, ... );
VOID strncpyt( CHAR *dst, CONST CHAR *src, INT len );
VOID strncatt( CHAR *dst, CONST CHAR *src, INT len );
CHAR *fgetst( CHAR *str, INT len, FILE *fp );


VOID parse_arg( va_list vl_t, CONST CHAR *types, CONST CHAR *format, ... );

#define parse_arg_start( _t, _f )	\
{									\
	va_list __vl;					\
	va_start( __vl, _t );			\
									\
	parse_arg						\
		(							\
			__vl,					\
			_t,						\
			_f,						\

#define parse_arg_stop				\
		);							\
	va_end( __vl );					\
}


CONST CHAR *get_filename( CONST CHAR *name );


#ifdef IS_LINUX
CHAR *strupr( CHAR *text );
CHAR *strlwr( CHAR *text );
#endif

CDECL_END

#endif // _STR_H_
