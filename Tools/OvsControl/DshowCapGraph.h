#ifndef __DShowGraph__
#define __DShowGraph__

#include "stdafx.h"
#include <DShow.h>
#include <string>
#include <list>
#include <vector>
#include <initguid.h>
#include "DsUtil.h"

typedef enum _DEST_TYPE_T
{
	DEST_TYPE_RTPMP,  
	DEST_TYPE_MP4,		
	DEST_TYPE_HLS_HD,
	DEST_TYPE_HLS_SRV,	
	DEST_TYPE_HLS_S3,	
	DEST_TYPE_RTSP_SRVR,		
	DEST_TYPE_RTP_REC,
	DEST_TYPE_UDP_REC,
} DEST_TYPE_T;

#define CSTR_NO_AUDIO       "Do not capture audio"
#define CSTR_NOVIDEO        "Do not capture video"
#define CSTR_DESKTOP_VIDEO  "Desktop"
#define CSTR_MCN_VCAP		"Onyx Virtual Camera"

#define VID_SRC_FORMAT_RGB32	1
#define VID_SRC_FORMAT_MJPEG	2
#define VID_SRC_FORMAT_YUVX     3		// YUV Variant

class CCaptureGraph : public CDsUtil
{
public:
    CCaptureGraph();
    virtual ~CCaptureGraph();
	static CCaptureGraph* Instance(); 

	int Init(
		const std::string moviePath, 
		const std::string audiodevice,
		const std::string videodevice, 
		const std::string vidSharedmemName, // used for Onyx Virtual Camera
		int               nDestType,
		int               nWidth,
		int               nHeight,
		bool horizontalMirroring=false);

    void Deinit();

    void pause();
    void start();
    void stop();

	void logMessage(std::string msg){};
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
	WCHAR* convertCStringToWString(const char* string);
	char* convertWStringToCString(WCHAR* string);

	HRESULT getCaptureDevice(
			REFCLSID clsidDeviceClass,	
			std::string DeviceName, 
			IBaseFilter ** ppSrcFilter);


	HRESULT SetVidEncProperties(
			int           nBitrate,
			int           nGopLen,
			int           nFrameStruct
			);

	HRESULT GetVidEncStats(
			int           *pnFramesEncoded
			);

	HRESULT SetHlsSrvFilterProperties(
		int           nPublisherId,
		int           nfLiveOnly,
		int           nDurationMs,
		int           nDestType,
		const char   *szM3u8File,
		const char   *pszParentFolder,
		const char   *pszBucket,
		const char   *pszHost,
		const char   *szAccessId,
		const char   *szSecKey
		);

	HRESULT GetHlsSrvFilterStats(
		int           nPublisherId,
		int           *pnState,
		int           *pnStreamTime,
		int           *pnOverRuns
		);

	HRESULT GetGraphTime(
		int            *pnCrntTimeMs
		);

	HRESULT GetRtmpStats(
			int           *pnVidFramesSent,
			int           *pnAudFramesSent,
			int           *pnStatusServe1,
			int           *pnStatusServe2
			);

	HRESULT SetRtmpParams(
			int           fEnableRecordServer1,
			int           fEnableRecordServer2
			);

	HRESULT SetVidEncOutputSize(
		long           lWidth,
		long           lHeight
		);

protected:
	DWORD mdwRegister;

    IGraphBuilder *pGraph;
    IMediaControl *pControl;
    IMediaEvent   *pEvent;
    IBaseFilter   *mpVidCapSource;
    IBaseFilter   *mpAudCapSource;
    IBaseFilter   *mpEncFilter;
	IBaseFilter   *mpSinkFilter;
    IMediaSeeking  *pSeeking;
    int videoWidth;
    int videoHeight;
	bool videoUpsidedown;
	
private:
	int BuildCaptureFileSaveGraph(std::string moviePath,  int nDestType);
};
 
#endif // __DShowGraph__