// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "OvsControl.h"
#include "PageRecord.h"
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "ConfigComn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REC_CONFIG_FILE           "\\orc.cfg"
#define SECTION_NAME              "UserPreference"
#define KEY_NAME_VID_BITRATE      "VidBitrate"
#define KEY_NAME_AUD_BITRATE      "AudBitrate"
#define KEY_NAME_GOP_LENGTH       "GopLength"
#define KEY_NAME_DEST_TYPE        "DestinationType"
#define KEY_NAME_AUD_INPUT        "AudioInput"
#define KEY_NAME_VID_INPUT        "CurrentVidInput"
#define MAX_ONYX_STREAMS          4

#define STATUS_TIMER_ID			100

static char* convertWStringToCString(WCHAR* wString)
{
    const int MAX_STRINGZ=500;
    static char ctext[MAX_STRINGZ+2];

    if (WideCharToMultiByte(CP_ACP, 0, wString, -1, ctext, MAX_STRINGZ,0,0) == 0)  {
        throw("[DSHOW] convertWStringToCString failed with no extra error info");
    }

    return ctext;
}

class CConfigRecord
{
public:
	CConfigRecord()
	{
	   char szPath[128] = "";
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);

		CString configFolder;
		GetConfigFoderPath(configFolder);
		std::string configMsc = configFolder + REC_CONFIG_FILE;
		std::string configOvs = configFolder + ONYX_CONFIG_FILE;
		char szValue[MAX_PATH];

		GetPrivateProfileStringA(SECTION_NAME, KEY_NAME_AUD_INPUT,CSTR_NO_AUDIO,szValue,MAX_PATH - 1,configMsc.c_str());
		mCrntAudioDevice = szValue;

		//GetPrivateProfileStringA(SECTION_NAME, "VideoInput",CSTR_MCN_VCAP,szValue,MAX_PATH - 1,configMsc.c_str());
		//mCrntVideoDevice = szValue;
		mCrntVideoDevice = CSTR_MCN_VCAP;

		mCrntDestFmt = GetPrivateProfileIntA(SECTION_NAME,KEY_NAME_DEST_TYPE,DEST_TYPE_MP4,configMsc.c_str());
		

		GetPrivateProfileStringA(SECTION_NAME, "Mp4OutputFileName","C:\\Users\\Public\\Videos\\ScreenRecording1.mp4",szValue,MAX_PATH - 1,configMsc.c_str());
		mMp4FileName =  szValue;
		GetPrivateProfileStringA(SECTION_NAME, "HlsOutputFileName","C:\\Users\\Public\\Videos\\ScreenRecording1.m3u8",szValue,MAX_PATH - 1,configMsc.c_str());
		mHlsFileName =  szValue;

