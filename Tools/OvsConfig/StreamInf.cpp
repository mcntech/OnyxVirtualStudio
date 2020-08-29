// StreamInf.cpp : implementation file
//

#include "stdafx.h"
#include "OvsConfig.h"
#include "StreamInf.h"
#include "DsUtil.h"
#include <vector>

// CStreamInf dialog

IMPLEMENT_DYNAMIC(CStreamInf, CDialog)

CStreamInf::CStreamInf(CWnd* pParent /*=NULL*/)
	: CDialog(CStreamInf::IDD, pParent)
	, m_StreamPath(_T(""))
	, m_PathCamera(_T(""))
	, m_Example(_T(""))
	, m_SelStreamPath(_T(""))
	, mSkypeId(_T(""))
	, m_LablePath(_T(""))
	, m_Passwd(_T(""))
{

}

CStreamInf::~CStreamInf()
{
}

void CStreamInf::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_STREAM_PATH, m_StreamPath);
	DDV_MaxChars(pDX, m_StreamPath, 256);
	DDX_Control(pDX, IDC_COMBO_CAMERA, m_ComboCamera);
	DDX_CBString(pDX, IDC_COMBO_CAMERA, m_PathCamera);
	DDX_Control(pDX, IDC_BUTTON_BROWS_PATH, m_BtnBrowse);
	DDX_Control(pDX, IDC_EDIT_STREAM_PATH, m_EditStreamPath);
	DDX_Control(pDX, IDC_EDIT_EXAMPLE, m_EditExample);
	DDX_Text(pDX, IDC_EDIT_EXAMPLE, m_Example);
	DDX_Text(pDX, IDC_STATIC_PATH, m_LablePath);
	DDX_Control(pDX, IDC_EDIT_PASSWD, m_CtrlPasswd);
	DDX_Text(pDX, IDC_EDIT_PASSWD, m_Passwd);
	DDX_Control(pDX, IDC_STATIC_PASSWD, m_LabelPasswd);
}


BEGIN_MESSAGE_MAP(CStreamInf, CDialog)
	ON_EN_CHANGE(IDC_EDIT_STREAM_PATH, &CStreamInf::OnEnChangeEditStreamPath)
	ON_BN_CLICKED(IDC_BUTTON_BROWS_PATH, &CStreamInf::OnBnClickedButtonBrowsPath)
	ON_BN_CLICKED(IDC_RADIO_STREAM_FILE, &CStreamInf::OnBnClickedRadioStreamFile)
	ON_BN_CLICKED(IDC_RADIO_STREAM_CAMERA, &CStreamInf::OnBnClickedRadioStreamCamera)
	ON_BN_CLICKED(IDC_RADIO_STREAM_RTSP, &CStreamInf::OnBnClickedRadioStreamRtsp)
	ON_BN_CLICKED(IDC_RADIO_STREAM_SKYPE, &CStreamInf::OnBnClickedRadioStreamSkype)
	ON_BN_CLICKED(IDC_RADIO_STREAM_SIP, &CStreamInf::OnBnClickedRadioStreamSip)
	ON_BN_CLICKED(IDOK, &CStreamInf::OnBnClickedOk)
	ON_CBN_SELENDOK(IDC_COMBO_CAMERA, &CStreamInf::OnCbnSelendokComboCamera)
END_MESSAGE_MAP()



int OnGetStreamFilePath( HWND hDlg, char *szMp4File) 
{
    OPENFILENAMEA ofn={0};

    // Fill in standard structure fields
    ofn.lStructSize       = sizeof(OPENFILENAME);
    ofn.hwndOwner         = hDlg;
	ofn.lpstrFilter       = "Media Files (*.mp4; *.m4v; *.wmv)\0*.mp4; *.m4v; *.wmv\0\0";

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

    BOOL bSuccess = GetOpenFileNameA((LPOPENFILENAMEA)&ofn);
    return bSuccess;
}


// CStreamInf message handlers

void CStreamInf::OnEnChangeEditStreamPath()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CStreamInf::OnBnClickedButtonBrowsPath()
{
	char szFilePath[MAX_PATH] = {0};
	if(!m_StreamPath.IsEmpty())
		strcpy(szFilePath, (char *)(LPCTSTR)m_StreamPath);
	if(OnGetStreamFilePath(this->m_hWnd, szFilePath) == TRUE){
		m_StreamPath = szFilePath;
		UpdateData(false);
	}
}

