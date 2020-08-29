// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
//#include "OnyxLiveCast.h"
#include "OvsControl.h"
#include "PageLiveCast.h"
#include "EditRtsp.h"
#include "EditRtpRec.h"
#include "EditUdpOut.h"
#include "RtmpEdit.h"
#include "RtspServerConfig.h"
#include "HlsOutConfigure.h"
#include "HlsPublishS3.h"
#include "DshowCapGraph.h"

#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "ConfigComn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define STATUS_TIMER_ID		100

typedef enum _CAPTURE_STATE_T
{
	CAPTURE_UNINIT,
	CAPTURE_STOP,
	CAPTURE_PAUSE,
	CAPTURE_RUN
} CAPTURE_STATE_T;

char* convertWStringToCString(WCHAR* wString)
{
    const int MAX_STRINGZ=500;
    static char ctext[MAX_STRINGZ+2];

    if (WideCharToMultiByte(CP_ACP, 0, wString, -1, ctext, MAX_STRINGZ,0,0) == 0)  {
        throw("convertWStringToCString failed with no extra error info");
    }
    return ctext;
}

#define KEY_NAME_WIDTH          "Width"
#define KEY_NAME_HEIGHT         "Height"
#define KEY_NAME_STREAM         "Video"
// CPageInputVideos message handlers

int CPageLivecast::CreatePropPages(CPageLivecast *pPages[], int nMaxPages)
{

	CString ConfigFile;
	GetConfigFoderPath(ConfigFile);
	ConfigFile += ONYX_CONFIG_FILE;
	int nStreams = 0;
	for (int i = 0; i < nMaxPages; i++)	{
		CPageLivecast *pPageRecorder = new CPageLivecast;

		CString Section;
		Section.Format("Output%d", i+1);

		int res = GetOnyxOutStreamInf(ConfigFile, Section, pPageRecorder->mAVSrcInf);
		if(res == 0) {

			CString Caption;
			Caption.Format("Stream_%dX%d", pPageRecorder->mAVSrcInf.mWidth, pPageRecorder->mAVSrcInf.mHeight);
			pPageRecorder->mAVSrcInf.mLabel = Caption;
			pPageRecorder->m_psp.dwFlags |= PSP_USETITLE;
			pPageRecorder->mStreamName = Caption;
			pPageRecorder->m_psp.pszTitle = pPageRecorder->mStreamName.c_str() ;

			pPages[nStreams] = (pPageRecorder);
			nStreams++;
		} else {
			delete pPageRecorder;
		}
	}
	return nStreams;
}


/////////////////////////////////////////////////////////////////////////////
// CPageLivecast property page

IMPLEMENT_DYNCREATE(CPageLivecast, CMFCPropertyPage)

CPageLivecast::CPageLivecast() : CMFCPropertyPage(CPageLivecast::IDD)
, m_EditAudioInput(_T(""))
, m_FileName(_T(""))
, m_fModified(0)
, m_StatusUpdateInterval(1000)
, mStreamName(_T("Stream0"))
, m_pCfg(NULL)
, m_pCap(NULL)
{

}

CPageLivecast::~CPageLivecast()
{
	if(m_pCfg)
		delete m_pCfg;
	if(m_pCap)
		delete m_pCap;
}

void CPageLivecast::DoDataExchange(CDataExchange* pDX)
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
	DDX_Control(pDX, IDC_COMBOEX_DEST_FORMAT, m_ComboExDestForamt);
}

BEGIN_MESSAGE_MAP(CPageLivecast, CMFCPropertyPage)
	ON_WM_ERASEBKGND()
//	ON_BN_CLICKED(IDC_BUTTON_BROWSE_OUT, &CPageLivecast::OnBnClickedButtonBrowseOut)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTON_RECORD, &CPageLivecast::OnBnClickedButonRecord)
	ON_BN_CLICKED(IDC_BUTON_PAUSE, &CPageLivecast::OnBnClickedButonPause)
	ON_BN_CLICKED(IDC_BUTON_RESUME, &CPageLivecast::OnBnClickedButonResume)
	ON_BN_CLICKED(IDC_BUTON_STOP, &CPageLivecast::OnBnClickedButonStop)

	ON_WM_HSCROLL()
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_BUTTON_CONFIGURE, &CPageLivecast::OnBnClickedButtonConfigure)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_SET_DEFAULTS, &CPageLivecast::OnBnClickedButtonSetDefaults)
	ON_CBN_SELCHANGE(IDC_COMBOEX_DEST_FORMAT, &CPageLivecast::OnCbnSelchangeComboexDestFormat)