		for (int i=0; i < MAX_ONYX_STREAMS; i++){

			CString Section;
			CString StreamSharedMemName;
			CAVSrcInf *pStrm = new CAVSrcInf;
			Section.Format("Output%d", i+1);
			int res = GetOnyxOutStreamInf(configOvs.c_str(), Section, *pStrm);
			if(res == 0){
				CString Caption;
				Caption.Format("Stream_%dX%d", pStrm->mWidth, pStrm->mHeight);
				pStrm->mLabel = Caption;
				mOnyxOutputStreams.push_back(pStrm);
				GetStreamUserParams(configMsc.c_str(), Caption, *pStrm);
			} else {
				delete pStrm;
			}
		}
		GetPrivateProfileStringA(SECTION_NAME, KEY_NAME_VID_INPUT,"Output1",szValue,MAX_PATH - 1,configMsc.c_str());
		mpCrntVcamStream = GetVidSrcStream(szValue);
	}
	
	void GetStreamUserParams(const char * cfgFile, const char *szSection, CAVSrcInf &AvSrcInf)
	{
		char szValue[MAX_PATH];
		GetPrivateProfileStringA(szSection, KEY_NAME_VID_BITRATE,"",szValue,MAX_PATH - 1,cfgFile);
		AvSrcInf.mVUserBitrate = atoi(szValue);
		if(AvSrcInf.mVUserBitrate == 0)
			AvSrcInf.mVUserBitrate = AvSrcInf.mVFairBitrate;

		GetPrivateProfileStringA(szSection, KEY_NAME_AUD_BITRATE,"64000",szValue,MAX_PATH - 1,cfgFile);
		AvSrcInf.mAUserBitrate = atoi(szValue);

		GetPrivateProfileStringA(szSection, KEY_NAME_GOP_LENGTH,"60",szValue,MAX_PATH - 1,cfgFile);
		AvSrcInf.mUserGopLen = atoi(szValue);
	}

	void SaveStreamUserParams(const char * cfgFile, const char *szSection, CAVSrcInf &AvSrcInf)
	{
		char szValue[MAX_PATH];

		itoa(AvSrcInf.mVUserBitrate,szValue, 10);
		WritePrivateProfileStringA(szSection, KEY_NAME_VID_BITRATE,szValue,cfgFile);

		itoa(AvSrcInf.mAUserBitrate,szValue, 10);
		WritePrivateProfileStringA(szSection, KEY_NAME_AUD_BITRATE,szValue,cfgFile);


		itoa(AvSrcInf.mUserGopLen,szValue, 10);
		WritePrivateProfileStringA(szSection, KEY_NAME_GOP_LENGTH,szValue,cfgFile);
	}

	static CConfigRecord *Instance()
	{
		if(!mInstance)
			mInstance = new CConfigRecord();
		return mInstance;
	}

	void Save()
	{
		CString configFolder;
		GetConfigFoderPath(configFolder);
		std::string configMsc = configFolder + REC_CONFIG_FILE;
		WritePrivateProfileStringA(SECTION_NAME, KEY_NAME_AUD_INPUT,mCrntAudioDevice.c_str(),configMsc.c_str());
		// WritePrivateProfileStringA(SECTION_NAME, "VideoInput",mCrntVideoDevice.c_str(),configMsc.c_str());
		WritePrivateProfileStringA(SECTION_NAME, "Mp4OutputFileName",mMp4FileName.c_str(),configMsc.c_str());
		WritePrivateProfileStringA(SECTION_NAME, "HlsOutputFileName",mHlsFileName.c_str(),configMsc.c_str());


		char szValue[32];
		if(mpCrntVcamStream)
			WritePrivateProfileStringA(SECTION_NAME, KEY_NAME_VID_INPUT, mpCrntVcamStream->mSectionName.c_str(),configMsc.c_str());

		itoa(mCrntDestFmt,szValue, 10);
		WritePrivateProfileStringA(SECTION_NAME,KEY_NAME_DEST_TYPE,szValue,configMsc.c_str());

		for (int i = 0; i < mOnyxOutputStreams.size(); i++) {
			CAVSrcInf *pStrm = mOnyxOutputStreams[i];
			SaveStreamUserParams(configMsc.c_str(), pStrm->mLabel.c_str(), *pStrm);
		}
	}

	CAVSrcInf *GetVidSrcStream(const char *szSection)
	{
		for (int i =0; i < mOnyxOutputStreams.size(); i++) {
			 CAVSrcInf * pStrm = mOnyxOutputStreams[i];
			 if(pStrm->mSectionName.compare(szSection) == 0){
				return pStrm;
			}
		}
		return NULL;
	}

	CAVSrcInf *GetVidSrcStreamForLabel(const char *szLabel)
	{
		for (int i =0; i < mOnyxOutputStreams.size(); i++) {
			 CAVSrcInf * pStrm = mOnyxOutputStreams[i];
			 if(pStrm->mLabel.compare(szLabel) == 0){
				return pStrm;
			}
		}
		return NULL;
	}

	~CConfigRecord()
	{
		WSACleanup();
	}


	std::string mCrntAudioDevice;
	std::string mCrntVideoDevice;

	std::string mCrntFileName;
	std::string mMp4FileName;
	std::string mHlsFileName;
	std::vector<CAVSrcInf *>      mOnyxOutputStreams;
	CAVSrcInf                     *mpCrntVcamStream;

	std::vector<CVidDestFmtInf *>  mDestFormats;
	int                            mCrntDestFmt;
	static CConfigRecord *mInstance;
};

CConfigRecord *CConfigRecord::mInstance = NULL;

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

/////////////////////////////////////////////////////////////////////////////
// CPageRecorder property page

IMPLEMENT_DYNCREATE(CPageRecorder, CMFCPropertyPage)

CPageRecorder::CPageRecorder() : CMFCPropertyPage(CPageRecorder::IDD)
, m_EditAudioInput(_T(""))
, m_FileName(_T(""))
, m_fModified(0)
, m_StatusUpdateInterval(1000)
, m_pCap(NULL)
{
}

