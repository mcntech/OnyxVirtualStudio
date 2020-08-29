#include "stdafx.h"
#include <commctrl.h>
#include <commdlg.h>
#include <Shlobj.h>
#include "McnScreenCap.h"
#include "DShowGraph.h"
#include <winsock2.h>
#include <ws2tcpip.h>

#define MAX_LOADSTRING 100
#define	WM_USER_SHELLICON WM_USER + 1

typedef enum _CAPTURE_STATE_T
{
	CAPTURE_UNINIT,
	CAPTURE_STOP,
	CAPTURE_PAUSE,
	CAPTURE_RUN
} CAPTURE_STATE_T;
// Global Variables:
HINSTANCE hInst;	// current instance
NOTIFYICONDATA nidApp;
HMENU hPopMenu;
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
TCHAR szApplicationToolTip[MAX_LOADSTRING];	    // the main window class name
//BOOL bRunning = FALSE;							// keep application state
CAPTURE_STATE_T    capState = CAPTURE_STOP;

HWND ghWnd = NULL;

void GetConfigFoderPath(std::string &cfgFolderPath)
{
	PWSTR romingFolder = NULL; 
	SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &romingFolder);
	std::string path = (CCaptureGraph::Instance())->convertWStringToCString(romingFolder);
	CoTaskMemFree(romingFolder);
	cfgFolderPath = path + "\\MCN\\OnyxVirtualStudio";
}

void GetEth0LocaIpAddr(std::string &strIpAddr)
{
	char hostname[256];
	struct sockaddr_in	addr;

	addrinfo* results;
	addrinfo hint;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family    = PF_INET; //is_v4 ? PF_INET : PF_INET6;
	hint.ai_socktype  = SOCK_STREAM; //is_dgram ? SOCK_DGRAM : SOCK_STREAM;

	if(gethostname (hostname, sizeof (hostname)) != 0){
		MessageBox(ghWnd, TEXT("Could not init network interface(1)!"), TEXT("Configure"), IDOK);
		return;
	}

	int res = getaddrinfo(hostname,  0, &hint, &results);
	if(res == 0) {
		memcpy(&addr, results->ai_addr, sizeof(struct sockaddr));
		freeaddrinfo(results);
		strIpAddr = inet_ntoa(addr.sin_addr);
	} else {
		MessageBox(ghWnd, TEXT("Could not init network interface(2)!"), TEXT("Configure"), IDOK);
	}
}

class CConfigure
{
public:
	CConfigure()
	{
	   char szPath[128] = "";
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);

		std::string configFolder;
		GetConfigFoderPath(configFolder);
		std::string configMsc = configFolder + "\\msc.cfg";
		char szValue[MAX_PATH];

		GetPrivateProfileStringA("UserPreference", "AudioInput",CSTR_NO_AUDIO,szValue,MAX_PATH - 1,configMsc.c_str());
		mCrntAudioDevice = szValue;

		GetPrivateProfileStringA("UserPreference", "VideoInput",CSTR_DESKTOP_VIDEO,szValue,MAX_PATH - 1,configMsc.c_str());
		mCrntVideoDevice = szValue;

		mDestType = GetPrivateProfileIntA("UserPreference", "DestinationType",DEST_TYPE_MP4,configMsc.c_str());
		
		// TODO: Configure

		GetPrivateProfileStringA("UserPreference", "HlsPort","59200",szValue,MAX_PATH - 1,configMsc.c_str());
		mHlsIpPort = szValue;

		GetPrivateProfileStringA("UserPreference", "HlsResourceName","stream.m3u8",szValue,MAX_PATH - 1,configMsc.c_str());
		mHlsResource = szValue;

		GetPrivateProfileStringA("UserPreference", "RtspPort","59400",szValue,MAX_PATH - 1,configMsc.c_str());
		mRtspIpPort = szValue;

		GetPrivateProfileStringA("UserPreference", "RtspResourceName","stream",szValue,MAX_PATH - 1,configMsc.c_str());
		mRtspResource = szValue;


