#include <tchar.h>
#include "DirectShowUtil.h"
#include <dmo.h>
#include <dmodshow.h>
#include <OgreStringConverter.h>
#include "VideoFrame.h"

DEFINE_GUID (CLSID_CColorConvertDMO,
0x98230571, 0x0087, 0x4204, 0xB0, 0x20, 0x32, 0x82, 0x53, 0x8E, 0x57, 0xD3);

#define CLIP_DIR	"c:/TestStreams/"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) {if(x) {(x)->Release(); (x)=NULL; }}
#endif

#define STRM_TYPE_UNKNOWN	0
#define STRM_TYPE_WMV	    1
#define STRM_TYPE_TS	    2
#define STRM_TYPE_MP4	    3

// TODO: Check extension
int checkFileType(const char *pFilePath)
{
	if (strstr(pFilePath, ".ts") || strstr(pFilePath, ".TS")){
		return STRM_TYPE_TS;
	} else if (strstr(pFilePath, ".wmv") || strstr(pFilePath, ".WMV")){
		return STRM_TYPE_WMV;
	} else if (strstr(pFilePath, ".mp4") || strstr(pFilePath, ".mp4")){
		return STRM_TYPE_MP4;
	} else {
		return STRM_TYPE_UNKNOWN;
	}
}

class CSampleGrabberCB : public ISampleGrabberCB
 {
 public:
	 CSampleGrabberCB(CDsPlayer *pParent)
	 {
		mRef = 0;
		mParent = pParent;
	 }

    STDMETHODIMP QueryInterface(REFIID riid, void **ppv) {     
		if(riid == IID_IUnknown || riid == IID_ISampleGrabberCB){
			*ppv = this;
			AddRef();
			return NOERROR;
		} else {
			return E_NOINTERFACE;
		}
    };                                                         
    STDMETHODIMP_(ULONG) AddRef() 
	{
		InterlockedIncrement(&mRef);
		return mRef;
    };                                                         
    STDMETHODIMP_(ULONG) Release() 
	{               
		long lRefCount = InterlockedDecrement(&mRef);
		if (0 == mRef)	{
			delete this;
		}
		return lRefCount;
    };

    virtual HRESULT STDMETHODCALLTYPE SampleCB( 
        double SampleTime,
        IMediaSample *pSample)
	{
		unsigned char *pData;
		if(mParent->mCompositorCB){
			long lSize = pSample->GetSize();
			pSample->GetPointer(&pData);
			mParent->mCompositorCB->NextFrameCB(pData, lSize, SampleTime);
		}
		return S_OK;
	}
    
    virtual HRESULT STDMETHODCALLTYPE BufferCB( 
        double SampleTime,
        BYTE *pBuffer,
        long BufferLen)
	{
		return S_OK;
	}

	long       mRef;
	CDsPlayer *mParent;
 };

CDsPlayer::CDsPlayer(CCompositorCB *pCompositorCB):
		mCompositorCB(pCompositorCB)
{
    // 1) CREATE DSDATA
    dsdata=new DirectShowData;
    // 3) INITIALIZE DIRECT SHOW
    HRESULT hr;

    hr=CoInitialize(NULL);
    if (FAILED(hr)) throw("[DSHOW] Error in co initialize");

    // initialize all pointers
    dsdata->pGraph=0;
    dsdata->pControl=0;
    dsdata->pEvent=0;
    dsdata->pGrabberF=0;
    dsdata->pGrabber=0;
    dsdata->pSeeking=0;
	dsdata->pVidSrc = 0;
	dsdata->verticalFlip = 0;
}

CDsPlayer::~CDsPlayer()
{

    unloadMovie();
    CoUninitialize();

    delete dsdata;
}

HRESULT AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister) 
{
    IMoniker * pMoniker = NULL;
    IRunningObjectTable *pROT = NULL;

    if (FAILED(GetRunningObjectTable(0, &pROT))) {
        return E_FAIL;
    }
    
    const size_t STRING_LENGTH = 256;

    WCHAR wsz[STRING_LENGTH];
 
   StringCchPrintfW(
        wsz, STRING_LENGTH, 
        L"FilterGraph %08x pid %08x", 
        (DWORD_PTR)pUnkGraph, 
        GetCurrentProcessId()
        );
    
    HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
    if (SUCCEEDED(hr)) {
        hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph,
            pMoniker, pdwRegister);
        pMoniker->Release();
    }
    pROT->Release();
    
    return hr;
}

void RemoveFromRot(DWORD pdwRegister)
{
    IRunningObjectTable *pROT;
    if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) {
        pROT->Revoke(pdwRegister);
        pROT->Release();
    }
}