CPageRecorder::~CPageRecorder()
{
	if(m_pCap)
		delete m_pCap;
}

void CPageRecorder::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_COMBO_AUDIO_INPUT, m_EditAudioInput);
	DDX_Text(pDX, IDC_EDIT_FILE_NAME, m_FileName);
	DDX_Control(pDX, IDC_BUTON_RECORD, m_BtnRecord);
	DDX_Control(pDX, IDC_BUTON_STOP, m_BtnStop);
	DDX_Control(pDX, IDC_BUTON_PAUSE, m_BtnPause);
	DDX_Control(pDX, IDC_BUTON_RESUME, m_BtnResume);
	DDX_Control(pDX, IDC_SLIDER_AUD_BITRATE, m_SliderAudBitrate);
	DDX_Control(pDX, IDC_SLIDER_VID_BITRATE, m_SliderVidBitrate);
	DDX_Control(pDX, IDC_SLIDER_GOPLENGTH, m_SliderGoplen);

	DDX_Control(pDX, IDC_COMBOEX_INPUT_SOURCE, m_ComboexSource);
}

BEGIN_MESSAGE_MAP(CPageRecorder, CMFCPropertyPage)
	ON_WM_ERASEBKGND()
//	ON_BN_CLICKED(IDC_BUTTON_BROWSE_OUT, &CPageRecorder::OnBnClickedButtonBrowseOut)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTON_RECORD, &CPageRecorder::OnBnClickedButonRecord)
	ON_BN_CLICKED(IDC_BUTON_PAUSE, &CPageRecorder::OnBnClickedButonPause)
	ON_BN_CLICKED(IDC_BUTON_RESUME, &CPageRecorder::OnBnClickedButonResume)
	ON_BN_CLICKED(IDC_BUTON_STOP, &CPageRecorder::OnBnClickedButonStop)
	ON_CBN_SELCHANGE(IDC_COMBOEX_INPUT_SOURCE, &CPageRecorder::OnCbnSelchangeComboexDestFmt)
	ON_WM_HELPINFO()
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_COMBO_VIDEO_INPUT, &CPageRecorder::OnCbnSelchangeComboVideoInput)
	ON_BN_CLICKED(IDC_BUTTON_SET_DEFAULTS, &CPageRecorder::OnBnClickedButtonSetDefaults)
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CPageRecorder::ConfigBtn(CMFCButton &Btn, int nToolTipId)
{
	CString Tooltip;

	Btn.SetFaceColor(PANEL_BCKGND,  TRUE );
	Btn.m_nFlatStyle = CMFCButton::BUTTONSTYLE_NOBORDERS;
	Btn.m_bTopImage = TRUE;
	Btn.m_nAlignStyle = CMFCButton::ALIGN_CENTER;
	Btn. m_bDrawFocus = FALSE;
	Btn.SizeToContent();

	if(Tooltip.LoadStringA(nToolTipId)) {
		Btn.SetTooltip(Tooltip);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPageRecorder message handlers

BOOL CPageRecorder::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();
	m_Background.LoadBitmap(IDB_BITMAP_WELCOME); //Load bitmap
	m_pEditBkBrush = new CBrush(RGB(0, 0, 0x50));
	m_pStaticBkBrush = new CBrush(RGB(0xB1, 0xC1, 0xEC));

	m_BtnRecord.SetImage(RECORDU, RECORDD, RECORDX);
	ConfigBtn(m_BtnRecord, IDS_TTIP_RECORD);


	m_BtnStop.SetImage(STOPU, STOPD, STOPX);
	ConfigBtn(m_BtnStop, IDS_TTIP_STOP);

	m_BtnPause.SetImage(PAUSEU, PAUSED, PAUSEX);
	ConfigBtn(m_BtnPause, IDS_TTIP_PAUSE);

	m_BtnResume.SetImage(RESUMEU, RESUMED, RESUMEX);
	ConfigBtn(m_BtnResume, IDS_TTIP_RESUME);

	std::vector<std::string> mAudDevices;
	std::vector<std::string> mVidDevices;
	CCaptureGraph *pCap = new CCaptureGraph;
	m_pCap = pCap;
	CConfigRecord *pCfg = CConfigRecord::Instance();
	m_pCfg = pCfg;
	CreateSourceSelectionList();

	/* Fill Audio Input Source Combobox */
	pCap->GetDeviceList(CAPTURE_DEVICE_CAREGORY_AUDIO, mAudDevices);
	if(!mAudDevices.empty()){
		SendDlgItemMessage(IDC_COMBO_AUDIO_INPUT, CB_ADDSTRING, 0, (LPARAM)CSTR_NO_AUDIO); 
		for (int i=0; i < mAudDevices.size(); i++){
			SendDlgItemMessage(IDC_COMBO_AUDIO_INPUT, CB_ADDSTRING, 0, (LPARAM)mAudDevices[i].c_str()); 
		}
		m_EditAudioInput = pCfg->mCrntAudioDevice.c_str();
	}

	/* Fill Video Source Combo box */
	//pCap->GetDeviceList(CAPTURE_DEVICE_CAREGORY_VIDEO, mVidDevices);
	//if(!mVidDevices.empty())

	for (int i=0; i < pCfg->mOnyxOutputStreams.size(); i++){
		int nWidth, nHeight;
		CString Section;
		CAVSrcInf *pStrm = pCfg->mOnyxOutputStreams[i];
			
		SendDlgItemMessage(IDC_COMBO_VIDEO_INPUT, CB_INSERTSTRING, i, (LPARAM)pStrm->mLabel.c_str()); 
		SendDlgItemMessage(IDC_COMBO_VIDEO_INPUT, CB_SETITEMDATA, i, (LPARAM)i); 
	}

	if(pCfg->mpCrntVcamStream) {
		SendDlgItemMessage(IDC_COMBO_VIDEO_INPUT, CB_SELECTSTRING, 0, (LPARAM)pCfg->mpCrntVcamStream->mLabel.c_str()); 
	}
	
	if(pCfg->mCrntDestFmt == DEST_TYPE_MP4){
		m_FileName = pCfg->mMp4FileName.c_str(); 
	} else if(pCfg->mCrntDestFmt == DEST_TYPE_HLS_HD){
		m_FileName  = pCfg->mHlsFileName.c_str(); 
	}
	UpdateBitrateRanges();
	mCapState = CAPTURE_STOP;
	Updatebuttons();
	UpdateData(FALSE);

	return TRUE;
}

static int STRMLIST_IMG[]=
{
	IDB_BITMAP_MP4U,
	IDB_BITMAP_HLSU
};

void CPageRecorder::SetComboExItem(CComboBoxEx *pCombo, int row, LPCTSTR itemText)
{
	COMBOBOXEXITEM item;
	memset(&item, 0, sizeof(item));
	//item.mask = CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_TEXT;
	item.mask =  CBEIF_TEXT;

	item.iItem = row;
	item.iSelectedImage = item.iImage = row;
	//item.iIndent = m_lstIndents [i];
	item.pszText = (LPTSTR)(LPCTSTR) itemText;
	item.cchTextMax = strlen(itemText);

	pCombo->InsertItem(&item);
}

void CPageRecorder::CreateSourceSelectionList()
{
	CConfigRecord *pCfg = CConfigRecord::Instance();
	int i = 0;
	CVidDestFmtInf *pDestFmt = new CVidDestFmtInf("MP4", DEST_TYPE_MP4);
	pCfg->mDestFormats.push_back(pDestFmt);

	pDestFmt = new CVidDestFmtInf("HLS", DEST_TYPE_HLS_HD);
	pCfg->mDestFormats.push_back(pDestFmt);

	for(int i=0; i < pCfg->mDestFormats.size(); i++) {
		CVidDestFmtInf *pDestFmt =pCfg->mDestFormats[i];
		//SendDlgItemMessage(IDC_COMBOEX_INPUT_SOURCE, CB_INSERTSTRING, i, (LPARAM)pDestFmt->mLabel.c_str()); 
		//SendDlgItemMessage(IDC_COMBOEX_INPUT_SOURCE, CB_SETITEMDATA, 0, pDestFmt->mFmt); 
		SetComboExItem(&m_ComboexSource, i, pDestFmt->mLabel.c_str());
		m_ComboexSource.SetItemData(i, pDestFmt->mFmt);
		if(pCfg->mCrntDestFmt == pDestFmt->mFmt)
			m_ComboexSource.SetCurSel(i); 
	}

	m_ComboexSource.ShowWindow(SW_SHOW);
}

void CPageRecorder::OnBnClickedButonRecord()
{
	UpdateData(TRUE);
	CCaptureGraph *pCap = m_pCap;
	CConfigRecord *pCfg = CConfigRecord::Instance();

	CAVSrcInf *pStrm = pCfg->mpCrntVcamStream;
	int nIndex = SendDlgItemMessage(IDC_COMBO_VIDEO_INPUT, CB_GETCURSEL, 0, 0);
	
	if(nIndex != CB_ERR ) {
		int nId = SendDlgItemMessage(IDC_COMBO_VIDEO_INPUT, CB_GETITEMDATA, nIndex,NULL); 
		pStrm = pCfg->mOnyxOutputStreams[nId];
	} else {
		AfxMessageBox("Video Source stream not specified");
		return;
	}
	

	pCfg->mCrntAudioDevice = m_EditAudioInput;
	pCfg->mCrntFileName = m_FileName;

	switch(pCfg->mCrntDestFmt){
		case DEST_TYPE_MP4:
			pCfg->mMp4FileName = pCfg->mCrntFileName;
			break;
		case DEST_TYPE_HLS_HD:
			pCfg->mHlsFileName = pCfg->mCrntFileName;
			break;
	}
	
	if(pStrm) {
		if(pCap->Init(pCfg->mCrntFileName, pCfg->mCrntAudioDevice, 
			pCfg->mCrntVideoDevice, pStrm->mSharedMemName, pCfg->mCrntDestFmt, pStrm->mWidth, pStrm->mHeight, false) == 0){
				pCap->SetVidEncProperties(pStrm->mVUserBitrate, pStrm->mUserGopLen, 0);
			pCap->start();
			mCapState = CAPTURE_RUN;
			Updatebuttons();
		} else {
			SetDlgItemText(IDC_EDIT_RECORD_STATUS, "Recording Failed.");
		}
	}
}

void CPageRecorder::OnBnClickedButonPause()
{
	CConfigRecord *pCfg = CConfigRecord::Instance();
	CCaptureGraph *pCap = m_pCap;
	pCap->pause();
	mCapState = CAPTURE_PAUSE;
	Updatebuttons();
}

void CPageRecorder::OnBnClickedButonResume()
{
	CConfigRecord *pCfg = CConfigRecord::Instance();
	CCaptureGraph *pCap = m_pCap;
	pCap->start();
	mCapState = CAPTURE_RUN;
	Updatebuttons();
}

void CPageRecorder::OnBnClickedButonStop()
{
	CConfigRecord *pCfg = CConfigRecord::Instance();
	CCaptureGraph *pCap = m_pCap;
	pCap->stop();
	pCap->Deinit();
	mCapState = CAPTURE_STOP;
	Updatebuttons();
}

void CPageRecorder::Updatebuttons()
{
	if (  mCapState == CAPTURE_STOP) {
		GetDlgItem(IDC_BUTON_RECORD)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTON_STOP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTON_PAUSE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTON_RESUME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTON_PAUSE)->EnableWindow(FALSE);
		SetDlgItemText(IDC_EDIT_RECORD_STATUS, "Stop.");
	} else  if (mCapState == CAPTURE_RUN) {
		GetDlgItem(IDC_BUTON_RECORD)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTON_STOP)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTON_PAUSE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTON_PAUSE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTON_RESUME)->ShowWindow(SW_HIDE);
		SetDlgItemText(IDC_EDIT_RECORD_STATUS, "Recording..");

		if(m_StatusUpdateInterval > 0) {
			SetTimer(STATUS_TIMER_ID, m_StatusUpdateInterval,NULL);
		}

	} else  if (mCapState == CAPTURE_PAUSE) {
		GetDlgItem(IDC_BUTON_RECORD)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTON_STOP)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTON_PAUSE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTON_RESUME)->ShowWindow(SW_SHOW);
		SetDlgItemText(IDC_EDIT_RECORD_STATUS, "Recording paused.");
	}
}

