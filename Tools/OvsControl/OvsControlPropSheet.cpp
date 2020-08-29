#include "stdafx.h"
#include "AppFeatureConfig.h"
#include "OvsControl.h"
#include "OvsControlPropSheet.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ONYX_WINDOW_CAPTION   "Onyx Virtual Studio"

HANDLE ghLogFile = INVALID_HANDLE_VALUE;
/////////////////////////////////////////////////////////////////////////////
// COvsControlPropSheet

IMPLEMENT_DYNAMIC(COvsControlPropSheet, CMFCPropertySheet)

void COvsControlPropSheet::AddTabIcon(int nResId)
{
	CBitmap tmpBmp;
	tmpBmp.LoadBitmapA(nResId);
	m_ImgList.Add(&tmpBmp,RGB(255,0, 255));
	tmpBmp.DeleteObject();
}

COvsControlPropSheet::COvsControlPropSheet(CWnd* pParentWnd)
:CMFCPropertySheet(IDS_CAPTION, pParentWnd)
{
	BOOL b32BitIcons = TRUE;

	if (afxGlobalData.m_nBitsPerPixel < 16)	{
		b32BitIcons = FALSE;
	}

	SetLook(CMFCPropertySheet::PropSheetLook_OutlookBar);
#if 0
	SetIconsList(b32BitIcons ? IDB_ICONS32 : IDB_ICONS, 32);
#else
	m_ImgList.Create(32,32,ILC_MASK|ILC_COLOR32, 5, 5);
#endif
	//AddPage(&m_PageWelcome);	

	AddPage(&m_PageInputVideos);
	AddTabIcon(IDB_TAB_ICON_MIXER);

	AddPage(&m_PageLayout);
	AddTabIcon(IDB_TAB_ICON_LAYOUT);

	for (int i=0; i < MAX_LIVECAST_PAGES; i++){
		m_pPageLivecast[i] = NULL;
	}

	int nPages = CPageLivecast::CreatePropPages(m_pPageLivecast, MAX_LIVECAST_PAGES);
	for (int i=0; i < nPages; i++) {
		AddPage(m_pPageLivecast[i]);
		AddTabIcon(IDB_TAB_ICON_LIVECAST);
	}

	//AddPage(&m_PagePublish);
	AddPage(&m_PageRecorder);
	AddTabIcon(IDB_TAB_ICON_RECORD);

	AddPage(&m_PageSkype);
	AddTabIcon(IDB_TAB_ICON_SKYPE);


	SetIconsList(m_ImgList);
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
#ifdef EN_APPUP
	m_pInAppUnlockingSampleApp = NULL;
	m_pCrashReport = NULL;
	m_pItemIf = NULL;
#endif
	m_fEvalMode = false;
}

COvsControlPropSheet::~COvsControlPropSheet()
{
#ifdef EN_APPUP
	if(m_pInAppUnlockingSampleApp) {
		// When m_pADPApplication is deleted m_pCrashReport will be deleted
		delete m_pInAppUnlockingSampleApp;
	}
	if(m_pItemIf)
		delete m_pItemIf;
#endif
	for (int i=0; i < MAX_LIVECAST_PAGES; i++){
		if(m_pPageLivecast[i])
			delete m_pPageLivecast[i];
	}

}

BEGIN_MESSAGE_MAP(COvsControlPropSheet, CMFCPropertySheet)
	ON_WM_QUERYDRAGICON()
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_BUTTON_LAUNCH_ONYX, OnBtnLaunchOnyx)
	ON_WM_HELPINFO()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COvsControlPropSheet message handlers