int CDsPlayer::BuildWmvGraph(std::string moviePath)
{
	HRESULT hr;
    IFileSourceFilter *pFS=NULL;
    IBaseFilter *pReader=NULL;
    IDMOWrapperFilter* pWrap = NULL; 
	IBaseFilter *pFilter = NULL;
	IBaseFilter * pRender = NULL;
	int res = -1;

	WCHAR* filepath = util_convertCStringToWString(moviePath.c_str());    
	hr = CoCreateInstance(CLSID_WMAsfReader, NULL,
			CLSCTX_INPROC, IID_IBaseFilter, (void **)&pReader);
	if(FAILED(hr)) {
		logMessage("[DSHOW] Can not create asf reader!"); 
		goto Exit;
	}
	hr = dsdata->pGraph->AddFilter(pReader, L"WM Reader");
	if(FAILED(hr)) {
		logMessage("[DSHOW] Can not add reader!"); 
		goto Exit;
	}
	dsdata->pVidSrc = pReader;

	hr = pReader->QueryInterface(IID_IFileSourceFilter, (void **) &pFS);
	if(FAILED(hr)) {
		logMessage("[DSHOW] Can not get IFileSourceFilter!"); 
		goto Exit;
	}
	pFS->Load(filepath, NULL);

	hr = CoCreateInstance(CLSID_DMOWrapperFilter, NULL,
			CLSCTX_INPROC, IID_IBaseFilter, (void **)&pFilter);
	if(FAILED(hr)){
		logMessage("[DSHOW] Can not create DMOWrapperFilter!"); 
		goto Exit;
	}
	hr = pFilter->QueryInterface(IID_IDMOWrapperFilter, (void **)&pWrap);
	if(FAILED(hr)){
		logMessage("[DSHOW] Can not get IDMOWrapperFilter!"); 
		goto Exit;
	}
	if (SUCCEEDED(hr)) {     // Initialize the filter.
		hr = pWrap->Init(CLSID_CWMVDecMediaObject, DMOCATEGORY_VIDEO_DECODER);
		if(FAILED(hr)){
			logMessage("[DSHOW]Can not create WMV decoder!"); 
			goto Exit;
		}
	}

	if (SUCCEEDED(hr)) {     // Add the filter to the graph.
		hr = dsdata->pGraph->AddFilter(pFilter, L"My DMO");
		if (FAILED(hr)) {
			logMessage("[DSHOW] Error in adding WMV Decoder to the graph");
			goto Exit;
		}
	}
	
	// Connect the src and wmv 
	hr = ConnectFilters(dsdata->pGraph, pReader, pFilter); 
	if(FAILED(hr)) {
		logMessage("[DSHOW] Unsupported media type!"); 
		goto Exit;
	}

	// Connect the wmv and grabber 
	hr = ConnectFilters(dsdata->pGraph, pFilter, dsdata->pGrabberF); 
	if(FAILED(hr)) {
		logMessage("[DSHOW] Can't connect decoder and grabber!"); 
		goto Exit;
	}


	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pRender);
	if(FAILED(hr)) {
		logMessage("[DSHOW] Can not create NullRenderer!"); 
		goto Exit;
	}

	dsdata->pGraph->AddFilter(pRender, L"Render");

	hr = ConnectFilters(dsdata->pGraph, dsdata->pGrabberF, pRender); 
	if(FAILED(hr)) {
		logMessage("[DSHOW] Can't connect render and grabber!"); 
		goto Exit;
	}
	res = 0;

Exit:
	SAFE_RELEASE(pFS)
	SAFE_RELEASE(pWrap)
	SAFE_RELEASE(pFilter)
	SAFE_RELEASE(pRender)

	return res;
}

