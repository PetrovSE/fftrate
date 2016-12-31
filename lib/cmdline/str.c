//================================================================
//  Includes
//----------------------------------------------------------------
#include <stdio.h>
#include <ctype.h>

#include "../swin.h"
#include "../types.h"
#include "../array.h"
#include "../str.h"

#include "strlib.h"


//================================================================
//  ROM
//----------------------------------------------------------------
STATIC CONST BOOLBOX g_check_box[] =
{
	{	"OFF",	"ON",	},
	{	"[ ]",	"[X]",	},
	{	" ",	"*",	},
};


//================================================================
//  String functions
//----------------------------------------------------------------
VOID str_clear_left( CHAR *str )
{
	CHAR *ptr = str;

	for( ; is_space( *ptr ) && !is_term( *ptr ) ; ptr ++ );

	if( ptr != str )
		arrmove( str, ptr, strlen( ptr ) + 1 );
}


VOID str_clear_rght( CHAR *str )
{
	CHAR *ptr = &str[strlen(str)];

	for( ; ptr > str ; ptr -- )
	{
		CHAR ch = ptr[-1];

		if( !is_space( ch ) )
			break;
	}

	*ptr = STR_TERM;
}


VOID str_clear( CHAR *str )
{
	str_clear_left( str );
	str_clear_rght( str );
}


VOID str_clear_comment( CHAR *str, CONST CHAR *comm )
{
	CHAR *ptr = strstr( str, comm );

	if( !invalid_ptr( ptr ) )
		*ptr = STR_TERM;
}


BOOL str_only_digit( CONST CHAR *str )
{
	loopinf
	{
		CHAR ch = *str ++;

		if( is_term( ch ) )
			break;

		if( !is_digit( ch ) )
			return FALSE;
	}

	return TRUE;
}


BOOL str_replace( CHAR *str, INT len, CONST CHAR *templ1, CONST CHAR *templ2 )
{
	CHAR *start = str;
	INT len1 = (INT)strlen( templ1 );
	INT len2 = (INT)strlen( templ2 );

	loopinf
	{
		CHAR *ptr = strstr( start, templ1 );

		if( invalid_ptr( ptr ) )
			break;

		if( ( (INT)strlen( str ) + len2 - len1 ) >= len )
			return FALSE;

		start = &ptr[len2];
		arrmove( start, &ptr[len1], strlen( &ptr[len1] ) + 1 );
		arrmove( ptr, templ2, len2 );
	}

	return TRUE;
}


INT symcasecmp( CHAR sym1, CHAR sym2 )
{
	CHAR str1[2] = { sym1, STR_TERM };
	CHAR str2[2] = { sym2, STR_TERM };

	return strcasecmp( str1, str2 );
}


#ifdef IS_WIN32
CONST CHAR *strcasestr( CONST CHAR *str1, CONST CHAR *str2 )
{
	INT len1 = (INT)strlen( str1 );
	INT len2 = (INT)strlen( str2 );
	INT pos;

	for( pos = 0 ; pos <= ( len1 - len2 ) ; pos ++ )
	{
		INT n;

		for( n = 0 ; n < len2 ; n ++ )
		{
			if( symcasecmp( str1[pos + n], str2[n] ) )
				break;
		}

		if( n == len2 )
			return &str1[pos];
	}

	return NULL;
}
#endif

//================================================================
//  Template functions
//----------------------------------------------------------------
CONST CHAR *str_fix_null( CONST CHAR *str )
{
	return invalid_ptr( str ) ? "" : str;
}


CONST CHAR *bool_box( BOOL val, DWORD type )
{
	CONST BOOLBOX *box;

	switch( type )
	{
	case BOOL_BOX_ONOFF:
		box = &g_check_box[0];
		break;

	case BOOL_BOX_CHECK:
		box = &g_check_box[1];
		break;

	case BOOL_BOX_STAR:
		box = &g_check_box[2];
		break;

	default:
		return "";
	}

	return val ? box->s_true : box->s_false;
}


