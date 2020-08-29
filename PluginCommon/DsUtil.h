#ifndef __DsUtil__
#define __DsUtil__

#include <DShow.h>
#include <dmodshow.h>
#include <string>
#include <list>
#include <vector>


DEFINE_GUID(CLSID_McnH264EncMediaObject,   0x53a8c3d6, 0x987b, 0x4edb, 0x80, 0x6c, 0x1, 0xd5, 0xe3, 0x63, 0x66, 0x22);
DEFINE_GUID(CLSID_McnAacEncMediaObject,    0x1bc60f2e, 0x480b, 0x4c07, 0xba, 0x23, 0xf8, 0xf5, 0x2c, 0x9e, 0xd7, 0x66);
DEFINE_GUID(IID_IDMOWrapperFilter,         0x52d6f586, 0x9f0f, 0x4824, 0x8f, 0xc8, 0xe3, 0x2c, 0xa0, 0x49, 0x30, 0xc2);
DEFINE_GUID(CLSID_HlsSrvFilter,            0xab8463f6, 0x0371, 0x4cc2, 0x8e, 0x30, 0x15, 0x53, 0x38, 0xc1, 0xac, 0x99);
DEFINE_GUID(CLSID_McnMp4mux,               0x9423df4, 0xd60f, 0x4c64, 0xb7, 0x6a, 0xf9, 0x32, 0xaa, 0x1c, 0x55, 0x11);
DEFINE_GUID(CLSID_McnSampleFeeder,         0x8c5f8f8c, 0xd2b6, 0x4f40, 0x8b, 0x6, 0x3b, 0x84, 0xb4, 0x19, 0x27, 0x77);
DEFINE_GUID(CLSID_McnDtCap,                0xa860203d, 0x270, 0x48a1, 0x80, 0x80, 0x96, 0x13, 0xfc, 0x6c, 0xbc, 0x33);
DEFINE_GUID(IID_IMcnSampleFeeder,          0xa380cd88, 0xbb23, 0x4207, 0xa7, 0x7e, 0xb3, 0x12, 0x5c, 0xf7, 0xd1, 0x44);
DEFINE_GUID(CLSID_RtspSrvFilter,           0x326db561, 0xab97, 0x4158, 0xa9, 0xb4, 0x89, 0x5, 0x87, 0x36, 0x50, 0x44);
DEFINE_GUID(IID_IMediaObject,              0xd8ad0f58, 0x5494, 0x4102, 0x97, 0xc5, 0xec, 0x79, 0x8e, 0x59, 0xbc, 0xf4);
DEFINE_GUID(CLSID_McnVCam,                 0xc6c26010, 0xb3e5, 0x4783, 0xb8, 0xaf, 0xac, 0x8a, 0x24, 0x3a, 0xee, 0xdd);
DEFINE_GUID(CLSID_RtmpPublish,             0x979e05b9, 0x6ac, 0x4190, 0x95, 0x9d, 0xd, 0xcd, 0xd9, 0x48, 0xd8, 0xee);

DEFINE_GUID(CLSID_H264EncFilterIMsdk,      0xe9024e3d, 0xfe91, 0x475c, 0x9e, 0xa8, 0xcc, 0x4a, 0xe7, 0xa8, 0xf8, 0xaa);
DEFINE_GUID(CLSID_McnColorConvFilter,      0xc6d45e4f, 0x4eff, 0x4a4b, 0xbd, 0xaf, 0xf0, 0x72, 0x29, 0x35, 0xe4, 0xaa);

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) {if(x) {(x)->Release(); (x)=NULL; }}
#endif

#define CAPTURE_DEVICE_CAREGORY_AUDIO	1 
#define CAPTURE_DEVICE_CAREGORY_VIDEO	2

#define DEVICE_PREFIX                   "device://"
#define DEVICE_INSTANCE_SEPARATOR       " Instance="