int CDsPlayer::BuildRtspGraph(std::string moviePath)
{
	HRESULT hr;
    IFileSourceFilter *pFS=NULL;
    IBaseFilter       *pReader=NULL;
	IBaseFilter       *pDecFilter = NULL;;
	IBaseFilter       *pRender = NULL;
    IDMOWrapperFilter *pWrap = NULL; 	
	
	int res = -1;

	WCHAR* filepath = util_convertCStringToWString(moviePath.c_str());    

	hr = CoCreateInstance(CLSID_MCNTsRtspSrc, NULL,
			CLSCTX_INPROC, IID_IBaseFilter, (void **)&pReader);
	if(FAILED(hr)) {
		logMessage("[DSHOW]Creating MCNTsRtspSrc: Failed!"); 
		goto Exit;
	}

	hr = dsdata->pGraph->AddFilter(pReader, L"TS Reader");
	if(FAILED(hr)) {
		logMessage("[DSHOW]Adding MCNTsRtspSrc: Failed!"); 
		goto Exit;
	}
	dsdata->pVidSrc = pReader;

	hr = pReader->QueryInterface(IID_IFileSourceFilter, (void **) &pFS);
	if(FAILED(hr)) {
		logMessage("[DSHOW]Query  MCNTsRtspSrc for IFileSourceFilter : Failed!"); 
		goto Exit;
	}

	pFS->Load(filepath, NULL);


	hr = CoCreateInstance(CLSID_DMOWrapperFilter, NULL,
			CLSCTX_INPROC, IID_IBaseFilter, (void **)&pDecFilter);
	if(FAILED(hr)) 	{
		logMessage("[DSHOW] Careating DMOWrapperFilter: filed!"); 
		goto Exit;
	}
	hr = pDecFilter->QueryInterface(IID_IDMOWrapperFilter, (void **)&pWrap);
	if(FAILED(hr)) {
		logMessage("[DSHOW] Query IDMOWrapperFilter: filed!"); 
		goto Exit;
	}	
	if (SUCCEEDED(hr)) {     // Initialize the filter.
		hr = pWrap->Init(CLSID_McnH264MediaObject, DMOCATEGORY_VIDEO_DECODER);
		if(FAILED(hr)) {
			logMessage("[DSHOW] Crating McnH264MediaObject: Filed!"); 
			goto Exit;
		}
	}

	if (SUCCEEDED(hr)) {     // Add the filter to the graph.
		hr = dsdata->pGraph->AddFilter(pDecFilter, L"My DMO");
		if (FAILED(hr)) {
			throw("[DSHOW] Adding DecFilter filed");
			goto Exit;
		}
	}

	// Connect the src and decoder 
	hr = ConnectFilters(dsdata->pGraph, pReader, pDecFilter); 
	if(FAILED(hr)) {
		logMessage("[DSHOW][DSHOW] Connect Reader and DecFilter filed"); 
		goto Exit;
	}
	// Connect the wmv and grabber 
	hr = ConnectFilters(dsdata->pGraph, pDecFilter, dsdata->pGrabberF); 
	if(FAILED(hr)) {
		logMessage("[DSHOW] Connect DecFilter and Grabber filed!"); 
		goto Exit;
	}


	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pRender);
	if(FAILED(hr)) {
		logMessage("[DSHOW] Creating Null renderer: Failed!"); 
		goto Exit;
	}
	dsdata->pGraph->AddFilter(pRender, L"Render");

	hr = ConnectFilters(dsdata->pGraph, dsdata->pGrabberF, pRender); 
	if(FAILED(hr)) {
		logMessage("[DSHOW] Can't connect grabber and render!"); 
		goto Exit;
	}
	res = 0;
Exit:
    SAFE_RELEASE(pFS)
	SAFE_RELEASE(pDecFilter)
	SAFE_RELEASE(pRender)
    SAFE_RELEASE(pWrap) 	

	return res;

}

int CDsPlayer::BuildTsFilePlayGraph(std::string moviePath)
{
	HRESULT hr;
    IFileSourceFilter *pFS=NULL;
    IBaseFilter *pReader=NULL;
	IBaseFilter *pDecFilter = NULL;
	IBaseFilter *pRender = NULL;
	int Result = -1;		

	WCHAR* filepath = util_convertCStringToWString(moviePath.c_str());    

	hr = CoCreateInstance(CLSID_TSSOurce, NULL,
			CLSCTX_INPROC, IID_IBaseFilter, (void **)&pReader);
	if(FAILED(hr)) {
		logMessage("[DSHOW] Unsupported media type!"); 
	}

	hr = dsdata->pGraph->AddFilter(pReader, L"TS Reader");
	if(FAILED(hr)) {
		logMessage("[DSHOW] Unsupported media type!"); 
		goto Exit;
	}
	dsdata->pVidSrc = pReader;

	hr = pReader->QueryInterface(IID_IFileSourceFilter, (void **) &pFS);
	if(FAILED(hr)) {
		logMessage("[DSHOW] Unsupported media type!"); 
		goto Exit;
	}


	pFS->Load(filepath, NULL);

	hr = CoCreateInstance(CLSID_McnH264DecoderFilter, NULL,
			CLSCTX_INPROC, IID_IBaseFilter, (void **)&pDecFilter);
	if(FAILED(hr)) {
		logMessage("[DSHOW] Unsupported media type!"); 
		goto Exit;
	}


	if (SUCCEEDED(hr)) {     // Add the filter to the graph.
		hr = dsdata->pGraph->AddFilter(pDecFilter, L"H264 Dec");
		if (FAILED(hr)) {
			logMessage("[DSHOW] Error in adding H264 Dec to the graph");
			goto Exit;
		}

	}
	
	// Connect the src and wmv 
	hr = ConnectFilters(dsdata->pGraph, pReader, pDecFilter); 
	if(FAILED(hr)) {
		logMessage("[DSHOW] Unsupported media type!"); 
		goto Exit;
	}


	// Connect the wmv and grabber 
	hr = ConnectFilters(dsdata->pGraph, pDecFilter, dsdata->pGrabberF); 
	if(FAILED(hr)) {
		logMessage("[DSHOW] Unsupported media type!"); 
		goto Exit;
	}



	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pRender);
	if(FAILED(hr)) {
		logMessage("[DSHOW] Unsupported media type!"); 
		goto Exit;
	}


	dsdata->pGraph->AddFilter(pRender, L"Render");

	hr = ConnectFilters(dsdata->pGraph, dsdata->pGrabberF, pRender); 
	if(FAILED(hr)) {
		logMessage("[DSHOW] Can't create render!"); 
		goto Exit;
	}
	Result = 0;