		std::string strIpAddr;
		GetEth0LocaIpAddr(strIpAddr);
		mRtspIpAddr = strIpAddr;
		mHlsIpAddr = strIpAddr;
		mHlsUrl = "http://" + mRtspIpAddr + ":" + mHlsIpPort + "/" + mHlsResource;
		mHRtspUrl = "rtsp://" + mHlsIpAddr + ":" + mRtspIpPort + "/" + mRtspResource;
		GetPrivateProfileStringA("UserPreference", "Mp4OutputFileName","C:\\Users\\Public\\Videos\\ScreenRecording1.mp4",szValue,MAX_PATH - 1,configMsc.c_str());
		mMp4FileName =  szValue;
		GetPrivateProfileStringA("UserPreference", "HlsOutputFileName","C:\\Users\\Public\\Videos\\ScreenRecording1.m3u8",szValue,MAX_PATH - 1,configMsc.c_str());
		mHlsFileName =  szValue;

		GetPrivateProfileStringA("UserPreference", "Bitrate","4000000",szValue,MAX_PATH - 1,configMsc.c_str());
		mBitrate = atoi(szValue);

		GetPrivateProfileStringA("UserPreference", "GopLength","15",szValue,MAX_PATH - 1,configMsc.c_str());
		mGopLength = atoi(szValue);

		GetPrivateProfileStringA("UserPreference", "FrameRate","15",szValue,MAX_PATH - 1,configMsc.c_str());
		mFrameRate = atoi(szValue);
	}
	
	static CConfigure *Instance()
	{
		if(!mInstance)
			mInstance = new CConfigure();
		return mInstance;
	}

	void Save()
	{
		std::string configFolder;
		GetConfigFoderPath(configFolder);
		std::string configMsc = configFolder + "\\msc.cfg";
		WritePrivateProfileStringA("UserPreference", "AudioInput",mCrntAudioDevice.c_str(),configMsc.c_str());
		WritePrivateProfileStringA("UserPreference", "VideoInput",mCrntVideoDevice.c_str(),configMsc.c_str());
		WritePrivateProfileStringA("UserPreference", "Mp4OutputFileName",mMp4FileName.c_str(),configMsc.c_str());
		WritePrivateProfileStringA("UserPreference", "HlsOutputFileName",mHlsFileName.c_str(),configMsc.c_str());

		WritePrivateProfileStringA("UserPreference", "HlsPort",mHlsIpPort.c_str(),configMsc.c_str());
		WritePrivateProfileStringA("UserPreference", "HlsResourceName",mHlsResource.c_str(),configMsc.c_str());

		WritePrivateProfileStringA("UserPreference", "RtspPort",mRtspIpPort.c_str(),configMsc.c_str());
		WritePrivateProfileStringA("UserPreference", "RtspResourceName",mRtspResource.c_str(),configMsc.c_str());

		char szValue[32];
		itoa(mDestType,szValue, 10);
		WritePrivateProfileStringA("UserPreference", "DestinationType",szValue,configMsc.c_str());

		itoa(mBitrate,szValue, 10);
		WritePrivateProfileStringA("UserPreference", "Bitrate",szValue,configMsc.c_str());


		itoa(mGopLength,szValue, 10);
		WritePrivateProfileStringA("UserPreference", "GopLength",szValue,configMsc.c_str());

		itoa(mFrameRate,szValue, 10);
		WritePrivateProfileStringA("UserPreference", "FrameRate",szValue,configMsc.c_str());
	}
	~CConfigure()
	{
		WSACleanup();
	}
	int mDestType;
	std::string mCrntAudioDevice;
	std::string mCrntVideoDevice;
	std::string mCrntFileName;
	std::string mMp4FileName;
	std::string mHlsFileName;
	
	std::string mHlsUrl;
	std::string mHlsIpAddr;
	std::string mHlsIpPort;
	std::string mHlsResource;
	
	std::string mHRtspUrl;
	std::string mRtspIpAddr;
	std::string mRtspIpPort;
	std::string mRtspResource;

	int         mBitrate;
	int         mGopLength;
	int         mFrameRate;
	static CConfigure *mInstance;
};

CConfigure *CConfigure::mInstance = NULL;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

INT_PTR CALLBACK	ConfigureDlgProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	HANDLE hSingleInstanceMutex = CreateMutex(NULL, TRUE, L"OvsCaptureInstance");
	DWORD dwError = GetLastError();
	if (dwError == ERROR_ALREADY_EXISTS)
	{
		return FALSE;
	}
	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MCNSCREENCAP, szWindowClass, MAX_LOADSTRING);
	
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MCNSCREENCAP));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MCNSCREENCAP));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MCNSCREENCAP);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;
   HICON hMainIcon;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   hMainIcon = LoadIcon(hInstance,(LPCTSTR)MAKEINTRESOURCE(IDI_MCNSCREENCAP)); 

   nidApp.cbSize = sizeof(NOTIFYICONDATA); // sizeof the struct in bytes 
   nidApp.hWnd = (HWND) hWnd;              //handle of the window which will process this app. messages 
   nidApp.uID = IDI_MCNSCREENCAP;           //ID of the icon that willl appear in the system tray 
   nidApp.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; //ORing of all the flags 
   nidApp.hIcon = hMainIcon; // handle of the Icon to be displayed, obtained from LoadIcon 
   nidApp.uCallbackMessage = WM_USER_SHELLICON; 
   LoadString(hInstance, IDS_APPTOOLTIP,nidApp.szTip,MAX_LOADSTRING);
   Shell_NotifyIcon(NIM_ADD, &nidApp); 

   return TRUE;
}