class CCapDevice
{
public:
	std::string mFriendlyName;
	std::string mDevicePath;
};

class CDsUtil
{
public:

void UtilDeleteMediaType(AM_MEDIA_TYPE *pmt)
{
    if (pmt == NULL) {
        return;
    }

    if (pmt->cbFormat != 0)   {
        CoTaskMemFree((PVOID)pmt->pbFormat);
        pmt->cbFormat = 0;
        pmt->pbFormat = NULL;
    }

    if (pmt->pUnk != NULL)  {
        pmt->pUnk->Release();
        pmt->pUnk = NULL;
    }

    CoTaskMemFree((PVOID)pmt);
}

HRESULT ConnectFilters(   
				IGraphBuilder *pGraph,    
				IBaseFilter *pSrc,    
				IBaseFilter *pDest)
{   
    HRESULT         hr = S_OK;
    IEnumPins *     pEnumPin = NULL;
    IPin *          pConnectedPin = NULL, * pPin;
    PIN_DIRECTION   PinDirection;
    ULONG           ulFetched;
	int             fDone = 0;
    // Enumerate all pins on the filter
    hr = pSrc->EnumPins( &pEnumPin );

    if(SUCCEEDED(hr))  {
        // Step through every pin, looking for the output pins
        while (!fDone && S_OK == pEnumPin->Next( 1L, &pPin, &ulFetched))  {
            // Is this pin connected?  We're not interested in connected pins.
            hr = pPin->ConnectedTo(&pConnectedPin);
            if (pConnectedPin) {
                pConnectedPin->Release();
                pConnectedPin = NULL;
			} else {
				if (VFW_E_NOT_CONNECTED == hr) {
					hr = pPin->QueryDirection( &PinDirection );
					if ( ( S_OK == hr ) && ( PinDirection == PINDIR_OUTPUT ) ) {
						hr = ConnectFilters(pGraph, pPin, pDest); 
						if(S_OK == hr)
							fDone = 1;
					}
				}
				pPin->Release();
			}
        }
    }

    // Release pin enumerator
    pEnumPin->Release();
	return (fDone ? S_OK : VFW_E_NOT_CONNECTED);
}   

	
HRESULT ConnectFilters(   
				IGraphBuilder *pGraph,
				IPin *pOutPin,      
				IBaseFilter *pDest)
{   

	// Find an input pin on the downstream filter.   
	IPin *pIn = 0;   
	HRESULT hr = GetUnconnectedPin(pDest, PINDIR_INPUT, &pIn);   
	if (FAILED(hr))	{   
		return hr;   
	}   

	// Try to connect them.   
	hr = pGraph->ConnectDirect(pOutPin, pIn, NULL);   
	pIn->Release();   
	return hr;   
}   


HRESULT GetUnconnectedPin(   
				IBaseFilter *pFilter,
				PIN_DIRECTION PinDir,
				IPin **ppPin)
{   
	*ppPin = 0;   
	IEnumPins *pEnum = 0;   
	IPin *pPin = 0;   
	HRESULT hr = pFilter->EnumPins(&pEnum);   
	if (FAILED(hr)) {   
		return hr;   
	}   
	while (pEnum->Next(1, &pPin, NULL) == S_OK) {   
		PIN_DIRECTION ThisPinDir;   
		pPin->QueryDirection(&ThisPinDir);   
		if (ThisPinDir == PinDir)  {   
			IPin *pTmp = 0;   
			hr = pPin->ConnectedTo(&pTmp);   
			if (SUCCEEDED(hr)){   
				pTmp->Release();   
			}   else {   
				pEnum->Release();   
				*ppPin = pPin;   
				return S_OK;   
			}   
		}   
		pPin->Release();   
	}   
	pEnum->Release();   
	return E_FAIL;   
}   


HRESULT GetDeviceList(
		int deviceType,
		std::vector<std::string>& mDeviceList)
{
    HRESULT hr;
    IBaseFilter * pSrc = NULL;
    IMoniker *pMoniker =NULL;
    ULONG cFetched;
	if(!mDeviceList.empty()){
		mDeviceList.clear();
	}

   ICreateDevEnum *pDevEnum = NULL;

    hr = CoCreateInstance (CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
                           IID_ICreateDevEnum, (void **) &pDevEnum);
    if (FAILED(hr))  {
        return hr;
    }

    IEnumMoniker *pClassEnum = NULL;
	if(deviceType == CAPTURE_DEVICE_CAREGORY_AUDIO) {
		hr = pDevEnum->CreateClassEnumerator (CLSID_AudioInputDeviceCategory, &pClassEnum, 0);
		if (FAILED(hr))  {
			return hr;
		}
	} else if (deviceType == CAPTURE_DEVICE_CAREGORY_VIDEO){
		hr = pDevEnum->CreateClassEnumerator (CLSID_VideoInputDeviceCategory, &pClassEnum, 0);
		if (FAILED(hr))  {
			return hr;
		}
	}
    if (pClassEnum == NULL) {
        return E_FAIL;
    }

    while (S_OK == (pClassEnum->Next (1, &pMoniker, &cFetched)))  {

		char szMonikerName[256] = {0};

        IPropertyBag *pBag=0;
        hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
        if(SUCCEEDED(hr)) {
            VARIANT var;
            var.vt = VT_BSTR;
            hr = pBag->Read(L"FriendlyName", &var, NULL);
            if(hr == NOERROR) {
				std::string DeviceName;
				WideCharToMultiByte(CP_ACP,0,var.bstrVal,-1,szMonikerName,MAX_PATH,0,0);
				DeviceName =  (char *)szMonikerName;
				mDeviceList.push_back(DeviceName);
                SysFreeString(var.bstrVal);
            }
            pBag->Release();
        } 
		SAFE_RELEASE(pMoniker)
    } 

Exit:
	SAFE_RELEASE(pClassEnum)
	SAFE_RELEASE(pDevEnum)
    return hr;
}

HRESULT GetVidCapDeviceList(
		std::vector<CCapDevice *>& mDeviceList
		)
{
    HRESULT hr;
    IBaseFilter * pSrc = NULL;
    IMoniker *pMoniker =NULL;
    ULONG cFetched;
	if(!mDeviceList.empty()){
		mDeviceList.clear();
	}

	ICreateDevEnum *pDevEnum = NULL;
	IEnumMoniker *pClassEnum = NULL;
    hr = CoCreateInstance (CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
                           IID_ICreateDevEnum, (void **) &pDevEnum);
    if (FAILED(hr))  {
        return hr;
    }
	
	hr = pDevEnum->CreateClassEnumerator (CLSID_VideoInputDeviceCategory, &pClassEnum, 0);
	if (FAILED(hr))  {
		return hr;
	}

    if (pClassEnum == NULL) {
        return E_FAIL;
    }

    while (S_OK == (pClassEnum->Next (1, &pMoniker, &cFetched)))  {

		char szMonikerName[256] = {0};

        IPropertyBag *pBag=0;
        hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
        if(SUCCEEDED(hr)) {
			CCapDevice *pDevice = new CCapDevice;
            VARIANT var;
            var.vt = VT_BSTR;
            hr = pBag->Read(L"FriendlyName", &var, NULL);
            if(hr == NOERROR) {
				WideCharToMultiByte(CP_ACP,0,var.bstrVal,-1,szMonikerName,MAX_PATH,0,0);
				pDevice->mFriendlyName = (char *)szMonikerName;
                SysFreeString(var.bstrVal);
            }

            hr = pBag->Read(L"DevicePath", &var, NULL);
            if(hr == NOERROR) {
				WideCharToMultiByte(CP_ACP,0,var.bstrVal,-1,szMonikerName,MAX_PATH,0,0);
				pDevice->mDevicePath = (char *)szMonikerName;
                SysFreeString(var.bstrVal);
            }
			mDeviceList.push_back(pDevice);
            pBag->Release();
        } 
		SAFE_RELEASE(pMoniker)
    } 

Exit:
	SAFE_RELEASE(pClassEnum)
	SAFE_RELEASE(pDevEnum)
    return hr;
}

HRESULT SetAudioProperties(
				IBaseFilter   *pAudFilter,
				int           nFrequency,
				int           nChannels,
				int           nFrameSizeInSamples
				)
{
	HRESULT hr=0;
	IPin *pPin=0;
	IAMBufferNegotiation *pNeg=0;
	IAMStreamConfig *pCfg=0;

	// Determine audio properties
	int nBytesPerSample = 2;

	long lBytesPerSecond = (long) (nBytesPerSample * nFrequency * nChannels);
	long lBufferSize = nFrameSizeInSamples * nBytesPerSample * nChannels;

	do {
		hr = GetUnconnectedPin(pAudFilter, PINDIR_OUTPUT, &pPin);
		if (SUCCEEDED(hr)) {
			// Get buffer negotiation interface
			hr = pPin->QueryInterface(IID_IAMBufferNegotiation, (void **)&pNeg);
				if (FAILED(hr)) {
				pPin->Release();
				break;
			}

			// Set the buffer size based on selected settings
			ALLOCATOR_PROPERTIES prop={0};
			prop.cbBuffer = lBufferSize;
			prop.cBuffers = 6;
			prop.cbAlign = nBytesPerSample * nChannels;
			hr = pNeg->SuggestAllocatorProperties(&prop);
			pNeg->Release();

			// Now set the actual format of the audio data
			hr = pPin->QueryInterface(IID_IAMStreamConfig, (void **)&pCfg);
			if (FAILED(hr)) {
				pPin->Release();
				break;
			}            

			// Read current media type/format
			AM_MEDIA_TYPE *pmt={0};
			hr = pCfg->GetFormat(&pmt);

			if (SUCCEEDED(hr)) {
				// Fill in values for the new format
				WAVEFORMATEX *pWF = (WAVEFORMATEX *) pmt->pbFormat;
				pWF->nChannels = (WORD) nChannels;
				pWF->nSamplesPerSec = nFrequency;
				pWF->nAvgBytesPerSec = lBytesPerSecond;
				pWF->wBitsPerSample = (WORD) (nBytesPerSample * 8);
				pWF->nBlockAlign = (WORD) (nBytesPerSample * nChannels);

				// Set the new formattype for the output pin
				hr = pCfg->SetFormat(pmt);
				UtilDeleteMediaType(pmt);
			}
            // Release interfaces
            pCfg->Release();
            pPin->Release();
		}
	} while(0);
	return hr;
}

HRESULT GetCaptureDevice(
		REFCLSID clsidDeviceClass,				// CLSID_VideoInputDeviceCategory
		std::string DeviceName, 
		IBaseFilter ** ppSrcFilter)
{
    HRESULT hr;
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

    IEnumMoniker *pClassEnum = NULL;

    hr = pDevEnum->CreateClassEnumerator (clsidDeviceClass, &pClassEnum, 0);
    if (FAILED(hr))  {
        return hr;
    }

    if (pClassEnum == NULL) {
        return E_FAIL;
    }

    while (S_OK == (pClassEnum->Next (1, &pMoniker, &cFetched)))  {

		char szMonikerName[256] = {0};

        IPropertyBag *pBag=0;
        hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
        if(SUCCEEDED(hr)) {
            VARIANT var;
            var.vt = VT_BSTR;
            hr = pBag->Read(L"FriendlyName", &var, NULL);
            if(hr == NOERROR) {
				WideCharToMultiByte(CP_ACP,0,var.bstrVal,-1,szMonikerName,MAX_PATH,0,0);
				if(stricmp(DeviceName.c_str(), szMonikerName) == 0)
					fFound = 1;
                SysFreeString(var.bstrVal);
            }
            pBag->Release();
        } 


		if(fFound) {
			hr = pMoniker->BindToObject(0,0,IID_IBaseFilter, (void**)&pSrc);
			if (FAILED(hr)) {
				goto Exit;
			}
			break;
		}
		SAFE_RELEASE(pMoniker)
    } 
    *ppSrcFilter = pSrc;

Exit:

	SAFE_RELEASE(pClassEnum)
	SAFE_RELEASE(pDevEnum)
    return hr;
}

void NukeDownstream(
	IGraphBuilder *pGraph,
	IBaseFilter *pf)
{
    IPin *pP=0, *pTo=0;
    ULONG u;
    IEnumPins *pins = NULL;
    PIN_INFO pininfo;

    if (!pf)
        return;

    HRESULT hr = pf->EnumPins(&pins);
    pins->Reset();

    while(hr == NOERROR) {
        hr = pins->Next(1, &pP, &u);
        if(hr == S_OK && pP) {
            pP->ConnectedTo(&pTo);
            if(pTo) {
                hr = pTo->QueryPinInfo(&pininfo);
                if(hr == NOERROR) {
                    if(pininfo.dir == PINDIR_INPUT) {
                        NukeDownstream(pGraph, pininfo.pFilter);
                        pGraph->Disconnect(pTo);
                        pGraph->Disconnect(pP);
                        pGraph->RemoveFilter(pininfo.pFilter);
                    }
                    pininfo.pFilter->Release();
                }
                pTo->Release();
            }
            pP->Release();
        }
    }
    if(pins)
        pins->Release();
}

int CheckFormat(
		VIDEO_STREAM_CONFIG_CAPS *pScc,
		long lWidth,
		long lHeight
		)
{
	if(lWidth >= pScc->MinOutputSize.cx && lWidth <= pScc->MaxOutputSize.cx &&
		lHeight >= pScc->MinOutputSize.cy && lHeight <= pScc->MaxOutputSize.cy){
			if((pScc->OutputGranularityX > 0 && ((lWidth - pScc->MinOutputSize.cx) % pScc->OutputGranularityX == 0)) &&
				(pScc->OutputGranularityY > 0 && ((lHeight - pScc->MinOutputSize.cy) % pScc->OutputGranularityY == 0))){
				return true;
		}
	}
}
int ChooseVidFormat(
		IGraphBuilder *pGraph,
		IBaseFilter   *pCapFilter,
		GUID          subType,
		long          lWidth,
		long          lHeight
					)
{
	HRESULT hr;
	int ret = -1;
	IAMStreamConfig *pCfg = NULL;
	IPin *pPin=0;
	GetUnconnectedPin(pCapFilter, PINDIR_OUTPUT, &pPin);
	if(pPin){
		hr = pPin->QueryInterface(
			IID_IAMStreamConfig, (void**)&pCfg);
		if(hr == S_OK){
			int iCount = 0, iSize = 0;
			hr = pCfg->GetNumberOfCapabilities(&iCount, &iSize);
			bool fDone = false;
			// Check the size to make sure we pass in the correct structure.
			if (iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS)) {
				// Use the video capabilities structure.
				for (int iFormat = 0; iFormat < iCount && !fDone; iFormat++)   {
					VIDEO_STREAM_CONFIG_CAPS scc;
					AM_MEDIA_TYPE *pmtConfig;
					hr = pCfg->GetStreamCaps(iFormat, &pmtConfig, (BYTE*)&scc);
					if (SUCCEEDED(hr))	{
						/* Examine the format, and possibly use it. */
						if ((pmtConfig->majortype == MEDIATYPE_Video) &&
							(pmtConfig->subtype == subType) &&
							(pmtConfig->formattype == FORMAT_VideoInfo) &&
							(pmtConfig->cbFormat >= sizeof (VIDEOINFOHEADER)) &&
							(pmtConfig->pbFormat != NULL)) {
							VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*)pmtConfig->pbFormat;
							// pVih contains the detailed format information.
							if(((pVih->bmiHeader.biWidth == lWidth) && (pVih->bmiHeader.biHeight == lHeight)) ||
								CheckFormat(&scc, lWidth, lHeight)){
								pVih->bmiHeader.biWidth = lWidth;
								pVih->bmiHeader.biHeight = lHeight;
								pVih->bmiHeader.biSizeImage = DIBSIZE(pVih->bmiHeader);
								hr = pCfg->SetFormat(pmtConfig);
								if(hr == S_OK)
									fDone = true;
									ret = 0;
							}
						}
					}
					// Delete the media type when you are done.
					if (pmtConfig->cbFormat != 0)CoTaskMemFree((PVOID)pmtConfig->pbFormat);
					if (pmtConfig->pUnk != NULL)   pmtConfig->pUnk->Release();
					CoTaskMemFree((PVOID)pmtConfig);
				}
			}
		}
	}
	SAFE_RELEASE(pCfg)
	SAFE_RELEASE(pPin)
	return ret;
}

