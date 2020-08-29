#ifndef __CaptureGraph__
#define __CaptureGraph__
 
#include <DShow.h>
#include <Ogre.h>
#include <OgreVector2.h>

#include <initguid.h>
#include "DsUtil.h"
#define USE_MCN_VCAM

WCHAR* util_convertCStringToWString(const char* string);

// {A380CD88-BB23-4207-A77E-B3125CF7D144}
MIDL_INTERFACE("A380CD88-BB23-4207-A77E-B3125CF7D144")
IMcnSampleFeeder : public IUnknown
{
public:
    
    virtual HRESULT STDMETHODCALLTYPE SetMediaType( 
        const AM_MEDIA_TYPE *pType) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE GetConnectedMediaType( 
        AM_MEDIA_TYPE *pType) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE SendBuffer( 
		 char *pBuffer,
         long BufferSize
        ) = 0;
};


class CCaptureGraph : CDsUtil
{
public:
    CCaptureGraph();
    virtual ~CCaptureGraph();

	int BuildCaptureFileSaveGraph(std::string moviePath);
	int BuildAudioCaptureSubGraph(IBaseFilter *pAudCapFilter, IBaseFilter * pSink);

    int Init(const Ogre::String& moviePath, const Ogre::String& audDevicePath, bool horizontalMirroring=false);
    void Deinit();

    void pause();
    void start();
    void stop();

    int sendFrame(char *pBuffer, int *pnSize);
	
	void logMessage(std::string msg)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage(msg);
	};
	int getWidth() 
	{ 
		return videoWidth;
	}
	int getHeight()
	{
		return videoHeight;
	}
	bool getUpsidedown()
	{
		return videoUpsidedown;
	}

	void SetVidEncProperties(
		int           nBitrate,
		int           nGopLen,
		int           nFrameStruct
		);

protected:
	DWORD mdwRegister;

    IGraphBuilder *pGraph;
    IMediaControl *pControl;
    IMediaEvent   *pEvent;
    IMediaSeeking  *pSeeking;

    IBaseFilter   *mpVidCapSrc;
    IBaseFilter   *pAudCapSource;
    IBaseFilter   *mpEncFilter;
    IMcnSampleFeeder *pISampleFeeder;

    int videoWidth;
    int videoHeight;
	bool videoUpsidedown;
};
 
#endif // __CaptureGraph__