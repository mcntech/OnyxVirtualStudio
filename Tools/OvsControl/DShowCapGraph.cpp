#include "stdafx.h"


#include "Dbg.h"
#include <initguid.h>
#include <dmodshow.h>
#include "DShowCapGraph.h"
#include "iMcnVcam.h"
#include <dmo.h>
#include "mfx_filter_externals.h"
#include "iMcnRtmpPublish.h"
#include "iMcnHlsSrvFilter.h"

CCaptureGraph::CCaptureGraph()
{
    HRESULT hr;

    hr=CoInitialize(NULL);
    if (FAILED(hr)) {
		DbgOut(DBG_LVL_ERR, ("convertWStringToCString Error in co initialize"));
	}

    // initialize all pointers
    pGraph=0;
    pControl=0;
    pEvent=0;
    mpVidCapSource=0;
	mpAudCapSource = 0;
	mpEncFilter = 0;
	mpSinkFilter = NULL;
    pSeeking=0;
}

CCaptureGraph::~CCaptureGraph()
{
    Deinit();
    CoUninitialize();
}


int CCaptureGraph::BuildCaptureFileSaveGraph(std::string outputUrl, int nDestType)
{
	HRESULT hr = S_OK;
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
	} else 	if(nDestType == DEST_TYPE_HLS_S3) {
		hr = CoCreateInstance(CLSID_HlsSrvFilter, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pSink);
		strFilePath = "http://" + outputUrl;
	} else 	if(nDestType == DEST_TYPE_RTSP_SRVR) {
		hr = CoCreateInstance(CLSID_RtspSrvFilter, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pSink);
		strFilePath = outputUrl;
	} else if(nDestType == DEST_TYPE_HLS_SRV) {
		hr = CoCreateInstance(CLSID_HlsSrvFilter, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pSink);
		strFilePath = outputUrl;
	}  else if(nDestType == DEST_TYPE_RTPMP) {
		hr = CoCreateInstance(CLSID_RtmpPublish, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pSink);
		strFilePath = outputUrl;
	}
	if(FAILED(hr)) {
		DbgOut(DBG_LVL_ERR,("BuildCaptureFileSaveGraph: Failed to create Sink filter for Type %d",nDestType)); 
		goto Exit;
	}

	pGraph->AddFilter(pSink, L"Render");

	hr = pSink->QueryInterface(IID_IFileSinkFilter, (void **) &pFS);
	if(FAILED(hr)) {
		DbgOut(DBG_LVL_ERR,("BuildCaptureFileSaveGraph: query failed for IFileSinkFilter!")); 
		goto Exit;
	}

	WCHAR* wcFilePath = convertCStringToWString(strFilePath.c_str());    
	pFS->SetFileName(wcFilePath, NULL);
	if(FAILED(hr)){
		DbgOut(DBG_LVL_ERR,("BuildCaptureFileSaveGraph: Failed to set stream destination!")); 
		goto Exit;
	}

	if(mpVidCapSource) {
		hr=pGraph->AddFilter(mpVidCapSource, L"Video Capture Source");
		if (FAILED(hr)) {
			DbgOut(DBG_LVL_ERR,("BuildCaptureFileSaveGraph: Error in adding Capture Source to the graph"));
			goto Exit;
		}
		hr = CoCreateInstance(CLSID_McnColorConvFilter, NULL,
				CLSCTX_INPROC, IID_IBaseFilter, (void **)&pDecFilter);
		if(FAILED(hr)){
			DbgOut(DBG_LVL_ERR,("BuildCaptureFileSaveGraph: Failed to create McnColorConvFilter!")); 
			goto Exit;
		}


		if (SUCCEEDED(hr)) {     // Add the filter to the graph.
			hr = pGraph->AddFilter(pDecFilter, L"Dec");
			if (FAILED(hr)) {
				DbgOut(DBG_LVL_ERR,("BuildCaptureFileSaveGraph: Error in adding Color Space converter to the graph"));
				goto Exit;
			}
		}
		hr = CoCreateInstance(CLSID_H264EncFilterIMsdk, NULL,
				CLSCTX_INPROC, IID_IBaseFilter, (void **)&pEncFilter);
		if(FAILED(hr)){
			DbgOut(DBG_LVL_ERR,("BuildCaptureFileSaveGraph: ailed to create H264EncFilterIMsdk!")); 
			goto Exit;
		}
		if (SUCCEEDED(hr)) {     // Add the filter to the graph.
			hr = pGraph->AddFilter(pEncFilter, L"Enc Filter");
			if (FAILED(hr)) {
				DbgOut(DBG_LVL_ERR,("BuildCaptureFileSaveGraph: Error in adding Enc filter to the graph"));
				goto Exit;
			}
		}
		hr = ConnectFilters(pGraph, mpVidCapSource, pDecFilter); 
		if(FAILED(hr)) {
			DbgOut(DBG_LVL_ERR,("BuildCaptureFileSaveGraph: Error in connecting VidCapSource to DecFilter!")); 
			goto Exit;
		}
		hr = ConnectFilters(pGraph, pDecFilter, pEncFilter); 
		if(FAILED(hr)) {
			DbgOut(DBG_LVL_ERR,("BuildCaptureFileSaveGraph: Error in connecting dec filter to enc filter!")); 
			goto Exit;
		}
		hr = ConnectFilters(pGraph, pEncFilter, pSink); 
		if(FAILED(hr)) {
			DbgOut(DBG_LVL_ERR,("BuildCaptureFileSaveGraph: Error in connecting enc filter to sink filter!")); 
			goto Exit;
		}
	}
	// Builds Audio capture sub graph
	if(mpAudCapSource) {
		hr = CoCreateInstance(CLSID_DMOWrapperFilter, NULL,
				CLSCTX_INPROC, IID_IBaseFilter, (void **)&pAudEncFilter);
		if(FAILED(hr)){
			DbgOut(DBG_LVL_ERR,("BuildCaptureFileSaveGraph: Can not create  DMOWrapperFilter!")); 
			goto Exit;
		}

		hr = pAudEncFilter->QueryInterface(IID_IDMOWrapperFilter, (void **)&pAudWrap);
		if(FAILED(hr)){
			DbgOut(DBG_LVL_ERR,("BuildCaptureFileSaveGraph: query failed IDMOWrapperFilter!")); 
			goto Exit;
		}
		if (SUCCEEDED(hr)) {     // Initialize the filter.
			hr = pAudWrap->Init(CLSID_McnAacEncMediaObject, DMOCATEGORY_AUDIO_ENCODER);
			if(FAILED(hr)){
				DbgOut(DBG_LVL_ERR,("BuildCaptureFileSaveGraph: Can not init  DMOCATEGORY_AUDIO_ENCODER to Warapper!")); 
				goto Exit;
			}
		}

		if (SUCCEEDED(hr)) {     // Add the filter to the graph.
			hr = pGraph->AddFilter(pAudEncFilter, L"MCN Aud Enc DMO");
			if (FAILED(hr)) {
				DbgOut(DBG_LVL_ERR,("BuildCaptureFileSaveGraph: Error in adding MCN Aud Enc DMO the graph"));
				goto Exit;
			}
		}
		if (SUCCEEDED(hr)) {     // Add the filter to the graph.
			hr = pGraph->AddFilter(mpAudCapSource, L"Audio Capture Source");
			if (FAILED(hr)) {
				DbgOut(DBG_LVL_ERR,("BuildCaptureFileSaveGraph: Error in adding MCN Aud Enc DMO the graph"));
				goto Exit;
			}
		}

		// Connect the src and enc 
		hr = ConnectFilters(pGraph, mpAudCapSource, pAudEncFilter); 
		if(FAILED(hr)) {
			DbgOut(DBG_LVL_ERR,("BuildCaptureFileSaveGraph: Unsupported media type!")); 
			goto Exit;
		}
		hr = ConnectFilters(pGraph, pAudEncFilter, pSink); 
		if(FAILED(hr)) {
			DbgOut(DBG_LVL_ERR,("BuildCaptureFileSaveGraph: Can't connect AudEncFilter to sink!")); 
			goto Exit;
		}
	}
	mpEncFilter = pEncFilter;
	mpSinkFilter = pSink;
	res = 0;