void CPageRecorder::SaveConfig()
{
	UpdateData(TRUE);

	CCaptureGraph *pCap = m_pCap;
	CConfigRecord *pCfg = CConfigRecord::Instance();

	pCfg->mMp4FileName = m_FileName;
	pCfg->mHlsFileName = m_HlsFileName;

	pCfg->mCrntAudioDevice = m_EditAudioInput;

	int nIndex = SendDlgItemMessage(IDC_COMBO_VIDEO_INPUT, CB_GETCURSEL, 0, NULL); 
	int nItem = SendDlgItemMessage(IDC_COMBO_VIDEO_INPUT, CB_GETITEMDATA, nIndex, NULL); 

	pCfg->Save();
}
BOOL CPageRecorder::OnApply()
{
	SaveConfig();
	m_fModified = false;
	return CMFCPropertyPage::OnApply();
}

BOOL CPageRecorder::OnEraseBkgnd(CDC* pDC)
{
	CMFCPropertyPage::OnEraseBkgnd(pDC);
	if(m_Background.m_hObject){

		CRect rect;
		GetClientRect(&rect);
		CDC dc;
		dc.CreateCompatibleDC(pDC);
		CBitmap* pOldBitmap = dc.SelectObject(&m_Background);

		BITMAP bmap;
		m_Background.GetBitmap(&bmap);
		pDC->StretchBlt(0, 0, rect.Width(),rect.Height(), &dc,0, 0,bmap.bmWidth,bmap.bmHeight, SRCCOPY);

		dc.SelectObject(pOldBitmap);
	}
	return TRUE;
}