Exit:
    SAFE_RELEASE(pFS)
	SAFE_RELEASE(pDecFilter)
	SAFE_RELEASE(pRender)

	return Result;
}


int CDsPlayer::BuildMp4Graph(std::string moviePath)
{
	HRESULT hr;
    IFileSourceFilter *pFS=NULL;
    IBaseFilter *pReader=NULL;
	IBaseFilter *pDecFilter = NULL;
	IBaseFilter *pRender = NULL;
    IDMOWrapperFilter* pWrap = NULL; 
	int Result = -1;		

	WCHAR* filepath = util_convertCStringToWString(moviePath.c_str());    

	hr = CoCreateInstance(CLSID_McnMp4Demux, NULL,
			CLSCTX_INPROC, IID_IBaseFilter, (void **)&pReader);
	if(FAILED(hr)) {
		logMessage("[DSHOW] Unsupported media type!"); 
	}

	hr = dsdata->pGraph->AddFilter(pReader, L"MP4 Reader");
	if(FAILED(hr)) {
		logMessage("[DSHOW] Unsupported media type!"); 
		goto Exit;
	}
	dsdata->pVidSrc = pReader;

	hr = pReader->QueryInterface(IID_IFileSourceFilter, (void **) &pFS);
	if(FAILED(hr)) {
		logMessage("[DSHOW] Unsupported media type!"); 
		goto Exit;
	}

	pFS->Load(filepath, NULL);

#if 0
	hr = CoCreateInstance(CLSID_CMPEG2VidDecoderDS, NULL,
			CLSCTX_INPROC, IID_IBaseFilter, (void **)&pDecFilter);
	if(FAILED(hr)) {
		logMessage("[DSHOW] Unsupported media type!"); 
		goto Exit;
	}

#else

	hr = CoCreateInstance(CLSID_DMOWrapperFilter, NULL,
			CLSCTX_INPROC, IID_IBaseFilter, (void **)&pDecFilter);
	if(FAILED(hr)) 	{
		logMessage("[DSHOW] Unsupported media type!"); 
		goto Exit;
	}
	hr = pDecFilter->QueryInterface(IID_IDMOWrapperFilter, (void **)&pWrap);
	if(FAILED(hr)) {
		logMessage("[DSHOW] Unsupported media type!"); 
		goto Exit;
	}	
	if (SUCCEEDED(hr)) {     // Initialize the filter.
		hr = pWrap->Init(CLSID_McnH264MediaObject, DMOCATEGORY_VIDEO_DECODER);
		if(FAILED(hr)) {
			logMessage("[DSHOW] Unsupported media type!"); 
			goto Exit;
		}
	}
#endif



	if (SUCCEEDED(hr)) {     // Add the filter to the graph.
		hr = dsdata->pGraph->AddFilter(pDecFilter, L"H264 Dec");
		if (FAILED(hr)) {
			logMessage("[DSHOW] Error in adding H264 Dec to the graph");
			goto Exit;
		}

	}
	
	// Connect the src and wmv 
	hr = ConnectFilters(dsdata->pGraph, pReader, pDecFilter); 
	if(FAILED(hr)) {
		logMessage("[DSHOW] Unsupported media type!"); 
		goto Exit;
	}

	hr = ConnectFilters(dsdata->pGraph, pDecFilter, dsdata->pGrabberF); 
	if(FAILED(hr)) {
		logMessage("[DSHOW] Unsupported media type!"); 
		goto Exit;
	}



	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pRender);
	if(FAILED(hr)) {
		logMessage("[DSHOW] Unsupported media type!"); 
		goto Exit;
	}


	dsdata->pGraph->AddFilter(pRender, L"Render");

	hr = ConnectFilters(dsdata->pGraph, dsdata->pGrabberF, pRender); 
	if(FAILED(hr)) {
		logMessage("[DSHOW] Can't create render!"); 
		goto Exit;
	}

	Result = 0;
Exit:
    SAFE_RELEASE(pFS)
	SAFE_RELEASE(pDecFilter)
	SAFE_RELEASE(pRender)
    SAFE_RELEASE(pWrap)
	return Result;
}

