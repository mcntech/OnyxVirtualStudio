// OnyxControl.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "PluginControl.h"

#ifdef __cplusplus    // If used by C++ code, 
extern "C" {          // we need to export the C interface
#endif
 
__declspec(dllexport) int __cdecl ocntrlPostMsg(ONYX_MSG_T *pMsg) 
{ 
	if(gMemMsg) {
		ONYX_CONTROL_MSG_T *pCtrlMsg = (ONYX_CONTROL_MSG_T *)gMemMsg;
		if(!pCtrlMsg->fReady){
			memcpy(&pCtrlMsg->msg, pMsg, sizeof(ONYX_MSG_T));
			pCtrlMsg->fReady = 1;
			return 0;
		}
	}
	return -1;
} 
 
__declspec(dllexport) int __cdecl ocntrlGetMsg(ONYX_MSG_T *pMsg) 
{ 
	if(gMemMsg){
		ONYX_CONTROL_MSG_T *pCtrlMsg = (ONYX_CONTROL_MSG_T *)gMemMsg;
		if(pCtrlMsg->fReady) {
			memcpy(pMsg, &pCtrlMsg->msg, sizeof(ONYX_MSG_T));
			pCtrlMsg->fReady = 0;
			return 0;
		}
	}
	return -1;
}

__declspec(dllexport) int __cdecl ocntrlPostStatus(ONYX_STATUS_T *pMsg) 
{ 
	if(gMemMsg) {
		ONYX_STATUS_MSG_T *pStatusMsg = (ONYX_STATUS_MSG_T *)((char *)gMemMsg + sizeof(ONYX_CONTROL_MSG_T));
		//if(!pStatusMsg->fReady)
		// Always update. Race condition may not caus harm
		{
			memcpy(&pStatusMsg->msg, pMsg, sizeof(ONYX_STATUS_T));
			pStatusMsg->fReady = 1;
			return 0;
		}
	}
	return -1;
} 
 
__declspec(dllexport) int __cdecl ocntrlGetStatus(ONYX_STATUS_T *pMsg) 
{ 
	if(gMemMsg){
		ONYX_STATUS_MSG_T *pStatusMsg = (ONYX_STATUS_MSG_T *)((char *)gMemMsg + sizeof(ONYX_CONTROL_MSG_T));
		//if(pStatusMsg->fReady) 
		// Always read. Race condition may not cause harm
		{
			memcpy(pMsg, &pStatusMsg->msg, sizeof(ONYX_STATUS_T));
			pStatusMsg->fReady = 0;
			return 0;
		}
	}
	return -1;
}

#ifdef __cplusplus
}
#endif