//void CPageRecorder::OnBnClickedButtonBrowseOut()
//{
//	CCaptureGraph *pCap = CCaptureGraph::Instance();
//	CConfigRecord *pCfg = CConfigRecord::Instance();
//	char szCaptureFile[MAX_PATH] = {0};
//	if(OnCaptureFile(m_hWnd, szCaptureFile, pCfg->mCrntDestFmt)){
//		SetDlgItemText(IDC_EDIT_FILE_NAME, szCaptureFile);
//		m_fModified = true;
//		SetModified(1);
//	}
//}

HBRUSH CPageRecorder::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CMFCPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);

	if (CTLCOLOR_STATIC == nCtlColor)  {
		if(pWnd->GetDlgCtrlID() == IDC_EDIT_RECORD_STATUS
#if 0
			|| pWnd->GetDlgCtrlID() == IDC_EDIT_VID_BITRATE
			|| pWnd->GetDlgCtrlID() == IDC_EDIT_AUD_BITRATE
			|| pWnd->GetDlgCtrlID() == IDC_EDIT_GOP_LENGTH
#endif
			){
			pDC->SetTextColor(RGB(0, 255, 0));
			pDC->SetBkColor(RGB(0, 0, 0x80));
			return (HBRUSH)(m_pEditBkBrush->GetSafeHandle());
		} else {
			//pDC->SetBkMode(TRANSPARENT);
			//return (HBRUSH)GetStockObject(NULL_BRUSH);
			return (HBRUSH)(m_pStaticBkBrush->GetSafeHandle());
		}
	} else if ( nCtlColor == CTLCOLOR_EDIT ) {
		pDC->SetTextColor(RGB(0, 255, 0));
		pDC->SetBkColor(RGB(0, 0, 0));
		return (HBRUSH)(m_pEditBkBrush->GetSafeHandle());
	}
	return hbr;
}