HRESULT CDsPlayer::FindCaptureDevice(std::string DeviceName, IBaseFilter ** ppSrcFilter)
{
    HRESULT hr = E_FAIL;
    IBaseFilter * pSrc = NULL;
    IMoniker *pMoniker =NULL;
    ULONG cFetched;
	BOOL fFound = false;
    if (!ppSrcFilter)
        return E_POINTER;
   
    // Create the system device enumerator
   ICreateDevEnum *pDevEnum = NULL;

    hr = CoCreateInstance (CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
                           IID_ICreateDevEnum, (void **) &pDevEnum);
    if (FAILED(hr))  {
        return hr;
    }

    // Create an enumerator for the video capture devices
    IEnumMoniker *pClassEnum = NULL;

    hr = pDevEnum->CreateClassEnumerator (CLSID_VideoInputDeviceCategory, &pClassEnum, 0);
    if (FAILED(hr))  {
        return hr;
    }

    // If there are no enumerators for the requested type, then 
    // CreateClassEnumerator will succeed, but pClassEnum will be NULL.
    if (pClassEnum == NULL) {
        return hr;
    }

    while (S_OK == (pClassEnum->Next (1, &pMoniker, &cFetched)))  {

		// Display name
		LPOLESTR strMonikerName=0;
		hr = pMoniker->GetDisplayName(NULL, NULL, &strMonikerName);
		if (FAILED(hr)){
			return hr;
		}
		TCHAR szMonikerName[256] = {0};

		if(wcslen(strMonikerName) > MAX_PATH)
			return ERROR_FILENAME_EXCED_RANGE;
		if(!WideCharToMultiByte(CP_ACP,0,strMonikerName,-1,szMonikerName,MAX_PATH,0,0))
			return ERROR_INVALID_NAME;


        IPropertyBag *pBag=0;
        hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
        if(SUCCEEDED(hr)) {
			std::string strDevice;
            VARIANT var;
            var.vt = VT_BSTR;
            hr = pBag->Read(L"FriendlyName", &var, NULL);
            if(hr == NOERROR) {
				WideCharToMultiByte(CP_ACP,0,var.bstrVal,-1,szMonikerName,MAX_PATH,0,0);
				strDevice = szMonikerName;
                SysFreeString(var.bstrVal);
            }
			
			strDevice += DEVICE_INSTANCE_SEPARATOR;

            hr = pBag->Read(L"DevicePath", &var, NULL);
            if(hr == NOERROR) {
				WideCharToMultiByte(CP_ACP,0,var.bstrVal,-1,szMonikerName,MAX_PATH,0,0);
				strDevice += szMonikerName;
                SysFreeString(var.bstrVal);
            }
			
			if(stricmp(DeviceName.c_str(), strDevice.c_str()) == 0)
				fFound = 1;

            pBag->Release();
        } 


		if(fFound) {
			hr = pMoniker->BindToObject(0,0,IID_IBaseFilter, (void**)&pSrc);
			if (FAILED(hr)) {
				return hr;
			}
			break;
		}
		SAFE_RELEASE(pMoniker)
    } 
	if(pSrc) {
		*ppSrcFilter = pSrc;
	} else {
		logMessage("Can not find:" + DeviceName); 
		hr = E_FAIL;
	}

Exit:

	SAFE_RELEASE(pClassEnum)
	SAFE_RELEASE(pDevEnum)
    return hr;
}

#define DEF_CAP_WIDTH      640
#define DEF_CAP_HEIGHT     480

