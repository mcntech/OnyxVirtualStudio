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
#include "PagePublish.h"

#include "JdAwsS3.h"
#include "HlsOutJdAws.h"
#include "JdAwsS3UpnpHttpConnection.h"
#include "JdAwsConfig.h"
#include "ConfigComn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CONTENT_STR_HTML			"text/html"
#define CONTENT_STR_M3U8			"audio/x-mpegurl"
#define CONTENT_STR_MP2T			"application/octet-stream"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPagepublish property page

IMPLEMENT_DYNCREATE(CPagepublish, CMFCPropertyPage)

CPagepublish::CPagepublish() : CMFCPropertyPage(CPagepublish::IDD)
	, m_S3HostName(_T(""))
	, m_S3SecurityKey(_T(""))
	, m_S3BucketName(_T(""))
	, m_VideoChannel(_T(""))
	, m_LocalFolder(_T(""))
	, m_S3AccessId(_T(""))

{
}

CPagepublish::~CPagepublish()
{
}

void CPagepublish::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_S3_HOST_NAME, m_S3HostName);
	DDX_Text(pDX, IDC_EDIT_S3_SECURITY_KEY, m_S3SecurityKey);
	DDX_Text(pDX, IDC_EDIT_S3_BUCKET, m_S3BucketName);
	DDX_Text(pDX, IDC_EDIT_S3_FOLDER, m_VideoChannel);
	DDX_Text(pDX, IDC_EDIT_LOCAL_FOLDER, m_LocalFolder);
	DDX_Text(pDX, IDC_EDIT_S3_ACCESS_ID, m_S3AccessId);
	DDX_Control(pDX, IDC_UPLOAD_PROGRESS1, m_ProgressCrntFile);
	DDX_Control(pDX, IDC_UPLOAD_PROGRESS2, m_CtrlProgressTotal);
	DDX_Control(pDX, IDC_EDIT_STATUS, m_CtrlStatus);

}

BEGIN_MESSAGE_MAP(CPagepublish, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_START_UPLOAD, &CPagepublish::OnBnClickedButtonStartUpload)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL_UPLOAD, &CPagepublish::OnBnClickedButtonCancelUpload)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_CHANNEL, &CPagepublish::OnBnClickedButtonClearChannel)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_FOLDER, &CPagepublish::OnBnClickedButtonBrowseFolder)

	ON_EN_CHANGE(IDC_EDIT_S3_ACCESS_ID, &CPagepublish::OnEnChangeEditS3AccessId)
	ON_EN_CHANGE(IDC_EDIT_S3_SECURITY_KEY, &CPagepublish::OnEnChangeEditS3SecurityKey)
	ON_EN_CHANGE(IDC_EDIT_S3_BUCKET, &CPagepublish::OnEnChangeEditS3Bucket)
	ON_EN_CHANGE(IDC_EDIT_S3_FOLDER, &CPagepublish::OnEnChangeEditS3Folder)
	ON_EN_CHANGE(IDC_EDIT_LOCAL_FOLDER, &CPagepublish::OnEnChangeEditLocalFolder)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPagepublish message handlers

BOOL CPagepublish::InitParams()
{
	char szValue[MAX_PATH] = {0};
	GetPrivateProfileStringA("S3", "HostName","s3.amazonaws.com",szValue,MAX_PATH - 1,m_ConfigFile);
	m_S3HostName = szValue;

	GetPrivateProfileStringA("S3", "SecurityKey","",szValue,MAX_PATH - 1,m_ConfigFile);
	m_S3SecurityKey = szValue;

	GetPrivateProfileStringA("S3", "BucketName","",szValue,MAX_PATH - 1,m_ConfigFile);
	m_S3BucketName = szValue;

	GetPrivateProfileStringA("S3", "AccessId","",szValue,MAX_PATH - 1,m_ConfigFile);
	m_S3AccessId = szValue;

	GetPrivateProfileStringA("Channel", "Name","channel1",szValue,MAX_PATH - 1,m_ConfigFile);
	m_VideoChannel = szValue;

	GetPrivateProfileStringA("Channel", "LocalFolder","",szValue,MAX_PATH - 1,m_ConfigFile);
	m_LocalFolder = szValue;


	return TRUE;
}

BOOL CPagepublish::SaveParams()
{
	char szValue[MAX_PATH] = {0};
	UpdateData(true);
	if(!m_S3HostName.IsEmpty()){
		WritePrivateProfileStringA("S3", "HostName",m_S3HostName, m_ConfigFile);
	}

	if(!m_S3SecurityKey.IsEmpty()){
		WritePrivateProfileStringA("S3", "SecurityKey",m_S3SecurityKey, m_ConfigFile);
	}

	if(!m_S3SecurityKey.IsEmpty()){
		WritePrivateProfileStringA("S3", "BucketName",m_S3BucketName, m_ConfigFile);
	}

	if(!m_S3SecurityKey.IsEmpty()){
		WritePrivateProfileStringA("S3", "AccessId",m_S3AccessId, m_ConfigFile);
	}

	if(!m_S3SecurityKey.IsEmpty()){
		WritePrivateProfileStringA("Channel", "Name",m_VideoChannel, m_ConfigFile);
	}

	if(!m_S3SecurityKey.IsEmpty()){
		WritePrivateProfileStringA("Channel", "LocalFolder",m_LocalFolder, m_ConfigFile);
	}

	return TRUE;
}