//================================================================
//  I/O functions
//----------------------------------------------------------------
STATIC VOID str_terminate( CHAR *str, INT len )
{
	if( more_zero( len ) )
		str[len - 1] = STR_TERM;
}


VOID snprintft( CHAR *str, INT len, CONST CHAR *format, ... )
{
	va_list vl;
	va_start( vl, format );

	vsnprintf( str, len, format, vl );
	str_terminate( str, len );

	va_end( vl );
}


VOID strncpyt( CHAR *dst, CONST CHAR *src, INT len )
{
	if( more_zero( len ) )
	{
		arrmove( dst, src, min( len, (INT)strlen( src ) + 1 ) );
		str_terminate( dst, len );
	}
}


VOID strncatt( CHAR *dst, CONST CHAR *src, INT len )
{
	if( more_zero( len ) )
	{
		INT pos  = min( len, (INT)strlen( dst ) + 1 ) - 1;
		INT size = min( len - pos, (INT)strlen( src ) + 1 );

		arrmove( &dst[pos], src, size );
		str_terminate( dst, len );
	}
}


CHAR *fgetst( CHAR *str, INT len, FILE *fp )
{
	if( fgets( str, len, fp ) == NULL )
		return NULL;

	str_terminate( str, len );
	return str;
}


//================================================================
//  Parse arguments function
//----------------------------------------------------------------
VOID parse_arg( va_list vl_t, CONST CHAR *types, CONST CHAR *format, ... )
{
	if
	(
		!invalid_ptr( types ) 
		&&
		!invalid_ptr( format )
	)
	{
		for( ; *types ; types ++ )
		{
			CONST CHAR *fmt = format;

			va_list vl_f;
			va_start( vl_f, format );

			//----------------------------------------------------------------

			loopinf
			{
				for( ; *fmt && ( *fmt == ' ' || *fmt == '\t' ) ; fmt ++ );
				if( strlen( fmt ) < 2 )
					break;

				//----------------------------------------------------------------

				if( fmt[1] == *types )
				{
					switch( fmt[0] )
					{
					case 'd':
						COPY_ARGS_BY_TYPE( DWORD, DWORD, vl_f, vl_t );
						break;

					case 'w':
						COPY_ARGS_BY_TYPE( WORD, DWORD, vl_f, vl_t );
						break;

					case 'b':
						COPY_ARGS_BY_TYPE( BOOL, BOOL, vl_f, vl_t );
						break;

					case 'L':
						COPY_ARGS_BY_TYPE( LONGLONG, LONGLONG, vl_f, vl_t );
						break;

					case 'p':
						COPY_ARGS_BY_TYPE( PVOID, PVOID, vl_f, vl_t );
						break;
					}

					break;
				}

				//----------------------------------------------------------------

				fmt += 2;
				va_arg( vl_f, PVOID );
			}

			va_end( vl_f );
		}
	}
}


//================================================================
//  File name functions
//----------------------------------------------------------------
CONST CHAR *get_filename( CONST CHAR *name )
{
	CONST CHAR sep[2] = { FILE_SYS_SEP, STR_TERM };

	loopinf
	{
		CONST CHAR *ptr = strstr( name, sep );

		if( invalid_ptr( ptr ) )
			break;

		name = ptr + 1;
	}

	return name;
}


//================================================================
//  To upper or lower case
//----------------------------------------------------------------
#ifdef IS_LINUX

CHAR *strupr( CHAR *text )
{
	CHAR *ptr = text;

	for( ; *ptr ; ptr ++ )
	{
		*ptr = toupper( *ptr );
	}

	return text;
}


CHAR *strlwr( CHAR *text )
{
	CHAR *ptr = text;

	for( ; *ptr ; ptr ++ )
		*ptr = tolower( *ptr );

	return text;
}

#endif // IS_LINUX
