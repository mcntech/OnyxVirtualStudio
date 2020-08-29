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
#include "PageSkype.h"
#include "SkypeConfig.h"
#include <windows.h>

#include <gdiplus.h>
using namespace Gdiplus;

#define COLUMN_IMG			0
#define COLUMN_ID			1
#define COLUMN_DISP_NAME	2
#define COLUMN_ONLINE		3

#define SKYPE_IMG_W			16
#define SKYPE_IMG_H			16
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

 __declspec(dllimport) CSkypeIf* WINAPI CreateSkypeIfInstance(); 

CSkypeInf* CSkypeInf::mInstance = NULL;
CSkypeInf *CSkypeInf::Instance()
{
  if (!mInstance)
      mInstance = new CSkypeInf();
  return mInstance;
}


BEGIN_MESSAGE_MAP(CStatusLed, CStatic)
   ON_WM_PAINT()
   ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CStatusLed, CStatic)

void CStatusLed::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect Rect;
	GetClientRect(&Rect);

	CDC MemDC;
	MemDC.CreateCompatibleDC(&dc);
	CBitmap Bmp;
	if(m_fState)
		Bmp.LoadBitmapA(IDB_BITMAP_LED_ON);
	else 
		Bmp.LoadBitmapA(IDB_BITMAP_LED_OFF);
	int SavedDC = MemDC.SaveDC();
	MemDC.SelectObject(&Bmp);

   dc.BitBlt(0,0,Rect.Width(),Rect.Height(),&MemDC,0,0,SRCCOPY);
   MemDC.RestoreDC(SavedDC);
}

void CStatusLed::SetState(bool fOn) 
{
	m_fState = fOn;
	InvalidateRect(NULL);
}
/////////////////////////////////////////////////////////////////////////////
// CPageSkype property page

IMPLEMENT_DYNCREATE(CPageSkype, CMFCPropertyPage)

CPageSkype::CPageSkype() : CMFCPropertyPage(CPageSkype::IDD)
, m_SkypeId(_T(""))
, m_Passwd(_T(""))
, m_LoginOk(false)
, m_SkypeStatus(_T(""))
, m_fRingingForMe(false)
, m_fCalling(false)
, m_strAutoReply(_T(""))
, mOnyxShareMemName(_T("SkypeVideo#1"))
{
	
}

CPageSkype::~CPageSkype()
{
}

void CPageSkype::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SKYPE_ID, m_SkypeId);
	DDX_Text(pDX, IDC_EDIT_PASSWD, m_Passwd);
	DDX_Text(pDX, IDC_EDIT_SKYPE_STATUS, m_SkypeStatus);
	DDX_Control(pDX, IDC_LIST_SKYPE_CONTACTS, m_SkypeContacts);
	DDX_Control(pDX, IDC_AUTO_REPLY, m_AutoReply);
	DDX_Text(pDX, IDC_EDIT_AUTO_REPLY, m_strAutoReply);
	DDX_Control(pDX, IDC_STATIC_VIDEO_IN, m_VideoInStatus);
	DDX_Control(pDX, IDC_STATIC_VIDEO_OUT, m_VideoOutStatus);
	DDX_Control(pDX, IDC_BUTTON_SKYPE_CALL, m_BtnAccept);
	DDX_Control(pDX, IDC_BUTTON_SKYPE_HANGUP, m_BtnReject);
	DDX_Control(pDX, IDC_BUTTON_SKYPE_CONFIG, m_BtnSkypeConfig);
	DDX_Control(pDX, IDC_BUTTON_LOGIN, m_BtnLogin);
	DDX_Control(pDX, IDC_BUTTON_LOGOUT, m_BtnLogout);
}

BEGIN_MESSAGE_MAP(CPageSkype, CMFCPropertyPage)

	ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CPageSkype::OnBnClickedButtonLogin)
	ON_BN_CLICKED(IDC_BUTTON_LOGOUT, &CPageSkype::OnBnClickedButtonLogout)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_SKYPE_CALL, &CPageSkype::OnBnClickedButtonSkypeCall)
	ON_BN_CLICKED(IDC_BUTTON_SKYPE_HANGUP, &CPageSkype::OnBnClickedButtonSkypeHangup)
	ON_BN_CLICKED(IDC_AUTO_REPLY, &CPageSkype::OnBnClickedAutoReply)
	ON_BN_CLICKED(IDC_BUTTON_ADD_AUTOREPLY, &CPageSkype::OnBnClickedButtonAddAutoreply)
	ON_BN_CLICKED(IDC_BUTTON_DEL_AUTOREPLY, &CPageSkype::OnBnClickedButtonDelAutoreply)
