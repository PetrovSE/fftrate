//================================================================
//  Includes
//----------------------------------------------------------------
#include <string.h>

#include "../swin.h"
#include "../types.h"
#include "../array.h"
#include "../str.h"
#include "pluglib.h"


//================================================================
//  Local function
//----------------------------------------------------------------
STATIC BOOL plugin_local_set_default( PPLUGINSTATE p_plugin )
{
	INT merit = PLUG_MERIT_UNUSED;
	INT cnt;

	p_plugin->p_curr = NULL;

	for( cnt = 0 ; ; cnt ++ )
	{
		CONST PLUGINFO *p_system = p_plugin->p_sys[cnt];

		if( invalid_ptr( p_system ) )
			break;
		
		if( merit < p_system->merit )
		{
			merit				= p_system->merit;
			p_plugin->p_curr	= p_system;
		}
	}

	return !invalid_ptr( p_plugin->p_curr );
}


//================================================================
//  Plugin functions
//----------------------------------------------------------------
VOID plugin_addref( PPLUGINSTATE p_plugin )
{
	p_plugin->n_instances ++;
}


VOID plugin_release( PPLUGINSTATE p_plugin )
{
	if( p_plugin->n_instances )
		p_plugin->n_instances --;
}


BOOL plugin_check( PPLUGINSTATE p_plugin )
{
	if( !invalid_ptr( p_plugin->p_curr ) )
		return TRUE;

	return plugin_local_set_default( p_plugin );
}


BOOL plugin_select( PPLUGINSTATE p_plugin, CONST CHAR *system_name )
{
	INT cnt;

	//----------------------------------------------------------------

	if( non_zero( p_plugin->n_instances ) )
		return FALSE;

	//----------------------------------------------------------------

	if( invalid_ptr( system_name ) )
		return plugin_local_set_default( p_plugin );

	//----------------------------------------------------------------

	for( cnt = 0 ; ; cnt ++ )
	{
		CONST PLUGINFO *p_system = p_plugin->p_sys[cnt];

		if( invalid_ptr( p_system ) )
			break;

		if( p_system->merit == PLUG_MERIT_UNUSED )
			continue;

		if( strcmp( p_system->system_name, system_name ) == 0 )
		{
			p_plugin->p_curr = p_system;
			return TRUE;
		}
	}

	//----------------------------------------------------------------

	return FALSE;
}


CONST PLUGINFO *plugin_enum( PPLUGINSTATE p_plugin, INT system_number )
{
	INT cnt;

	for( cnt = 0 ; ; cnt ++ )
	{
		CONST PLUGINFO *p_system = (CONST PLUGINFO *)p_plugin->p_sys[cnt];

		if( invalid_ptr( p_system ) )
			break;

		if( p_system->merit == PLUG_MERIT_UNUSED )
			continue;

		if( system_number == 0 )
			return p_system;

		system_number --;
	}

	return NULL;
}


CONST PLUGINFO *plugin_info( PPLUGINSTATE p_plugin )
{
	if( !plugin_check( p_plugin ) )
		return NULL;

	return (PLUGINFO *)p_plugin->p_curr;
}


BOOL plugin_update_devinfo( PDEVINFO device_info )
{
	if( invalid_ptr( device_info ) )
		return FALSE;

	if( strlen( device_info->desc ) == 0 )
		strncpyt( device_info->desc, device_info->id, MAX_STRING );

	return TRUE;
}
