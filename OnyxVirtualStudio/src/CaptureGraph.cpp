#include "CaptureGraph.h"
#include <dmo.h>
#include <dmodshow.h>
#include <OgreStringConverter.h>

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) {if(x) {(x)->Release(); (x)=NULL; }}
#endif

#define STRM_TYPE_UNKNOWN	    0
#define CAPTURE_TYPE_MP4_FILE   1
#define CAPTURE_TYPE_HLS_FILE   2
#define CAPTURE_TYPE_HLS_HTTP   3
#define CAPTURE_TYPE_HLS_S3		4

// TODO: Check extension
static int checkFileType(const char *pFilePath)
{
	if (strstr(pFilePath, ".mp4") || strstr(pFilePath, ".MP4")){
		return CAPTURE_TYPE_MP4_FILE;
	} else if (strstr(pFilePath, ".m3u8") || strstr(pFilePath, ".M3U8") || strstr(pFilePath, ".cfg")){
		if(strncmp(pFilePath, "http:", strlen("http:")) == 0)
			return CAPTURE_TYPE_HLS_HTTP;
		else if (strstr(pFilePath, ".cfg"))
			return  CAPTURE_TYPE_HLS_S3;
		else
			return CAPTURE_TYPE_HLS_FILE;

	} else {
		return STRM_TYPE_UNKNOWN;
	}
}


CCaptureGraph::CCaptureGraph()
{
    HRESULT hr;

    hr=CoInitialize(NULL);
    if (FAILED(hr)) throw("[DSHOW] Error in co initialize");

    // initialize all pointers
    pGraph=0;
    pControl=0;
    pEvent=0;
    mpVidCapSrc=0;
    pISampleFeeder=0;
    pSeeking=0;
	pAudCapSource = NULL;
	mpEncFilter = NULL;
}

