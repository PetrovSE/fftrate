#ifndef _PLUG_LIB_H_
#define _PLUG_LIB_H_

//================================================================
// Includes
//----------------------------------------------------------------
#include "../plug.h"


//================================================================
//  Definitions
//----------------------------------------------------------------
#define MAX_PLUGINS						32


//================================================================
//  Type definitions
//----------------------------------------------------------------
typedef struct
{
	CONST VOID	*p_sys[MAX_PLUGINS];
	CONST VOID	*p_curr;
	INT			n_instances;
	
} PLUGINSTATE, *PPLUGINSTATE;


//================================================================
//  Macros definitions
//----------------------------------------------------------------
#define PLUG( _syst, _plug, _fn )			( (_syst *)(_plug).p_curr )->_fn

//----------------------------------------------------------------

#define PLUG_FN_TYPE( _fn )					( *fp_##_fn )
#define PLUG_FN_DESC( _fn )					fp_##_fn			_fn

//----------------------------------------------------------------

#define PLUG_INTERFACE_START				\
typedef struct								\
{											\
	CONST PLUGINFO	info;

#define PLUG_INTERFACE_STOP( _syst )		\
} _syst, *P ## _syst;						\

//----------------------------------------------------------------

#define PLUG_ENUM_START( _plug )			\
STATIC PLUGINSTATE _plug = {	{
#define PLUG_ENUM_STOP						}	};

//----------------------------------------------------------------

#define PLUG_MODULE_EXTERN( _syst, _mod )	EXTERN CONST _syst	_mod

//----------------------------------------------------------------

#define PLUG_MODULE_START( _syst, _mod, _merit, _name, _desc, _def_dev )	\
CONST _syst _mod =															\
{																			\
	{																		\
		_name,																\
		_desc,																\
		_def_dev,															\
		_merit,																\
	},
	
#define PLUG_MODULE_STOP					};

#define PLUG_MODULE_PURE( _syst, _mod )		\
CONST _syst _mod =							\
{											\
	{										\
		"",									\
		"",									\
		"",									\
		PLUG_MERIT_UNUSED,					\
	},										\
											\
	NULL,									\
}

//----------------------------------------------------------------

#define PLUG_FUNCTIONS( _id, _plug )						\
BOOL _id ## _system_select( CONST CHAR *system_name )		\
{															\
	return plugin_select( _plug, system_name );				\
}															\
															\
CONST PLUGINFO *_id ## _system_enum( INT system_number )	\
{															\
	return plugin_enum( _plug, system_number );				\
}															\
															\
CONST PLUGINFO *_id ## _system_info( VOID )					\
{															\
	return plugin_info( _plug );							\
}


//================================================================
//  Function prototypes
//----------------------------------------------------------------
VOID plugin_addref( PPLUGINSTATE p_plugin );
VOID plugin_release( PPLUGINSTATE p_plugin );

BOOL plugin_check( PPLUGINSTATE p_plugin );
BOOL plugin_select( PPLUGINSTATE p_plugin, CONST CHAR *system_name );

CONST PLUGINFO *plugin_enum( PPLUGINSTATE p_plugin, INT system_number );
CONST PLUGINFO *plugin_info( PPLUGINSTATE p_plugin );

BOOL plugin_update_devinfo( PDEVINFO device_info );

#endif // _PLUG_LIB_H_