END_MESSAGE_MAP()

void CPageLivecast::ConfigBtn(CMFCButton &Btn, int nToolTipId)
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
// CPageLivecast message handlers

BOOL CPageLivecast::OnInitDialog()
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
	
	m_pCfg = new CConfigLiveCast(mStreamName.c_str(), mAVSrcInf);

	m_pCap = new CCaptureGraph;
	CCaptureGraph *pCap = m_pCap;

	CConfigLiveCast *pCfg = m_pCfg;

	CreateDestSelectionList();

	/* Fill Audio Input Source Combobox */
	pCap->GetDeviceList(CAPTURE_DEVICE_CAREGORY_AUDIO, mAudDevices);
	if(!mAudDevices.empty()){
		SendDlgItemMessage(IDC_COMBO_AUDIO_INPUT, CB_ADDSTRING, 0, (LPARAM)CSTR_NO_AUDIO); 
		for (int i=0; i < mAudDevices.size(); i++){
			SendDlgItemMessage(IDC_COMBO_AUDIO_INPUT, CB_ADDSTRING, 0, (LPARAM)mAudDevices[i].c_str()); 
		}
		m_EditAudioInput = pCfg->mCrntAudioDevice.c_str();
	}

	if(pCfg->mDestType == DEST_TYPE_RTPMP){
		m_FileName  = pCfg->mRtmpUrl.c_str() ; 
		if(pCfg->mRtmpUrlBackup.length()) {
			m_FileName += ",";
			m_FileName += pCfg->mRtmpUrlBackup.c_str();
		}
	}

	mCapState = CAPTURE_STOP;
	UpdateBitrateRanges();
	Updatebuttons();
	UpdateData(FALSE);

	return TRUE;
}


void CPageLivecast::SetComboExItem(CComboBoxEx *pCombo, int row, CString itemText)
{
	COMBOBOXEXITEM item;
	memset(&item, 0, sizeof(item));
	//item.mask = CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_TEXT;
	item.mask =  CBEIF_TEXT;

	item.iItem = row;
	item.iSelectedImage = item.iImage = row;
	//item.iIndent = m_lstIndents [i];
	item.pszText = (LPTSTR)(LPCTSTR) itemText;
	item.cchTextMax = itemText.GetLength();

	pCombo->InsertItem(&item);
}