CCaptureGraph::~CCaptureGraph()
{
    Deinit();
    CoUninitialize();
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

int CCaptureGraph::BuildAudioCaptureSubGraph(IBaseFilter *pAudCapFilter, IBaseFilter * pSink)
{
	HRESULT hr;
	IBaseFilter *pAudEncFilter = NULL;
	IDMOWrapperFilter* pAudWrap = NULL; 


	hr = CoCreateInstance(CLSID_DMOWrapperFilter, NULL,
			CLSCTX_INPROC, IID_IBaseFilter, (void **)&pAudEncFilter);
	if(FAILED(hr)){
		logMessage("[DSHOW] Unsupported media type!"); 
		goto Exit;
	}
	hr = pAudEncFilter->QueryInterface(IID_IDMOWrapperFilter, (void **)&pAudWrap);
	if(FAILED(hr)){
		logMessage("[DSHOW] Unsupported media type!"); 
		goto Exit;
	}
	if (SUCCEEDED(hr)) {     // Initialize the filter.
		hr = pAudWrap->Init(CLSID_McnAacEncMediaObject, DMOCATEGORY_AUDIO_ENCODER);
		if(FAILED(hr)){
			logMessage("[DSHOW] Unsupported media type!"); 
			goto Exit;
		}
	}

	if (SUCCEEDED(hr)) {     // Add the filter to the graph.
		hr = pGraph->AddFilter(pAudEncFilter, L"MCN Aud Enc DMO");
		if (FAILED(hr)) {
			logMessage("[DSHOW] Error in adding MCN Aud Enc DMO the graph");
			goto Exit;
		}
	}
	if (SUCCEEDED(hr)) {     // Add the filter to the graph.
		hr = pGraph->AddFilter(pAudCapSource, L"Audio Capture Source");
		if (FAILED(hr)) {
			logMessage("[DSHOW] Error in adding MCN Aud Enc DMO the graph");
			goto Exit;
		}
	}

	// Connect the src and enc 
	hr = ConnectFilters(pGraph, pAudCapSource, pAudEncFilter); 
	if(FAILED(hr)) {
		logMessage("[DSHOW] Unsupported media type!"); 
		goto Exit;
	}
	hr = ConnectFilters(pGraph, pAudEncFilter, pSink); 
	if(FAILED(hr)) {
		logMessage("[DSHOW] Can't create render!"); 
		goto Exit;
	}

Exit:
	SAFE_RELEASE(pAudWrap)
	SAFE_RELEASE(pAudEncFilter)
	return 0;

}
int CCaptureGraph::BuildCaptureFileSaveGraph(std::string moviePath)
{
	HRESULT hr;
    IFileSinkFilter *pFS=NULL;
    IDMOWrapperFilter* pWrap = NULL; 
	IBaseFilter *pEncFilter = NULL;
	IBaseFilter * pSink = NULL;
	int res = -1;

	WCHAR* filepath = util_convertCStringToWString(moviePath.c_str());    

	hr = CoCreateInstance(CLSID_DMOWrapperFilter, NULL,
			CLSCTX_INPROC, IID_IBaseFilter, (void **)&pEncFilter);
	if(FAILED(hr)){
		logMessage("[DSHOW] Unsupported media type!"); 
		goto Exit;
	}
	hr = pEncFilter->QueryInterface(IID_IDMOWrapperFilter, (void **)&pWrap);
	if(FAILED(hr)){
		logMessage("[DSHOW] Unsupported media type!"); 
		goto Exit;
	}
	if (SUCCEEDED(hr)) {     // Initialize the filter.
		hr = pWrap->Init(CLSID_McnH264EncMediaObject, DMOCATEGORY_VIDEO_ENCODER);
		if(FAILED(hr)){
			logMessage("[DSHOW] Unsupported media type!"); 
			goto Exit;
		}
	}

	if (SUCCEEDED(hr)) {     // Add the filter to the graph.
		hr = pGraph->AddFilter(pEncFilter, L"My DMO");
		if (FAILED(hr)) {
			logMessage("[DSHOW] Error in adding WMV Decoder to the graph");
			goto Exit;
		}
	}
	
	// Connect the src and wmv 
	hr = ConnectFilters(pGraph, mpVidCapSrc, pEncFilter); 
	if(FAILED(hr)) {
		logMessage("[DSHOW] Unsupported media type!"); 
		goto Exit;
	}

	int nFileType = checkFileType(moviePath.c_str());
	if(nFileType == CAPTURE_TYPE_MP4_FILE) {
		hr = CoCreateInstance(CLSID_McnMp4mux, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pSink);
		if(FAILED(hr)) {
			logMessage("[DSHOW] Unsupported media type!"); 
			goto Exit;
		}
	} else if(nFileType == CAPTURE_TYPE_HLS_FILE || nFileType == CAPTURE_TYPE_HLS_HTTP || nFileType == CAPTURE_TYPE_HLS_S3) {
		hr = CoCreateInstance(CLSID_HlsSrvFilter, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pSink);
		if(FAILED(hr)) {
			logMessage("[DSHOW] Unsupported media type!"); 
			goto Exit;
		}
	} else {
		logMessage("[DSHOW] Unknown Destination!"); 
		goto Exit;
	}
	pGraph->AddFilter(pSink, L"Render");

	hr = ConnectFilters(pGraph, pEncFilter, pSink); 
	if(FAILED(hr)) {
		logMessage("[DSHOW] Can't create render!"); 
		goto Exit;
	}
	hr = pSink->QueryInterface(IID_IFileSinkFilter, (void **) &pFS);
	if(FAILED(hr)) {
		logMessage("[DSHOW] Unsupported media type!"); 
		goto Exit;
	}
	pFS->SetFileName(filepath, NULL);

	if(pAudCapSource) {
		BuildAudioCaptureSubGraph(pAudCapSource, pSink);
	}
	mpEncFilter = pEncFilter;
	res = 0;
Exit:
	SAFE_RELEASE(pFS)
	SAFE_RELEASE(pWrap)
	//SAFE_RELEASE(pEncFilter)
	SAFE_RELEASE(pSink)

	return res;
}

int CCaptureGraph::Init(
		const Ogre::String& destPath, 
		const Ogre::String& audDevicePath, 
		bool horizontalMirroring)
{
	HRESULT hr;
	int res = -1;

	// destroy previous movie objects (if any)
	Deinit();

	// create filter graph and get interfaces
	hr=CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
		IID_IGraphBuilder, (void**) &pGraph);
	if (FAILED(hr)) {
		throw("[DSHOW] Error in creating graph");
		goto Exit;
	}


	hr =  AddToRot(pGraph , &mdwRegister);
	if (FAILED(hr)) {
		throw("[DSHOW] Error in AddToRot");
		goto Exit;
	}

	hr=pGraph->QueryInterface(IID_IMediaControl, (void**) &pControl);
	if (FAILED(hr)) {
		throw("[DSHOW] Error in querying media control");
		goto Exit;
	}

	hr=pGraph->QueryInterface(IID_IMediaEvent, (void**) &pEvent);
	if (FAILED(hr)) {
		throw("[DSHOW] Error in querying media event");
		goto Exit;
	}


	hr=pGraph->QueryInterface(IID_IMediaSeeking, (void**) &pSeeking);
	if (FAILED(hr)) {
		throw("[DSHOW] Error in querying seeking interface");
		goto Exit;
	}
	// create vid capture source
#ifdef USE_MCN_VCAM
	hr=CoCreateInstance(CLSID_McnVCam, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, (void**)&mpVidCapSrc);
	if (FAILED(hr)) {
		throw("[DSHOW] Error in creating sample grabber");
		goto Exit;
	}
#else
	hr=CoCreateInstance(CLSID_McnSampleFeeder, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, (void**)&mpVidCapSrc);
	if (FAILED(hr)) {
		throw("[DSHOW] Error in creating sample grabber");
		goto Exit;
	}
#endif
	// add sample grabber to the graph
	hr=pGraph->AddFilter(mpVidCapSrc, L"Sample Feeder");
	if (FAILED(hr)) {
		throw("[DSHOW] Error in adding sample grabber to the graph");
		goto Exit;
	}

	{
	
#ifndef USE_MCN_VCAM
		hr = mpVidCapSrc->QueryInterface(IID_IMcnSampleFeeder,
			(void**)&pISampleFeeder);
		if (FAILED(hr)) {
			throw("[DSHOW] Error getting connected media type info");
			goto Exit;
		}
#endif

		// set sample grabber media type
		AM_MEDIA_TYPE mt;
		ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
		mt.majortype = MEDIATYPE_Video;
		mt.subtype = MEDIASUBTYPE_RGB32;
		mt.formattype = FORMAT_VideoInfo;
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

#ifndef USE_MCN_VCAM
		hr=pISampleFeeder->SetMediaType(&mt);
		if (FAILED(hr)) {
			throw("[DSHOW] Error in setting sample grabber media type");
			goto Exit;
		}
#endif
	}

	if(!audDevicePath.empty()){
		std::string DeviceName;
		if(strnicmp(audDevicePath.c_str(), "device://", strlen("device://")) == 0){
			DeviceName = audDevicePath.c_str() + strlen ("device://");
		}
		GetCaptureDevice(CLSID_AudioInputDeviceCategory, DeviceName, &pAudCapSource);
		if(pAudCapSource) {
			SetAudioProperties(pAudCapSource, 44100, 2, 1024);
		}
	}
	videoUpsidedown = false;
	if(BuildCaptureFileSaveGraph(destPath) != 0) 
					goto Exit;

	res = 0;
Exit:
	return res;
}