int CDsPlayer::BuildCaptureGraph(std::string moviePath, int nCapWidth, int nCapHeight)
{
	HRESULT hr;
	int Result = -1;
    IBaseFilter *pReader=NULL;
	IBaseFilter *pDecFilter = NULL;
	IBaseFilter * pRender = NULL;
	IDMOWrapperFilter* pWrap = NULL; 

	GUID OutputSubType = MEDIASUBTYPE_MJPG;
	hr = FindCaptureDevice(moviePath, &pReader);
	if(FAILED(hr)) {
		logMessage("[DSHOW] Can not open camera!"); 
		goto Exit;
	}
	// Set preferred size
	if (ChooseVidFormat(dsdata->pGraph,  pReader, MEDIASUBTYPE_MJPG, nCapWidth,nCapHeight) == 0) {
		OutputSubType = MEDIASUBTYPE_MJPG;
		//logMessage("[DSHOW] Can notset MJPEG output type. Trying YUV!"); 
	} else 	if(ChooseVidFormat(dsdata->pGraph,  pReader, MEDIASUBTYPE_YUY2, nCapWidth,nCapHeight) == 0){
		OutputSubType = MEDIASUBTYPE_YUY2;
	} else if (ChooseVidFormat(dsdata->pGraph,  pReader, MEDIASUBTYPE_RGB24, nCapWidth,nCapHeight) == 0) {
		OutputSubType = MEDIASUBTYPE_RGB24;
	} else  if(nCapWidth > DEF_CAP_WIDTH || nCapHeight > DEF_CAP_HEIGHT) {
		logMessage("BuildCaptureGraph: Can not required resolution. Try 640x460!");
		if (ChooseVidFormat(dsdata->pGraph,  pReader, MEDIASUBTYPE_MJPG, DEF_CAP_WIDTH,DEF_CAP_HEIGHT) == 0) {
			OutputSubType = MEDIASUBTYPE_MJPG;
			//logMessage("[DSHOW] Can notset MJPEG output type. Trying YUV!"); 
		} else if(ChooseVidFormat(dsdata->pGraph,  pReader, MEDIASUBTYPE_YUY2, DEF_CAP_WIDTH,DEF_CAP_HEIGHT) == 0){
			OutputSubType = MEDIASUBTYPE_YUY2;
		} else if (ChooseVidFormat(dsdata->pGraph,  pReader, MEDIASUBTYPE_RGB24, DEF_CAP_WIDTH,DEF_CAP_HEIGHT) == 0) {
			OutputSubType = MEDIASUBTYPE_RGB24;
		} else {
			logMessage("Can not set default format 640x480!");
			goto Exit;
		}
	} else {
		logMessage("BuildCaptureGraph: Can not set format 640x480!");
		goto Exit;
	}
	
	hr = dsdata->pGraph->AddFilter(pReader, L"TS Reader");
	if(FAILED(hr)) {
		logMessage("[DSHOW] Unsupported media type!"); 
		goto Exit;
	}
	dsdata->pVidSrc = pReader;

	if(OutputSubType == MEDIASUBTYPE_MJPG) {

		hr = CoCreateInstance(CLSID_MjpegDec, NULL,
				CLSCTX_INPROC, IID_IBaseFilter, (void **)&pDecFilter);
		if(FAILED(hr)) {
			logMessage("[DSHOW] Can not create MJPRG Decoder!"); 
			goto Exit;
		}

		if (SUCCEEDED(hr)) {     // Add the filter to the graph.
			hr = dsdata->pGraph->AddFilter(pDecFilter, L"Dec");
			if (FAILED(hr)) {
				logMessage("[DSHOW] Error in adding MJPEG Dec to the graph");
				goto Exit;
			}
		}
	} else {
#if 0
		hr = CoCreateInstance(CLSID_Colour, NULL,
				CLSCTX_INPROC, IID_IBaseFilter, (void **)&pDecFilter);
		if(FAILED(hr)) {
			logMessage("[DSHOW] Can not create colorspace converter!"); 
			goto Exit;
		}
#else
		hr = CoCreateInstance(CLSID_DMOWrapperFilter, NULL,
				CLSCTX_INPROC, IID_IBaseFilter, (void **)&pDecFilter);
		if(FAILED(hr)){
			logMessage("[DSHOW] Can not create DMOWrapperFilter!"); 
			goto Exit;
		}
		hr = pDecFilter->QueryInterface(IID_IDMOWrapperFilter, (void **)&pWrap);
		if(FAILED(hr)){
			logMessage("[DSHOW] Can not get IDMOWrapperFilter!"); 
			goto Exit;
		}
		if (SUCCEEDED(hr)) {     // Initialize the filter.
			hr = pWrap->Init(CLSID_CColorConvertDMO, DMOCATEGORY_VIDEO_EFFECT);
			if(FAILED(hr)){
				logMessage("[DSHOW]Can not create CColorConvertDMO!"); 
				goto Exit;
			}
		}
#endif


		if (SUCCEEDED(hr)) {     // Add the filter to the graph.
			hr = dsdata->pGraph->AddFilter(pDecFilter, L"Dec");
			if (FAILED(hr)) {
				logMessage("[DSHOW] Error in adding Color Space converter to the graph");
				goto Exit;
			}
		}
	}
	// Connect the src and wmv 
	hr = ConnectFilters(dsdata->pGraph, pReader, pDecFilter); 
	if(FAILED(hr)) {
		logMessage("[DSHOW] Can not connect camera and decoder!"); 
		goto Exit;
	}


	// Connect the wmv and grabber 
	hr = ConnectFilters(dsdata->pGraph, pDecFilter, dsdata->pGrabberF); 
	if(FAILED(hr)) {
		logMessage("[DSHOW] Can not connect decoder and grabber!"); 
		goto Exit;
	}

	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pRender);
	if(FAILED(hr)) {
		logMessage("[DSHOW] Can not create NULL renderer!"); 
		goto Exit;
	}


	dsdata->pGraph->AddFilter(pRender, L"Render");

	hr = ConnectFilters(dsdata->pGraph, dsdata->pGrabberF, pRender); 
	if(FAILED(hr)) {
		logMessage("[DSHOW] Can't create render!"); 
		goto Exit;
	}
	Result = 0;

Exit:
	SAFE_RELEASE(pWrap);
	SAFE_RELEASE(pDecFilter);
	SAFE_RELEASE(pRender);

	return Result;
}

