//================================================================
//  Includes
//----------------------------------------------------------------
#include <stdio.h>

#include "../swin.h"
#include "../types.h"
#include "../array.h"
#include "../cmdline.h"
#include "../str.h"
#include "../thr.h"
#include "../kbhit.h"
#include "../menu.h"


//================================================================
// ROM
//----------------------------------------------------------------
CONST MENUITEM MenuSeparator = ITEM_SEPARATOR;


//================================================================
// RAM
//----------------------------------------------------------------
STATIC BOOL g_stop_menu = FALSE;


//================================================================
// Work functions
//----------------------------------------------------------------
STATIC INT menu_local_len( CONST MENUITEM *items )
{
	INT len;
	for( len = 0 ; !invalid_ptr( items[len].desc ) ; len ++ );
	return len;
}


STATIC VOID menu_local_print_key( WORD sym )
{
	switch( sym )
	{
	case KEY_ESC:
		printf( "ESC" );
		break;

	case KEY_F( 1 ):
	case KEY_F( 2 ):
	case KEY_F( 3 ):
	case KEY_F( 4 ):
	case KEY_F( 5 ):
	case KEY_F( 6 ):
	case KEY_F( 7 ):
	case KEY_F( 8 ):
	case KEY_F( 9 ):
	case KEY_F( 10 ):
	case KEY_F( 11 ):
	case KEY_F( 12 ):
		printf( "F%c", get_low_byte( sym ) - KEY_FN_SHIFT + '0' );
		break;

	default:
		printf( "%c", (CHAR)sym );
	}
}


//================================================================
// Menu functions
//----------------------------------------------------------------
VOID menu_print( CONST MENU *menu )
{
	CONST MENUITEM *item = menu->items;
	INT len = (INT)( strlen( menu->title ) + strlen( GAP_IN ) + strlen( GAP_OUT ) );
	
	//----------------------------------------------------------------
	
	printf( "\n" );
	fprintfill( stdout, '=', len );
	
	printf( "\n%s%s%s\n", GAP_IN, menu->title, GAP_OUT );
	
	fprintfill( stdout, '-', len );
	printf( "\n" );
	
	//----------------------------------------------------------------
	
	while( !invalid_ptr( item->desc ) )
	{
		if( item->sym != MENU_NULL_SYM )
		{
			menu_local_print_key( item->sym );
			printf( " - " );
		}

		printf( "%s", item->desc );
			
		if( !invalid_ptr( item->f_title ) )
		{
			CHAR title[MAX_STRING] = "";
			
			item->f_title( item, title, MAX_STRING );
			printf( "%s", title );
		}
		
		printf( "\n" );
		item ++;
	}

	printf( "> " );
	fflush( stdout );
}



STATIC CONST MENUITEM *menu_wait( CONST MENU *menu )
{
	keyb_mode_set( KEYB_MODE_PRESS | KEYB_MODE_NOECHO );

	loopinf
	{
		BOOL b_repaint = TRUE;

		if( !invalid_ptr( menu->f_runtime ) )
			b_repaint = menu->f_runtime();

		if( b_repaint )
			menu_print( menu );

		//----------------------------------------------------------------

		if( !more_zero( menu->timeout ) || kbhit() )
		{
			CONST MENUITEM *item = menu->items;
			WORD wh = getkey();

			while( wh != MENU_NULL_SYM && !invalid_ptr( item->desc ) )
			{
				if( wh == item->sym || symcasecmp( (CHAR)item->sym, (CHAR)wh ) == 0 )
				{
					menu_local_print_key( wh );
					printf( "\n" );

					keyb_mode_reset();
					return item;
				}

				item ++;
			}

			continue;
		}

		//----------------------------------------------------------------

		if( more_zero( menu->timeout ) )
			thr_sleep( menu->timeout );
	}
	
	return NULL;
}


VOID menu_stop( VOID )
{
	g_stop_menu = TRUE;
}


BOOL menu_parse( CONST MENU *menu )
{
	g_stop_menu = FALSE;

	while( !g_stop_menu )
	{
		CONST MENUITEM *item = menu_wait( menu );

		if( invalid_ptr( item ) )
			return FALSE;

		if( invalid_ptr( item->desc ) )
			continue;

		if( !invalid_ptr( item->f_do ) )
			item->f_do( item );

		if( !invalid_ptr( item->next_menu ) && !invalid_ptr( item->next_menu->items[0].desc ) )
			menu = item->next_menu;
	}

	return TRUE;
}


//================================================================
// Menu keyboard functions
//----------------------------------------------------------------
BOOL menu_yes_no( CONST CHAR *quest )
{
	BOOL ret = FALSE;
	CHAR ch;

	//----------------------------------------------------------------

	if( !invalid_ptr( quest ) )
		printf( "%s", quest );

	//----------------------------------------------------------------

	keyb_mode_set( KEYB_MODE_PRESS | KEYB_MODE_NOECHO );

	loopinf
	{
		WORD wh = getkey();
		ch = (CHAR)wh;

		if( wh != ch )
			continue;

		if( symcasecmp( ch, 'Y' ) == 0 )
		{
			ret = TRUE;
			break;
		}

		if( symcasecmp( ch, 'N' ) == 0 )
		{
			break;
		}
	}

	keyb_mode_reset();
	printf( "%c\n", ch );

	return ret;
}


VOID menu_press( CONST CHAR *quest )
{
	printf( "%s", quest );

	keyb_mode_set( KEYB_MODE_PRESS | KEYB_MODE_NOECHO );
	getkey();
	keyb_mode_reset();
}


//================================================================
// Item functions
//----------------------------------------------------------------
VOID menu_final_item( MENUITEM *item )
{
	if( !invalid_ptr( item ) )
		item->desc = NULL;
}


VOID menu_copy_item( MENUITEM *dst, CONST MENUITEM *src )
{
	arrcpy_unit( dst, src );
}


VOID menu_add( MENU *menu, INT pos, INT count )
{
	if
	(
		!invalid_ptr( menu )
		&&
		pos >= 0
		&&
		count > 0
	)
	{
		MENUITEM *items = menu->items;
		INT len = menu_local_len( items );

		if( pos < len )
		{
			count = min( count, MENU_MAX_ITEMS - len + pos );

			arrmove
				(
					&items[pos + count],
					&items[pos],
					len - pos
				);
		}
	}
}


VOID menu_fill_item
(
	MENUITEM		*item,
	CONST CHAR		*desc,

	CHAR			sym,
	CONST MENU		*next_menu,

	pfn_do			f_do,
	pfn_title		f_title,

	CONST MENUMISC	*misc
)
{
	if( !invalid_ptr( item ) )
	{
		item->desc	= desc;
		item->sym	= sym;
	
		item->f_do		= f_do;
		item->f_title	= f_title;

		item->next_menu = next_menu;

		if( !invalid_ptr( misc ) )
			item->misc = *misc;
	}
}
