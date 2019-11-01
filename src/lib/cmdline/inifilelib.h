#ifndef _INI_FILE_LIB_H_
#define _INI_FILE_LIB_H_


//================================================================
//  Definitions
//----------------------------------------------------------------
#define INI_IDX_TRUE				0x0
#define INI_IDX_FALSE				0x1


#define INI_DATA( _p )				( (_p)->p_data )
#define INI_DATA_IS_VALID( _p )		( !invalid_ptr( INI_DATA( _p ) ) )

#define INI_DATA_BOOL( _p )			( *( (BOOL *)INI_DATA( _p ) ) )
#define INI_DATA_INT( _p )			( *( (INT *)INI_DATA( _p ) ) )
#define INI_DATA_DATA( _p )			( *( (DATA *)INI_DATA( _p ) ) )
#define INI_DATA_STR( _p )			( (CHAR *)INI_DATA( _p ) )

#define INI_DEF_COMMENT				"#"


//================================================================
//  Type definitions
//----------------------------------------------------------------
typedef struct
{
	CONST CHAR	*name[2];

} INIBOOLPAIR, *PINIBOOLPAIR;

#endif // _INI_FILE_LIB_H_