//	ON_NOTIFY(HDN_ITEMDBLCLICK, 0, &CPageSkype::OnHdnItemdblclickListSkypeAutoreply)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SKYPE_CONTACTS, &CPageSkype::OnNMDblclkListSkypeAutoreply)
//	ON_EN_CHANGE(IDC_EDIT_AUTO_REPLY, &CPageSkype::OnEnChangeEditAutoReply)
ON_EN_KILLFOCUS(IDC_EDIT_AUTO_REPLY, &CPageSkype::OnEnKillfocusEditAutoReply)
ON_EN_CHANGE(IDC_EDIT_AUTO_REPLY, &CPageSkype::OnEnChangeEditAutoReply)
ON_BN_CLICKED(IDC_BUTTON_SKYPE_CONFIG, &CPageSkype::OnBnClickedButtonSkypeConfig)
ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageSkype message handlers

static int dialogIdList[] = 
{
	IDC_BUTTON_SKYPE_CALL,
	IDC_BUTTON_LOGOUT,
	IDC_BUTTON_SKYPE_HANGUP,
	IDC_AUTO_REPLY,
	IDC_LIST_SKYPE_CONTACTS, 
	IDC_EDIT_AUTO_REPLY,
	IDC_EDIT_CONTACT,
	IDC_STATIC_CONTACTLIST,
	IDC_STATIC_VIDEO_OUT,
	IDC_STATIC_VIDEO_IN,
	IDC_STATIC_LABLE_VID_IN,
	IDC_STATIC_LABLE_VID_OUT,
	IDC_STATIC_LABEL_AUTO_REPLY,
	IDC_STATIC_LABLE_CONTACT,
	IDC_BUTTON_SKYPE_CONFIG
};

BOOL CPageSkype::HideDialogDeatils()
{
	int nCount = sizeof(dialogIdList) / sizeof(int);
	for (int i=0; i < nCount ; i++){
		GetDlgItem(dialogIdList[i])->ShowWindow(SW_HIDE);
	}
	return TRUE;
}

BOOL CPageSkype::HideLoginDeatils()
{
	GetDlgItem(IDC_EDIT_SKYPE_ID)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_PASSWD)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_ACCOUNT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_PASSWD)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON_LOGIN)->ShowWindow(SW_HIDE);

	return TRUE;
}

BOOL CPageSkype::ShowDialogDetails()
{
	int nCount = sizeof(dialogIdList) / sizeof(int);
	for (int i=0; i < nCount ; i++){
		GetDlgItem(dialogIdList[i])->ShowWindow(SW_SHOW);
	}
	return TRUE;
}

BOOL CPageSkype::ShowLoginDetails()
{
	GetDlgItem(IDC_EDIT_SKYPE_ID)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT_PASSWD)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_ACCOUNT)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_PASSWD)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_BUTTON_LOGIN)->ShowWindow(SW_SHOW);

	return TRUE;
}

void CPageSkype::ConfigBtn(CMFCButton &Btn, int nToolTipId)
{
	CString Tooltip;
	Btn.SetFaceColor(PANEL_BCKGND,  TRUE );
	Btn.m_nFlatStyle = CMFCButton::BUTTONSTYLE_NOBORDERS;
	Btn.m_bDrawFocus = FALSE;
	Btn.m_nAlignStyle = CMFCButton::ALIGN_LEFT;
	Btn.SizeToContent();

	if(Tooltip.LoadStringA(nToolTipId)) {
		Btn.SetTooltip(Tooltip);
	}
}

