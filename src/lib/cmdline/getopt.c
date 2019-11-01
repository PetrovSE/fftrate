//================================================================
//  Includes
//----------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//================================================================
//  Check OS
//----------------------------------------------------------------
#include "../swin.h"
#if defined(IS_WIN32)

#include "../types.h"
#include "../array.h"
#include "../mathex.h"
#include "../getoptw.h"
#include "../cmdline.h"
#include "../str.h"
#include "getoptlib.h"


//================================================================
//  Globally-defined variables
//----------------------------------------------------------------
CHAR	*optarg	= NULL;
INT		optind	= 0;
INT		opterr	= 1;
INT		optopt	= '?';


//================================================================
//  Functions
//----------------------------------------------------------------


//================================================================
//  Reverse_args_elements:  reverses num elements starting at args
//----------------------------------------------------------------
STATIC void reverse_args_elements( CHAR *args[], INT num )
{
	INT  i;
	CHAR *tmp;

	for( i = 0; i < ( num >> 1 ); i ++ )
	{
		tmp = args[i];
		args[i] = args[num - i - 1];
		args[num - i - 1] = tmp;
	}
}


//================================================================
//  Permute: swap two blocks of args-elements given their lengths
//----------------------------------------------------------------
STATIC void permute( CHAR *args[], INT len1, INT len2 )
{
	reverse_args_elements( args, len1 );
	reverse_args_elements( args, len1 + len2 );
	reverse_args_elements( args, len2 );
}


//================================================================
//  is_option: is this args-element an option or the end of the option list?
//----------------------------------------------------------------
STATIC INT is_option( CHAR *args_element, INT only )
{
	return
		( args_element == NULL )
		||
		( args_element[0] == '-' )
		||
		( only && args_element[0] == '+' );
}


