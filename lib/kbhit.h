#ifndef _KBHIT_H_
#define _KBHIT_H_


//================================================================
//  Includes
//----------------------------------------------------------------
#include "interface.h"
#include "mathex.h"

#if defined(IS_WIN32)
#include <conio.h>
#endif


//================================================================
//  Definitions
//----------------------------------------------------------------
#define KEY_CODE( _p, _s )		make_word( _p, _s )
#define KEY_SYMB( _s )			KEY_CODE( 0, _s )

#define KEY_ESC					KEY_CODE( 0, 27 )
#define KEY_SPACE				KEY_CODE( 0, 32 )
#define KEY_ENTER				KEY_CODE( 0, 13 )

#define KEY_HOME 				KEY_CODE( 0, 71 )
#define KEY_END 				KEY_CODE( 0, 79 )

#define KEY_BACKSPACE			KEY_CODE( 0,  8 )
#define KEY_DC					KEY_CODE( 0, 83 )

#define KEY_PPAGE 				KEY_CODE( 0, 73 )
#define KEY_NPAGE				KEY_CODE( 0, 81 )

#define KEY_LEFT				KEY_CODE( 0, 75 )
#define KEY_RIGHT				KEY_CODE( 0, 77 )
#define KEY_UP					KEY_CODE( 0, 72 )
#define KEY_DOWN				KEY_CODE( 0, 80 )

#define KEY_FN_SHIFT			58
#define KEY_F( _n )				KEY_CODE( 1, (_n) + KEY_FN_SHIFT )
#define KEY_IC					KEY_CODE( 1, 82 )

#define KEY_UNKNOWN				KEY_CODE( 0, 0 )


#define KEYB_MODE_PRESS			0x0001
#define KEYB_MODE_ECHO			0x0002
#define KEYB_MODE_NOECHO		0x0004


//================================================================
//  Function prototypes
//----------------------------------------------------------------
CDECL_BEGIN

#if defined(IS_LINUX)

VOID keyb_mode_set( DWORD mode );
VOID keyb_mode_reset( VOID );
INT kbhit( VOID );
WORD getkey( VOID );

#elif defined(IS_WIN32)

#define keyb_mode_set( _m )
#define keyb_mode_reset()
#define kbhit()					_kbhit()
WORD getkey( VOID );

#else

#define keyb_mode_set( _m )
#define keyb_mode_reset()
#define kbhit()					1
#define getkey()				getchar()
#endif

CDECL_END

#endif // _KBHIT_H_
