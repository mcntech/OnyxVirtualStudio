#include "stdafx.h"
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "Dbg.h"

extern "C" int gDbgLvl = DBG_LVL_TRACE;
#define THIS_MODULE "OvsControl "

extern HANDLE ghLogFile;

extern "C" void DbgOutL(char *szError,...)
{
	char	szBuff[1024];
    va_list vl;

    va_start(vl, szError);
	sprintf(szBuff, THIS_MODULE);
    vsprintf(szBuff + strlen(szBuff), szError, vl);
    strcat(szBuff, "\r\n");
    OutputDebugStringA(szBuff);
	if(ghLogFile != INVALID_HANDLE_VALUE) {
		DWORD dwEritten = 0;
		WriteFile(ghLogFile, szBuff, strlen(szBuff), &dwEritten, NULL);
	}
	va_end(vl);
}
