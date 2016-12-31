#ifndef _LOGFILELIB_H_
#define _LOGFILELIB_H_


//================================================================
//  Definitions
//----------------------------------------------------------------
#define LOG_DUMP					16
#define LOG_DUMP_STEP				4
#define LOG_DUMP_DELAY				32


//================================================================
//  Type definitions
//----------------------------------------------------------------
typedef struct 
{
	FILE		*fp;
	HSECTION	h_section;

	CHAR		prefix[MAX_STRING];
	INT			dump;
	INT			delay;

} LOGFILE, *PLOGFILE;

#endif // _LOGFILELIB_H_