void Init()
{
	// user defined message that will be sent as the notification message to the Window Procedure 
}

long mscGetFileSize(const char *fileName)
{    
	BOOL                        fOk;    
	WIN32_FILE_ATTRIBUTE_DATA   fileInfo;    
	if (NULL == fileName)        
		return -1;    
	fOk = GetFileAttributesExA(fileName, GetFileExInfoStandard, (void*)&fileInfo);    
	if (!fOk)        
		return -1;    
	return (long)fileInfo.nFileSizeLow;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
    POINT lpClickPoint;
	CConfigure *pCfg = CConfigure::Instance();
	switch (message)
	{

	case WM_USER_SHELLICON: 
		// systray msg callback 
		switch(LOWORD(lParam)) 
		{   
			case WM_RBUTTONDOWN: 
			{
				//UINT uFlag = MF_BYPOSITION|MF_STRING;
				GetCursorPos(&lpClickPoint);
				hPopMenu = CreatePopupMenu();
				InsertMenu(hPopMenu,0xFFFFFFFF,MF_BYPOSITION|MF_STRING,IDM_ABOUT,_T("About"));
				//if ( capState == CAP_STATE_RUN )	{
				//	uFlag |= MF_GRAYED;
				//}
				if (  capState == CAPTURE_STOP) {
					InsertMenu(hPopMenu,0xFFFFFFFF,MF_BYPOSITION|MF_STRING,IDM_START,_T("Start"));
					InsertMenu(hPopMenu,0xFFFFFFFF,MF_BYPOSITION|MF_STRING,IDM_CONFIGURE,_T("Configure"));
					InsertMenu(hPopMenu,0xFFFFFFFF,MF_BYPOSITION|MF_STRING,IDM_EXIT,_T("Exit"));
				} else  if (capState == CAPTURE_RUN) {
					InsertMenu(hPopMenu,0xFFFFFFFF,MF_BYPOSITION|MF_STRING,IDM_STOP,_T("Stop"));
					InsertMenu(hPopMenu,0xFFFFFFFF,MF_BYPOSITION|MF_STRING,IDM_PAUSE,_T("Pause"));				
				} else  if (capState == CAPTURE_PAUSE) {
					InsertMenu(hPopMenu,0xFFFFFFFF,MF_BYPOSITION|MF_STRING,IDM_RESUME,_T("Resume"));
					InsertMenu(hPopMenu,0xFFFFFFFF,MF_BYPOSITION|MF_STRING,IDM_STOP,_T("Stop"));				
				}
									
				SetForegroundWindow(hWnd);
				TrackPopupMenu(hPopMenu,TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_BOTTOMALIGN,lpClickPoint.x, lpClickPoint.y,0,hWnd,NULL);
			}
				return TRUE; 

			case WM_MOUSEMOVE: 
			{
				TCHAR tMsg[128] = {0};
				long ulFileSize = 0;
				if(capState == CAPTURE_RUN) {
					if(pCfg->mDestType == DEST_TYPE_MP4) {
						long ulFileSize = mscGetFileSize(pCfg->mCrntFileName.c_str()) / (1024 * 1024);
						_stprintf(tMsg,TEXT("OvsCapture\rCaptured %dMB"),ulFileSize);
						wcscpy(nidApp.szTip, tMsg);
					}
				} else {
					_stprintf(tMsg,TEXT("OvsCapture\rNot Capturing"),ulFileSize);
				}
				wcscpy(nidApp.szTip, tMsg);
				Shell_NotifyIcon(NIM_MODIFY, &nidApp); 
				int i = 0;
			}
			return TRUE;
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;

			case IDM_CONFIGURE:
				{
					int nResult = DialogBox(hInst, MAKEINTRESOURCE(IDD_CONFIGURE), hWnd, ConfigureDlgProc);
				}
				break;

			case IDM_STOP:
				{
					CCaptureGraph *pCap = CCaptureGraph::Instance();
					pCap->stop();
					pCap->Deinit();
					capState = CAPTURE_STOP;
				}
				break;
			case IDM_PAUSE:
				{
					CCaptureGraph *pCap = CCaptureGraph::Instance();
					pCap->pause();
					capState = CAPTURE_PAUSE;

				}
				break;
			case IDM_RESUME:
				{
					CCaptureGraph *pCap = CCaptureGraph::Instance();
					pCap->start();
					capState = CAPTURE_RUN;
				}
				break;

			case IDM_START:
				{
					CCaptureGraph *pCap = CCaptureGraph::Instance();
					switch(pCfg->mDestType){
						case DEST_TYPE_MP4:
							pCfg->mCrntFileName = pCfg->mMp4FileName;
							break;
						case DEST_TYPE_HLS_HD:
							pCfg->mCrntFileName = pCfg->mHlsFileName;
							break;
						case DEST_TYPE_HLS_NET:
							pCfg->mCrntFileName = pCfg->mHlsUrl;
							break;
						case DEST_TYPE_RTSP:
							pCfg->mCrntFileName = pCfg->mHRtspUrl;
							break;
					}

					if(pCap->Init(pCfg->mCrntFileName, pCfg->mCrntAudioDevice, 
						pCfg->mCrntVideoDevice, pCfg->mDestType,false) == 0){
						pCap->SetVidEncProperties(pCfg->mBitrate, pCfg->mGopLength, 0);
						pCap->start();
						capState = CAPTURE_RUN;
					} else {
						MessageBox(ghWnd, TEXT("Could not run capture!"), TEXT("OvsCapture Error"), IDOK);
					}
				}
				break;
			case IDM_EXIT:
				Shell_NotifyIcon(NIM_DELETE,&nidApp);
				DestroyWindow(hWnd);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
		/*
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;*/
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

int OnCaptureFile( HWND hDlg, char *szMp4File, int nDestType) 
{
    OPENFILENAMEA ofn={0};

    // Fill in standard structure fields
    ofn.lStructSize       = sizeof(OPENFILENAME);
    ofn.hwndOwner         = hDlg;
	if(nDestType == DEST_TYPE_MP4){
		ofn.lpstrFilter       = "MP4 Files (*.mp4; *.m4v)\0*.mp4; *.m4v\0All Files (*.*)\0*.*\0\0";
	} else if(nDestType == DEST_TYPE_HLS_HD) {
		ofn.lpstrFilter       = "M3U8 Files (*.m3u8)\0*.m3u8\0\0";
	}

    ofn.lpstrCustomFilter = NULL;
    ofn.nFilterIndex      = 1;
    ofn.nMaxFile          = MAX_PATH;
    ofn.lpstrFile         = szMp4File;
    ofn.lpstrTitle        = "Specify file to save captured data...\0";
    ofn.lpstrFileTitle    = NULL;
    ofn.lpstrDefExt       = "mp4\0";
    ofn.Flags             = OFN_OVERWRITEPROMPT;//OFN_HIDEREADONLY  | OFN_SHAREAWARE | 
                            //OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
    ofn.lpstrInitialDir   =  "c:\\\0";

    BOOL bSuccess = GetSaveFileNameA((LPOPENFILENAMEA)&ofn);
    return bSuccess;
}

void UpdateUi(HWND hDlg)
{
}


INT_PTR CALLBACK ConfigureDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	CConfigure *pCfg = CConfigure::Instance();
	switch (message)
	{
	case WM_INITDIALOG:
		{
			std::vector<std::string> mAudDevices;
			std::vector<std::string> mVidDevices;
			CCaptureGraph *pCap = CCaptureGraph::Instance();

			/* Fill Audio Input Source Combobox */
			pCap->GetDeviceList(CAPTURE_DEVICE_CAREGORY_AUDIO, mAudDevices);
			if(!mAudDevices.empty()){
				WCHAR *wtext = pCap->convertCStringToWString(CSTR_NO_AUDIO);
				SendDlgItemMessage(hDlg, IDC_COMBO_AUDIO_INPUT, CB_ADDSTRING, 0, (LPARAM)wtext); 
				for (int i=0; i < mAudDevices.size(); i++){
					wtext = pCap->convertCStringToWString(mAudDevices[i].c_str());
					SendDlgItemMessage(hDlg, IDC_COMBO_AUDIO_INPUT, CB_ADDSTRING, 0, (LPARAM)wtext); 
				}
				wtext = pCap->convertCStringToWString(pCfg->mCrntAudioDevice.c_str());
				SendDlgItemMessage(hDlg, IDC_COMBO_AUDIO_INPUT, CB_SELECTSTRING, 0, (LPARAM)wtext); 
			}

			/* Fill Video Source Combo box */
			pCap->GetDeviceList(CAPTURE_DEVICE_CAREGORY_VIDEO, mVidDevices);
			if(!mVidDevices.empty()){
				WCHAR *wtext = pCap->convertCStringToWString(CSTR_NOVIDEO);
				SendDlgItemMessage(hDlg, IDC_COMBO_VIDEO_INPUT, CB_ADDSTRING, 0, (LPARAM)wtext); 
				wtext = pCap->convertCStringToWString(CSTR_DESKTOP_VIDEO);
				SendDlgItemMessage(hDlg, IDC_COMBO_VIDEO_INPUT, CB_ADDSTRING, 0, (LPARAM)wtext); 

				for (int i=0; i < mVidDevices.size(); i++){
					wtext = pCap->convertCStringToWString(mVidDevices[i].c_str());
					SendDlgItemMessage(hDlg, IDC_COMBO_VIDEO_INPUT, CB_ADDSTRING, 0, (LPARAM)wtext); 
				}

				wtext = pCap->convertCStringToWString(pCfg->mCrntVideoDevice.c_str());
				SendDlgItemMessage(hDlg, IDC_COMBO_VIDEO_INPUT, CB_SELECTSTRING, 0, (LPARAM)wtext); 
			}

			WCHAR *wtext = pCap->convertCStringToWString(pCfg->mHlsFileName.c_str());
			SetDlgItemText(hDlg, IDC_EDIT_FILE_NAME_HLS, wtext); 

			wtext = pCap->convertCStringToWString(pCfg->mMp4FileName.c_str());
			SetDlgItemText(hDlg, IDC_EDIT_FILE_NAME_MP4, wtext); 

			wtext = pCap->convertCStringToWString(pCfg->mHRtspUrl.c_str());
			SetDlgItemText(hDlg, IDC_EDIT_RTSP_ADDR, wtext); 

			wtext = pCap->convertCStringToWString(pCfg->mHlsUrl.c_str());
			SetDlgItemText(hDlg, IDC_EDIT_HLS_ADDR, wtext); 

			CheckRadioButton(hDlg, IDC_RADIO_MP4, IDC_RADIO_RTSP, IDC_RADIO_MP4 + pCfg->mDestType);
			char szTmp[64];
			sprintf(szTmp, "%d", pCfg->mBitrate);
			wtext = pCap->convertCStringToWString(szTmp);
			SetDlgItemText(hDlg, IDC_EDIT_BITRATE, wtext); 

			sprintf(szTmp, "%d", pCfg->mGopLength);
			wtext = pCap->convertCStringToWString(szTmp);
			SetDlgItemText(hDlg, IDC_EDIT_GOP_LENGTH, wtext); 

			sprintf(szTmp, "%d", pCfg->mFrameRate);
			wtext = pCap->convertCStringToWString(szTmp);
			SetDlgItemText(hDlg, IDC_EDIT_FRAME_RATE, wtext); 
		}
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			case IDOK:
			{
				CCaptureGraph *pCap = CCaptureGraph::Instance();
				WCHAR wtext[256+2];
				GetDlgItemText(hDlg, IDC_EDIT_FILE_NAME_MP4, wtext, 256); 
				pCfg->mMp4FileName = pCap->convertWStringToCString(wtext);
				GetDlgItemText(hDlg, IDC_EDIT_FILE_NAME_HLS, wtext, 256); 
				pCfg->mHlsFileName = pCap->convertWStringToCString(wtext);


				int nSelId = SendDlgItemMessage(hDlg, IDC_COMBO_AUDIO_INPUT, CB_GETCURSEL, 0, 0); 
				SendDlgItemMessage(hDlg, IDC_COMBO_AUDIO_INPUT, CB_GETLBTEXT , nSelId, (LPARAM)wtext); 
				pCfg->mCrntAudioDevice = pCap->convertWStringToCString(wtext);
				
				nSelId = SendDlgItemMessage(hDlg, IDC_COMBO_VIDEO_INPUT, CB_GETCURSEL, 0, 0); 
				SendDlgItemMessage(hDlg, IDC_COMBO_VIDEO_INPUT, CB_GETLBTEXT , nSelId, (LPARAM)wtext); 
				pCfg->mCrntVideoDevice = pCap->convertWStringToCString(wtext);

				std::string szTmp;
				GetDlgItemText(hDlg, IDC_EDIT_BITRATE, wtext, 256); 
				szTmp = pCap->convertWStringToCString(wtext);
				sscanf(szTmp.c_str(), "%d", &pCfg->mBitrate);


				GetDlgItemText(hDlg, IDC_EDIT_GOP_LENGTH, wtext, 256); 
				szTmp = pCap->convertWStringToCString(wtext);
				sscanf(szTmp.c_str(), "%d", &pCfg->mGopLength);

				GetDlgItemText(hDlg, IDC_EDIT_FRAME_RATE, wtext, 256); 
				szTmp = pCap->convertWStringToCString(wtext);
				sscanf(szTmp.c_str(), "%d", &pCfg->mFrameRate);

				pCfg->Save();
				EndDialog(hDlg, IDOK);
				return (INT_PTR)TRUE;
			}
			break;

			case IDCANCEL:
			{
				EndDialog(hDlg, IDCANCEL);
				return (INT_PTR)TRUE;
			}
			break;
			case IDC_COMBO_AUDIO_INPUT:
			{
				if (HIWORD(wParam) == CBN_SELCHANGE)
				{
					CCaptureGraph *pCap = CCaptureGraph::Instance();
					WCHAR wtext[256+2];
					int nSelId = SendDlgItemMessage(hDlg, IDC_COMBO_AUDIO_INPUT, CB_GETCURSEL, 0, 0); 
					SendDlgItemMessage(hDlg, IDC_COMBO_AUDIO_INPUT, CB_GETLBTEXT , 0, (LPARAM)wtext); 
					pCfg->mCrntAudioDevice = pCap->convertWStringToCString(wtext);
					return (INT_PTR)TRUE;
				}
			}
			break;
			case IDC_COMBO_VIDEO_INPUT:
			{
				if (HIWORD(wParam) == CBN_SELCHANGE)
				{
					CCaptureGraph *pCap = CCaptureGraph::Instance();
					WCHAR wtext[256+2];
					int nSelId = SendDlgItemMessage(hDlg, IDC_COMBO_VIDEO_INPUT, CB_GETCURSEL, 0, 0); 
					SendDlgItemMessage(hDlg, IDC_COMBO_VIDEO_INPUT, CB_GETLBTEXT , 0, (LPARAM)wtext); 
					pCfg->mCrntVideoDevice = pCap->convertWStringToCString(wtext);
					return (INT_PTR)TRUE;
				}
			}
			break;

			case IDC_BUTTON_BROWSE_MP4:
			{
				CCaptureGraph *pCap = CCaptureGraph::Instance();
				char szCaptureFile[MAX_PATH] = {0};
				if(OnCaptureFile(hDlg, szCaptureFile, DEST_TYPE_MP4)){
					WCHAR *wtext = pCap->convertCStringToWString(szCaptureFile);
					SetDlgItemText(hDlg, IDC_EDIT_FILE_NAME_MP4, wtext);
				}
			}
			break;
			case IDC_BUTTON_BROWSE_HLS:
			{
				CCaptureGraph *pCap = CCaptureGraph::Instance();
				char szCaptureFile[MAX_PATH] = {0};
				if(OnCaptureFile(hDlg, szCaptureFile, DEST_TYPE_HLS_HD)){
					WCHAR *wtext = pCap->convertCStringToWString(szCaptureFile);
					SetDlgItemText(hDlg, IDC_EDIT_FILE_NAME_HLS, wtext);
				}
			}
			break;

			case IDC_RADIO_MP4:
			{
				pCfg->mDestType = DEST_TYPE_MP4;
				UpdateUi(hDlg);
			}
			break;
			case IDC_RADIO_HLS_HD:
			{
				pCfg->mDestType = DEST_TYPE_HLS_HD;
				UpdateUi(hDlg);
			}
			break;
			case IDC_RADIO_HLS_NET:
			{
				pCfg->mDestType = DEST_TYPE_HLS_NET;
				UpdateUi(hDlg);
			}
			break;
			case IDC_RADIO_RTSP:
			{
				pCfg->mDestType = DEST_TYPE_RTSP;
				UpdateUi(hDlg);
			}
			break;
		}
		return (INT_PTR)TRUE;
	}
	return (INT_PTR)FALSE;
}

