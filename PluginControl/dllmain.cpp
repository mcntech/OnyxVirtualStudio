// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "PluginControl.h"
#define SHMEMSIZE 4096 
 
LPVOID gMemMsg = NULL;      // pointer to shared memory
static HANDLE hMapObject = NULL;  // handle to file mapping

BOOL APIENTRY DllMain(  HMODULE hModule,
						DWORD fdwReason,              // reason called 
						LPVOID lpvReserved
					 )
{
    BOOL fInit, fIgnore; 
 
    switch (fdwReason) 
    { 
		case DLL_PROCESS_ATTACH: 
			hMapObject = CreateFileMapping( 
				INVALID_HANDLE_VALUE,   // use paging file
				NULL,                   // default security attributes
				PAGE_READWRITE,         // read/write access
				0,                      // size: high 32-bits
				sizeof(ONYX_CONTROL_MSG_T) + sizeof(ONYX_STATUS_MSG_T),              // size: low 32-bits
				TEXT("OnyxControl")); // name of map object
			if (hMapObject == NULL) 
				return FALSE; 
 
			fInit = (GetLastError() != ERROR_ALREADY_EXISTS); 

			gMemMsg = MapViewOfFile( 
				hMapObject,     // object to map view of
				FILE_MAP_WRITE, // read/write access
				0,              // high offset:  map from
				0,              // low offset:   beginning
				0);             // default: map entire file
			if (gMemMsg == NULL) 
				return FALSE; 
 
			if (fInit) 
				memset(gMemMsg, '\0', sizeof(ONYX_CONTROL_MSG_T) + sizeof(ONYX_STATUS_MSG_T)); 
 
			break; 
 
        case DLL_THREAD_ATTACH: 
            break; 
 
        case DLL_THREAD_DETACH: 
            break; 

		case DLL_PROCESS_DETACH: 
			fIgnore = UnmapViewOfFile(gMemMsg); 
			fIgnore = CloseHandle(hMapObject); 
		break; 
 
		default: 
			break; 
	} 
 
	return TRUE; 
	UNREFERENCED_PARAMETER(hModule); 
	UNREFERENCED_PARAMETER(lpvReserved); 
}

// The export mechanism used here is the __declspec(export)
// method supported by Microsoft Visual Studio, but any
// other export method supported by your development
// environment may be substituted.

#ifdef __cplusplus    // If used by C++ code, 
extern "C" {          // we need to export the C interface
#endif
 
__declspec(dllexport) VOID __cdecl SetSharedMem(LPWSTR lpszBuf) 
{ 
    LPWSTR lpszTmp; 
    DWORD dwCount=1;
 
    lpszTmp = (LPWSTR) gMemMsg; 
 
    while (*lpszBuf && dwCount<SHMEMSIZE)   {
        *lpszTmp++ = *lpszBuf++; 
        dwCount++;
    }
    *lpszTmp = '\0'; 
} 
 
__declspec(dllexport) VOID __cdecl GetSharedMem(LPWSTR lpszBuf, DWORD cchSize) 
{ 
    LPWSTR lpszTmp; 
 
    lpszTmp = (LPWSTR) gMemMsg; 
 
    while (*lpszTmp && --cchSize) 
        *lpszBuf++ = *lpszTmp++; 
    *lpszBuf = '\0'; 
}
#ifdef __cplusplus
}
#endif