bool IsVidFormatSupported(
		IGraphBuilder *pGraph,
		IBaseFilter   *pCapFilter,
		GUID          subType,
		long          lWidth,
		long          lHeight
		)
{
	HRESULT hr;
	bool ret = false;
	IAMStreamConfig *pCfg = NULL;
	IPin *pPin=0;
	GetUnconnectedPin(pCapFilter, PINDIR_OUTPUT, &pPin);
	if(pPin){
		hr = pPin->QueryInterface(
			IID_IAMStreamConfig, (void**)&pCfg);
		if(hr == S_OK){
			int iCount = 0, iSize = 0;
			hr = pCfg->GetNumberOfCapabilities(&iCount, &iSize);
			bool fDone = false;
			// Check the size to make sure we pass in the correct structure.
			if (iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS)) {
				// Use the video capabilities structure.
				for (int iFormat = 0; iFormat < iCount && !fDone; iFormat++)   {
					VIDEO_STREAM_CONFIG_CAPS scc;
					AM_MEDIA_TYPE *pmtConfig;
					hr = pCfg->GetStreamCaps(iFormat, &pmtConfig, (BYTE*)&scc);
					if (SUCCEEDED(hr))	{
						/* Examine the format, and possibly use it. */
						if ((pmtConfig->majortype == MEDIATYPE_Video) &&
							(pmtConfig->subtype == subType) &&
							(pmtConfig->formattype == FORMAT_VideoInfo) &&
							(pmtConfig->cbFormat >= sizeof (VIDEOINFOHEADER)) &&
							(pmtConfig->pbFormat != NULL)) {
							VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*)pmtConfig->pbFormat;
							// pVih contains the detailed format information.
							if(((pVih->bmiHeader.biWidth == lWidth) && (pVih->bmiHeader.biHeight == lHeight)) ||
								CheckFormat(&scc, lWidth, lHeight)){
									fDone = true;
									ret = true;
							}
						}
					}
					// Delete the media type when you are done.
					if (pmtConfig->cbFormat != 0)CoTaskMemFree((PVOID)pmtConfig->pbFormat);
					if (pmtConfig->pUnk != NULL)   pmtConfig->pUnk->Release();
					CoTaskMemFree((PVOID)pmtConfig);
				}
			}
		}
	}
	SAFE_RELEASE(pCfg)
	SAFE_RELEASE(pPin)
	return ret;
}

