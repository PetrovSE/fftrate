//================================================================
//  Includes
//----------------------------------------------------------------
#include <stdio.h>
#include <math.h>

#include "../../lib/swin.h"
#include "../../lib/types.h"
#include "../../lib/array.h"
#include "../../lib/inifile.h"


//================================================================
//  RAM
//----------------------------------------------------------------
#define N_OF_WILD				4

STATIC CHAR name[N_OF_WILD][MAX_STRING] = { "" };

STATIC CHAR		val_str[MAX_STRING] = "";
STATIC BOOL		val_bool;
STATIC INT		val_int;
STATIC DATA		val_data;


//================================================================
//  Descriptor
//----------------------------------------------------------------
STATIC INI_BEGIN( ini_values )
	SET_COMMENT( "//" ),
	SET_STOP( "stop" ),

	SET_STR( "val_str",	val_str, MAX_STRING ),
	SET_BOOL( "val_bool",	val_bool, INI_BOOL_STYLE_ENDIS ),
	SET_INT( "val_int",	val_int	),
	SET_DATA( "val_data", val_data ),

	SET_WILD( name[0], MAX_STRING ),
	SET_WILD( name[1], MAX_STRING ),
	SET_WILD( name[2], MAX_STRING ),
	SET_WILD( name[3], MAX_STRING ),
INI_END	


//================================================================
//  Main
//----------------------------------------------------------------
INT main( VOID )
{
	FILE *fp;
	INT n;

	//================================================================

	printf( "Open ini file... " );

	fp = inifile_open( "test.ini", FILE_OPEN_READ );
	if( invalid_ptr( fp ) )
	{
		printf( "Failed!\n" );
		return 1;
	}

	printf( "Ok.\n" );

	inifile_load( fp, ini_values );
	fp = inifile_close( fp );

	printf( "\nval_str: %s\n", val_str );
	printf( "val_bool: %d\n", val_bool );
	printf( "val_int: %d\n", val_int );
	printf( "val_data: %f\n", val_data );


	for( n = 0 ; n < N_OF_WILD ; n ++ )
		printf( "name[%d]: %s\n", n, name[n] );

	//================================================================

	printf( "\nCreate ini file... " );
	
	fp = inifile_open( "test_out.ini", FILE_OPEN_WRITE );
	if( invalid_ptr( fp ) )
	{
		printf( "Failed!\n" );
		return 1;
	}
	
	printf( "Ok.\n" );

	inifile_save( fp, ini_values );
	fp = inifile_close( fp );

	return 0;
}