BOOL CPageSkype::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();
	m_Background.LoadBitmap(IDB_BITMAP_WELCOME); //Load bitmap
	m_pEditBkBrush = new CBrush(RGB(0, 0, 0x50));
	
	HideDialogDeatils();

	m_pSkypeIf = CreateSkypeIfInstance();
	if(m_pSkypeIf->Start() == 0) {
		m_pSkypeIf->SetCallback(this);
		m_pSkypeIf->SetConfig(mOnyxShareMemName);
	} else {
		m_SkypeStatus = "Failed to start Skype runtime";
	}

	m_SkypeContacts.InsertColumn(COLUMN_IMG,       _T(" "),           LVCFMT_CENTER, 20);
	m_SkypeContacts.InsertColumn(COLUMN_ID,        _T("Skype Id"),    LVCFMT_LEFT, 80);
	m_SkypeContacts.InsertColumn(COLUMN_DISP_NAME, _T("Name"),        LVCFMT_LEFT,160);
	//m_SkypeContacts.InsertColumn(COLUMN_ONLINE,  _T("Online"),      LVCFMT_LEFT, 50);

	m_SkypeContacts.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES);
	m_ImgListSkype.Create(SKYPE_IMG_W, SKYPE_IMG_H, ILC_COLOR24/*ILC_MASK*/, 16, 16);  
    m_SkypeContacts.SetImageList(&m_ImgListSkype, LVSIL_SMALL); 

	m_AutoReply.SetImage(IDB_BITMAP_GEN_DISABLEU, IDB_BITMAP_GEN_DISABLED);
	m_AutoReply.SetCheckedImage(IDB_BITMAP_GEN_ENABLEU, IDB_BITMAP_GEN_ENABLED);
	ConfigBtn(m_AutoReply, IDS_TTIP_AUTO_REPLY);

	m_BtnAccept.SetImage(IDB_BITMAP_PHONEACCEPTU, IDB_BITMAP_PHONEACCEPTD);
	ConfigBtn(m_BtnAccept, IDS_TTIP_SKYPE_CALL);

	m_BtnReject.SetImage(IDB_BITMAP_PHONEREJECTU, IDB_BITMAP_PHONEREJECTD);
	ConfigBtn(m_BtnReject, IDS_TTIP_SKYPE_HANGUP);

	m_BtnLogin.SetImage(IDB_BITMAP_LOGINU, IDB_BITMAP_LOGIND);
	ConfigBtn(m_BtnLogin, IDS_TTIP_LOGIN);

	m_BtnLogout.SetImage(IDB_BITMAP_LOGOUTU, IDB_BITMAP_LOGOUTD);
	ConfigBtn(m_BtnLogout, IDS_TTIP_LOGOUT);

	m_BtnSkypeConfig.SetImage(IDB_BITMAP_AUDCFGU, IDB_BITMAP_AUDCFGD);
	ConfigBtn(m_BtnSkypeConfig, IDS_TTIP_SKYPE_CONFIG);
	
	ResetVideoStatus();

	UpdateData(FALSE);
	SetDefID(IDC_BUTTON_LOGIN);
	return TRUE;  // return TRUE unless you set the focus to a control

}

void CPageSkype::OnBnClickedButtonLogin()
{
	UpdateData(TRUE);
	m_LoginOk = true; //TODO

	CSkypeInf *Instance = CSkypeInf::Instance();
	Instance->m_Passwd = m_Passwd;
	Instance->m_SkypeId = m_SkypeId;

	if(m_pSkypeIf->Login(m_SkypeId, m_Passwd) == 0) {
	} else {
		m_SkypeStatus = "Failed to Login. Please check password or id";
	}
	UpdateData(0);
}

void CPageSkype::OnBnClickedButtonLogout()
{
	CSkypeInf *Instance = CSkypeInf::Instance();
	m_pSkypeIf->Logout();
	Instance->m_LoginOk = false;

}

BOOL CPageSkype::OnEraseBkgnd(CDC* pDC)
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