#define MAX_PROP_NAME 32
HRESULT SetVidEncProperties(
		IBaseFilter   *pEncFilter,
		int           nBitrate,
		int           nGopLen,
		int           nFrameStruct
		)
{
	HRESULT hr=0;
    IDMOWrapperFilter* pWrap = NULL; 
	IMediaObject *pDmo = NULL;
	VARIANT var;
	IPropertyBag* pProp = NULL;

	hr = pEncFilter->QueryInterface(IID_IDMOWrapperFilter, (void **)&pWrap);
	if(FAILED(hr)){
		goto Exit;
	}
	hr = pWrap->QueryInterface( IID_IMediaObject, (void**)&pDmo );
	if(FAILED(hr)){
		goto Exit;
	}
	hr = pDmo->QueryInterface( IID_IPropertyBag, (void**)&pProp );

	if(FAILED(hr)){
		goto Exit;
	}
	OLECHAR propBitrate[MAX_PROP_NAME] = L"Bitrate";
	VariantInit( &var );
	var.vt = VT_I4;
	var.lVal = (long)nBitrate; 
	hr = pProp->Write( propBitrate, &var );

Exit:
	SAFE_RELEASE(pWrap)
	SAFE_RELEASE(pDmo)
	return hr;
}
HRESULT SetVidEncOutputSize(
		IBaseFilter   *pEncFilter,
		long           lWidth,
		long           lHeight
		)
{
	HRESULT hr=0;
    IDMOWrapperFilter* pWrap = NULL; 
	IMediaObject *pDmo = NULL;
	VARIANT var;
	IPropertyBag* pProp = NULL;

	hr = pEncFilter->QueryInterface(IID_IDMOWrapperFilter, (void **)&pWrap);
	if(FAILED(hr)){
		goto Exit;
	}
	hr = pWrap->QueryInterface( IID_IMediaObject, (void**)&pDmo );
	if(FAILED(hr)){
		goto Exit;
	}
	hr = pDmo->QueryInterface( IID_IPropertyBag, (void**)&pProp );

	if(FAILED(hr)){
		goto Exit;
	}
	OLECHAR propBitrate[MAX_PROP_NAME] = L"OutputRect";
	VariantInit( &var );
	var.vt = VT_I8;
	var.llVal = (long long)lWidth << 32 | (long long)lHeight;
	hr = pProp->Write( propBitrate, &var );

Exit:
	SAFE_RELEASE(pWrap)
	SAFE_RELEASE(pDmo)
	return hr;
}