int CDsPlayer::loadMovie(
    const Ogre::String& filename,
	int nCapWidth, int nCapHeight
	)
{
	HRESULT hr;
	int res = -1;
	std::string moviePath =  /*CLIP_DIR +*/ filename;

	// destroy previous movie objects (if any)
	unloadMovie();

	// create filter graph and get interfaces
	hr=CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
		IID_IGraphBuilder, (void**) &dsdata->pGraph);
	if (FAILED(hr)) {
		throw("[DSHOW] Error in creating graph");
		goto Exit;
	}


	hr =  AddToRot(dsdata->pGraph , &mdwRegister);
	if (FAILED(hr)) {
		throw("[DSHOW] Error in AddToRot");
		goto Exit;
	}

	hr=dsdata->pGraph->QueryInterface(IID_IMediaControl, (void**) & dsdata->pControl);
	if (FAILED(hr)) {
		throw("[DSHOW] Error in querying media control");
		goto Exit;
	}

	hr=dsdata->pGraph->QueryInterface(IID_IMediaEvent, (void**) & dsdata->pEvent);
	if (FAILED(hr)) {
		throw("[DSHOW] Error in querying media event");
		goto Exit;
	}


	hr=dsdata->pGraph->QueryInterface(IID_IMediaSeeking, (void**) & dsdata->pSeeking);
	if (FAILED(hr)) {
		throw("[DSHOW] Error in querying seeking interface");
		goto Exit;
	}
	// create sample grabber
	hr=CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, (void**)&dsdata->pGrabberF);
	if (FAILED(hr)) {
		throw("[DSHOW] Error in creating sample grabber");
		goto Exit;
	}

	// add sample grabber to the graph
	hr=dsdata->pGraph->AddFilter(dsdata->pGrabberF, L"Sample Grabber");
	if (FAILED(hr)) {
		throw("[DSHOW] Error in adding sample grabber to the graph");
		goto Exit;
	}

	//if( BuildWmvGraph(moviePath) == 0) {

	{
	
	// get sample grabber object
		hr = dsdata->pGrabberF->QueryInterface(IID_ISampleGrabber,
			(void**)&dsdata->pGrabber);
		if (FAILED(hr)) {
			throw("[DSHOW] Error getting connected media type info");
		goto Exit;
	}

#if 1
		// set sample grabber media type
		AM_MEDIA_TYPE mt;
		ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
		mt.majortype = MEDIATYPE_Video;
		mt.subtype = MEDIASUBTYPE_RGB32;
		mt.formattype = FORMAT_VideoInfo;
#if 0
			const DWORD bits888[3] = {0xFF0000,0x00FF00,0x0000FF};
			VIDEOINFO VidInf = {0};
			VIDEOINFO *pVidInf = &VidInf;
			mt.pbFormat = (BYTE *)pVidInf;
			mt.cbFormat = sizeof(VIDEOINFO);
			mt.lSampleSize = 640 * 480 * 4;
			pVidInf->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			pVidInf->bmiHeader.biWidth = 640;
			pVidInf->bmiHeader.biHeight = 480;
			pVidInf->AvgTimePerFrame = 3003;
			pVidInf->bmiHeader.biCompression = BI_RGB;
			pVidInf->bmiHeader.biBitCount = 32;
			pVidInf->bmiHeader.biPlanes = 1;
			pVidInf->bmiHeader.biSizeImage = mt.lSampleSize;
			for(int i = 0; i < 3; i++)
				pVidInf->TrueColorInfo.dwBitMasks[i] = bits888[i];

#endif
		hr=dsdata->pGrabber->SetMediaType(&mt);
		if (FAILED(hr)) throw("[DSHOW] Error in setting sample grabber media type");
#endif
		dsdata->pGrabber->SetOneShot(FALSE);
		dsdata->pGrabber->SetBufferSamples(TRUE);

		CSampleGrabberCB *pGrabberCb = new CSampleGrabberCB(this);
		//pGrabberCb->AddRef();
		dsdata->pGrabber->SetCallback(pGrabberCb, 0);
	}

	if(strnicmp(moviePath.c_str(), "rtsp://", strlen("rtsp://")) == 0){
		if(BuildRtspGraph(moviePath) != 0){
			goto Exit;
		}
	} else if(strnicmp(moviePath.c_str(), "device://", strlen("device://")) == 0){
		std::string DeviceName = moviePath.c_str() + strlen ("device://");
		// TODO: Detect
		dsdata->verticalFlip = 1;

		if(BuildCaptureGraph(DeviceName, nCapWidth, nCapHeight) != 0)
			goto Exit;
	} else {
		int FileType = checkFileType(moviePath.c_str());
		switch(FileType){
			case STRM_TYPE_TS :
			if(BuildTsFilePlayGraph(moviePath) != 0) 
				goto Exit;
			break;
			
			case STRM_TYPE_WMV:
			{
				// TODO: Detect
				dsdata->verticalFlip = 1;
				if(BuildWmvGraph(moviePath) != 0) 
					goto Exit;
			}
			break;
			case STRM_TYPE_MP4:
			{
				if(BuildMp4Graph(moviePath) != 0) 
					goto Exit;
			}
			break;

			case STRM_TYPE_UNKNOWN:
			default:
				goto Exit;
				break;
		}

	}
	{
		// get video information
		AM_MEDIA_TYPE mtt = {0};
		hr=dsdata->pGrabber->GetConnectedMediaType(&mtt);
		if (FAILED(hr)) throw("[DSHOW] Error getting connected media type info");

		VIDEOINFOHEADER *vih = (VIDEOINFOHEADER*) mtt.pbFormat;
		dsdata->videoWidth=vih->bmiHeader.biWidth;
		dsdata->videoHeight=vih->bmiHeader.biHeight;
		// microsoft's help version of free media type
		if (mtt.cbFormat != 0)	{
			CoTaskMemFree((PVOID)mtt.pbFormat);
			mtt.cbFormat = 0;
			mtt.pbFormat = NULL;
		}
		if (mtt.pUnk != NULL){
			mtt.pUnk->Release();
			mtt.pUnk = NULL;
		}
	}

	res = 0;