Exit:
	SAFE_RELEASE(pAudWrap)
	SAFE_RELEASE(pAudEncFilter)

	SAFE_RELEASE(pFS)
	SAFE_RELEASE(pWrap)
	//SAFE_RELEASE(pEncFilter)
	//SAFE_RELEASE(pSink)
	SAFE_RELEASE(pDecFilter)
	return res;
}

int CCaptureGraph::Init(
		const std::string outputUrl, 
		const std::string audDeviceName, 
		const std::string vidDeviceName,
		const std::string vidSharedmemName, // used for Onyx Virtual Camera
		int               nDestType,
		int               nWidth,
		int               nHeight,
		bool horizontalMirroring)
{
	HRESULT hr;
	int res = -1;
	// destroy previous movie objects (if any)
	Deinit();

	DbgOut(DBG_LVL_MSG,("Init outputUrl=%s aud=%s vid=%s nDestType=%d w=%d h=%d", outputUrl.c_str(), audDeviceName.c_str(), vidSharedmemName.c_str(), nDestType, nWidth, nHeight));
	// create filter graph and get interfaces
	hr=CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
		IID_IGraphBuilder, (void**) &pGraph);
	if (FAILED(hr)) {
		DbgOut(DBG_LVL_ERR,("Init: Error in creating graph"));
		goto Exit;
	}


	hr=pGraph->QueryInterface(IID_IMediaControl, (void**) &pControl);
	if (FAILED(hr)) {
		DbgOut(DBG_LVL_ERR,("Init Error in querying media control"));
		goto Exit;
	}

	hr=pGraph->QueryInterface(IID_IMediaEvent, (void**) &pEvent);
	if (FAILED(hr)) {
		DbgOut(DBG_LVL_ERR,("Init Error in querying media event"));
		goto Exit;
	}


	hr=pGraph->QueryInterface(IID_IMediaSeeking, (void**) &pSeeking);
	if (FAILED(hr)) {
		DbgOut(DBG_LVL_ERR,("Init Error in querying seeking interface"));
		goto Exit;
	}

	if (strcmp(vidDeviceName.c_str(),  CSTR_DESKTOP_VIDEO) == 0) {
		hr=CoCreateInstance(CLSID_McnDtCap, NULL, CLSCTX_INPROC_SERVER,
			IID_IBaseFilter, (void**)&mpVidCapSource);
	} else if (strcmp(vidDeviceName.c_str(),  CSTR_MCN_VCAP) == 0) {
		hr=CoCreateInstance(CLSID_McnVCam, NULL, CLSCTX_INPROC_SERVER,
					IID_IBaseFilter, (void**)&mpVidCapSource);
		if(hr == S_OK) {
			SetVidVerticalFlip(mpVidCapSource, true);
		}
		IMcnVCamFilter *mcnVCam = NULL;
		hr = mpVidCapSource->QueryInterface(IID_IMcnVCamFilter, (void**) &mcnVCam);
		if (FAILED(hr)) {
			DbgOut(DBG_LVL_ERR,("Init Error in queryingIMcnVCamFilter"));
			goto Exit;
		} else {
			mcnVCam->put_Source((char *)vidSharedmemName.c_str());
		}
		SAFE_RELEASE(mcnVCam);

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
	DbgOut(DBG_LVL_MSG,("init Complete %d", res));
Exit:
	return res;
}

