// PluginSkype.cpp : Defines the exported functions for the DLL application.


//
// Define SKYPE_DIST_ROOT in system=>advenaced=>environment variables
// Include Folders
//$(SKYPE_DIST_ROOT)\interfaces\skype\cpp_embedded\src\api;$(SKYPE_DIST_ROOT)\interfaces\skype\cpp_embedded\src\ipc;$(SKYPE_DIST_ROOT)\interfaces\skype\cpp_embedded\src\types;$(SKYPE_DIST_ROOT)\interfaces\skype\cpp_embedded\src\platform\threading;$(SKYPE_DIST_ROOT)\interfaces\skype\cpp_embedded\src\platform\transport;$(SKYPE_DIST_ROOT)\interfaces\skype\cpp_embedded\src\platform\transport\tcp;$(SKYPE_DIST_ROOT)\ipc\cpp;$(SKYPE_DIST_ROOT)\ipc\cpp\platform\se;$(SKYPE_DIST_ROOT)\ipc\cpp\ssl\cyassl\include;$(SKYPE_DIST_ROOT)\ipc\cpp\ssl\cyassl\ctaocrypt\include;$(SKYPE_DIST_ROOT)\ipc\cpp\ssl\cyassl\include\openssl;$(SKYPE_DIST_ROOT)\interfaces\skype\cpp_embedded\src\platform\threading\win
// Library Foldera
//$(SKYPE_DIST_ROOT)\interfaces\skype\cpp_embedded\build;$(SKYPE_DIST_ROOT)\bin\windows-x86

#ifndef __PLUGIN_SKYPE_H_
#define __PLUGIN_SKYPE_H_

#include <skype-embedded_2.h>
#include <string>
#include <WinShm.hpp>
#include <VideoTransportClient.hpp>
#include "PluginSkypeIf.h"

class QSKSignalDispatcher
{
public:
	virtual void SendToLogSignal                (std::string);
    virtual void EnableCallBtnSignal            (bool){}
    virtual void ToggleCallBtnSignal            (bool){}
    virtual void OtherSessionClosed             ();
};

//---------------------------------------------------------------------------------------
// Account class declaration

class QSKAccount : public Account
{
public:
    typedef DRef<QSKAccount, Account> Ref;
    typedef DRefs<QSKAccount, Account> Refs;

    QSKAccount(unsigned int oid, SERootObject* root) : Account(oid, root) {}
    void OnChange(int prop);
};

class QSKContact : public Contact
{
public:
  typedef DRef<QSKContact, Contact> Ref;
  typedef DRefs<QSKContact, Contact> Refs;
  QSKContact(unsigned int oid, SERootObject* root) : Contact(oid, root) {};

  void OnChange(int prop);
};

//---------------------------------------------------------------------------------------
// ContactGroup class declaration

class QSKContactGroup : public ContactGroup
{
public:
    typedef DRef<QSKContactGroup, ContactGroup> Ref;
    typedef DRefs<QSKContactGroup, ContactGroup> Refs;

    QSKContactGroup(unsigned int oid, SERootObject* root) : ContactGroup(oid, root) {}

	QSKContact::Refs contactList;
    void OnChange(const ContactRef& contact);
};


//---------------------------------------------------------------------------------------
// Video class declaration

class QSKParticipant;

class QSKVideo : public Video
{
public:
    typedef DRef<QSKVideo, Video> Ref;
    typedef DRefs<QSKVideo, Video> Refs;
    typedef DRef<QSKParticipant, Participant> QSKParticipantRef;

    bool                incomingStream;
    bool                localPreview;

    QSKParticipantRef   participant;

    QSKVideo(unsigned int oid, SERootObject* root);
    void OnChange(int prop);
};


//---------------------------------------------------------------------------------------
// Participant class declaration

class QSKParticipant : public Participant
{
public:
    typedef DRef<QSKParticipant, Participant> Ref;
    typedef DRefs<QSKParticipant, Participant> Refs;

    bool            isMyself;
    QSKVideo::Ref   video;