Exit:
	return res;
}

Ogre::Vector2 CDsPlayer::getMovieDimensions()
{
    return Ogre::Vector2(dsdata->videoWidth, dsdata->videoHeight);
}

void CDsPlayer::unloadMovie()
{
    if (dsdata->pGraph==0)
        return;

    SAFE_RELEASE(dsdata->pGrabber)
    SAFE_RELEASE(dsdata->pGrabberF)
    SAFE_RELEASE(dsdata->pSeeking)
    SAFE_RELEASE(dsdata->pControl)
    SAFE_RELEASE(dsdata->pEvent)
	
	RemoveFromRot(mdwRegister);
	if(dsdata->pVidSrc){
		NukeDownstream(dsdata->pGraph, dsdata->pVidSrc);
		SAFE_RELEASE(dsdata->pVidSrc)
	}

    SAFE_RELEASE(dsdata->pGraph)
}

void CDsPlayer::pauseMovie()
{
    // pause!
    if (dsdata->pControl)
        dsdata->pControl->Pause();
}

void CDsPlayer::playMovie()
{
    // play!
	if (dsdata->pControl){
		OAFilterState Sate;
		dsdata->pControl->GetState(1000, &Sate);
        dsdata->pControl->Run();
	}
}

void CDsPlayer::rewindMovie()
{
    if (!dsdata->pSeeking) return;

    // rewind!
    LONGLONG p1=0;
    LONGLONG p2=0;

    dsdata->pSeeking->SetPositions(
        &p1, AM_SEEKING_AbsolutePositioning, &p2, AM_SEEKING_NoPositioning);
}

void CDsPlayer::stopMovie()
{
    // stop!
    if (dsdata->pControl)
        dsdata->pControl->Stop();
}

// TODO: Remove this. It is supeseded by callback mechanism
int CDsPlayer::getFrame(char *pBuffer, int *pnSize)
{
    HRESULT hr;
    unsigned int i, idx;
    int x, y;
    BYTE* bmpTmp;

    // only do this if there is a graph that has been set up
    if (!dsdata->pGraph)
        return -1;

    // Find the required buffer size.
    long cbBuffer = 0;
	if(isEndOfPlay()){
		rewindMovie();
		int n =0;
	}
    hr = dsdata->pGrabber->GetCurrentBuffer(&cbBuffer, NULL);
    if (cbBuffer<=0)  {
        // nothing to do here yet
        return -1;
    }

    hr = dsdata->pGrabber->GetCurrentBuffer(&cbBuffer, (long*)pBuffer);
    if (hr==E_INVALIDARG || hr==VFW_E_NOT_CONNECTED || hr==VFW_E_WRONG_STATE)  {
        // we aren't buffering samples yet, do nothing
        delete[] pBuffer;
        return -1;
    }
    if (FAILED(hr)) throw("[DSHOW] Failed at GetCurrentBuffer!");
	*pnSize = cbBuffer;
	return 0;
}


bool CDsPlayer::isEndOfPlay()
{
    bool fRes = false;
    HRESULT hr;

    if (dsdata->pEvent!=NULL){
        long ev, p1, p2;

        hr=dsdata->pEvent->GetEvent(&ev, &p1, &p2, 0);
		if(hr == S_OK)	{
            // check for completion
            if (ev==EC_COMPLETE) {
                fRes =  true;
            }
            hr=dsdata->pEvent->FreeEventParams(ev, p1, p2);
        }
    }
    return fRes;
}

bool CDsPlayer::isPlayingMovie()
{
    OAFilterState pfs;
    HRESULT hr;

    if (dsdata->pEvent!=NULL){
        long ev, p1, p2;

        while (E_ABORT!=dsdata->pEvent->GetEvent(&ev, &p1, &p2, 0)){
            // check for completion
            if (ev==EC_COMPLETE) {
                pauseMovie();
                return false;
            }

            // release event params
            hr=dsdata->pEvent->FreeEventParams(ev, p1, p2);
            if (FAILED(hr)) {
                pauseMovie();
                return false;
            }
        }
    }

    if (dsdata->pControl!=NULL) {
        hr=dsdata->pControl->GetState(0, &pfs);
        if (FAILED(hr)) {
            pauseMovie();
            return false;
        }
        return pfs==State_Running;
    }

    // it hasn't even been initialized!
    return false;
}

WCHAR* util_convertCStringToWString(const char* string)
{
    const int MAX_STRINGZ=500;
    static WCHAR wtext[MAX_STRINGZ+2];

    if (strlen(string)>MAX_STRINGZ)
        return 0;

    // convert text to wchar
    if (MultiByteToWideChar(
        CP_ACP,// ansi code page
        0,// flags
        string,// orig string
        -1,// calculate len
        wtext,// where to put the string
        MAX_STRINGZ)// maximum allowed path
        ==0)
    {
        throw("[DSHOW] convertCStringToWString failed with no extra error info");
    }

    return wtext;
}

