#ifndef _INTERFACE_H_
#define _INTERFACE_H_


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

#endif // _INTERFACE_H_
