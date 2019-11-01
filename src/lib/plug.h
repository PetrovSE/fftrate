#ifndef _PLUG_H_
#define _PLUG_H_

//================================================================
//  Definitions
//----------------------------------------------------------------
#define PLUG_MERIT_UNUSED			0
#define PLUG_MERIT_LOW				10
#define PLUG_MERIT_DEF				20


//================================================================
//  Type definitions
//----------------------------------------------------------------
typedef struct
{
	CONST CHAR	*system_name;
	CONST CHAR	*system_desc;
	CONST CHAR	*default_dev;
	INT			merit;

} PLUGINFO, *PPLUGINFO;

typedef struct
{
	CHAR	id[MAX_STRING];
	CHAR	desc[MAX_STRING];
	
} DEVINFO, *PDEVINFO;


//================================================================
//  Macros definitions
//----------------------------------------------------------------
#define PLUG_PROTOTYPE( _plug )									\
BOOL _plug ## _system_select( CONST CHAR *system_name );		\
CONST PLUGINFO * _plug ## _system_enum( INT system_number );	\
CONST PLUGINFO * _plug ## _system_info( VOID );

#endif // _PLUG_H_