void CPageRecorder::OnDestroy()
{
	CMFCPropertyPage::OnDestroy();

	delete m_pEditBkBrush;
	delete m_pStaticBkBrush;
}

void CPageRecorder::UpdateBitrateRanges()
{
	CAVSrcInf *pStrm = m_pCfg->mpCrntVcamStream;
	if(pStrm) {
		int nAudBitrate =  pStrm->mAUserBitrate;
		int nVidBitrate =  pStrm->mVUserBitrate;
		int nGopLength = pStrm->mUserGopLen;

		m_SliderAudBitrate.SetRange(16000,128000);
		m_SliderAudBitrate.SetPos(nAudBitrate);
		m_SliderAudBitrate.SetLineSize(1000);

		m_SliderVidBitrate.SetRange(pStrm->mVMinBitrate,pStrm->mVMaxBitrate);
		m_SliderVidBitrate.SetLineSize(10000);
		m_SliderVidBitrate.SetPos(nVidBitrate);
		m_SliderGoplen.SetRange(1,300);
		m_SliderGoplen.SetPos(nGopLength);
		SetDlgItemInt(IDC_EDIT_AUD_BITRATE, nAudBitrate);
		SetDlgItemInt(IDC_EDIT_VID_BITRATE, nVidBitrate);
		SetDlgItemInt(IDC_EDIT_GOP_LENGTH, nGopLength);
	}
}