void CPageLivecast::OnBnClickedButonRecord()
{
	UpdateData(TRUE);
	CCaptureGraph *pCap = m_pCap;
	CConfigLiveCast *pCfg = m_pCfg;
	
	std::string StreamSharedMemName = mAVSrcInf.mSharedMemName;

	pCfg->mCrntAudioDevice = m_EditAudioInput;
	pCfg->mCrntFileName = m_FileName;

	switch(pCfg->mDestType){

		case DEST_TYPE_HLS_SRV:
			pCfg->mCrntFileName = pCfg->mHlsUrl;
			break;

		case DEST_TYPE_HLS_S3:
			pCfg->mCrntFileName = pCfg->mHlsPublishS3AccessId + ":" + pCfg->mHlsPublishS3SecKey + "@" + pCfg->mHlsS3Url;
			break;

		case DEST_TYPE_RTSP_SRVR:
			pCfg->mCrntFileName = pCfg->mHRtspUrl;
			break;

		case DEST_TYPE_RTPMP:
			pCfg->mCrntFileName = pCfg->mRtmpUrl;
			if(pCfg->mRtmpUrlBackup.length()) {
				pCfg->mCrntFileName += ",";
				pCfg->mCrntFileName += pCfg->mRtmpUrlBackup;
			}
			break;

		case DEST_TYPE_RTP_REC:
			break;

		case DEST_TYPE_UDP_REC:
			break;

	}
	SetDlgItemText(IDC_EDIT_RECORD_STATUS, "Initializing...");
	if(pCap->Init(pCfg->mCrntFileName, pCfg->mCrntAudioDevice,
			pCfg->mCrntVideoDevice, StreamSharedMemName, 
			pCfg->mDestType, mAVSrcInf.mWidth, mAVSrcInf.mHeight, false) == 0) {
		pCap->SetVidEncProperties(mAVSrcInf.mVUserBitrate, mAVSrcInf.mUserGopLen, 0);
		pCap->SetRtmpParams(pCfg->mRecordServer1, pCfg->mRecordServer2);

		switch(pCfg->mDestType){
			case DEST_TYPE_HLS_SRV:
				{
					int nDuration = atoi(pCfg->mHlsSrvSegmentDuration.c_str());
					int fLiveOnly = atoi(pCfg->mHlsSrvLiveOnly.c_str());
					std::string m3u8file = pCfg->mHlsSrvStream + ".m3u8";
					pCap->SetHlsSrvFilterProperties(0, fLiveOnly, nDuration * 1000, pCfg->mDestType, 
						m3u8file.c_str(),
						pCfg->mHlsSrvFolder.c_str(),
						pCfg->mHlsSrvRoot.c_str(),
						NULL,
						NULL,
						NULL
						);
				}
				break;

			case DEST_TYPE_HLS_S3:
				{
					int nDuration = atoi(pCfg->mHlsPublishSegmentDuration.c_str());
					int fLiveOnly = atoi(pCfg->mHlsPublishLiveOnly.c_str());
					std::string m3u8file = pCfg->mHlsPublishStream + ".m3u8";
					pCap->SetHlsSrvFilterProperties(0, fLiveOnly, nDuration * 1000, pCfg->mDestType, 
						m3u8file.c_str(),
						pCfg->mHlsPublishFolder.c_str(),
						pCfg->mHlsPublishS3Bucket.c_str(),
						pCfg->mHlsPublishS3Host.c_str(),
						pCfg->mHlsPublishS3AccessId.c_str(),
						pCfg->mHlsPublishS3SecKey.c_str()
						);
				}
				break;
		}

		pCap->start();
		mCapState = CAPTURE_RUN;
		Updatebuttons();
	} else {
		SetDlgItemText(IDC_EDIT_RECORD_STATUS, "Failed to start recording.");
		//MessageBox(ghWnd, TEXT("Could not run capture!"), TEXT("OvsCapture Error"), IDOK);
	}
}

void CPageLivecast::OnBnClickedButonPause()
{
	CConfigLiveCast *pCfg = m_pCfg;
	CCaptureGraph *pCap = m_pCap;
	pCap->pause();
	mCapState = CAPTURE_PAUSE;
	Updatebuttons();
}

void CPageLivecast::OnBnClickedButonResume()
{
	CConfigLiveCast *pCfg = m_pCfg;
	CCaptureGraph *pCap = m_pCap;
	pCap->start();
	mCapState = CAPTURE_RUN;
	Updatebuttons();
}

void CPageLivecast::OnBnClickedButonStop()
{
	CConfigLiveCast *pCfg = m_pCfg;
	CCaptureGraph *pCap = m_pCap;
	pCap->stop();
	pCap->Deinit();
	mCapState = CAPTURE_STOP;
	Updatebuttons();
}

void CPageLivecast::Updatebuttons()
{
	if (  mCapState == CAPTURE_STOP) {
		GetDlgItem(IDC_BUTON_RECORD)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTON_STOP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTON_PAUSE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTON_RESUME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTON_PAUSE)->EnableWindow(FALSE);
		SetDlgItemText(IDC_EDIT_RECORD_STATUS, "Stop.");
		GetDlgItem(IDC_SLIDER_AUD_BITRATE)->EnableWindow(TRUE);
		GetDlgItem(IDC_SLIDER_VID_BITRATE)->EnableWindow(TRUE);
		GetDlgItem(IDC_SLIDER_GOPLENGTH)->EnableWindow(TRUE);
	} else  if (mCapState == CAPTURE_RUN) {
		GetDlgItem(IDC_BUTON_RECORD)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTON_STOP)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTON_PAUSE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTON_PAUSE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTON_RESUME)->ShowWindow(SW_HIDE);
		SetDlgItemText(IDC_EDIT_RECORD_STATUS, "Initializing...");

		GetDlgItem(IDC_SLIDER_AUD_BITRATE)->EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER_VID_BITRATE)->EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER_GOPLENGTH)->EnableWindow(FALSE);

		if(m_StatusUpdateInterval > 0) {
			SetTimer(STATUS_TIMER_ID, m_StatusUpdateInterval,NULL);
	}

	} else  if (mCapState == CAPTURE_PAUSE) {
		GetDlgItem(IDC_BUTON_RECORD)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTON_STOP)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTON_PAUSE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTON_RESUME)->ShowWindow(SW_SHOW);
		SetDlgItemText(IDC_EDIT_RECORD_STATUS, "Streaming paused.");
	}
}

