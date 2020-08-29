#ifndef __ONYX_CONTROL_H__
#define __ONYX_CONTROL_H__
#include "OnyxControl.h"

typedef struct _ONYX_CONTROL_MSG_T
{
	bool        fReady;
	ONYX_MSG_T  msg;
} ONYX_CONTROL_MSG_T;

typedef struct _ONYX_STATUS_MSG_T
{
	bool        fReady;
	ONYX_STATUS_T  msg;
} ONYX_STATUS_MSG_T;

extern LPVOID gMemMsg;      // pointer to shared memory

#endif //__ONYX_CONTROL_H__