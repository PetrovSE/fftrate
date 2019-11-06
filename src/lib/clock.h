#ifndef _CLOCK_H_
#define _CLOCK_H_


//================================================================
//  Includes
//----------------------------------------------------------------
#include "interface.h"
#include "types.h"


#ifdef CLOCK_DISABLE

//================================================================
//  Macros definitions
//----------------------------------------------------------------
#define CPUCLOCKS_INIT( _cpu_clocks_name )
#define CPUCLOCKS_RESET( _cpu_clocks )

#define CPUCLOCKS_START( _cpu_clocks )
#define CPUCLOCKS_STOP( _cpu_clocks )

#define CPUCLOCKS_GETTIME( _cpu_clocks )			D( 0.0 )
#define CPUCLOCKS_GETCLOCKS( _cpu_clocks )			0

#else // CLOCK_DISABLE

//================================================================
//  Type definitions
//----------------------------------------------------------------
typedef struct
{
	LONGLONG	ccStartClocks;
	LONGLONG	ccSumClocks;

} CPUCLOCKS, *PCPUCLOCKS;


//================================================================
//  Macros definitions
//----------------------------------------------------------------
#define CPUCLOCKS_INIT( _cpu_clocks_name )			CPUCLOCKS _cpu_clocks_name

#define CPUCLOCKS_RESET( _cpu_clocks )		\
{											\
	(_cpu_clocks).ccStartClocks = 0;		\
	(_cpu_clocks).ccSumClocks = 0;			\
}

#define CPUCLOCKS_START( _cpu_clocks )		\
{											\
	(_cpu_clocks).ccStartClocks =			\
		clock_get_cpu_counter();			\
}

#define CPUCLOCKS_STOP( _cpu_clocks )		\
{											\
	(_cpu_clocks).ccSumClocks +=			\
		clock_get_cpu_counter()				\
		-									\
		(_cpu_clocks).ccStartClocks;		\
}

#define CPUCLOCKS_GETTIME( _cpu_clocks )			( (DATA)(_cpu_clocks).ccSumClocks / (DATA)clock_get_cpu_freq() )
#define CPUCLOCKS_GETCLOCKS( _cpu_clocks )			( (_cpu_clocks).ccSumClocks )

#endif // CLOCK_DISABLE


//================================================================
//  Function prototypes
//----------------------------------------------------------------
CDECL_BEGIN

LONGLONG clock_get_cpu_counter( VOID );
LONGLONG clock_get_cpu_freq( VOID );

CDECL_END

#endif // _CLOCK_H_