BOOL COvsControlPropSheet::OnInitDialog()
{
	BOOL bResult = CMFCPropertySheet::OnInitDialog();

	// Add "About..." menu item to system menu.
	CString Msg;
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	
	CString AppFolderPath;
	CString LogFile;
	GetConfigFoderPath(AppFolderPath);
	LogFile = AppFolderPath + "\\Ovs.log";
	ghLogFile = CreateFileA(LogFile,
								GENERIC_WRITE, 
								FILE_SHARE_WRITE | FILE_SHARE_READ,NULL,
								CREATE_ALWAYS,(DWORD) 0,0);


	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bValidString;
		CString strAboutMenu;
		bValidString = strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	m_psh.dwFlags |= PSH_HASHELP;


	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE); // Set big icon
	SetIcon(m_hIcon, FALSE); // Set small icon

	CButton* btnOk=(CButton*)GetDlgItem(IDOK);
	btnOk->ShowWindow(SW_HIDE);
	CButton* btnCancel=(CButton*)GetDlgItem(IDCANCEL);
	btnCancel->ShowWindow(SW_HIDE);
	//CButton* btnHelp=(CButton*)GetDlgItem(IDHELP);
	//btnHelp->ShowWindow(SW_HIDE);
	CButton* btnApply = (CButton*)GetDlgItem( ID_APPLY_NOW);
	SetDlgItemTextA(ID_APPLY_NOW, "Save Changes");

	// Add minimize
	::SetWindowLong( m_hWnd, GWL_STYLE, GetStyle() | WS_MINIMIZEBOX );
	// add the minimize command to the system menu
	GetSystemMenu( FALSE )->InsertMenu( -1, MF_BYPOSITION | MF_STRING,	SC_ICON, "Minimize" );	CRect rect, tabrect;

	int width;
	//Get button sizes and positions
	GetDlgItem(IDOK)->GetWindowRect(rect);
	GetTabControl()->GetWindowRect(tabrect);
	ScreenToClient(rect); 
	ScreenToClient(tabrect);
	
	//New button - width, height and Y-coordiate of IDOK
	//           - X-coordinate of tab control
	width = rect.Width();
	rect.left = tabrect.left; rect.right = tabrect.left + width;
	
	//Create new "Add" button and set standard font
	m_BtnLaunchOnyx.Create("Launch Onyx", BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE|WS_TABSTOP,	rect, this, IDC_BUTTON_LAUNCH_ONYX);
	m_BtnLaunchOnyx.SetFont(GetFont());

	CRect wndRect;
	GetWindowRect (wndRect);
	HMONITOR monitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST); 
	MONITORINFO info; 
	info.cbSize = sizeof(MONITORINFO); 
	GetMonitorInfo(monitor, &info); 
	int monitor_width = info.rcMonitor.right - info.rcMonitor.left; 
	int monitor_height = info.rcMonitor.bottom - info.rcMonitor.top; 
	DWORD dwX = (info.rcWork.right - wndRect.Width())/2;
	DWORD dwY = info.rcWork.bottom - wndRect.Height() - 100;
	SetWindowPos (NULL, dwX, dwY, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

#ifdef EN_APPUP
	try {
		m_pInAppUnlockingSampleApp = new Application(ADP_DEBUG_APPLICATIONID);
		m_pCrashReport = new COvsControlCrashReport();
		m_pInAppUnlockingSampleApp->SetCrashReport(m_pCrashReport);
#ifdef EN_IN_APP_PURCHASE
		m_pItemIf = m_pInAppUnlockingSampleApp->CreateInAppItem(L"multibitrate", L"en-US", true);
		bool bPurchased = m_pItemIf->IsPurchased();
		if(!bPurchased){
			bPurchased = m_pItemIf->Purchase(m_pItemIf->GetProperty(ITEM_PRICE).c_str(), m_pItemIf->GetProperty(ITEM_CURRENCY).c_str());
		}
		if(!bPurchased) {
			Msg.Format("Item not purchased");
			AfxMessageBox(Msg);
		} else {
			std::wstring wsToken = m_pItemIf->GetSignedToken();
		}
#endif
	}

	catch (Intel::Adp::InitializationException& e) {
		Msg.Format("Can not connect to AppUp Center(code=%d).\r\nContinue in evaluation mode ?",e.code());
		if( AfxMessageBox(Msg, IDOK) != IDOK)
			EndDialog(0);
		m_fEvalMode = true;
	} 
	catch (Intel::Adp::UnauthorizedException& e) {
		Msg.Format("Can not get authorization form AppUp Center(code=%d).\r\nContinue in evaluation mode ?",e.code());
		if( AfxMessageBox(Msg, IDOK) != IDOK)
			EndDialog(0);
		m_fEvalMode = true;
	} 
	catch (Intel::Adp::AdpRuntimeException& e) {
		Msg.Format("Can not get authorization form AppUp Center(code=%d).\r\nContinue in evaluation mode ?",e.code());
		if( AfxMessageBox(Msg, IDOK) != IDOK)
			EndDialog(0);
		m_fEvalMode = true;
	}
    catch (Intel::Adp::InAppItemException& e) {
		Msg.Format("Can not get authorization form AppUp Center(code=%d).\r\nContinue in evaluation mode ?",e.code());
		if( AfxMessageBox(Msg, IDOK) != IDOK)
			EndDialog(0);
		m_fEvalMode = true;
	}
#endif

	return bResult;
}