void CPageLivecast::SaveConfig()
{
	UpdateData(TRUE);

	CCaptureGraph *pCap = m_pCap;
	CConfigLiveCast *pCfg = m_pCfg;

	pCfg->mCrntAudioDevice = m_EditAudioInput;

	pCfg->Save(mAVSrcInf);
}
BOOL CPageLivecast::OnApply()
{
	SaveConfig();
	m_fModified = false;
	return CMFCPropertyPage::OnApply();
}

BOOL CPageLivecast::OnEraseBkgnd(CDC* pDC)
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


HBRUSH CPageLivecast::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void CPageLivecast::OnDestroy()
{
	CMFCPropertyPage::OnDestroy();

	delete m_pEditBkBrush;
	delete m_pStaticBkBrush;
}


BOOL CPageLivecast::OnHelpInfo(HELPINFO* pHelpInfo)
{
	// TODO: Add your message handler code here and/or call default

	return CMFCPropertyPage::OnHelpInfo(pHelpInfo);
}

BOOL CPageLivecast::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR	*lpnm = (NMHDR *) lParam;
	if(lpnm->code == PSN_HELP) {
		CString cstrHelpFile = "OnyxVirtualStudio.chm";
		::HtmlHelp(this->m_hWnd, cstrHelpFile, HH_HELP_FINDER, 0);
		return TRUE;
	}

	return CMFCPropertyPage::OnNotify(wParam, lParam, pResult);
}

void CPageLivecast::UpdateEncodeParams()
{

}

void CPageLivecast::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	char szTmp[32];
	CSliderCtrl*pSld = (CSliderCtrl*)pScrollBar;
	if(*pSld == m_SliderAudBitrate) {
		int nAudBitrate = m_SliderAudBitrate.GetPos() / 1000 * 1000;
		itoa(nAudBitrate, szTmp, 10);
		mAVSrcInf.mAUserBitrate = nAudBitrate;
		SetDlgItemText(IDC_EDIT_AUD_BITRATE, szTmp);
		SetModified(1);
	} else if(*pSld == m_SliderVidBitrate) {
		int nVidBitrate = m_SliderVidBitrate.GetPos() / 10000 * 10000;
		itoa(nVidBitrate, szTmp, 10);
		mAVSrcInf.mVUserBitrate = nVidBitrate;
		SetDlgItemText(IDC_EDIT_VID_BITRATE, szTmp);
		SetModified(1);
	} else  if(*pSld == m_SliderGoplen) {
		int nGoplen = m_SliderGoplen.GetPos();
		itoa(nGoplen, szTmp, 10);
		SetDlgItemText(IDC_EDIT_GOP_LENGTH, szTmp);
		mAVSrcInf.mUserGopLen = nGoplen;
		SetModified(1);
	}

	UpdateEncodeParams();
	CMFCPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPageLivecast::OnBnClickedButtonConfigure()
{
	SelectDestinationFormat(DEST_TYPE_RTPMP);
}

