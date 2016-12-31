//================================================================
//  Includes
//----------------------------------------------------------------
#include <stdio.h>
#include <string.h>

#include "../swin.h"
#include "../types.h"
#include "../array.h"
#include "../mathex.h"
#include "../inifile.h"
#include "../str.h"

#include "inifilelib.h"


//================================================================
//  ROM
//----------------------------------------------------------------
STATIC CONST INIBOOLPAIR g_ini_bool_pairs[] =
{
	{	{ "true",		"false"		}	},	// INI_BOOL_STYLE_TRFL
	{	{ "yes",		"no"		}	},	// INI_BOOL_STYLE_YESNO
	{	{ "enable",		"disable"	},	},	// INI_BOOL_STYLE_ENDIS
	{	{ "on",			"off"		},	},	// INI_BOOL_STYLE_ONOFF
	{	{ "1",			"0"			},	},	// INI_BOOL_STYLE_NUM
};

STATIC CONST INT g_n_of_ini_pairs = numelems( g_ini_bool_pairs );


//================================================================
//  Common functions
//----------------------------------------------------------------
STATIC VOID get_name( CONST CHAR *str, CHAR *name, INT len )
{
	CHAR *ptr;

	strncpyt( name, str, len );
	ptr = strstr( name, "=" );

	if( !invalid_ptr( ptr ) )
	{
		*ptr = STR_TERM;
		str_clear_rght( name );
	}
}


STATIC VOID get_arg( CONST CHAR *str, CHAR *arg, INT len )
{
	CHAR *ptr;
	
	strncpyt( arg, str, len );
	ptr = strstr( arg, "=" );

	if( !invalid_ptr( ptr ) )
	{
		strncpyt( arg, ptr + 1, len );
		str_clear_left( arg );
	}
	else
		arg[0] = STR_TERM;
}


STATIC BOOL get_boolean_val( CONST CHAR *val )
{
	INT n;

	if( strlen( val ) == 0 )
		return TRUE;

	for( n = 0 ; n < g_n_of_ini_pairs ; n ++ )
	{
		CONST INIBOOLPAIR *p_pair = &g_ini_bool_pairs[n];

		if( strcasecmp( val, p_pair->name[INI_IDX_TRUE] ) == 0 )
			return TRUE;

		if( strcasecmp( val, p_pair->name[INI_IDX_FALSE] ) == 0 )
			return FALSE;
	}

	return atoi( val ) != 0;
}


//================================================================
//  Open & Close functions
//----------------------------------------------------------------
FILE *inifile_open( CONST CHAR *name, INT mode )
{
	CONST CHAR *smode;

	switch( mode )
	{
	case FILE_OPEN_READ:
		smode = "r";
		break;

	case FILE_OPEN_WRITE:
		smode = "w";
		break;
		
	default:
		return NULL;
	}

	return fopen( name, smode );
}


FILE *inifile_close( FILE *fp )
{
	if( !invalid_ptr( fp ) )
		fclose( fp );

	return NULL;
}


//================================================================
//  Load function
//----------------------------------------------------------------
BOOL inifile_load( FILE *fp, PINIVAL p_values )
{

	INT n_wild_count = 0;

	while( !invalid_ptr( fp ) && !invalid_ptr( p_values ) )
	{
		CONST CHAR *p_comment = INI_DEF_COMMENT;
		CONST INIVAL *p_enum;

		for( p_enum = p_values ; !invalid_ptr( p_enum->name ) ; p_enum ++ )
		{
			if( p_enum->type == INI_VALTYPE_COMMENT )
			{
				p_comment = p_enum->name;
				break;
			}
		}

		//----------------------------------------------------------------

		loopinf
		{
			CHAR in_str[MAX_STRING];
			CHAR name[MAX_STRING];
			CHAR arg[MAX_STRING];

			//----------------------------------------------------------------

			if( invalid_ptr( fgetst( in_str, MAX_STRING, fp ) ) )
				break;

			//----------------------------------------------------------------

			str_clear_comment( in_str, p_comment );
			str_clear( in_str );

			get_name( in_str, name, MAX_STRING );
			get_arg( in_str, arg, MAX_STRING );
			
			if( strlen( name ) == 0 )
				continue;

			//----------------------------------------------------------------

			for( p_enum = p_values ; !invalid_ptr( p_enum->name ) ; p_enum ++ )
			{
				if( strcasecmp( name, p_enum->name ) == 0 )
				{
					switch( p_enum->type )
					{
					case INI_VALTYPE_STOP:
						return TRUE;


					case INI_VALTYPE_BOOL:
						if( INI_DATA_IS_VALID( p_enum ) )
							INI_DATA_BOOL( p_enum ) = get_boolean_val( arg );
						break;


					case INI_VALTYPE_INT:
						if( INI_DATA_IS_VALID( p_enum ) )
							INI_DATA_INT( p_enum ) = atoi( arg );
						break;

					
					case INI_VALTYPE_DATA:
						if( INI_DATA_IS_VALID( p_enum ) )
							INI_DATA_DATA( p_enum ) = (DATA)atof( arg );
						break;


					case INI_VALTYPE_STR:
						if( INI_DATA_IS_VALID( p_enum ) )
							strncpyt( INI_DATA_STR( p_enum ), arg, p_enum->reserved );
						break;
					}
					
					break;
				}
			}


			//----------------------------------------------------------------

			if( invalid_ptr( p_enum->name ) )
			{
				INT count = n_wild_count;

				for( p_enum = p_values ; !invalid_ptr( p_enum->name ) ; p_enum ++ )
				{
					if( p_enum->type == INI_VALTYPE_WILD && INI_DATA_IS_VALID( p_enum ) )
					{
						if( count == 0 )
						{
							strncpyt( INI_DATA_STR( p_enum ), name, p_enum->reserved );
							n_wild_count ++;
							break;
						}

						count --;
					}
				}
			}

			//----------------------------------------------------------------
		}

		return TRUE;
	}

	return FALSE;
}


//================================================================
//  Save function
//----------------------------------------------------------------
BOOL inifile_save( FILE *fp, CONST INIVAL *p_values )
{
	while( !invalid_ptr( fp ) && !invalid_ptr( p_values ) )
	{
		CONST INIVAL *p_enum = p_values;

		for( ; !invalid_ptr( p_enum->name ) ; p_enum ++ )
		{
			if( !INI_DATA_IS_VALID( p_enum ) )
				continue;

			switch( p_enum->type )
			{
			case INI_VALTYPE_BOOL:
				{
					INI_BOOL_STYLE style = p_enum->reserved;
					INT idx   = INI_DATA_BOOL( p_enum ) ? INI_IDX_TRUE : INI_IDX_FALSE;

					style = max( style, 0 );
					style = min( style, g_n_of_ini_pairs - 1 );

					fprintf( fp, "%s = %s\n", p_enum->name, g_ini_bool_pairs[style].name[idx] );
				}
				break;


			case INI_VALTYPE_INT:
				fprintf( fp, "%s = %d\n", p_enum->name, INI_DATA_INT( p_enum ) );
				break;


			case INI_VALTYPE_DATA:
				fprintf( fp, "%s = %g\n", p_enum->name, INI_DATA_DATA( p_enum ) );
				break;


			case INI_VALTYPE_STR:
				fprintf( fp, "%s = %s\n", p_enum->name, INI_DATA_STR( p_enum ) );
				break;

				
			case INI_VALTYPE_WILD:
				fprintf( fp, "%s\n", INI_DATA_STR( p_enum ) );
				break;
			}
		}

		return TRUE;
	}

	return FALSE;
}