BOOL CPagepublish::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();
	m_Background.LoadBitmap(IDB_BITMAP_WELCOME); //Load bitmap
	m_pEditBkBrush = new CBrush(RGB(0, 0, 0));

	WSADATA wsaData;
	int iResult;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        MessageBox("WinSock Initialization Failed", "Error");
    }


	GetConfigFoderPath(m_ConfigFile);
	m_ConfigFile += "\\hls.cfg";
	m_ProgressCrntFile.SetRange(0,100);
	m_ProgressCrntFile.SetPos(0);
	m_CtrlProgressTotal.SetPos(0);
	InitParams();

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
}

long GetFileSize(const char *fileName)
{    
	BOOL                        fOk;    
	WIN32_FILE_ATTRIBUTE_DATA   fileInfo;    
	if (NULL == fileName)        
		return -1;    
	fOk = GetFileAttributesEx(fileName, GetFileExInfoStandard, (void*)&fileInfo);    
	if (!fOk)        
		return -1;    
	return (long)fileInfo.nFileSizeLow;
}

#define FILE_READ_SIZE	(1024 * 1024)

long long FindFolderSize(const char *szFolder)
{
	long long llSize = 0;
	HANDLE hFind; 
	WIN32_FIND_DATA data; 
	std::string localFolder = szFolder;
	localFolder =  localFolder + "\\*.*";

	hFind = FindFirstFile(localFolder.c_str(), &data); 
	if (hFind != INVALID_HANDLE_VALUE) {   
		do {     
			if(stricmp(data.cFileName, ".") == 0 || strcmp(data.cFileName, "..") == 0)
				continue;
			std::string fileName = szFolder; 
			fileName +=  "/";
			fileName += data.cFileName;

			HANDLE hFile = CreateFileA(fileName.c_str(),
										GENERIC_READ , 
										FILE_SHARE_WRITE | FILE_SHARE_READ,NULL,
										OPEN_EXISTING,(DWORD) 0,0);
			if(hFile == INVALID_HANDLE_VALUE) {
				continue;
			}
			DWORD dwFileSizeHigh = 0;;
			DWORD dwFileSize = GetFileSize(hFile, &dwFileSizeHigh);
			llSize += dwFileSize;
			CloseHandle(hFile);
		}  while (FindNextFile(hFind, &data));   
		FindClose(hFind); 
	}
	return llSize;
}
void CPagepublish::threadUploadFolder()
{
	std::string pathFolder = m_VideoChannel;
	std::string fileNameOnServer;
	char *pData = (char *)malloc(FILE_READ_SIZE);
	int nLen;
	std::string  typeContent;
	int nTimeOut;
	long long llFolderBytesRead = 0;
	
	if(m_LocalFolder.IsEmpty()){
		MessageBox("Please Select Folder to Publish", "Channel Publishing Error");
	}
	if(m_S3BucketName.IsEmpty()) {
		MessageBox("Please Specify S3 Bucket name", "Channel Publishing Error");
	}

	if(m_S3AccessId.IsEmpty()) {
		MessageBox("Please Specify S3 Acces ID", "Channel Publishing Error");
	}

	if(m_S3SecurityKey.IsEmpty()) {
		MessageBox("Please Specify S3 Security Key", "Channel Publishing Error");
	}

	long long llFolderSize = FindFolderSize(m_LocalFolder);
	if(llFolderSize == 0){
		MessageBox("Empty folder", "Channel Publishing Error");
		return;
	}

	CHlsOutJdS3 HlsOut(m_S3BucketName, m_S3HostName, m_S3AccessId, m_S3SecurityKey);

	HANDLE hFind; 
	WIN32_FIND_DATA data; 
	std::string localFolder = m_LocalFolder;
	localFolder +=  "\\*.*";
	hFind = FindFirstFile(localFolder.c_str(), &data); 
	if (hFind != INVALID_HANDLE_VALUE) {   
		do {     
			//printf("%s\n", data.cFileName);
			if(stricmp(data.cFileName, ".") == 0 || strcmp(data.cFileName, "..") == 0)
				continue;
			std::string fileName = m_LocalFolder + "/";
			fileName += data.cFileName;
			fileNameOnServer = data.cFileName;
			HANDLE hFile = CreateFileA(fileName.c_str(),
										GENERIC_READ , 
										FILE_SHARE_WRITE | FILE_SHARE_READ,NULL,
										OPEN_EXISTING,(DWORD) 0,0);
			if(hFile == INVALID_HANDLE_VALUE) {
				//return -1;			// Get Size
				// TODO: MessageBox or status
				std::string msg = "Coild not open file" + fileName;
				MessageBox(msg.c_str(), "Error");
				break;
			} else {
				std::string msg = "Uploading : " + fileName;
				m_CtrlStatus.SetWindowTextA( msg.c_str());
			}
			
			if(strstr(fileNameOnServer.c_str(), ".m3u8") || strstr(fileNameOnServer.c_str(), ".M3U8")){
				typeContent = CONTENT_STR_M3U8;
			} else if(strstr(fileNameOnServer.c_str(), ".html") || strstr(fileNameOnServer.c_str(), ".HTML")){
				typeContent = CONTENT_STR_HTML;
			} else {
				typeContent = CONTENT_STR_MP2T;
			}
			DWORD dwFileSizeHigh;
			DWORD dwFileSize = GetFileSize(hFile, &dwFileSizeHigh);
			if(HlsOut.Start(pathFolder.c_str(), fileNameOnServer.c_str(), dwFileSize, NULL, 0, typeContent.c_str()) != JD_OK){
				std::string msg = "Uploading Failed : " + fileName;
				m_CtrlStatus.SetWindowTextA(msg.c_str());
				//UpdateData(false);
				break;
			}
			DWORD dwBytesRead = 0;
			DWORD dwBytesReadTotal = 0;
			m_ProgressCrntFile.SetPos(0);
			while (dwBytesReadTotal < dwFileSize && m_fRunUpload){
				ReadFile(hFile, (LPVOID)pData, FILE_READ_SIZE, &dwBytesRead, NULL);
				if(dwBytesRead > 0) {
					HlsOut.Continue(pData, dwBytesRead);
					dwBytesReadTotal += dwBytesRead;
					
					/* Update Progress */
					llFolderBytesRead += dwBytesRead;
					int nProgPercentCrntFile = (int)((double)dwBytesReadTotal / dwFileSize * 100);
					int nProgPercentFolder = (int)((double)llFolderBytesRead / llFolderSize * 100);
					m_ProgressCrntFile.SetPos(nProgPercentCrntFile);
					m_CtrlProgressTotal.SetPos(nProgPercentFolder);
				}
			}
			HlsOut.End(NULL, 0);

			CloseHandle(hFile);
		} while (m_fRunUpload && FindNextFile(hFind, &data));   
		FindClose(hFind); 
		if(m_fRunUpload) {
			m_CtrlStatus.SetWindowTextA("Completed.");
		} else {
			m_CtrlStatus.SetWindowTextA("Cancelled.");
		}
	} 
}
UINT CPagepublish::StartThread(LPVOID pParam)
{
	CPagepublish *pDlg = (reinterpret_cast<CPagepublish*>(pParam));
	pDlg->m_fRunUpload = 1;
	pDlg->threadUploadFolder();
	return 0;
}

