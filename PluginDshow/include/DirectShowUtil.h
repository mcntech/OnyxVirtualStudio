#ifndef __DirectShowUtil__
#define __DirectShowUtil__
 
#include <DShow.h>
#include <Ogre.h>
#include <OgreVector2.h>
#include <initguid.h>
#include <DsUtil.h>

DEFINE_GUID(CLSID_SampleGrabber,  0xc1f400a0, 0x3f08,0x11d3, 0x9f, 0x0b, 0x00, 0x60, 0x08, 0x03, 0x9e, 0x37);
DEFINE_GUID(IID_ISampleGrabber,   0x6B652FFF, 0x11FE, 0x4fce, 0x92, 0xAD, 0x02, 0x66, 0xB5, 0xD7, 0xC7, 0x8F);

DEFINE_GUID(IID_ISampleGrabberCB, 0x0579154A, 0x2B53, 0x4994, 0xB0, 0xD0, 0xE7, 0x73, 0x14, 0x8E, 0xFF, 0x85);
//DEFINE_GUID(CLSID_NullRenderer,   0xd11dfe19, 0x8864, 0x4a60, 0xb2, 0x6c, 0x55, 0x2f, 0x9a, 0xa4, 0x72, 0xe1);
DEFINE_GUID(CLSID_NullRenderer,   0xC1F400A4, 0x3F08, 0x11D3, 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37);

DEFINE_GUID(CLSID_CWMVDecMediaObject,0x82d353df, 0x90bd, 0x4382, 0x8b, 0xc2, 0x3f, 0x61, 0x92, 0xb7, 0x6e, 0x34);
DEFINE_GUID(CLSID_WMAsfReader,       0x187463a0, 0x5bb7, 0x11d3, 0xac, 0xbe, 0x0, 0x80, 0xc7, 0x5e, 0x24, 0x6e);

// LG Smart TV SDK
DEFINE_GUID(CLSID_TSSOurce,          0xF9FC285E, 0xAC6D, 0x44D6, 0x81, 0xE2, 0x6F, 0x57, 0xCB, 0xFA, 0xF0, 0x07);

DEFINE_GUID(CLSID_McnH264DecoderFilter, 0x51751ec4, 0xe794, 0x4393, 0x8e, 0xca, 0x9b, 0x8a, 0x60, 0xc6, 0x5a, 0xfa);
DEFINE_GUID(CLSID_MCNTsRtspSrc,         0xfcfafe5b, 0xc781, 0x4e95, 0x89, 0x99, 0x92, 0xc5, 0x2f, 0xa2, 0x36, 0x11);
EXTERN_GUID(CLSID_McnH264MediaObject,   0x3a82ba53, 0xddf2, 0x4565, 0xbc, 0xfd, 0x4a, 0xb8, 0x4, 0xd9, 0x1a, 0xca);
EXTERN_GUID(CLSID_McnMp4Demux,          0x860019cb, 0x76f6, 0x4506, 0x9d, 0x8b, 0x6e, 0xb1, 0x24, 0x2f, 0xbb, 0x55);

EXTERN_GUID(CLSID_CMPEG2VidDecoderDS,   0x212690FB, 0x83E5, 0x4526, 0x8F, 0xD7, 0x74, 0x47, 0x8B, 0x79, 0x39, 0xCD);  



MIDL_INTERFACE("0579154A-2B53-4994-B0D0-E773148EFF85")
ISampleGrabberCB : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE SampleCB( 
        double SampleTime,
        IMediaSample *pSample) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE BufferCB( 
        double SampleTime,
        BYTE *pBuffer,
        long BufferLen) = 0;
    
};
    

MIDL_INTERFACE("6B652FFF-11FE-4fce-92AD-0266B5D7C78F")
ISampleGrabber : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE SetOneShot( 
        BOOL OneShot) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE SetMediaType( 
        const AM_MEDIA_TYPE *pType) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE GetConnectedMediaType( 
        AM_MEDIA_TYPE *pType) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE SetBufferSamples( 
        BOOL BufferThem) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE GetCurrentBuffer( 
        /* [out][in] */ long *pBufferSize,
        /* [out] */ long *pBuffer) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE GetCurrentSample( 
        /* [retval][out] */ IMediaSample **ppSample) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE SetCallback( 
        ISampleGrabberCB *pCallback,
        long WhichMethodToCallback) = 0;
    
};
    
class CCompositorCB
{
public:
	virtual int NextFrameCB(unsigned char *pData, long lSize, double Pts) = 0;
};

struct DirectShowData
{
    IGraphBuilder *pGraph;
    IMediaControl *pControl;
    IMediaEvent *pEvent;
    IBaseFilter *pGrabberF;

    IBaseFilter *pVidDecF;

    ISampleGrabber *pGrabber;
    IMediaSeeking *pSeeking;
    IVideoWindow *pWindow;
    IBaseFilter *pVidSrc;
    IBaseFilter *pAudSrc;

    int videoWidth;
    int videoHeight;
	int verticalFlip;
};
 
 
WCHAR* util_convertCStringToWString(const char* string);


class CDsPlayer : public CDsUtil
{
public:
    CDsPlayer(CCompositorCB *pCompositorCB=NULL);

    virtual ~CDsPlayer();

	int BuildWmvGraph(std::string moviePath);

	int BuildRtspGraph(std::string moviePath);

	int BuildTsFilePlayGraph(std::string moviePath);
	int BuildMp4Graph(std::string moviePath);
	int BuildCaptureGraph(std::string moviePath, int nCapWidth, int nCapHeight);
	HRESULT FindCaptureDevice(std::string DeviceName, IBaseFilter ** ppSrcFilter);
    int loadMovie(const Ogre::String& moviePath, int nCapWidth = 0, int nCapHeight = 0);

    Ogre::Vector2 getMovieDimensions();

    void unloadMovie();

    void pauseMovie();

    void playMovie();

    void rewindMovie();

    void stopMovie();
    bool isPlayingMovie();
	bool isEndOfPlay();

    int getFrame(char *pBuffer, int *pnSize);
	
	void logMessage(std::string msg)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("PlugInDshow:" + msg);
	};
	int getWidth() 
	{ 
		if(dsdata) return dsdata->videoWidth;  else return 0;
	}
	int getHeight()
	{
		if(dsdata) return dsdata->videoHeight;  else return 0;
	}
	int getVerticalFlip()
	{
		if(dsdata) return dsdata->verticalFlip;  else return 0;
	}

public:
	CCompositorCB    *mCompositorCB;

protected:
	DirectShowData* dsdata;
	DWORD mdwRegister;
};

#endif // __DirectShowUtil__