    QSKParticipant(unsigned int oid, SERootObject* root);

    void OnChange(int prop);
    void GetVideoIfAvailable();
    void OnIncomingDTMF(const DTMF& dtmf);
};

//---------------------------------------------------------------------------------------
// Conversation class declaration

class QSKConversation : public Conversation
{
public:
    typedef DRef<QSKConversation, Conversation> Ref;
    typedef DRefs<QSKConversation, Conversation> Refs;

    QSKParticipant::Refs allParticipants;

    QSKConversation(unsigned int oid, SERootObject* root) : Conversation(oid, root)
    {
        this->GetParticipants(allParticipants);
        fetch(allParticipants);
    }
	void OnChange(int prop);
	void OnParticipantListChange();
  
	bool callFinished;
	std::string dbgId;
};


//---------------------------------------------------------------------------------------
// Skype class declaration

class QSKSkype : public Skype
{
public:
    bool                    isLoggedIn;
    bool                    isLiveSessionUp;
    bool                    isVideoCapable;
    bool                    isPreviewVideoOk;
	bool                    mfAutoAnswer;
	bool                    mfAcceptCall;
	bool                    mfUserActed;
    SEString                videoDeviceName;
    SEString                videoDeviceId;

    QSKAccount::Ref         account;
    QSKConversation::Ref    liveSession;
    QSKVideo::Ref           previewVideo;

    ContactGroup::Ref       allBuddiesGroup;
    ContactRefs             allBuddies;
    QSKConversation::Refs   inbox;

    QSKSkype() : Skype()
    {
        isLoggedIn          = false;
        isLiveSessionUp     = false;
        isVideoCapable      = false;
        isPreviewVideoOk    = false;
		mfAutoAnswer        = false;
		mfAcceptCall		= false;
		mfUserActed 		= false;
    }

    void OnAvailableVideoDeviceListChange();

    void OnConversationListChange(
        const ConversationRef &conversation,
        const Conversation::LIST_TYPE &type,
        const bool &added);

	void PlaceCall(const char *szCallee);

    int Login (SEString accountName, SEString password);
	int Logout();

	int SelectLocalCaptureDevice();

    Account*        newAccount(int oid)         { return new QSKAccount(oid, this);      }
    ContactGroup*   newContactGroup(int oid)    { return new QSKContactGroup(oid, this); }
    Video*          newVideo(int oid)           { return new QSKVideo(oid, this);        }
    Participant*    newParticipant(int oid)     { return new QSKParticipant(oid, this);  }
    Conversation*   newConversation(int oid)    
	{ 
		return new QSKConversation(oid, this); 
	}
	Contact*        newContact(int oid)         { return new QSKContact(oid, this);}
};

class CVideoFrame;

// Equivqlent to VideoWidget in Skype video_call sample without GUI
class CVideoRx
{
public:
	CVideoRx()
	{
		mFrameCount = 0;
		mActive = false;
	}
	~CVideoRx(){}
    void renderStart();
    void renderStop();

	int getKey() 
	{ 
		return ipc.key(); 
	}
	void setParticipantName (std::string newName)
	{
		mParticipantName = newName;
	}

	int getFrame(CVideoFrame *pFrm);

	int getWidth()
	{
		if(mActive) {
			VideoTransportBase::bufferstruct *buffer = ipc.getFrame();
			if(buffer)
				return buffer->width;
		}
		return 0;
	}
	int getHeight()
	{
		if(mActive) {
			VideoTransportBase::bufferstruct *buffer = ipc.getFrame();
			if(buffer)
				return buffer->height;
		}
		return 0;
	}
	int CopyFrameToSharedMem(char *pSharedBuf);
private:
    #ifdef WIN32
        VideoTransportClient <WinShm> ipc;
    #else
        VideoTransportClient <PosixShm> ipc;
    #endif

        int currentwidth;
        int timerId;
		std::string mParticipantName;

		int mCurrentwidth;
		int mFrameCount;
		bool mActive;
};