void CPageLivecast::SelectDestinationFormat(int nSel)
{
	CConfigLiveCast *pCfg = m_pCfg;
	switch(nSel) {
		case DEST_TYPE_RTSP_SRVR:
		{
			CRtspServerConfig dlg;
			dlg.m_StreamName = pCfg->mRtspResource.c_str();
			dlg.m_Port = atoi(pCfg->mRtspIpPort.c_str());
			if(dlg.DoModal() == IDOK){
				pCfg->mHRtspUrl = dlg.m_StreamName;
				pCfg->mRtspIpPort = dlg.m_Port;
				pCfg->mHRtspUrl = "rtsp://" + pCfg->mRtspIpAddr + ":" + pCfg->mRtspIpPort + "/" + pCfg->mRtspResource; 
				SetDlgItemText(IDC_EDIT_FILE_NAME, pCfg->mHRtspUrl.c_str());
				m_fModified = true;
				SetModified(1);
				pCfg->mDestType = nSel;
			}
		}
		break;

		case DEST_TYPE_RTPMP:
		{
			CRtmpEdit dlg;
			dlg.m_EditRtmpAddr = pCfg->mRtmpUrl.c_str();
			dlg.m_EditRtmpAddrBackup = pCfg->mRtmpUrlBackup.c_str();
			dlg.m_fRecordPrimaryServer = pCfg->mRecordServer1;
			dlg.m_fRecordSecondServer = pCfg->mRecordServer2;
			if(dlg.DoModal() == IDOK){
				pCfg->mRtmpUrl = dlg.m_EditRtmpAddr;
				pCfg->mRtmpUrlBackup = dlg.m_EditRtmpAddrBackup;
				pCfg->mRecordServer1 = dlg.m_fRecordPrimaryServer;
				pCfg->mRecordServer2 = dlg.m_fRecordSecondServer;

				std::string tmp = pCfg->mRtmpUrl;
				if(pCfg->mRtmpUrlBackup.length()) {
					tmp += ",";
					tmp += pCfg->mRtmpUrlBackup;
				}
				SetDlgItemText(IDC_EDIT_FILE_NAME, tmp.c_str());
				m_fModified = true;
				SetModified(1);
				pCfg->mDestType = nSel;
			}
		}
		break;


		case DEST_TYPE_RTP_REC:
		{
			CEditRtpRec dlg;
			dlg.m_HostAddr = pCfg->mRtpRecHost.c_str();
			dlg.m_RemotePort = pCfg->mRtpRecRemotePort;
			dlg.m_UserId = pCfg->mRtpRecUserId.c_str();
			dlg.m_Passwd = pCfg->mRtpRecPasswd.c_str();
			dlg.m_StreamName = pCfg->mRtpRecStream.c_str();

			if(dlg.DoModal() == IDOK){
				pCfg->mRtpRecHost = dlg.m_HostAddr;;
				pCfg->mRtpRecRemotePort = dlg.m_RemotePort;
				pCfg->mRtpRecUserId = dlg.m_UserId;
				pCfg->mRtpRecPasswd = dlg.m_Passwd;
				pCfg->mRtpRecStream = dlg.m_StreamName;

				char RtpRecDest[1024] = {0};
				sprintf(RtpRecDest, "rtsp://%s:%s@%s:%d/%s", pCfg->mRtpRecUserId.c_str(), pCfg->mRtpRecPasswd.c_str(), pCfg->mRtpRecHost.c_str(), dlg.m_RemotePort, pCfg->mRtpRecStream.c_str());
				
				SetDlgItemText(IDC_EDIT_FILE_NAME, RtpRecDest);
				m_fModified = true;
				SetModified(1);
				pCfg->mDestType = nSel;
			}
		}
		break;

		case DEST_TYPE_UDP_REC:
		{
			CEditUdpOut dlg;
			dlg.m_HostAddr = pCfg->mRtpRecHost.c_str();
			dlg.m_RemotePort = pCfg->mRtpRecRemotePort;

			if(dlg.DoModal() == IDOK){
				pCfg->mRtpRecHost = dlg.m_HostAddr;;
				pCfg->mRtpRecRemotePort = dlg.m_RemotePort;

				char Dest[1024] = {0};
				sprintf(Dest, "udp://%s:%d", pCfg->mRtpRecHost.c_str(), dlg.m_RemotePort);
				
				SetDlgItemText(IDC_EDIT_FILE_NAME, Dest);
				m_fModified = true;
				SetModified(1);
				pCfg->mDestType = nSel;
			}
		}
		break;
		case DEST_TYPE_HLS_SRV:
		{
			CHlsOutConfigure dlg;
			dlg.m_StreamName = pCfg->mHlsSrvStream.c_str();
			dlg.m_Port = atoi(pCfg->mHlsSrvPort.c_str());
			dlg.m_szFolder = pCfg->mHlsSrvFolder.c_str();
			dlg.m_szSrvRoot = pCfg->mHlsSrvRoot.c_str();
			dlg.m_SegmentDuration = atoi(pCfg->mHlsSrvSegmentDuration.c_str());
			dlg.m_fLiveOnly = atoi(pCfg->mHlsSrvLiveOnly.c_str());

			if(dlg.DoModal() == IDOK){
				char szValue[32];
				pCfg->mHlsSrvStream = dlg.m_StreamName;
				pCfg->mHlsSrvFolder = dlg.m_szFolder;
				pCfg->mHlsSrvRoot = dlg.m_szSrvRoot;
				itoa(dlg.m_SegmentDuration, szValue, 10);
				pCfg->mHlsSrvSegmentDuration = szValue;
				itoa(dlg.m_fLiveOnly, szValue, 10);
				pCfg->mHlsSrvLiveOnly = szValue;


				itoa(dlg.m_Port, szValue, 10);
				pCfg->mHlsSrvPort = szValue;

				char Dest[1024] = {0};
				pCfg->mHlsUrl = "http://" + pCfg->mHlsSrvAddr + ":" + pCfg->mHlsSrvPort + "/" + pCfg->mHlsSrvFolder + "/" + pCfg->mHlsSrvStream + ".m3u8";
				sprintf(Dest, "%s", pCfg->mHlsUrl.c_str());
				
				SetDlgItemText(IDC_EDIT_FILE_NAME, Dest);
				m_fModified = true;
				SetModified(1);
				pCfg->mDestType = nSel;
			}
		}
		break;
		case DEST_TYPE_HLS_S3:
		{
			CHlsPublishS3 dlg;
			dlg.m_szStream = pCfg->mHlsPublishStream.c_str();
			dlg.m_szHost = pCfg->mHlsPublishS3Host.c_str();
			dlg.m_szBucket = pCfg->mHlsPublishS3Bucket.c_str();
			dlg.m_szFolder = pCfg->mHlsPublishFolder.c_str();
			dlg.m_szAccessId = pCfg->mHlsPublishS3AccessId.c_str();
			dlg.m_szSecurityKey = pCfg->mHlsPublishS3SecKey.c_str();
			dlg.m_SegmentDuration = pCfg->mHlsPublishSegmentDuration.c_str();
			dlg.m_fLiveOnly = atoi(pCfg->mHlsPublishLiveOnly.c_str());

			if(dlg.DoModal() == IDOK){
				pCfg->mHlsPublishStream = dlg.m_szStream;
				pCfg->mHlsPublishS3Host = dlg.m_szHost;
				pCfg->mHlsPublishS3Bucket = dlg.m_szBucket;
				pCfg->mHlsPublishFolder = dlg.m_szFolder;
				pCfg->mHlsPublishS3AccessId = dlg.m_szAccessId;
				pCfg->mHlsPublishS3SecKey = dlg.m_szSecurityKey;
				pCfg->mHlsPublishSegmentDuration = dlg.m_SegmentDuration;
				if(dlg.m_fLiveOnly)
					pCfg->mHlsPublishLiveOnly = "1";
				else
					pCfg->mHlsPublishLiveOnly = "0";
				pCfg->mHlsS3Url = pCfg->mHlsPublishS3Bucket + "." + pCfg->mHlsPublishS3Host + "/" + pCfg->mHlsPublishFolder + "/" + pCfg->mHlsPublishStream + ".m3u8";

				char Dest[1024] = {0};
				sprintf(Dest, "http://%s", pCfg->mHlsS3Url.c_str());

				SetDlgItemText(IDC_EDIT_FILE_NAME, Dest);
				m_fModified = true;
				SetModified(1);
				pCfg->mDestType = nSel;

			}
		}
		break;
	}
}


