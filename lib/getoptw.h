#ifndef _GETOPTW_H_
#define _GETOPTW_H_


//================================================================
//  Check OS
//----------------------------------------------------------------
#if defined(IS_WIN32)

//================================================================
//  Includes
//----------------------------------------------------------------
#include "interface.h"


//================================================================
//  Macros defined by this include file
//----------------------------------------------------------------
#define no_argument				0x0
#define required_argument		0x1
#define optional_argument		0x2


//================================================================
//  Types defined by this include file
//----------------------------------------------------------------
struct option
{
	CHAR	*name;		// The name of the long option
	INT		has_arg;    // One of the above macros
	INT		*flag;		// Determines if getopt_long() returns a
						// value for a long option; if it is
						// non-NULL, 0 is returned as a function
						// value and the value of val is stored in
						// the area pointed to by flag.  Otherwise,
						// val is returned.
	
	INT		val;		// Determines the value to return if flag is NULL.

};

typedef struct option	OPTLONG, *POPTLONG;


//================================================================
//  Declale interfaces
//----------------------------------------------------------------
CDECL_BEGIN

//================================================================
//  Externally-defined variables
//----------------------------------------------------------------
EXTERN CHAR	*optarg;
EXTERN INT	optind;
EXTERN INT	opterr;
EXTERN INT	optopt;


//================================================================
//  Function prototypes
//----------------------------------------------------------------
INT getopt( INT n_args, CHAR *args[], CONST CHAR *p_optstring );
INT getopt_long
(
	INT				n_args,
	CHAR			*args[],
	CONST CHAR		*p_shortopts,
	CONST OPTLONG	*longopts,
	INT				*longind
);

INT getopt_long_only
(
	INT				n_args,
	CHAR			*args[],
	CONST CHAR		*p_shortopts,
	CONST OPTLONG	*longopts,
	INT				*longind
);

CDECL_END

// IS_WIN32
#elif defined(IS_LINUX)

//================================================================
//  Includes
//----------------------------------------------------------------
#include <getopt.h>

#endif // IS_LINUX


//=============================================================================
//  Definitions
//-----------------------------------------------------------------------------
#define GETOPT_BEGIN( _name )		const struct option _name[] = {
#define GETOPT_END					{ NULL, 0, NULL, 0 } };

#define GETOPT_ITEM( _name, _arg, _flag, _val )		{ STR( _name ), _arg, _flag, _val }
#define GETOPT_ITEM_SIMPLE( _name)					GETOPT_ITEM( _name, no_argument, NULL, 0 )
#define GETOPT_ITEM_SYM( _name, _val )				GETOPT_ITEM( _name, no_argument, NULL, _val )

#endif // _GETOPTW_H_