HBRUSH CPageSkype::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{

	if (CTLCOLOR_STATIC == nCtlColor)  {
		if(pWnd->GetDlgCtrlID() == IDC_EDIT_SKYPE_STATUS){
			pDC->SetTextColor(RGB(0, 255, 0));
			pDC->SetBkColor(RGB(0, 0, 0x80));
			return (HBRUSH)(m_pEditBkBrush->GetSafeHandle());
		} else {
			pDC->SetBkMode(TRANSPARENT);
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
	} else if ( nCtlColor == CTLCOLOR_EDIT ) {
		pDC->SetTextColor(RGB(0, 255, 0));
		pDC->SetBkColor(RGB(0, 0, 0));
		return (HBRUSH)(m_pEditBkBrush->GetSafeHandle());
	}

	HBRUSH hbr = CMFCPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
		return hbr;
}

void CPageSkype::OnDestroy()
{
	CMFCPropertyPage::OnDestroy();
	m_pSkypeIf->Stop();
	delete m_pEditBkBrush;
}

int CPageSkype::LoginStatus(int StatusId, void *pInfo)
{
	switch(StatusId)
	{
		case SKYPE_STATUS_LOGGED_OUT:
			m_SkypeStatus = "Logged out.";
			m_SkypeContacts.DeleteAllItems();
			HideDialogDeatils();
			ShowLoginDetails();
			InvalidateRect(NULL, TRUE);
			break;
		case SKYPE_STATUS_LOGGED_OUT_AND_PWD_SAVED:
			m_SkypeStatus = "Logged out.";
			break;

		case SKYPE_STATUS_CONNECTING_TO_P2P:
			m_SkypeStatus = "Connecting...";
			break;

		case SKYPE_STATUS_CONNECTING_TO_SERVER:
			m_SkypeStatus = "Connecting to server...";
			break;

		case SKYPE_STATUS_LOGGING_IN:
			m_SkypeStatus = "Logging in...";
			break;

		case SKYPE_STATUS_INITIALIZING:
			m_SkypeStatus = "Initializing...";
			break;

		case SKYPE_STATUS_LOGGED_IN:
			{
				CSkypeInf *Instance = CSkypeInf::Instance();
				m_SkypeStatus = "Logged in";
				Instance->m_LoginOk = m_LoginOk;
				HideLoginDeatils();
				ShowDialogDetails();
				InvalidateRect(NULL, TRUE);
			}

			break;

		case SKYPE_STATUS_LOGGING_OUT:
			m_SkypeStatus = "Logging out...";
			break;
		default:
			m_SkypeStatus.Format("Status Id=%d", StatusId) ;
			break;
	}
	if(SKYPE_STATUS_LOGGED_OUT == StatusId) {
		CString logoutReason;
		SKYPE_LOGOUT_INF_T *pLogutInfo = (SKYPE_LOGOUT_INF_T *)pInfo;
		switch (pLogutInfo->whyDidWeLogout)
		{
			case SKYPE_LOGOUT_CALLED:
				break;
			case SKYPE_LOGOUT_GENERIC:
				logoutReason = "Generic failure";
				break;

			case SKYPE_INCORRECT_PASSWORD:
				logoutReason = "Incorrect password or id";
				break;
			case SKYPE_INCORRECT_ID:
				logoutReason = "Incorrect id";
				break;

		}
		if(logoutReason.GetLength()) {
			m_SkypeStatus = m_SkypeStatus + " : " + logoutReason;
		}
	}
	SetDlgItemText(IDC_EDIT_SKYPE_STATUS,m_SkypeStatus);
	//UpdateData(FALSE);
	return 0;
}

void CPageSkype::ResetVideoStatus()
{
	m_VideoInStatus.SetState(false);
	m_VideoOutStatus.SetState(false);
}

int CPageSkype::VideoStatus(int StatusId, void *pInfo)
{
	switch(StatusId)
	{
		case SKYPE_VIDEO_INCOMING_VIDEO_OPEN:
		{
			CBitmap btmap;
			btmap.LoadBitmapA(IDB_BITMAP_LED_ON);
			m_VideoInStatus.SetState(true);
		}
		break;
		case SKYPE_VIDEO_INCOMING_VIDEO_CLOSE:
		{
			CBitmap btmap;
			btmap.LoadBitmapA(IDB_BITMAP_LED_OFF);
			m_VideoInStatus.SetState(false);
		}
		break;
		case SKYPE_VIDEO_OUTGOING_VIDEO_OPEN:
		{
			CBitmap btmap;
			btmap.LoadBitmapA(IDB_BITMAP_LED_ON);
			m_VideoOutStatus.SetState(true);
		}
		break;
		case SKYPE_VIDEO_OUTGOING_VIDEO_CLOSE:
		{
			CBitmap btmap;
			btmap.LoadBitmapA(IDB_BITMAP_LED_OFF);
			m_VideoOutStatus.SetState(false);
		}
		break;


	}
	return 0;
}

int CPageSkype::ConversationStatus(int StatusId, void *pInfo)
{
	switch(StatusId)
	{
		case SKYPE_CONVERSATION_INCOMING_CALL_RINGING:
		{
			CString Message = "Call form : ";
			CString Particpant;
			m_fRingingForMe = true;
			if(pInfo){
				Message = Message + (const char *)pInfo;
			}
			SetDlgItemText(IDC_EDIT_SKYPE_STATUS,Message);
		}
		break;
	}
	return 0;
}

int CPageSkype::ParticipantStatus(int StatusId, void *pInfo)
{
	CString Message;
	if(pInfo)
		Message = (const char *)pInfo;
	switch(StatusId)
	{
		case SKYPE_PARTICIPANT_STATUS_UNKNOWN:
			Message += ":?";
		break;
		case SKYPE_PARTICIPANT_STATUS_RINGING:
			Message += ":Inviting..";
			break;
		case SKYPE_PARTICIPANT_STATUS_SPEAKING:
			Message += ": joined session";
			break;
		case SKYPE_PARTICIPANT_STATUS_LISTENING:
			Message += ": joined session";
			break;
		case SKYPE_PARTICIPANT_STATUS_VOICE_STOPPED:
			Message += ": dropped from session";
			break;
	}
	if(StatusId != SKYPE_PARTICIPANT_STATUS_UNKNOWN)
		SetDlgItemText(IDC_EDIT_SKYPE_STATUS,Message);
	return 0;
}
void CPageSkype::OnBnClickedButtonSkypeCall()
{
	if (m_fRingingForMe) {
		m_pSkypeIf->AceptIncomingCall(true);
		m_fRingingForMe = false;
	} else /*(m_fCalling)*/ {
		char pszContact[256] = {0};
		GetDlgItemText(IDC_EDIT_CONTACT, pszContact, 256);
		m_pSkypeIf->PlaceCall(pszContact);
		SetDlgItemText(IDC_EDIT_SKYPE_STATUS,"Skype Request...");
	}
}

void CPageSkype::OnBnClickedButtonSkypeHangup()
{
	m_pSkypeIf->HangupCall();
}

void CPageSkype::LiveSessionClosed()
{
	SetDlgItemText(IDC_EDIT_SKYPE_STATUS,"Call Complete");	
}

void CPageSkype::OtherSessionClosed()
{
}


void CPageSkype::OnBnClickedAutoReply()
{
	m_pSkypeIf->EnableAutoAnswer(m_AutoReply.IsChecked());
}

void CPageSkype::OnBnClickedButtonAddAutoreply()
{
}

void CPageSkype::OnBnClickedButtonDelAutoreply()
{
	// TODO: Add your control notification handler code here
}


int CPageSkype::PaintImage(const char *pFrameBuff, int nLen, CImageList &ImgListSkype)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	Status status;
	HGLOBAL	hMem = ::GlobalAlloc(GMEM_MOVEABLE,nLen);
	
	LPVOID pImage = ::GlobalLock(hMem);
	memcpy(pImage, pFrameBuff, nLen);
	::GlobalUnlock(hMem);

	CComPtr<IStream> spStream;
	HRESULT hr = ::CreateStreamOnHGlobal(hMem,FALSE,&spStream);


	CBitmap tmpbitmap;
	//if(w > 0 && h > 0)   
	if(1){
#if 0
		Image image(spStream);
		int w = image.GetWidth();
		int h = image.GetHeight();
		RectF destRect(0,0, SKYPE_IMG_W, SKYPE_IMG_H);
		CDC dc;
		dc.CreateCompatibleDC(NULL);
		tmpbitmap.CreateCompatibleBitmap(&dc, w,h/*SKYPE_IMG_W, SKYPE_IMG_H*/);
		CBitmap* pbmpOld = dc.SelectObject(&tmpbitmap);
		Graphics graphics(dc.GetSafeHdc());
		status = graphics.GetLastStatus();
		if(Ok == status)  {
			status = graphics.DrawImage(&image, destRect, 0,0,w,h,UnitPixel);
		}
		dc.SelectObject(pbmpOld);
#else
		Bitmap bitmap(spStream);
		int w = bitmap.GetWidth();
		int h = bitmap.GetHeight();
		
		Gdiplus::Bitmap resizeBitmap(SKYPE_IMG_W, SKYPE_IMG_H, bitmap.GetPixelFormat());
		Graphics graphics(&resizeBitmap);
		graphics.DrawImage(&bitmap, 0, 0, SKYPE_IMG_W, SKYPE_IMG_W);

		HBITMAP hBitmap = NULL;
		status = resizeBitmap.GetHBITMAP(Color(0,0,0), &hBitmap);
		if(Ok == status)  {
			tmpbitmap.Attach(hBitmap);
		}
#endif	
	} else {
		tmpbitmap.LoadBitmap(IDB_BITMAP_SKYPE_IMG_16X16);
	}
	ImgListSkype.Add(&tmpbitmap, RGB(255,0, 255));
	tmpbitmap.DeleteObject();
	GlobalFree(hMem);
	GdiplusShutdown(gdiplusToken);

	return 0;
}