void CPageRecorder::OnCbnSelchangeComboexDestFmt()
{
	int nSel = m_ComboexSource.GetCurSel();
	CConfigRecord *pCfg = CConfigRecord::Instance();
	int nDestFmt = m_ComboexSource.GetItemData(nSel); 
	pCfg->mCrntDestFmt = nDestFmt;

	CCaptureGraph *pCap = m_pCap;
	
	char szCaptureFile[MAX_PATH] = {0};
	if(OnCaptureFile(m_hWnd, szCaptureFile, pCfg->mCrntDestFmt)){
		SetDlgItemText(IDC_EDIT_FILE_NAME, szCaptureFile);
		m_fModified = true;
		SetModified(1);
	} else {
		// Restore previous type
	}
}

BOOL CPageRecorder::OnHelpInfo(HELPINFO* pHelpInfo)
{
	// TODO: Add your message handler code here and/or call default

	return CMFCPropertyPage::OnHelpInfo(pHelpInfo);
}

BOOL CPageRecorder::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR	*lpnm = (NMHDR *) lParam;
	if(lpnm->code == PSN_HELP) {
		CString cstrHelpFile = "OnyxVirtualStudio.chm";
		::HtmlHelp(this->m_hWnd, cstrHelpFile, HH_HELP_FINDER, 0);
		return TRUE;
	}

	return CMFCPropertyPage::OnNotify(wParam, lParam, pResult);
}


void CPageRecorder::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	char szTmp[32];
	CAVSrcInf *pStrm = m_pCfg->mpCrntVcamStream;
	CSliderCtrl*pSld = (CSliderCtrl*)pScrollBar;
	if(*pSld == m_SliderAudBitrate) {
		int nAudBitrate = m_SliderAudBitrate.GetPos() / 1000 * 1000;
		itoa(nAudBitrate, szTmp, 10);
		if(pStrm) {
			pStrm->mAUserBitrate = nAudBitrate;
		}
		SetDlgItemText(IDC_EDIT_AUD_BITRATE, szTmp);
		SetModified(1);
	} else if(*pSld == m_SliderVidBitrate) {
		int nVidBitrate = m_SliderVidBitrate.GetPos() / 10000 * 10000;
		itoa(nVidBitrate, szTmp, 10);
		if(pStrm) {
			pStrm->mVUserBitrate = nVidBitrate;
		}
		SetDlgItemText(IDC_EDIT_VID_BITRATE, szTmp);
		SetModified(1);
	} else  if(*pSld == m_SliderGoplen) {
		int nGoplen = m_SliderGoplen.GetPos();
		itoa(nGoplen, szTmp, 10);
		SetDlgItemText(IDC_EDIT_GOP_LENGTH, szTmp);
		if(pStrm) {
			pStrm->mUserGopLen = nGoplen;
		}
		SetModified(1);
	}

	CMFCPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CPageRecorder::OnCbnSelchangeComboVideoInput()
{
	int nId = SendDlgItemMessage(IDC_COMBO_VIDEO_INPUT, CB_GETCURSEL, 0, NULL); 
	if(nId != CB_ERR) {
		m_pCfg->mpCrntVcamStream = m_pCfg->mOnyxOutputStreams[nId];
		UpdateBitrateRanges();
	}
}


void CPageRecorder::OnBnClickedButtonSetDefaults()
{
	CAVSrcInf *pStrm = m_pCfg->mpCrntVcamStream;
	if(pStrm) {
		pStrm->mVUserBitrate =  pStrm->mVFairBitrate;
		pStrm->mAUserBitrate =  64000;
		pStrm->mUserGopLen = 60;
	}
	UpdateBitrateRanges();
}


void CPageRecorder::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == STATUS_TIMER_ID)  {

		if (mCapState == CAPTURE_RUN) {
			if(m_pCap) {
				int nFramesEncoded = 0;
				int nTimeMs = 0;
				m_pCap->GetGraphTime(&nTimeMs);
				m_pCap->GetVidEncStats(&nFramesEncoded);

				CString Status;
				int nSecs = nTimeMs / 1000;
				int nHrs = nSecs / 3600;
				int nMins = (nSecs % 3600) / 60;
				nSecs = nSecs % 60;
				Status.Format("\nStreaming...\r\nFrames Encoded: %d\r\nTime:%02d:%02d:%02d",nFramesEncoded, nHrs,  nMins, nSecs);
				SetDlgItemText(IDC_EDIT_RECORD_STATUS,Status);

			}
			SetTimer(STATUS_TIMER_ID, m_StatusUpdateInterval,NULL);
		}
	}

	CMFCPropertyPage::OnTimer(nIDEvent);
}
