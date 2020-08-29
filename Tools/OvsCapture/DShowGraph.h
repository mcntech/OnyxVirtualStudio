#ifndef __DShowGraph__
#define __DShowGraph__

#include "stdafx.h"
#include <DShow.h>
#include <string>
#include <list>
#include <vector>
#include <initguid.h>
#include "DsUtil.h"

#define DEST_TYPE_MP4		0
#define DEST_TYPE_HLS_HD	1
#define DEST_TYPE_HLS_NET	2
#define DEST_TYPE_RTSP		3

#define CSTR_NO_AUDIO       "Do not capture audio"
#define CSTR_NOVIDEO        "Do not capture video"
#define CSTR_DESKTOP_VIDEO  "Desktop"
#define CSTR_MCN_VCAP		"Onyx Virtual Camera"

#define VID_SRC_FORMAT_RGB32	1
#define VID_SRC_FORMAT_MJPEG	2

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
		int               nDestType,
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

protected:
	DWORD mdwRegister;

    IGraphBuilder *pGraph;
    IMediaControl *pControl;
    IMediaEvent   *pEvent;
    IBaseFilter   *mpVidCapSource;
    IBaseFilter   *mpAudCapSource;
    IBaseFilter   *mpEncFilter;

    IMediaSeeking  *pSeeking;
	int            mVidSrcFormat;
    int videoWidth;
    int videoHeight;
	bool videoUpsidedown;
	
private:
	int BuildCaptureFileSaveGraph(std::string moviePath,  int nDestType);
	static CCaptureGraph* mInstance; 
};
 
#endif // __DShowGraph__