//=============================================================================
class CVideoFrame
{
public:
	bool mReady;
	bool mInUse;
	int mIteration;

	CVideoFrame()
	{
		mBuffer = (unsigned char*)malloc(1280*720*4);
	}
	~CVideoFrame();
	
	void clear(){mReady = 0; mInUse=0;}

	int CVideoFrame::getWidth(){return mWidth;}
	int CVideoFrame::getHeight(){return mHeight;}
	int CVideoFrame::getStride(){return mStride;}

	unsigned char* getBuffer(){return mBuffer;}

	void CopyToHwTextureARGB(unsigned char* out,int destStride)
	{
		unsigned char *ysrc;
		unsigned char *ydest = out;
		if (mOrientation){
			ysrc = mBuffer + (mHeight - 1) * mWidth * 4;
		} else {
			ysrc = mBuffer;
		}

		for (int y = 0;y < mHeight; y += 1) 	{
			memcpy(ydest, ysrc, mWidth * 4);
			ydest += destStride * 4;
			if (mOrientation){			
				ysrc -= mStride * 4;
			} else {
				ysrc += mStride * 4;
			}
		}
	}


	int            mWidth;
	int            mHeight;
	int            mStride;
	int            mSize;
	int            mOrientation;
	unsigned char* mBuffer;
	unsigned long  mFrameNumber;
};

class CSkypeUi : public CSkypeIf
{
public:
	static CSkypeUi* Instance(); 
	PROCESS_INFORMATION mProcessInformation;
	STARTUPINFOA mStartupInfo;
	CVideoRx *mVidRx;
	//CVideoTx *mVidTx;

	void on_OpenIncomingVideo(Participant::Ref part);
	void on_CloseIncomingVideo(Participant::Ref part);
	void on_OpenLocalVideo();
	void on_CloseLocalVideo();
	void on_IncomingCall(int nStatusId,const char *party);
	void on_OutgoingCall(int nStatusId,const char *party);

	void on_UpdateStatus(int nStatusId, int whyDidWeLogout);
	void on_DtmfKeyReceived(Participant::Ref part, int dtmf);
	void on_callButtonLoginStatus(int nStatusId, char *szMsg);
	void on_LiveSessionClosed();
	void on_OtherSessionClosed();

	int Start();
	int Login(const char *szAccountName, const char *szPassword);
	int Logout();
	virtual int SetConfig(const char *szOnyxShareMemName)
	{
		mSharedMemName = szOnyxShareMemName;
		return 0;
	}
	int EnableAutoAnswer(bool fEnable);
	int AceptIncomingCall(bool fAccept);
	void PlaceCall(const char *szCallee);
	int HangupCall();
	int SetAutoReplyList(const char *pszList)
	{
		mAutoReplyList = pszList;
		return 0;
	}

	int Stop();
	int StopMemStream();
	static DWORD WINAPI DoBufferProcessing(void *pCtx);
	int InitSharedMem();
	int DeinitShareddMem();
	int GetContactList();
	int UpdateContactList();
	int GetAudioInputDeviceList(char *pszBuff, int nBufSize);
	int GetAudioOutputDeviceList(char *pszBuff, int nBufSize);
	int SelectAudioDevices(const char *pszAudInput, const char *pszAudAutput);
	CVideoRx* getVidRxStream(/*TODO Participant Name*/)
	{
		return mVidRx;
	}
	virtual int SetCallback(CSkypeCallback *pCallback)
	{
		mpCallback = pCallback;
		return 0;
	}
	std::string     mAutoReplyList;
	bool IsInAutoReplyList(const char *psContact);

private:
	CSkypeUi();
	int StartMemStream();
	bool			mRunMemStream;
	HANDLE			mMapFile;
	LPCTSTR			mSharedBuf;
	std::string     mSharedMemName;
	HANDLE          mThread;

	static CSkypeUi *mInstance; 
	CSkypeCallback  *mpCallback;

};

extern QSKSkype*            skype;
extern QSKSignalDispatcher* dispatcher;

#endif //__PLUGIN_SKYPE_H_