//================================================================
//  Includes
//----------------------------------------------------------------
#include "../swin.h"
#include "../types.h"
#include "../array.h"
#include "../mathex.h"
#include "../kbhit.h"

#include "kbhitlib.h"


#if defined(IS_LINUX)

//================================================================
//  ROM
//----------------------------------------------------------------
STATIC CONST KEYMAP g_key_map[] =
{
	{	KEY_ESC,		{  27, },	},						// Ecs
	{	KEY_BACKSPACE,	{ 127, },	},						// Backspace
	{	KEY_ENTER,		{  10, },	},						// Enter

	{	KEY_UP,			{  27,  91,  65, },	},				// Up
	{	KEY_DOWN,		{  27,  91,  66, },	},				// Down
	{	KEY_RIGHT,		{  27,  91,  67, },	},				// Right
	{	KEY_LEFT,		{  27,  91,  68, },	},				// Left

	{	KEY_F(  1 ),	{  27,  79,  80, },	},				// F1
	{	KEY_F(  2 ),	{  27,  79,  81, },	},				// F2
	{	KEY_F(  3 ),	{  27,  79,  82, },	},				// F3
	{	KEY_F(  4 ),	{  27,  79,  83, },	},				// F4

	{	KEY_F(  5 ),	{  27,  91,  49,  53, 126, },	},	// F5
	{	KEY_F(  6 ),	{  27,  91,  49,  55, 126, },	},	// F6
	{	KEY_F(  7 ),	{  27,  91,  49,  56, 126, },	},	// F7
	{	KEY_F(  8 ),	{  27,  91,  49,  57, 126, },	},	// F8

	{	KEY_F(  9 ),	{  27,  91,  50,  48, 126, },	},	// F9
	{	KEY_F( 10 ),	{  27,  91,  50,  49, 126, },	},	// F10
	{	KEY_F( 11 ),	{  27,  91,  50,  51, 126, },	},	// F11
	{	KEY_F( 12 ),	{  27,  91,  50,  52, 126, },	},	// F12

	{	KEY_IC,			{  27,  91,  50, 126, },	},		// Ins
	{	KEY_DC,			{  27,  91,  51, 126, },	},		// Del

	{	KEY_HOME,		{  27,  91,  72, },	},				// Home
	{	KEY_END,		{  27,  91,  70, },	},				// End

	{	KEY_PPAGE,		{  27,  91,  53, 126, },	},		// PgUp
	{	KEY_NPAGE,		{  27,  91,  54, 126, },	},		// PgDown
};


//================================================================
//  RAM
//----------------------------------------------------------------
STATIC BOOL			g_keyb_saved	= FALSE;
STATIC TERMIOS		g_saved_settings;


//================================================================
//  Keyboard functions
//----------------------------------------------------------------
VOID keyb_mode_set( DWORD mode )
{
	DWORD   mask;
	TERMIOS settings;

	tcgetattr( STDIN_FILENO, &settings );

	if( !g_keyb_saved )
	{
		g_keyb_saved		= TRUE;
		g_saved_settings	= settings;
	}

	//----------------------------------------------------------------

	for( mask = 0x1 ; mask ; mask <<= 1 )
	{
		if( mode & mask )
		{
			switch( mask )
			{
			case KEYB_MODE_PRESS:
				flag_reset( settings.c_lflag, ICANON );
				settings.c_cc[VTIME]	 = 0;
				settings.c_cc[VMIN]		 = 1;
				break;

			case KEYB_MODE_ECHO:
				flag_set( settings.c_lflag, ECHO );
				break;

			case KEYB_MODE_NOECHO:
				flag_reset( settings.c_lflag, ECHO );
				break;
			}
		}
	}

	//----------------------------------------------------------------

	tcsetattr( STDIN_FILENO, TCSANOW, &settings );
}


VOID keyb_mode_reset( VOID )
{
	if( g_keyb_saved )
		tcsetattr( STDIN_FILENO, TCSANOW, &g_saved_settings );
}


//================================================================
//  KB hit
//----------------------------------------------------------------
INT kbhit( VOID )
{
	TIMEVAL tv;
	fd_set rdfs;

	tv.tv_sec  = 0;
	tv.tv_usec = 0;

	FD_ZERO( &rdfs );
	FD_SET( STDIN_FILENO, &rdfs );

	if( select( STDIN_FILENO + 1, &rdfs, NULL, NULL, &tv ) == -1 )
		return 0;

	return non_zero( FD_ISSET( STDIN_FILENO, &rdfs ) );
}


//================================================================
//  Codes parsing
//----------------------------------------------------------------
WORD getkey( VOID )
{
	CHAR code[MAX_KEYB_CODE] = { 0 };
	INT  size, i;

	//----------------------------------------------------------------

	fflush( STDIN_FILENO );
	size = read( STDIN_FILENO, code, MAX_KEYB_CODE );

	//----------------------------------------------------------------

	for( i = 0 ; i < numelems(g_key_map) ; i ++ )
	{
		if( is_zero( strcmp( code, g_key_map[i].chmap ) ) )
			return g_key_map[i].code;
	}

	if( size == 1 )
		return KEY_SYMB( code[0] );

	return KEY_UNKNOWN;
}


#elif defined(IS_WIN32)

//================================================================
//  Codes parsing
//----------------------------------------------------------------
WORD getkey( VOID )
{
	BYTE pref = 0x0;
	BYTE code = (BYTE)_getch();

	if( is_zero( code ) )
	{
		pref = 0x1;
		code = (BYTE)_getch();
	}

	return KEY_CODE( pref, code );
}

#endif