HCURSOR COvsControlPropSheet::OnQueryDragIcon()
{
	return(HCURSOR) m_hIcon;
}

void COvsControlPropSheet::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg aboutDlg;
		aboutDlg.DoModal();
	}
	else
	{
		CMFCPropertySheet::OnSysCommand(nID, lParam);
	}
}

void COvsControlPropSheet::OnBtnLaunchOnyx()
{
	memset(&mProcessInformation, 0x00, sizeof(PROCESS_INFORMATION));
	memset(&mStartupInfo, 0, sizeof(STARTUPINFOA));
	mStartupInfo.cb = sizeof(STARTUPINFOA);

	int res = -1;
	const char *szOnyxPath = "OnyxVirtualStudio.exe";

	CRect rectWnd;    
	GetWindowRect (rectWnd);    
	//mStartupInfo.dwX = rectWnd.left;
	//mStartupInfo.dwY = 0;
	//mStartupInfo.dwFlags = STARTF_USEPOSITION;
	BOOL fResult = CreateProcessA(szOnyxPath, NULL, NULL, NULL, FALSE, 
						NORMAL_PRIORITY_CLASS, NULL, NULL, 
						&mStartupInfo, &mProcessInformation);

	if (!fResult) {
		MessageBox("Failed to launch Onyx Virtual Studio", "Onyx Control Panel", IDOK);
	} else {
		CString OnyxRenderingWindow = ONYX_WINDOW_CAPTION;
		CWnd *pOvsWnd = NULL;
		long lTimeOut = 2000;
		while(pOvsWnd == NULL && lTimeOut > 0) {
			pOvsWnd = FindWindow(NULL, OnyxRenderingWindow);
			if(pOvsWnd) {
				CRect rectOvs;    
				pOvsWnd->GetWindowRect(rectOvs);
				int nX = rectWnd.left + (rectWnd.Width() - rectOvs.Width()) / 2;
				int nY = rectWnd.top - rectOvs.Height();
				if(nY < 0) 
					nY = 0;
				pOvsWnd->SetWindowPos(NULL,nX, nY, 0, 0,SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
			} else {
				Sleep(500);
				lTimeOut -= 500;
			}
		}
	}
}

void COvsControlPropSheet::OnOK()
{

}

BOOL COvsControlPropSheet::OnHelpInfo(HELPINFO* pHelpInfo)
{
	// TODO: Add your message handler code here and/or call default
	CString cstrHelpFile = "OnyxVirtualStudio.chm";
	::HtmlHelp(this->m_hWnd, cstrHelpFile, HH_HELP_FINDER, 0);
	return TRUE;
	//return CMFCPropertySheet::OnHelpInfo(pHelpInfo);
}

BOOL COvsControlPropSheet::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	return CMFCPropertySheet::OnNotify(wParam, lParam, pResult);
}


void COvsControlPropSheet::OnDestroy()
{
	CMFCPropertySheet::OnDestroy();
	CWnd *pOvsWnd = NULL;
	pOvsWnd = FindWindow(NULL, ONYX_WINDOW_CAPTION);
	if(pOvsWnd) {
		pOvsWnd->PostMessageA(WM_CLOSE);
	}
	if(ghLogFile)
		CloseHandle(ghLogFile);
}
