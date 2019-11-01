
#undef	PROF_POINTS
#define PROF_POINTS		ExclusiveName


#ifdef PROF_INLC_ENUM
{
	PROF_MAKE_ENUM( NAME1 ),
};
#endif


#ifdef PROF_INLC_POINTS
{
	{ "NAME1" },

	PROF_END_TABLE
};
#endif