void CPageLivecast::OnTimer(UINT_PTR nIDEvent)
{
	CConfigLiveCast *pCfg = m_pCfg;
	if (nIDEvent == STATUS_TIMER_ID)  {

		if (mCapState == CAPTURE_RUN) {
			if(m_pCap) {
				int nFramesEncoded = 0;
				int nTimeMs = 0;
				CString Status;
				int nSecs = 0;
				int nHrs = 0;
				int nMins = 0;
				m_pCap->GetGraphTime(&nTimeMs);
				m_pCap->GetVidEncStats(&nFramesEncoded);
				nSecs = nTimeMs / 1000;
				nHrs = nSecs / 3600;
				nMins = (nSecs % 3600) / 60;
				nSecs = nSecs % 60;
				Status.Format("\nTime:%02d:%02d:%02d\r\nFrames Encoded: %d",nHrs,  nMins, nSecs,nFramesEncoded);
				if(m_pCfg->mDestType == DEST_TYPE_RTPMP) {
					int nVFramesSent = 0;
					int nAFramesSent = 0;
					int nServer1 = 0;
					int nServer2 = 0;
					CString RtmpStatus;
					m_pCap->GetRtmpStats(&nVFramesSent, &nAFramesSent, &nServer1, &nServer2);
					RtmpStatus.Format("\r\nServer1:%s\r\nServer2:%s", nServer1 ? "Connected" : "Not Connected", nServer2 ? "Connected" : "Not Connected");
					Status += RtmpStatus;

					CString FramesSent;
					FramesSent.Format("\r\nFrames Sent Aud:%d vid:%d", nAFramesSent, nVFramesSent);
					Status += FramesSent;
				}

				SetDlgItemText(IDC_EDIT_RECORD_STATUS,Status);

			}
			SetTimer(STATUS_TIMER_ID, m_StatusUpdateInterval,NULL);
		}
   }

	CMFCPropertyPage::OnTimer(nIDEvent);
}

