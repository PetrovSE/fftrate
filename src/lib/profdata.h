
//===================================================================================
//  Names definitions
//-----------------------------------------------------------------------------------
#ifdef PROF_MAIN
#define PROF_INCL	PROF_MAIN
#endif

#ifdef PROF_SLAVE
#define PROF_INCL	PROF_SLAVE
#endif

#ifdef PROF_EXTERN
#define PROF_INCL	PROF_EXTERN
#endif


//===================================================================================
//  Main inlude
//-----------------------------------------------------------------------------------
#include PROF_INCL


//===================================================================================
//  Enums definition
//-----------------------------------------------------------------------------------
CDECL_BEGIN

#if defined(PROF_MAIN) || defined(PROF_SLAVE)
	enum g_ProfilerPointIndex
	
	#define PROF_INLC_ENUM
	#include PROF_INCL
	#undef PROF_INLC_ENUM
#endif

//===================================================================================
//  Points definition
//-----------------------------------------------------------------------------------
#ifdef PROF_MAIN
	#ifdef PROFILING

	STATIC PROFPOINTINFO g_ProfilerPointStatic[] =
	#define PROF_INLC_POINTS
	#include PROF_INCL
	#undef PROF_INLC_POINTS
	
	PROFPOINTINFO *PROF_POINTS = g_ProfilerPointStatic;

	#else  // PROFILING
	PROFPOINTINFO *PROF_POINTS = NULL;
	#endif // PROFILING
#endif


//===================================================================================
//  Extern definition
//-----------------------------------------------------------------------------------
#if defined(PROF_EXTERN) || defined(PROF_SLAVE)
EXTERN PROFPOINTINFO *PROF_POINTS;
#endif


CDECL_END

//===================================================================================
//  Undef name
//-----------------------------------------------------------------------------------
#undef PROF_INCL
#undef PROF_MAIN
#undef PROF_SLAVE
#undef PROF_EXTERN
