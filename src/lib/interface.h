#ifndef _INTERFACE_H_
#define _INTERFACE_H_


//================================================================
//  Module info types
//----------------------------------------------------------------
#define LIBINFO_SHORTNAME			0x0
#define LIBINFO_LONGNAME			0x1
#define LIBINFO_COPYRIGHT			0x2
#define LIBINFO_VERSION				0x3
#define LIBINFO_BUILD				0x4


//================================================================
//  Assume C declarations for C++
//----------------------------------------------------------------
#ifdef __cplusplus
	#define CDECL_BEGIN			extern "C" {
	#define CDECL_END			}
#else
	#define CDECL_BEGIN
	#define CDECL_END
#endif


//================================================================
//  Handler definition
//----------------------------------------------------------------
#define DECL_HANDLE( _h )		\
	typedef struct _##_h##_		\
	{							\
		INT	unused_##_h;		\
	} * _h;


//================================================================
//  Information function
//----------------------------------------------------------------
typedef CONST CHAR * (*LIBINFO_POINTER)( INT );

#if defined(IS_PC_32)
#define LIBINFO_ARCH						"i386"
#elif defined(IS_PC_64)
#define LIBINFO_ARCH						"x86_64"
#elif defined(IS_ARM)
#define LIBINFO_ARCH						"arm"
#else
#define LIBINFO_ARCH						"unknown"
#endif

#if defined(IS_WIN32)
#define LIBINFO_SYS							"windows"
#elif defined(IS_LINUX)
#define LIBINFO_SYS							"linux"
#else
#define LIBINFO_SYS							"unknown"
#endif

#define LIBINFO_BUILD_STR					__DATE__ ", " __TIME__ ", " LIBINFO_SYS "-" LIBINFO_ARCH
#define LIBINFO_PROTOTYPE( _lib_name )		CONST CHAR * _lib_name ## _get_info( INT info_type )


#ifdef NOINFO

#define LIBINFO_FUNCTION( _lib_name, _short_name, _long_name, _copy_right, _version )	\
LIBINFO_PROTOTYPE( _lib_name )		\
{									\
	return NULL;					\
}

#else // NOINFO

#define LIBINFO_FUNCTION( _lib_name, _short_name, _long_name, _copy_right, _version )	\
LIBINFO_PROTOTYPE( _lib_name )		\
{									\
	switch( info_type )				\
	{								\
	case LIBINFO_SHORTNAME:			\
		return _short_name;			\
	case LIBINFO_LONGNAME:			\
		return _long_name;			\
	case LIBINFO_COPYRIGHT:			\
		return _copy_right;			\
	case LIBINFO_VERSION:			\
		return _version;			\
	case LIBINFO_BUILD:				\
		return LIBINFO_BUILD_STR;	\
	}								\
	return NULL;					\
}

#endif // NOINFO

#endif // _INTERFACE_H_
