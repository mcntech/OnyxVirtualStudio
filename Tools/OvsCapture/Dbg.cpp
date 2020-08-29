#include "stdafx.h"
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "Dbg.h"

#if defined (DEBUG) || defined (_DEBUG)
#define ENABLE_DBG_MSG
#endif
//#define ENABLE_DBG_MSG	// Forced for release
extern "C" int gDbgLvl = DBG_LVL_TRACE;
#define THIS_MODULE "McnScreenCap "

extern "C" void DbgOutL(char *szError,...)
{
	char	szBuff[256];
    va_list vl;

    va_start(vl, szError);
	sprintf(szBuff, THIS_MODULE);
    vsprintf(szBuff + strlen(szBuff), szError, vl);
    strcat(szBuff, "\r\n");
    OutputDebugStringA(szBuff);
	va_end(vl);
}
