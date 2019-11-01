#ifndef _ARRAY_H_
#define _ARRAY_H_


//================================================================
//  Includes
//----------------------------------------------------------------
#include <malloc.h>
#include <memory.h>
#include <string.h>


//================================================================
//  Definitions
//----------------------------------------------------------------
//#define SAFE_LEFT_MEMCPY


//================================================================
//  Macros definitions
//----------------------------------------------------------------
#define invalid_ptr( _ptr )					( (_ptr) == NULL )
#define numelems( _ptr )					(INT)( sizeof(_ptr) / sizeof( *(_ptr) ) )

#define arralloc_bytes( _ptr, _size )		( (_ptr) = malloc( _size ) )
#define arralloc( _ptr, _num )				arralloc_bytes( _ptr, (_num) * sizeof( *(_ptr) ) )
#define arralloc_unit( _ptr )				arralloc( _ptr, 1 )

#define arrcheck( _ptr )					( !invalid_ptr( _ptr ) )
#define arrfree( _ptr )						\
	if( arrcheck( _ptr ) )					\
	{										\
		free( _ptr );						\
		(_ptr) = NULL;						\
	}

#define arrmove_bytes( _dst, _src, _size )	memmove( _dst, _src, _size )
#define arrmove( _dst, _src, _num )			arrmove_bytes( _dst, _src, (_num) * sizeof( *(_dst) ) )
#define arrmove_unit( _dst, _src )			arrmove( _dst, _src, 1 )


#ifdef SAFE_LEFT_MEMCPY
#define arrcpy_bytes( _dst, _src, _size )	memcpy( _dst, _src, _size )
#else // SAFE_LEFT_MEMCPY

//----------------------------------------------------------------
// Warning: testing only "left" copy!
//----------------------------------------------------------------
#define arrcpy_bytes( _dst, _src, _size )				\
if														\
(														\
	(PBYTE)(_src) >= (PBYTE)(_dst)						\
	&&													\
	( (PBYTE)(_src) - (PBYTE)(_dst) ) < (LONG)(_size)	\
)														\
{														\
	memmove( _dst, _src, _size );						\
}														\
else													\
{														\
	memcpy( _dst, _src, _size );						\
}

#endif //SAFE_LEFT_MEMCPY


#define arrcpy( _dst, _src, _num )			arrcpy_bytes( _dst, _src, (_num) * sizeof( *(_dst) ) )
#define arrcpy_unit( _dst, _src )			arrcpy( _dst, _src, 1 )

#define arrzero_bytes( _dst, _size )		memset( _dst, 0x0, _size )
#define arrzero( _dst, _num )				arrzero_bytes( _dst, (_num) * sizeof( *(_dst) ) )
#define arrzero_unit( _dst )				arrzero( _dst, 1 )

#endif //_ARRAY_H_