void CPageLivecast::UpdateBitrateRanges()
{
	CAVSrcInf *pStrm = &mAVSrcInf;
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

void CPageLivecast::CreateDestSelectionList()
{
	int i = 0;
	CVidDestFmtInf *pDestFmt = new CVidDestFmtInf("RTMP", DEST_TYPE_RTPMP);
	mDestFormats.push_back(pDestFmt);

	pDestFmt = new CVidDestFmtInf("RTSP", DEST_TYPE_RTSP_SRVR);
	mDestFormats.push_back(pDestFmt);

	pDestFmt = new CVidDestFmtInf("RTP", DEST_TYPE_RTP_REC);
	mDestFormats.push_back(pDestFmt);

	pDestFmt = new CVidDestFmtInf("UDP", DEST_TYPE_UDP_REC);
	mDestFormats.push_back(pDestFmt);

	pDestFmt = new CVidDestFmtInf("HLS SRV", DEST_TYPE_HLS_SRV);
	mDestFormats.push_back(pDestFmt);

	pDestFmt = new CVidDestFmtInf("HLS S3", DEST_TYPE_HLS_S3);
	mDestFormats.push_back(pDestFmt);

	for(int i=0; i < mDestFormats.size(); i++) {
		CVidDestFmtInf *pDestFmt = mDestFormats[i];
		SetComboExItem(&m_ComboExDestForamt, i, pDestFmt->mLabel.c_str());
		m_ComboExDestForamt.SetItemData(i, pDestFmt->mFmt);
		if(mCrntDestFmt == pDestFmt->mFmt)
			m_ComboExDestForamt.SetCurSel(i); 
	}

	m_ComboExDestForamt.ShowWindow(SW_SHOW);
}

void CPageLivecast::OnBnClickedButtonSetDefaults()
{
	CAVSrcInf *pStrm = &mAVSrcInf;
	if(pStrm) {
		pStrm->mVUserBitrate =  pStrm->mVFairBitrate;
		pStrm->mAUserBitrate =  64000;
		pStrm->mUserGopLen = 60;
	}
	UpdateBitrateRanges();
}


void CPageLivecast::OnCbnSelchangeComboexDestFormat()
{
	int nSel = m_ComboExDestForamt.GetCurSel();
	int nDestFmt = m_ComboExDestForamt.GetItemData(nSel); 
	mCrntDestFmt = nDestFmt;

	SelectDestinationFormat(mCrntDestFmt);
}