void CCaptureGraph::Deinit()
{
    if (pGraph==0)
        return;

    SAFE_RELEASE(pISampleFeeder)
    SAFE_RELEASE(pSeeking)
    SAFE_RELEASE(pControl)
    SAFE_RELEASE(pEvent)
	SAFE_RELEASE(mpEncFilter);

	RemoveFromRot(mdwRegister);

	if(mpVidCapSrc) {
		NukeDownstream(pGraph, mpVidCapSrc);
		SAFE_RELEASE(mpVidCapSrc)
	}
    SAFE_RELEASE(pGraph)
}

void CCaptureGraph::pause()
{
    // pause!
    if (pControl)
        pControl->Pause();
}

void CCaptureGraph::start()
{
    // play!
	if (pControl){
		OAFilterState Sate;
		pControl->GetState(1000, &Sate);
        pControl->Run();
	}
}

void CCaptureGraph::stop()
{
    // stop!
    if (pControl)
        pControl->Stop();
}

void CCaptureGraph::SetVidEncProperties(
	int           nBitrate,
	int           nGopLen,
	int           nFrameStruct
	)
{
	if(mpEncFilter)
		CDsUtil::SetVidEncProperties(mpEncFilter, nBitrate, nGopLen, nFrameStruct);
}

int CCaptureGraph::sendFrame(char *pBuffer, int *pnSize)
{
    HRESULT hr;

#ifdef USE_MCN_VCAM

#else
    // only do this if there is a graph that has been set up
    if (!pGraph || !pISampleFeeder)
        return -1;

    hr = pISampleFeeder->SendBuffer(pBuffer,*pnSize);
    if (hr==E_INVALIDARG || hr==VFW_E_NOT_CONNECTED || hr==VFW_E_WRONG_STATE)  {
        // we aren't buffering samples yet, do nothing
        return -1;
    }
	if (FAILED(hr)) {
		throw("[DSHOW] Failed at GetCurrentBuffer!");
	}
#endif
	return 0;
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