//================================================================
// getopt_internal:  the function that does all the dirty work
//----------------------------------------------------------------
STATIC INT getopt_internal
(
	INT				n_args,
	CHAR			*args[],
	CONST CHAR		*p_shortopts,
	CONST OPTLONG	*longopts,
	INT				*longind,
	INT				only
)
{
	GETOPT_ORDERING ordering = PERMUTE;
	STATIC INT optwhere = 0;
	INT  permute_from = 0;
	INT  num_nonopts = 0;
	INT  optindex = 0;
	INT  match_chars = 0;
	CHAR *possible_arg = NULL;
	INT  longopt_match = -1;
	INT  has_arg = -1;
	CHAR *cp = NULL;
	INT  arg_next = 0;


	//----------------------------------------------------------------
	// first, deal with silly parameters and easy stuff
	//----------------------------------------------------------------
	if( is_zero( n_args ) || invalid_ptr( args ) || ( invalid_ptr( p_shortopts ) && invalid_ptr( longopts ) ) )
	{
		return optopt = '?';
	}

	if( optind >= n_args || args[optind] == NULL )
	{
		return EOF;
	}

	if( is_zero( strcmp( args[optind], "--" ) ) )
	{
		optind++;
		return EOF;
	}

	//----------------------------------------------------------------
	// if this is our first time through
	//----------------------------------------------------------------
	if( is_zero( optind ) )
	{
		optind = optwhere = 1;
	}

	//----------------------------------------------------------------
	// define ordering
	//----------------------------------------------------------------
	if( p_shortopts != NULL && ( *p_shortopts == '-' || *p_shortopts == '+' ) )
	{
		ordering = ( *p_shortopts == '-' ) ? RETURN_IN_ORDER : REQUIRE_ORDER;
		p_shortopts ++;
	}
	else
		ordering = ( getenv( "POSIXLY_CORRECT" ) != NULL ) ? REQUIRE_ORDER : PERMUTE;

	//----------------------------------------------------------------
	// based on ordering, find our next option, if we're at the beginning of one
	//----------------------------------------------------------------
	if( optwhere == 1 )
	{
		switch( ordering )
		{
		case PERMUTE:
			permute_from = optind;
			num_nonopts  = 0;

			while( !is_option( args[optind], only ) )
			{
				optind		++;
				num_nonopts	++;
			}

			if( args[optind] == NULL )
			{
				optind = permute_from;
				return EOF;
			}
			else
			{
				if( is_zero( strcmp( args[optind], "--" ) ) )
				{
					permute( args + permute_from, num_nonopts, 1 );
					optind = permute_from + 1;
					return EOF;
				}
			}
			break;

		case RETURN_IN_ORDER:
			if( !is_option( args[optind], only ) )
			{
				optarg = args[optind ++];
				return optopt = 1;
			}
			break;

		case REQUIRE_ORDER:
			if( !is_option( args[optind], only ) )
				return EOF;

			break;
		}
	}
	//----------------------------------------------------------------
	// we've got an option, so parse it
	//----------------------------------------------------------------


	//----------------------------------------------------------------
	// first, is it a long option?
	//----------------------------------------------------------------
	if
	(
		longopts != NULL
		&&
		(
			is_zero( memcmp( args[optind], "--", 2 ) )
			||
			( only && args[optind][0] == '+' ) 
		)
		&&
		optwhere == 1
	)
	{
		//----------------------------------------------------------------
		// handle long options
		//----------------------------------------------------------------
		if( is_zero( memcmp( args[optind], "--", 2 ) ) )
			optwhere = 2;

		longopt_match	= -1;
		possible_arg	= strchr( args[optind] + optwhere, '=' );

		if( possible_arg == NULL )
		{
			match_chars		= (INT)strlen( args[optind] );
			possible_arg	= args[optind] + match_chars;
			match_chars		= match_chars - optwhere;
		}
		else
			match_chars = (INT)( ( possible_arg - args[optind] ) - optwhere );

		for( optindex = 0 ; longopts[optindex].name != NULL ; optindex ++ )
		{
			if( is_zero( memcmp( args[optind] + optwhere, longopts[optindex].name, match_chars ) ) )
			{
				if( match_chars == (INT)strlen( longopts[optindex].name ) )
				{
					longopt_match = optindex;
					break;
				}
				else
				{
					if( less_zero( longopt_match ) )
					{
						longopt_match = optindex;
					}
					else
					{
						if( opterr )
						{
							fprintf
								(
									stderr,
									"%s: option `%s' is ambiguous "
									"(could be `--%s' or `--%s')\n",
									args[0],
									args[optind],
									longopts[longopt_match].name,
									longopts[optindex].name
								);
						}

						return optopt = '?';
					}
				}
			}
		}

		if( more_eq_zero( longopt_match ) )
			has_arg = longopts[longopt_match].has_arg;
	}


	//----------------------------------------------------------------
	// if we didn't find a long option, is it a short option?
	//----------------------------------------------------------------
	if( less_zero( longopt_match ) && p_shortopts != NULL )
	{
		cp = strchr( p_shortopts, args[optind][optwhere] );
		if( cp == NULL )
		{
			//----------------------------------------------------------------
			// couldn't find option in p_shortopts
			//----------------------------------------------------------------
			if( opterr )
			{
				fprintf
					(
						stderr,
						"%s: invalid option -- `-%c'\n",
						args[0],
						args[optind][optwhere]
					);
			}

			optwhere ++;
			if( is_term( args[optind][optwhere] ) )
			{
				optind ++;
				optwhere = 1;
			}

			return optopt = '?';
		}

		has_arg			= ( ( cp[1] == ':' ) ? ( ( cp[2] == ':' ) ? optional_argument : required_argument ) : no_argument );
		possible_arg	= args[optind] + optwhere + 1;
		optopt			= *cp;
	}

	
	//----------------------------------------------------------------
	// get argument and reset optwhere
	//----------------------------------------------------------------
	arg_next = 0;
	switch( has_arg )
	{
	case optional_argument:
		if( *possible_arg == '=' )
			possible_arg ++;

		if( !is_term( *possible_arg ) )
		{
			optarg		= possible_arg;
			optwhere	= 1;
		}
		else
			optarg = NULL;
		break;

	case required_argument:
		if( *possible_arg == '=' )
			possible_arg ++;

		if( !is_term( *possible_arg ) )
		{
			optarg		= possible_arg;
			optwhere	= 1;
		}
		else 
		{
			if( optind + 1 >= n_args )
			{
				if( opterr )
				{
					fprintf( stderr, "%s: argument required for option `", args[0] );

					if( more_eq_zero( longopt_match ) )
						fprintf( stderr, "--%s'\n", longopts[longopt_match].name );
					else
						fprintf( stderr, "-%c'\n", invalid_ptr( cp ) ? ' ' : *cp );
				}

				optind ++;
				return optopt = ':';
			}
			else
			{
				optarg = args[optind + 1];
				arg_next = 1;
				optwhere = 1;
			}
		}
		break;

	case no_argument:
		if( less_zero( longopt_match ) )
		{
			optwhere ++;
			if( is_term( args[optind][optwhere] ) )
				optwhere = 1;
		}
		else
			optwhere = 1;

		optarg = NULL;
		break;
	}


	//----------------------------------------------------------------
	// do we have to permute or otherwise modify optind?
	//----------------------------------------------------------------
	if( ordering == PERMUTE && optwhere == 1 && non_zero( num_nonopts ) )
	{
		permute( args + permute_from, num_nonopts, arg_next + 1);
		optind = permute_from + 1 + arg_next;
	}
	else 
	{
		if( optwhere == 1 )
			optind = optind + 1 + arg_next;
	}

	//----------------------------------------------------------------
	// finally return
	//----------------------------------------------------------------
	if( more_eq_zero( longopt_match ) )
	{
		if( longind != NULL )
			*longind = longopt_match;

		if( longopts[longopt_match].flag != NULL )
		{
			*( longopts[longopt_match].flag ) = longopts[longopt_match].val;
			return 0;
		}
		else
			return longopts[longopt_match].val;
	}
	else
		return optopt;
}


//================================================================
//  getopt
//----------------------------------------------------------------
INT getopt( INT n_args, CHAR *args[], CONST CHAR *p_optstring )
{
	return getopt_internal( n_args, args, p_optstring, NULL, NULL, 0 );
}


//================================================================
//  getopt_long
//----------------------------------------------------------------
INT getopt_long
(
	INT				n_args,
	CHAR			*args[],
	CONST CHAR		*p_shortopts,
	CONST OPTLONG	*longopts,
	INT				*longind
)
{
	return getopt_internal( n_args, args, p_shortopts, longopts, longind, 0 );
}


//================================================================
//  getopt_long_only
//----------------------------------------------------------------
INT getopt_long_only
(
	INT				n_args,
	CHAR			*args[],
	CONST CHAR		*p_shortopts,
	CONST OPTLONG	*longopts,
	INT				*longind
)
{
	return getopt_internal( n_args, args, p_shortopts, longopts, longind, 1 );
}

#endif // IS_WIN32