void CPagepublish::StopThread()
{
	m_fRunUpload = 0;
	WaitForSingleObject(m_Thread ,2000);
}
void CPagepublish::OnBnClickedButtonStartUpload()
{
	UpdateData(true);
	m_Thread = AfxBeginThread(StartThread, this);
}

void CPagepublish::OnBnClickedButtonCancelUpload()
{
	StopThread();
}

void CPagepublish::OnBnClickedButtonClearChannel()
{
	// TODO: Add your control notification handler code here
}


void CPagepublish::OnBnClickedButtonBrowseFolder()
{
	LPITEMIDLIST pidl     = NULL;
	BROWSEINFO   bi       = { 0 };
	BOOL         bResult  = FALSE;
	char		szFolder[MAX_PATH] = {0};


	bi.hwndOwner      = m_hWnd;
	bi.pszDisplayName = szFolder;
	bi.pidlRoot       = NULL;
	bi.lpszTitle      = "Please select folder to publish.";
	bi.ulFlags        = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;

	if ((pidl = SHBrowseForFolder(&bi)) != NULL) {
		bResult = SHGetPathFromIDList(pidl, szFolder);
		CoTaskMemFree(pidl);
	}
	if(bResult) {
		m_LocalFolder = szFolder;
	}
	UpdateData(false);
}

void CPagepublish::OnEnChangeEditS3AccessId()
{
	SetModified(1);
}

void CPagepublish::OnEnChangeEditS3SecurityKey()
{
	SetModified(1);
}

void CPagepublish::OnEnChangeEditS3Bucket()
{
	SetModified(1);
}

void CPagepublish::OnEnChangeEditS3Folder()
{
	SetModified(1);
}

void CPagepublish::OnEnChangeEditLocalFolder()
{
	SetModified(1);	
}

BOOL CPagepublish::OnApply()
{
	SaveParams();
	return CMFCPropertyPage::OnApply();
}

BOOL CPagepublish::OnEraseBkgnd(CDC* pDC)
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

HBRUSH CPagepublish::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CMFCPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);

	if (CTLCOLOR_STATIC == nCtlColor)  {
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	} else if ( nCtlColor == CTLCOLOR_EDIT ) {
		pDC->SetTextColor(RGB(0, 255, 0));
		pDC->SetBkColor(RGB(0, 0, 0));
		return (HBRUSH)(m_pEditBkBrush->GetSafeHandle());
	}
	return hbr;
}

void CPagepublish::OnDestroy()
{
	CMFCPropertyPage::OnDestroy();

	delete m_pEditBkBrush;
}
