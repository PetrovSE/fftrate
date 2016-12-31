#ifndef _MENU_H_
#define _MENU_H_


//=============================================================================
//  Definitions
//-----------------------------------------------------------------------------
#define GAP_IN						"|  "
#define GAP_OUT						"  |"
#define MENU_MAX_ITEMS				32


#define MENU_DECL( _n )				MENU _n
#define MENU_FULL( _n, _t, _f, _d )	MENU _n = { _t, _f, _d, {

#define MENU_BEGIN( _n, _t )		MENU_FULL( _n, _t, NULL, 0 )
#define MENU_END					STRUCT_TERMINATE } };


#define MENU_SET_TITLE( _h, _t )	(_h).title = _t

#define MENU_MAKE_MISC( _p, _v )	{ _p, _v }
#define MENU_EMPTY_MISC				MENU_MAKE_MISC( NULL, 0 )

#define MENU_MISC_PTR( _h )			(_h).misc.ptr
#define MENU_MISC_IVAL( _h )		(_h).misc.ival


#define MENU_RET_SYM				KEY_ESC
#define MENU_NULL_SYM				STR_TERM


#define ITEM_RET_DESC_MAIN			"Return to main menu"
#define ITEM_RET_DESC_PARENT		"Return to parent menu"


#define ITEM_FULL( _n, _s, _m, _d, _t, _p )	{ _n, _s, _m, _d, _t, _p }
#define ITEM_SIMPLE( _n, _s )				ITEM_FULL(			_n,	_s,				NULL,	NULL,	NULL,	MENU_EMPTY_MISC	)

#define ITEM_DO_TITLE( _n, _s, _d, _t )		ITEM_FULL(			_n,	_s,				NULL,	_d,		_t,		MENU_EMPTY_MISC	)
#define ITEM_DO( _n, _s, _d )				ITEM_DO_TITLE(		_n,	_s,				_d,		NULL	)

#define ITEM_NEXT_TITLE( _n, _s, _m, _t )	ITEM_FULL(			_n,	_s,				_m,		NULL,	_t,		MENU_EMPTY_MISC	)
#define ITEM_TITLE( _n, _t )				ITEM_FULL(			_n,	MENU_NULL_SYM,	NULL,	NULL,	_t,		MENU_EMPTY_MISC	)

#define ITEM_NEXT_DO( _n, _s, _m, _d )		ITEM_FULL(			_n,	_s,				_m,		_d,		NULL,	MENU_EMPTY_MISC	)
#define ITEM_NEXT( _n, _s, _m )				ITEM_NEXT_TITLE(	_n,	_s,				_m,		NULL	)

#define ITEM_RETURN_TO_MAIN( _m )			ITEM_NEXT( ITEM_RET_DESC_MAIN,		MENU_RET_SYM,	_m	)
#define ITEM_RETURN_TO_PARENT( _m )			ITEM_NEXT( ITEM_RET_DESC_PARENT,	MENU_RET_SYM,	_m	)

#define ITEM_SEPARATOR						ITEM_SIMPLE( "", MENU_NULL_SYM )


#define MENU_STR_YN							"(Y/N)?"
#define MENU_STR_ARE_YOU_SURE				"Are you sure " MENU_STR_YN
#define MENU_STR_PRESS_ANY_KEY				"Press any key ..."


//=============================================================================
//  Type definitions
//-----------------------------------------------------------------------------
#define SMENUITEM	struct TMENUITEM
#define SMENU		struct TMENU

typedef SMENUITEM	TMENUITEM;
typedef SMENU		TMENU;


typedef BOOL (*pfn_runtime)( VOID );
typedef VOID (*pfn_do)( CONST SMENUITEM * );
typedef VOID (*pfn_title)( CONST SMENUITEM *, CHAR *, INT );


typedef struct
{
	VOID	*ptr;
	INT		ival;

} MENUMISC, *PMENUMISC;


typedef struct TMENUITEM
{
	CONST CHAR	*desc;
	WORD		sym;
	
	CONST SMENU	*next_menu;

	pfn_do		f_do;
	pfn_title	f_title;

	MENUMISC	misc;

} MENUITEM, *PMENUITEM;


typedef struct TMENU
{
	CONST CHAR	*title;
	
	pfn_runtime	f_runtime;
	DWORD		timeout;

	MENUITEM	items[MENU_MAX_ITEMS];
	MENUMISC	misc;
	
} MENU, *PMENU;


//================================================================
//  Function prototypes
//----------------------------------------------------------------
CDECL_BEGIN

EXTERN CONST MENUITEM MenuSeparator;

VOID menu_print( CONST MENU *menu );
BOOL menu_parse( CONST MENU *menu );
VOID menu_stop( VOID );

BOOL menu_yes_no( CONST CHAR *quest );
VOID menu_press( CONST CHAR *quest );

VOID menu_final_item( MENUITEM *item );
VOID menu_copy_item( MENUITEM *dst, CONST MENUITEM *src );
VOID menu_add( MENU *menu, INT pos, INT count );
VOID menu_fill_item
(
	MENUITEM		*item,
	CONST CHAR		*desc,
	
	CHAR			sym,
	CONST MENU		*next_menu,
	
	pfn_do			f_do,
	pfn_title		f_title,

	CONST MENUMISC	*misc
);

CDECL_END

#endif // _MENU_H_
