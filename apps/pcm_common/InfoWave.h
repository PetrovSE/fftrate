#ifndef _PCM_INFOWAVE_H_
#define _PCM_INFOWAVE_H_

//================================================================
//  Function prototypes
//----------------------------------------------------------------
VOID PrintWaveSystemEnum( VOID );
VOID PrintWaveSystemInfo( CONST PLUGINFO *pSysInfo );

VOID PrintWaveDevEnum( CONST CHAR *szTitle, CONST PLUGINFO *pSysInfo, DWORD dwMode );
VOID PrintWaveDevInfo( CONST CHAR *szTitle, HWAVEDEV hWaveDev, BOOL bExclusive );

#endif // _PCM_INFOWAVE_H_