void CPageSkype::AddContact(SKYPE_CONTACT_INF_T *pContactInf)
{
	if(pContactInf == NULL) {
		m_SkypeContacts.DeleteAllItems();
	} else {
		int n = m_SkypeContacts.GetItemCount();
		LVITEM lvi;
		if(pContactInf->imgJpgData){
			PaintImage(pContactInf->imgJpgData, pContactInf->imgLen, m_ImgListSkype);
		} else {
			CBitmap tmpbitmap;
			tmpbitmap.LoadBitmap(IDB_BITMAP_SKYPE_IMG_16X16);
			m_ImgListSkype.Add(&tmpbitmap, RGB(255,0, 255));
			tmpbitmap.DeleteObject();
		}

		lvi.mask =  LVIF_IMAGE | LVIF_TEXT;
		lvi.iItem = n;
		lvi.iSubItem = 0;
		lvi.pszText = (LPTSTR)(LPCTSTR)NULL;
		lvi.iImage = n;		// There are 8 images in the image list
		m_SkypeContacts.InsertItem(&lvi);

		if(pContactInf->pszContactName && strlen(pContactInf->pszContactName) > 0){
			m_SkypeContacts.SetItemText(n,COLUMN_ID,  pContactInf->pszContactName);
		}
		if(pContactInf->pszContactFullName && strlen(pContactInf->pszContactFullName) > 0){
			m_SkypeContacts.SetItemText(n, COLUMN_DISP_NAME, pContactInf->pszContactFullName);
		}
	}
	InvalidateRect(NULL);
}
void CPageSkype::OnNMDblclkListSkypeAutoreply(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);

	POSITION pos = m_SkypeContacts.GetFirstSelectedItemPosition();
	if (pos != NULL) {
		int nItem = m_SkypeContacts.GetNextSelectedItem(pos);
		CString skypeId = m_SkypeContacts.GetItemText(nItem, COLUMN_ID);
		SetDlgItemText(IDC_EDIT_CONTACT, skypeId);
	}
	*pResult = 0;
}

