#include "stdafx.h"


#include "Dbg.h"
#include <initguid.h>
#include <dmodshow.h>
#include "DShowGraph.h"
#include <dmo.h>

CCaptureGraph* CCaptureGraph::mInstance = NULL;

CCaptureGraph::CCaptureGraph()
{
    HRESULT hr;

    hr=CoInitialize(NULL);
    if (FAILED(hr)) throw("[DSHOW] Error in co initialize");

    // initialize all pointers
    pGraph=0;
    pControl=0;
    pEvent=0;
    mpVidCapSource=0;
	mpAudCapSource = 0;
	mpEncFilter = 0;
    pSeeking=0;
}

CCaptureGraph::~CCaptureGraph()
{
    Deinit();
    CoUninitialize();
}

CCaptureGraph *CCaptureGraph::Instance()
{
  if (!mInstance)
      mInstance = new CCaptureGraph();
  return mInstance;
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

int CCaptureGraph::BuildCaptureFileSaveGraph(std::string outputUrl, int nDestType)
{
	HRESULT hr;
    IFileSinkFilter *pFS=NULL;
    IDMOWrapperFilter* pWrap = NULL; 
	IBaseFilter *pEncFilter = NULL;
	IBaseFilter * pSink = NULL;
	IBaseFilter * pDecFilter = NULL;
	IBaseFilter *pAudEncFilter = NULL;
	IDMOWrapperFilter* pAudWrap = NULL; 
	
	std::string strFilePath;

	int res = -1;


	if(nDestType == DEST_TYPE_MP4) {
		hr = CoCreateInstance(CLSID_McnMp4mux, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pSink);
		strFilePath = outputUrl;
	} else 	if(nDestType == DEST_TYPE_HLS_HD) {
		hr = CoCreateInstance(CLSID_HlsSrvFilter, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pSink);
		strFilePath = "file:///" + outputUrl;
	} else 	if(nDestType == DEST_TYPE_RTSP) {
		hr = CoCreateInstance(CLSID_RtspSrvFilter, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pSink);
		strFilePath = outputUrl;
	} else if(nDestType == DEST_TYPE_HLS_NET) {
		hr = CoCreateInstance(CLSID_HlsSrvFilter, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pSink);
		strFilePath = outputUrl;
	}
	if(FAILED(hr)) {
		DbgOut(DBG_LVL_ERR,("[DSHOW] Unsupported media type!")); 
		goto Exit;
	}

	pGraph->AddFilter(pSink, L"Render");

	hr = pSink->QueryInterface(IID_IFileSinkFilter, (void **) &pFS);
	if(FAILED(hr)) {
		DbgOut(DBG_LVL_ERR,("[DSHOW] Unsupported media type!")); 
		goto Exit;
	}

	WCHAR* wcFilePath = convertCStringToWString(strFilePath.c_str());    
	pFS->SetFileName(wcFilePath, NULL);
	if(FAILED(hr)){
		DbgOut(DBG_LVL_ERR,("[DSHOW] Failed to set stream destination!")); 
		goto Exit;
	}

	if(mpVidCapSource) {
		hr=pGraph->AddFilter(mpVidCapSource, L"Video Capture Source");
		if (FAILED(hr)) {
			throw("[DSHOW] Error in adding Capture Source to the graph");
			goto Exit;
		}
		if(mVidSrcFormat == VID_SRC_FORMAT_MJPEG){
			hr = CoCreateInstance(CLSID_MjpegDec, NULL,
					CLSCTX_INPROC, IID_IBaseFilter, (void **)&pDecFilter);
			if(FAILED(hr)) {
				logMessage("[DSHOW] Unsupported media type!"); 
				goto Exit;
			}
			if (SUCCEEDED(hr)) {     // Add the filter to the graph.
				hr = pGraph->AddFilter(pDecFilter, L"MJPEG Dec");
				if (FAILED(hr)) {
					DbgOut(DBG_LVL_ERR,("[DSHOW] Error in adding WMV Decoder to the graph"));
					goto Exit;
				}
			}

		}
		hr = CoCreateInstance(CLSID_DMOWrapperFilter, NULL,
				CLSCTX_INPROC, IID_IBaseFilter, (void **)&pEncFilter);
		if(FAILED(hr)){
			DbgOut(DBG_LVL_ERR,("[DSHOW] Unsupported media type!")); 
			goto Exit;
		}
		hr = pEncFilter->QueryInterface(IID_IDMOWrapperFilter, (void **)&pWrap);
		if(FAILED(hr)){
			DbgOut(DBG_LVL_ERR,("[DSHOW] Unsupported media type!")); 
			goto Exit;
		}
		if (SUCCEEDED(hr)) {     // Initialize the filter.
			hr = pWrap->Init(CLSID_McnH264EncMediaObject, DMOCATEGORY_VIDEO_ENCODER);
			if(FAILED(hr)){
				DbgOut(DBG_LVL_ERR,("[DSHOW] Unsupported media type!")); 
				goto Exit;
			}
		}

		if (SUCCEEDED(hr)) {     // Add the filter to the graph.
			hr = pGraph->AddFilter(pEncFilter, L"My DMO");
			if (FAILED(hr)) {
				DbgOut(DBG_LVL_ERR,("[DSHOW] Error in adding WMV Decoder to the graph"));
				goto Exit;
			}
		}
		if(mVidSrcFormat == VID_SRC_FORMAT_MJPEG){
			hr = ConnectFilters(pGraph, mpVidCapSource, pDecFilter); 
			if(FAILED(hr)) {
				DbgOut(DBG_LVL_ERR,("[DSHOW] Unsupported media type!")); 
				goto Exit;
			}
			hr = ConnectFilters(pGraph, pDecFilter, pEncFilter); 
			if(FAILED(hr)) {
				DbgOut(DBG_LVL_ERR,("[DSHOW] Unsupported media type!")); 
				goto Exit;
			}
		} else {
			hr = ConnectFilters(pGraph, mpVidCapSource, pEncFilter); 
			if(FAILED(hr)) {
				DbgOut(DBG_LVL_ERR,("[DSHOW] Unsupported media type!")); 
				goto Exit;
			}
		}
		hr = ConnectFilters(pGraph, pEncFilter, pSink); 
		if(FAILED(hr)) {
			DbgOut(DBG_LVL_ERR,("[DSHOW] Can't create render!")); 
			goto Exit;
		}
	}
	// Builds Audio capture sub graph
	if(mpAudCapSource) {
		hr = CoCreateInstance(CLSID_DMOWrapperFilter, NULL,
				CLSCTX_INPROC, IID_IBaseFilter, (void **)&pAudEncFilter);
		if(FAILED(hr)){
			DbgOut(DBG_LVL_ERR,("[DSHOW] Unsupported media type!")); 
			goto Exit;
		}

		hr = pAudEncFilter->QueryInterface(IID_IDMOWrapperFilter, (void **)&pAudWrap);
		if(FAILED(hr)){
			DbgOut(DBG_LVL_ERR,("[DSHOW] Unsupported media type!")); 
			goto Exit;
		}
		if (SUCCEEDED(hr)) {     // Initialize the filter.
			hr = pAudWrap->Init(CLSID_McnAacEncMediaObject, DMOCATEGORY_AUDIO_ENCODER);
			if(FAILED(hr)){
				DbgOut(DBG_LVL_ERR,("[DSHOW] Unsupported media type!")); 
				goto Exit;
			}
		}

		if (SUCCEEDED(hr)) {     // Add the filter to the graph.
			hr = pGraph->AddFilter(pAudEncFilter, L"MCN Aud Enc DMO");
			if (FAILED(hr)) {
				DbgOut(DBG_LVL_ERR,("[DSHOW] Error in adding MCN Aud Enc DMO the graph"));
				goto Exit;
			}
		}
		if (SUCCEEDED(hr)) {     // Add the filter to the graph.
			hr = pGraph->AddFilter(mpAudCapSource, L"Audio Capture Source");
			if (FAILED(hr)) {
				DbgOut(DBG_LVL_ERR,("[DSHOW] Error in adding MCN Aud Enc DMO the graph"));
				goto Exit;
			}
		}

		// Connect the src and enc 
		hr = ConnectFilters(pGraph, mpAudCapSource, pAudEncFilter); 
		if(FAILED(hr)) {
			DbgOut(DBG_LVL_ERR,("[DSHOW] Unsupported media type!")); 
			goto Exit;
		}
		hr = ConnectFilters(pGraph, pAudEncFilter, pSink); 
		if(FAILED(hr)) {
			DbgOut(DBG_LVL_ERR,("[DSHOW] Can't create render!")); 
			goto Exit;
		}
	}
	mpEncFilter = pEncFilter;
	res = 0;
Exit:
	SAFE_RELEASE(pAudWrap)
	SAFE_RELEASE(pAudEncFilter)

	SAFE_RELEASE(pFS)
	SAFE_RELEASE(pWrap)
	//SAFE_RELEASE(pEncFilter)
	SAFE_RELEASE(pSink)
	SAFE_RELEASE(pDecFilter)

	return res;
}

int CCaptureGraph::Init(
		const std::string outputUrl, 
		const std::string audDeviceName, 
		const std::string vidDeviceName, 
		int               nDestType,
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
	
	if (strcmp(vidDeviceName.c_str(),  CSTR_DESKTOP_VIDEO) == 0) {
		hr=CoCreateInstance(CLSID_McnDtCap, NULL, CLSCTX_INPROC_SERVER,
			IID_IBaseFilter, (void**)&mpVidCapSource);
		mVidSrcFormat = VID_SRC_FORMAT_RGB32;
	} else if (strcmp(vidDeviceName.c_str(),  CSTR_MCN_VCAP) == 0) {
		hr=CoCreateInstance(CLSID_McnVCam, NULL, CLSCTX_INPROC_SERVER,
					IID_IBaseFilter, (void**)&mpVidCapSource);
		if(hr == S_OK)
			SetVidVerticalFlip(mpVidCapSource, true);
		mVidSrcFormat = VID_SRC_FORMAT_RGB32;
	} else {
		// Assume MJPEG Camera
		GetCaptureDevice(CLSID_VideoInputDeviceCategory, vidDeviceName, &mpVidCapSource);
		mVidSrcFormat = VID_SRC_FORMAT_MJPEG;
		if(mpVidCapSource) {
			ChooseVidFormat(pGraph,  mpVidCapSource, MEDIASUBTYPE_MJPG, 640,480);
		}
	}
	if(!audDeviceName.empty()){
		GetCaptureDevice(CLSID_AudioInputDeviceCategory, audDeviceName, &mpAudCapSource);
		if(mpAudCapSource) {
			SetAudioProperties(mpAudCapSource, 44100, 2, 1024);
		}
	}
	videoUpsidedown = false;

	if(mpAudCapSource || mpVidCapSource){
		if(BuildCaptureFileSaveGraph(outputUrl,  nDestType) == 0) 
			res = 0;
	}
	
Exit:
	return res;
}


void CCaptureGraph::Deinit()
{
    if (pGraph==0)
        return;

	if(mpEncFilter)
		SAFE_RELEASE(mpEncFilter);

	if(mpVidCapSource)
		NukeDownstream(pGraph, mpVidCapSource);
    SAFE_RELEASE(mpVidCapSource)

	if(mpAudCapSource)
		NukeDownstream(pGraph, mpAudCapSource);

    SAFE_RELEASE(mpAudCapSource)

    SAFE_RELEASE(pSeeking)
    SAFE_RELEASE(pControl)
    SAFE_RELEASE(pEvent)
	
	RemoveFromRot(mdwRegister);

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

WCHAR* CCaptureGraph::convertCStringToWString(const char* string)
{
    const int MAX_STRINGZ=500;
    static WCHAR wtext[MAX_STRINGZ+2];

    if (strlen(string)>MAX_STRINGZ)
        return 0;

    // convert text to wchar
    if (MultiByteToWideChar(CP_ACP, 0, string, -1, wtext, MAX_STRINGZ) == 0)  {
        throw("[DSHOW] convertCStringToWString failed with no extra error info");
    }

    return wtext;
}

char* CCaptureGraph::convertWStringToCString(WCHAR* wString)
{
    const int MAX_STRINGZ=500;
    static char ctext[MAX_STRINGZ+2];

    if (WideCharToMultiByte(CP_ACP, 0, wString, -1, ctext, MAX_STRINGZ,0,0) == 0)  {
        throw("[DSHOW] convertWStringToCString failed with no extra error info");
    }

    return ctext;
}

HRESULT CCaptureGraph::SetVidEncProperties(
		int           nBitrate,
		int           nGopLen,
		int           nFrameStruct
		)
{
	HRESULT hr = S_FALSE;
	if(mpEncFilter){
		hr = CDsUtil::SetVidEncProperties(mpEncFilter, nBitrate, nGopLen, nFrameStruct);
	}
	return hr;
}
