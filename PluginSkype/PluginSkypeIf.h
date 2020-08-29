#ifndef __PluginSkypeIf_H__
#define __PluginSkypeIf_H__


typedef enum _SKYPEUI_LOGOUT_REASON_T
{
	SKYPE_LOGOUT_GENERIC,
	SKYPE_LOGOUT_CALLED,
	SKYPE_INCORRECT_PASSWORD,
	SKYPE_INCORRECT_ID,
}SKYPEUI_LOGOUT_REASON_T;

typedef struct _SKYPE_LOGOUT_INF_T
{
	int whyDidWeLogout;
} SKYPE_LOGOUT_INF_T;

typedef struct _SKYPE_CONTACT_INF_T
{
	const char *pszContactName;
	const char *pszContactFullName;
	const char *imgJpgData;
	int         imgLen;
} SKYPE_CONTACT_INF_T;

typedef enum _SKYPEUI_LOGIN_STATUS_ID_T
{
	SKYPE_STATUS_UNKNOWN,
	SKYPE_STATUS_LOGGED_OUT,
	SKYPE_STATUS_LOGGED_OUT_AND_PWD_SAVED,
	SKYPE_STATUS_CONNECTING_TO_P2P,
	SKYPE_STATUS_CONNECTING_TO_SERVER,
	SKYPE_STATUS_LOGGING_IN,
	SKYPE_STATUS_INITIALIZING,
	SKYPE_STATUS_LOGGED_IN,
	SKYPE_STATUS_LOGGING_OUT
} SKYPEUI_LOGIN_STATUS_ID_T;

typedef enum _SKYPEUI_VIDEO_STATUS_ID_T
{
	SKYPE_VIDEO_STATUS_UNKNOWN,
	SKYPE_VIDEO_INCOMING_VIDEO_OPEN,
	SKYPE_VIDEO_INCOMING_VIDEO_CLOSE,
	SKYPE_VIDEO_OUTGOING_VIDEO_OPEN,
	SKYPE_VIDEO_OUTGOING_VIDEO_CLOSE,
} SKYPEUI_VIDEO_STATUS_ID_T;

typedef enum _SKYPEUI_CONVERSATION_STATUS_ID_T
{
	SKYPE_CONVERSATION_STATUS_UNKNOWN,
	SKYPE_CONVERSATION_INCOMING_CALL_RINGING,
	SKYPE_CONVERSATION_STARTING,
	SKYPE_CONVERSATION_IM_LIVE,
	SKYPE_CONVERSATION_IM_DROPPED,
} SKYPEUI_CONVERSATION_STATUS_ID_T;

typedef enum _SKYPEUI_PARTICIPANT_STATUS_ID_T
{
	SKYPE_PARTICIPANT_STATUS_UNKNOWN,
	SKYPE_PARTICIPANT_STATUS_RINGING,
	SKYPE_PARTICIPANT_STATUS_SPEAKING,
	SKYPE_PARTICIPANT_STATUS_LISTENING,
	SKYPE_PARTICIPANT_STATUS_VOICE_STOPPED
} SKYPEUI_PARTICIPANT_STATUS_ID_T;

extern "C" class __declspec(dllexport) CSkypeCallback
{
public:
	virtual int LoginStatus(int StatusId, void *pInfo) = 0;
	virtual int VideoStatus(int StatusId, void *pInfo) = 0;
	virtual int ConversationStatus(int StatusId, void *pInfo) = 0;
	virtual int ParticipantStatus(int StatusId, void *pInfo) = 0;
	virtual void LiveSessionClosed() = 0;
	virtual void OtherSessionClosed() = 0;
	virtual void AddContact(SKYPE_CONTACT_INF_T *pContactInf) = 0;

};

extern "C" class __declspec(dllexport) CSkypeIf
{
public:
	virtual int Login(const char *szAccountName, const char *szPassword) = 0;
	virtual int Logout() = 0;
	virtual int Start() = 0;
	virtual int Stop() = 0;
	virtual int SetConfig(const char *szOnyxShareMemName) = 0;
	virtual int EnableAutoAnswer(bool fEnable) = 0;
	virtual int SetCallback(CSkypeCallback *pCallback) = 0;
	virtual int AceptIncomingCall(bool fAccept) = 0;
	virtual int HangupCall() = 0;
	virtual void PlaceCall(const char *szCallee) = 0;
	virtual int UpdateContactList() = 0;
	virtual int SetAutoReplyList(const char *szList) = 0;
	virtual int GetAudioInputDeviceList(char *pszBuff, int nBufSize) = 0;
	virtual int GetAudioOutputDeviceList(char *pszBuff, int nBufSize) = 0;
	virtual int SelectAudioDevices(const char *pszAudInput, const char *pszAudAutput) = 0;
};

#endif // __PluginSkypeIf_H__