HRESULT FindPin(   
		IBaseFilter   *pFilter,
		PIN_DIRECTION PinDir,
		IPin          **ppPin)
{   
	*ppPin = 0;   
	IEnumPins *pEnum = 0;   
	IPin *pPin = 0;   
	HRESULT hr = pFilter->EnumPins(&pEnum);   
	if (FAILED(hr)) {   
		return hr;   
	}   
	while (pEnum->Next(1, &pPin, NULL) == S_OK) {   
		PIN_DIRECTION ThisPinDir;   
		pPin->QueryDirection(&ThisPinDir);   
		if (ThisPinDir == PinDir)  {   
			pEnum->Release();   
			*ppPin = pPin;   
			return S_OK;   
		}   
		pPin->Release();   
	}   
	pEnum->Release();   
	return E_FAIL;   
}   


HRESULT SetVidVerticalFlip(IBaseFilter *pVidSrc, bool fEnable)
{
	IAMVideoControl *pAMVidControl = NULL;
	HRESULT hr = pVidSrc->QueryInterface(IID_IAMVideoControl, (void**)&pAMVidControl);

	if (SUCCEEDED(hr)){
		// Find the still pin.
		IPin *pPin = NULL;

		hr = FindPin(pVidSrc, PINDIR_OUTPUT, &pPin);

		if (SUCCEEDED(hr))   {
			long mode = 0;
			hr = pAMVidControl->GetMode(pPin, &mode);
			mode |= VideoControlFlag_FlipVertical;
			hr = pAMVidControl->SetMode(pPin, mode);
			pPin->Release();
		}
		pAMVidControl->Release();
	}
	return hr;
}

};

#endif // __DsUtil__