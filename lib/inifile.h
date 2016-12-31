#ifndef _INI_FILE_H_
#define _INI_FILE_H_


//================================================================
//  Includes
//----------------------------------------------------------------
#include "interface.h"


//================================================================
//  Definitions
//----------------------------------------------------------------
#define FILE_OPEN_READ				0x0
#define FILE_OPEN_WRITE				0x1


#define INI_VALTYPE_STOP			0x0000
#define INI_VALTYPE_COMMENT			0x0001

#define INI_VALTYPE_BOOL			0x0101
#define INI_VALTYPE_INT				0x0102
#define INI_VALTYPE_DATA			0x0104
#define INI_VALTYPE_STR				0x0108
#define INI_VALTYPE_WILD			0x0110


typedef enum
{
	INI_BOOL_STYLE_TRFL		= 0x0,
	INI_BOOL_STYLE_YESNO,
	INI_BOOL_STYLE_ENDIS,
	INI_BOOL_STYLE_ONOFF,
	INI_BOOL_STYLE_NUM,

} INI_BOOL_STYLE;


//================================================================
//  Type definitions
//----------------------------------------------------------------
typedef struct
{
	CONST CHAR		*name;
	DWORD			type;
	VOID			*p_data;
	DWORD			reserved;

} INIVAL, *PINIVAL;



//================================================================
//  Values pre-definitions
//----------------------------------------------------------------
#define INI_BEGIN( _n )					INIVAL _n[] = {
#define INI_END							STRUCT_TERMINATE };


#define SET_STOP( _name )				\
{										\
	_name,								\
	INI_VALTYPE_STOP,					\
}


#define SET_COMMENT( _name )			\
{										\
	_name,								\
	INI_VALTYPE_COMMENT,				\
}


#define SET_BOOL( _name, _val, _style )	\
{										\
	_name,								\
	INI_VALTYPE_BOOL,					\
	&(_val),							\
	_style,								\
}


#define SET_INT( _name, _val )			\
{										\
	_name,								\
	INI_VALTYPE_INT,					\
	&(_val),							\
}


#define SET_DATA( _name, _val )			\
{										\
	_name,								\
	INI_VALTYPE_DATA,					\
	&(_val),							\
}


#define SET_STR( _name, _val, _len )	\
{										\
	_name,								\
	INI_VALTYPE_STR,					\
	_val,								\
	_len,								\
}


#define SET_WILD( _val, _len )			\
{										\
	"",									\
	INI_VALTYPE_WILD,					\
	_val,								\
	_len,								\
}


//================================================================
//  Function prototypes
//----------------------------------------------------------------
CDECL_BEGIN

FILE *inifile_open( CONST CHAR *name, INT mode );
FILE *inifile_close( FILE *fp );

BOOL inifile_load( FILE *fp, PINIVAL p_values );
BOOL inifile_save( FILE *fp, CONST INIVAL *p_values );

CDECL_END

#endif // _INI_FILE_H_