void CPageSkype::OnEnKillfocusEditAutoReply()
{
	char szAutoreply[1024] = {0};
	GetDlgItemText(IDC_EDIT_AUTO_REPLY, szAutoreply, 1024);
	m_strAutoReply = szAutoreply;
	m_pSkypeIf->SetAutoReplyList(szAutoreply);
}

void CPageSkype::OnEnChangeEditAutoReply()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the __super::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CPageSkype::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//__super::OnOK();
	CMFCPropertyPage::OnOK();
}

void CPageSkype::OnBnClickedButtonSkypeConfig()
{
	CSkypeConfig dlg;
	dlg.m_pSkypeIf = m_pSkypeIf;
	dlg.m_AudioInput = m_AudioInput;
	dlg.m_AudioOutput = m_AudioOutput;
	if(dlg.DoModal() == IDOK){
		m_AudioInput = dlg.m_AudioInput;
		m_AudioOutput = dlg.m_AudioOutput;
		m_pSkypeIf->SelectAudioDevices(m_AudioInput, m_AudioOutput);
	}
}

BOOL CPageSkype::OnHelpInfo(HELPINFO* pHelpInfo)
{
	// TODO: Add your message handler code here and/or call default

	return CMFCPropertyPage::OnHelpInfo(pHelpInfo);
}

BOOL CPageSkype::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR	*lpnm = (NMHDR *) lParam;
	if(lpnm->code == PSN_HELP) {
		CString cstrHelpFile = "OnyxVirtualStudio.chm";
		::HtmlHelp(this->m_hWnd, cstrHelpFile, HH_HELP_FINDER, 0);
		return TRUE;
	}

	return __super::OnNotify(wParam, lParam, pResult);
}