void CCaptureGraph::Deinit()
{
    if (pGraph==0)
        return;

	if(mpEncFilter)
		SAFE_RELEASE(mpEncFilter);
	
	if(mpSinkFilter)
		SAFE_RELEASE(mpSinkFilter);

	if(mpVidCapSource)
		NukeDownstream(pGraph, mpVidCapSource);
    SAFE_RELEASE(mpVidCapSource)

	if(mpAudCapSource)
		NukeDownstream(pGraph, mpAudCapSource);

    SAFE_RELEASE(mpAudCapSource)

    SAFE_RELEASE(pSeeking)
    SAFE_RELEASE(pControl)
    SAFE_RELEASE(pEvent)
	
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
        HRESULT hr = pControl->Run();
		DbgOut(DBG_LVL_MSG,("CCaptureGraph::Start:Run result=0x%x", hr));
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
        DbgOut(DBG_LVL_ERR,(" convertCStringToWString failed with no extra error info"));
    }

    return wtext;
}

char* CCaptureGraph::convertWStringToCString(WCHAR* wString)
{
    const int MAX_STRINGZ=500;
    static char ctext[MAX_STRINGZ+2];

    if (WideCharToMultiByte(CP_ACP, 0, wString, -1, ctext, MAX_STRINGZ,0,0) == 0)  {
        DbgOut(DBG_LVL_ERR,("convertWStringToCString: convertWStringToCString failed with no extra error info"));
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
	IConfigureVideoEncoder *pConfig = NULL;

	if(mpEncFilter){
		hr = mpEncFilter->QueryInterface(IID_IConfigureVideoEncoder, (void **)&pConfig);
		if(FAILED(hr)){
			goto Exit;
		}
		 struct IConfigureVideoEncoder::Params Params;
		if(pConfig->GetParams(&Params) == S_OK) {
			Params.rc_control.bitrate = nBitrate / 1000;
			Params.ps_control.GopPicSize = nGopLen;
			pConfig->SetParams(&Params);
		}
	}
Exit:
	SAFE_RELEASE(pConfig)
	return hr;
}

HRESULT CCaptureGraph::GetVidEncStats(
		int           *pnFramesEncoded
		)
{
	HRESULT hr = S_FALSE;
	IConfigureVideoEncoder *pConfig = NULL;

	if(mpEncFilter){
		hr = mpEncFilter->QueryInterface(IID_IConfigureVideoEncoder, (void **)&pConfig);
		if(FAILED(hr)){
			goto Exit;
		}
		struct IConfigureVideoEncoder::Statistics Stats;
		 if(pConfig->GetRunTimeStatistics(&Stats) == S_OK) {
			 *pnFramesEncoded = Stats.frames_encoded;
			 hr = S_OK;
		}
	}

Exit:
	SAFE_RELEASE(pConfig)
	return hr;
}

HRESULT CCaptureGraph::SetHlsSrvFilterProperties(
	    int           nPublisherId,
		int           nfLiveOnly,
		int           nDurationMs,
		int           nDestinationType,
		const char   *szM3u8File,
		const char   *pszParentFolder,
		const char   *pszBucket,
		const char   *pszHost,
		const char   *szAccessId,
		const char   *szSecKey
		)
{
	HRESULT hr = S_FALSE;
	int  nPublishType;
	IMcnHlsSrvFilter *pConfig = NULL;
	if(nDestinationType == DEST_TYPE_HLS_S3)
		nPublishType = HLS_DEST_S3;
	else if(nDestinationType == DEST_TYPE_HLS_SRV)
		nPublishType = HLS_DEST_SRV;
	else
		nPublishType = HLS_DEST_DISC;

	if(mpSinkFilter){
		hr = mpSinkFilter->QueryInterface(IID_IMcnHlsSrvFilter, (void **)&pConfig);
		if(FAILED(hr)){
			goto Exit;
		}
		struct IMcnHlsSrvFilter::PublishParams Params;
		Params.fLiveOnly = nfLiveOnly;
		Params.nDuration = nDurationMs;
		Params.nDestType = nPublishType;
		Params.szM3u8File = szM3u8File;
		Params.pszParentFolder = pszParentFolder;
		Params.pszBucket = pszBucket;
		Params.pszHost = pszHost;
		Params.szAccessId = szAccessId;
		Params.szSecKey = szSecKey;
		pConfig->SetPublishParams(nPublisherId, &Params);
	}
Exit:
	SAFE_RELEASE(pConfig)
	return hr;
}

HRESULT CCaptureGraph::GetHlsSrvFilterStats(
		int           nPublisherId,
		int           *pnState,
		int           *pnStreamTime,
		int           *pnOverRuns
		)
{
	HRESULT hr = S_FALSE;
	IMcnHlsSrvFilter *pConfig = NULL;

	if(mpSinkFilter){
		hr = mpSinkFilter->QueryInterface(IID_IMcnHlsSrvFilter, (void **)&pConfig);
		if(FAILED(hr)){
			goto Exit;
		}
		struct IMcnHlsSrvFilter::PublishStatistics Stats;
		 if(pConfig->GetPublishRunTimeStatistics(nPublisherId, &Stats) == S_OK) {
			 *pnOverRuns = Stats.nBufferOverRuns;
			 *pnState = Stats.nState;
			 *pnStreamTime = Stats.nStreamTime;
			 hr = S_OK;
		}
	}

Exit:
	SAFE_RELEASE(pConfig)
	return hr;
}

HRESULT CCaptureGraph::GetGraphTime(
		int            *pnCrntTimeMs
		)
{
	HRESULT hr = S_FALSE;
	IConfigureVideoEncoder *pConfig = NULL;

	REFERENCE_TIME Start = 0;
	if(pSeeking) {
		pSeeking->GetCurrentPosition(&Start);
	}
	*pnCrntTimeMs = Start / 10000;
Exit:
	SAFE_RELEASE(pConfig)
	return hr;
}

HRESULT CCaptureGraph::GetRtmpStats(
		int           *pnVidFramesSent,
		int           *pnAudFramesSent,
		int           *pnStatusServe1,
		int           *pnStatusServe2
		)
{
	HRESULT hr = S_FALSE;
	IMcnRtmpPublish *pConfig = NULL;

	if(mpSinkFilter){
		hr = mpSinkFilter->QueryInterface(IID_IMcnRtmpPublish, (void **)&pConfig);
		if(FAILED(hr)){
			goto Exit;
		}
		struct IMcnRtmpPublish::Statistics Stats;
		 if(pConfig->GetStats(&Stats) == S_OK) {
			 *pnStatusServe1 = Stats.stausConnServer1 == IMcnRtmpPublish::RTMP_STATUS_OK;
			 *pnStatusServe2 = Stats.stausConnServer2 == IMcnRtmpPublish::RTMP_STATUS_OK;
			 *pnVidFramesSent = Stats.ulVidFramesSent;
			 *pnAudFramesSent = Stats.ulAudFramesSent;
			 hr = S_OK;
		}
	}

Exit:
	SAFE_RELEASE(pConfig)
	return hr;
}

HRESULT CCaptureGraph::SetRtmpParams(
			int           fEnableRecordServer1,
			int           fEnableRecordServer2
			)
{
	HRESULT hr = S_FALSE;
	IMcnRtmpPublish *pConfig = NULL;

	if(mpSinkFilter){
		hr = mpSinkFilter->QueryInterface(IID_IMcnRtmpPublish, (void **)&pConfig);
		if(FAILED(hr)){
			goto Exit;
		}
		struct IMcnRtmpPublish::Params Params;
		Params.fEableRecordServer1 = fEnableRecordServer1;
		Params.fEableRecordServer2 = fEnableRecordServer2;
		hr = pConfig->SetParams(&Params);
	}

Exit:
	SAFE_RELEASE(pConfig)
	return hr;


}

HRESULT CCaptureGraph::SetVidEncOutputSize(
		long           lWidth,
		long           lHeight
		)
{
	HRESULT hr = S_FALSE;
	if(mpEncFilter){
		hr = CDsUtil::SetVidEncOutputSize(mpEncFilter, lWidth, lHeight);
	}
	return hr;
}
