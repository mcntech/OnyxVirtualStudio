#include "stdafx.h"
#include <tchar.h>
#include <stdio.h>
#include "Dbg.h"

#if defined (DEBUG) || defined (_DEBUG)
#define ENABLE_DBG_MSG
#endif
//#define ENABLE_DBG_MSG	// Forced for release
extern "C" int gDbgLvl = DBG_LVL_MSG;
#define THIS_MODULE "PluginSkype "

extern "C" void DbgOutL(const char *szError,...)
{
	char	szBuff[256];
    va_list vl;

    va_start(vl, szError);
	sprintf(szBuff, THIS_MODULE);
    vsprintf(szBuff + strlen(THIS_MODULE), szError, vl);
    strcat(szBuff, "\r\n");
    OutputDebugStringA(szBuff);
	va_end(vl);
}