void CStreamInf::UpdateButtons()
{
	m_ComboCamera.ShowWindow(SW_HIDE);
	m_EditStreamPath.ShowWindow(SW_HIDE);
	m_BtnBrowse.ShowWindow(SW_HIDE);
	m_EditExample.ShowWindow(SW_HIDE);
	m_CtrlPasswd.ShowWindow(SW_HIDE);	
	m_LabelPasswd.ShowWindow(SW_HIDE);	
	switch(m_StreamType) 
	{
		case STREAM_TYPE_FILE:
			m_LablePath = "File";
			m_EditStreamPath.ShowWindow(SW_SHOW);
			m_BtnBrowse.ShowWindow(SW_SHOW);
			m_StreamPath = m_FilePath;
			UpdateData(false);
			break;
		case STREAM_TYPE_CAMERA:
			m_LablePath = "Camera";
			m_ComboCamera.ShowWindow(SW_SHOW);
			UpdateData(false);
			break;
		case STREAM_TYPE_RTSP:
			m_LablePath = "Rtsp";
			m_Example = "Example: rtsp://192.168.1.108:594000/stream";
			m_StreamPath = m_RtspPath;
			m_EditStreamPath.ShowWindow(SW_SHOW);
			m_EditExample.ShowWindow(SW_SHOW);
			UpdateData(false);
			break;
		case STREAM_TYPE_SKYPE:
			{
				m_LablePath = "SkypeID";
				m_CtrlPasswd.ShowWindow(SW_SHOW);	
				m_LabelPasswd.ShowWindow(SW_SHOW);
				m_StreamPath = "";
				m_Passwd = "";
				std::string tmp = m_SKypePath;
				size_t nPos = tmp.find(":");
				if(nPos != std::string::npos) {
					std::string tmp1 = tmp.substr(nPos + 1, tmp.size() - nPos);
					m_Passwd = tmp1.c_str();
					tmp1 = tmp.substr(0, nPos);
					m_StreamPath = tmp1.c_str();
				}

				m_Example = "Sype ID Format: user@skype";
				m_EditStreamPath.ShowWindow(SW_SHOW);
				m_EditExample.ShowWindow(SW_SHOW);
				UpdateData(false);
			}
			break;

		case STREAM_TYPE_SIP:
			m_LablePath = "SipID";
			m_StreamPath = m_SipPath;
			m_Example = "Example: sip:username@sip.mydomain.com";
			m_EditStreamPath.ShowWindow(SW_SHOW);
			m_EditExample.ShowWindow(SW_SHOW);
			UpdateData(false);
		break;
	}
}

void CStreamInf::OnBnClickedRadioStreamFile()
{
	m_StreamType = STREAM_TYPE_FILE;
	UpdateButtons();
}

void CStreamInf::OnBnClickedRadioStreamCamera()
{
	CDsUtil DsUtil;
	std::vector<std::string> mDeviceList;
	DsUtil.GetDeviceList(CAPTURE_DEVICE_CAREGORY_VIDEO, mDeviceList);

	m_ComboCamera.ResetContent();
	if(!mDeviceList.empty()){
		for (int i=0; i < mDeviceList.size(); i++){
			CString nameDevice="device://";
			nameDevice += mDeviceList[i].c_str();
			m_ComboCamera.AddString(nameDevice);
		}
	}
	m_StreamType = STREAM_TYPE_CAMERA;
	UpdateButtons();
}

void CStreamInf::OnBnClickedRadioStreamRtsp()
{
	m_StreamType = STREAM_TYPE_RTSP;
	UpdateButtons();
}

void CStreamInf::OnBnClickedRadioStreamSkype()
{
	m_StreamType = STREAM_TYPE_SKYPE;
	UpdateButtons();
}

void CStreamInf::OnBnClickedRadioStreamSip()
{
	m_StreamType = STREAM_TYPE_SIP;
	UpdateButtons();
}

BOOL CStreamInf::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_StreamType = STREAM_TYPE_FILE; // Default
	if(!m_SelStreamPath.IsEmpty()){
		if(m_SelStreamPath.Find("device:") != -1) {
			m_StreamType = STREAM_TYPE_CAMERA;
			m_PathCamera = m_SelStreamPath;
			CheckDlgButton(IDC_RADIO_STREAM_CAMERA, 1);
		} else if(m_SelStreamPath.Find("rtsp:") != -1) {
			m_StreamType = STREAM_TYPE_RTSP;
			m_RtspPath = m_SelStreamPath;
			CheckDlgButton(IDC_RADIO_STREAM_RTSP, 1);
		} else if(m_SelStreamPath.Find("sip:") != -1) {
			m_StreamType = STREAM_TYPE_SIP;
			m_SipPath = m_SelStreamPath;
			CheckDlgButton(IDC_RADIO_STREAM_SIP, 1);
		} else if(m_SelStreamPath.Find("@skype:") != -1) {
			m_SKypePath = m_SelStreamPath;
			m_StreamType = STREAM_TYPE_SKYPE;
			CheckDlgButton(IDC_RADIO_STREAM_SKYPE, 1);
		} else {
			m_StreamType = STREAM_TYPE_FILE;
			m_FilePath = m_SelStreamPath;
			CheckDlgButton(IDC_RADIO_STREAM_FILE, 1);
		}
	}
	UpdateButtons();
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CStreamInf::OnBnClickedOk()
{
	UpdateData(1);
	if(m_StreamType == STREAM_TYPE_CAMERA) {
		m_SelStreamPath = m_PathCamera;
	} else if(m_StreamType == STREAM_TYPE_SKYPE) {
		if(m_StreamPath.Find('@') == -1) {
			m_StreamPath = m_StreamPath + "@skype";
		}
		m_SKypePath = m_StreamPath + ":" + m_Passwd;
		m_SelStreamPath = m_SKypePath;
	} else {
		m_SelStreamPath = m_StreamPath;
	}
	OnOK();
}

void CStreamInf::OnCbnSelendokComboCamera()
{

}

void CStreamInf::OnStnClickedStaticPasswd2()
{
	// TODO: Add your control notification handler code here
}
