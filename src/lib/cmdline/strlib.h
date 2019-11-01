#ifndef _STRLIB_H_
#define _STRLIB_H_


//================================================================
//  Macros definitions
//----------------------------------------------------------------
#define COPY_ARGS_BY_TYPE( _out, _in, _vf, _vt )	*( (_out *)va_arg( _vf, PVOID ) ) = (_out)va_arg( _vt, _in )


//================================================================
//  Type definitions
//----------------------------------------------------------------
typedef struct
{
	CONST CHAR	*s_false;
	CONST CHAR	*s_true;

} BOOLBOX, *PBOOLBOX;

#endif // _STRLIB_H_
