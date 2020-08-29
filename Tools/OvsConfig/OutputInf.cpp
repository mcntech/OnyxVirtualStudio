// OutputInf.cpp : implementation file
//

#include "stdafx.h"
#include "OvsConfig.h"
#include "OutputInf.h"
#include <Shlobj.h>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "DsUtil.h"
#include <vector>

// COutputInf dialog

IMPLEMENT_DYNAMIC(COutputInf, CDialog)

COutputInf::COutputInf(CWnd* pParent /*=NULL*/)
	: CDialog(COutputInf::IDD, pParent)
	, m_HlsLanAddress(_T(""))
	, m_HlsS3Address(_T(""))
	, m_RtspLanAddress(_T(""))
	, m_Mp4OutputFile(_T(""))
	, m_HlsOutputFile(_T(""))
	, m_AudioInput(_T(""))
{

}

COutputInf::~COutputInf()
{
}

void COutputInf::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_OUTPUT_HLS_NET, m_HlsLanAddress);
	DDX_Text(pDX, IDC_EDIT_OUTPUT_HLS_S3, m_HlsS3Address);
	DDX_Text(pDX, IDC_EDIT_OUTPUT_RTSP, m_RtspLanAddress);
	DDX_Text(pDX, IDC_EDIT_OUTPUT_MP4, m_Mp4OutputFile);
	DDX_Text(pDX, IDC_EDIT_OUTPUT_HLS_HD, m_HlsOutputFile);
	DDX_CBString(pDX, IDC_COMBO_AUDIO_INPUT, m_AudioInput);
	DDX_Control(pDX, IDC_COMBO_AUDIO_INPUT, m_ComboAudioInput);
}


BEGIN_MESSAGE_MAP(COutputInf, CDialog)
	ON_BN_CLICKED(IDC_RADIO_OUTPUT_HLS_NET, &COutputInf::OnBnClickedRadioOutputHlsNet)
	ON_BN_CLICKED(IDC_RADIO_OUTPUT_HLS_S3, &COutputInf::OnBnClickedRadioOutputHlsS3)
	ON_BN_CLICKED(IDC_RADIO_OUTPUT_RTSP, &COutputInf::OnBnClickedRadioOutputRtsp)
	ON_BN_CLICKED(IDC_RADIO_OUTPUT_HLS_FILE, &COutputInf::OnBnClickedRadioOutputHlsFile)
	ON_BN_CLICKED(IDC_BUTTON_CONFIGURE_S3, &COutputInf::OnBnClickedButtonConfigureS3)
	ON_BN_CLICKED(IDC_BUTTON_BROWS_OUTPUT_MP4, &COutputInf::OnBnClickedButtonBrowsOutputMp4)
	ON_BN_CLICKED(IDC_BUTTON_BROWS_OUTPUT_HLS, &COutputInf::OnBnClickedButtonBrowsOutputHls)
	ON_BN_CLICKED(IDOK, &COutputInf::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_OUTPUT_MP4, &COutputInf::OnBnClickedRadioOutputMp4)
END_MESSAGE_MAP()


void GetConfigFoderPath(std::string &cfgFolderPath)
{
	PWSTR romingFolder = NULL; 
    const int MAX_STRINGZ=500;
    char path[MAX_STRINGZ+2];
	SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &romingFolder);
	WideCharToMultiByte(CP_ACP, 0, romingFolder, -1, path, MAX_STRINGZ,0,0);;
	CoTaskMemFree(romingFolder);
	cfgFolderPath = path;
	cfgFolderPath += "\\MCN\\OnyxVirtualStudio";
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
		MessageBox(NULL, TEXT("Could not init network interface(1)!"), TEXT("Configure"), IDOK);
		return;
	}

	int res = getaddrinfo(hostname,  0, &hint, &results);
	if(res == 0) {
		memcpy(&addr, results->ai_addr, sizeof(struct sockaddr));
		freeaddrinfo(results);
		strIpAddr = inet_ntoa(addr.sin_addr);
	} else {
		MessageBox(NULL, TEXT("Could not init network interface(2)!"), TEXT("Configure"), IDOK);
	}
}

int OnCaptureFile( HWND hDlg, char *szMp4File, int nDestType) 
{
    OPENFILENAMEA ofn={0};

    // Fill in standard structure fields
    ofn.lStructSize       = sizeof(OPENFILENAME);
    ofn.hwndOwner         = hDlg;
	if(nDestType == OUTPUT_TYPE_MP4){
		ofn.lpstrFilter       = "MP4 Files (*.mp4; *.m4v)\0*.mp4; *.m4v\0All Files (*.*)\0*.*\0\0";
	} else if(nDestType == OUTPUT_TYPE_HLS_HD) {
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

// COutputInf message handlers

void InitHlsLanAddress(std::string &HlsLanAddress)
{
	std::string strIpAddr;
	char szValue[MAX_PATH];
	std::string configFolder;
	GetConfigFoderPath(configFolder);
	std::string configMsc = configFolder + "\\msc.cfg";
	std::string HlsIpPort;
	std::string HlsResource;
	GetEth0LocaIpAddr(strIpAddr);

	GetPrivateProfileStringA("UserPreference", "HlsPort","59200",szValue,MAX_PATH - 1,configMsc.c_str());
	HlsIpPort = szValue;

	GetPrivateProfileStringA("UserPreference", "HlsResourceName","stream.m3u8",szValue,MAX_PATH - 1,configMsc.c_str());
	HlsResource = szValue;

	HlsLanAddress = "http://" + strIpAddr + ":" + HlsIpPort + "/" + HlsResource;
}

void InitRtspLanAddress(std::string &RtspLanAddress)
{
	std::string strIpAddr;
	char szValue[MAX_PATH];
	std::string configFolder;
	GetConfigFoderPath(configFolder);
	std::string configMsc = configFolder + "\\msc.cfg";
	std::string RtspIpPort;
	std::string RtspResource;
	GetEth0LocaIpAddr(strIpAddr);

	GetPrivateProfileStringA("UserPreference", "RtspPort","59400",szValue,MAX_PATH - 1,configMsc.c_str());
	RtspIpPort = szValue;

	GetPrivateProfileStringA("UserPreference", "RtspResourceName","stream",szValue,MAX_PATH - 1,configMsc.c_str());
	RtspResource = szValue;

	RtspLanAddress = "rtsp://" + strIpAddr + ":" + RtspIpPort + "/" + RtspResource;
}

void COutputInf::OnBnClickedRadioOutputHlsNet()
{
	m_StreamType = OUTPUT_TYPE_HLS_NET;
}

void COutputInf::OnBnClickedRadioOutputHlsS3()
{
	m_StreamType = OUTPUT_TYPE_HLS_S3;
}

void COutputInf::OnBnClickedRadioOutputRtsp()
{
	m_StreamType = OUTPUT_TYPE_RTSP;
}

void COutputInf::OnBnClickedRadioOutputHlsFile()
{
	m_StreamType = OUTPUT_TYPE_HLS_HD;
}

void COutputInf::OnBnClickedButtonConfigureS3()
{
	m_StreamType = OUTPUT_TYPE_HLS_S3;
}

void COutputInf::OnBnClickedRadioOutputMp4()
{
	m_StreamType = OUTPUT_TYPE_MP4;
}

void COutputInf::OnBnClickedButtonBrowsOutputMp4()
{
	char szFilePath[MAX_PATH] = {0};

	if(!m_Mp4OutputFile.IsEmpty())
		strcpy(szFilePath, (char *)(LPCTSTR)m_Mp4OutputFile);
	if(OnCaptureFile(this->m_hWnd, szFilePath, OUTPUT_TYPE_MP4) == TRUE){
		m_Mp4OutputFile = szFilePath;
		UpdateData(false);
	}
}

void COutputInf::OnBnClickedButtonBrowsOutputHls()
{
	char szFilePath[MAX_PATH] = {0};
	if(!m_HlsOutputFile.IsEmpty())
		strcpy(szFilePath, (char *)(LPCTSTR)m_HlsOutputFile);
	if(OnCaptureFile(this->m_hWnd, szFilePath, OUTPUT_TYPE_HLS_HD) == TRUE){
		m_HlsOutputFile = szFilePath;
		UpdateData(false);
	}
}

BOOL COutputInf::OnInitDialog()
{
	CDialog::OnInitDialog();
	std::string hlsAddress;
	std::string rtspAddress;

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	InitHlsLanAddress(hlsAddress);
	m_HlsLanAddress = hlsAddress.c_str();
	InitRtspLanAddress(rtspAddress);
	m_RtspLanAddress = rtspAddress.c_str();

	WSACleanup();

	if(!m_SelStreamPath.IsEmpty()){
		if(m_SelStreamPath.Find("rtsp:") != -1) {
			m_StreamType = OUTPUT_TYPE_RTSP;
			CheckDlgButton(IDC_RADIO_OUTPUT_RTSP, 1);
		} else if(m_SelStreamPath.Find("http:") != -1) {
			m_StreamType = OUTPUT_TYPE_HLS_NET;
			CheckDlgButton(IDC_RADIO_OUTPUT_HLS_NET, 1);
		} else if(m_SelStreamPath.Find(".mp4") != -1) {
			m_StreamType = OUTPUT_TYPE_MP4;
			m_Mp4OutputFile = m_SelStreamPath;
			CheckDlgButton(IDC_EDIT_OUTPUT_MP4, 1);
		} else if(m_SelStreamPath.Find(".m3u8") != -1) {
			m_StreamType = OUTPUT_TYPE_HLS_HD;
			m_HlsOutputFile = m_SelStreamPath;
			CheckDlgButton(IDC_RADIO_OUTPUT_HLS_FILE, 1);
		} else if(m_SelStreamPath.Find(".cfg") != -1) {
			m_StreamType = IDC_EDIT_OUTPUT_HLS_S3;
			m_HlsS3Address = m_SelStreamPath;
			CheckDlgButton(IDC_RADIO_OUTPUT_HLS_S3, 1);
		}
	}

	CDsUtil DsUtil;
	std::vector<std::string> mDeviceList;
	DsUtil.GetDeviceList(CAPTURE_DEVICE_CAREGORY_AUDIO, mDeviceList);

	m_ComboAudioInput.ResetContent();
	if(!mDeviceList.empty()){
		CString nameDevice="No Audio Input";
		m_ComboAudioInput.AddString(nameDevice);
		for (int i=0; i < mDeviceList.size(); i++){
			CString nameDevice="device://";
			nameDevice += mDeviceList[i].c_str();
			m_ComboAudioInput.AddString(nameDevice);
		}
	}

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COutputInf::OnBnClickedOk()
{
	//UpdateData(1);
	if(m_StreamType == OUTPUT_TYPE_HLS_NET) {
		m_SelStreamPath = m_HlsLanAddress;
	} else if(m_StreamType == OUTPUT_TYPE_HLS_HD) {
		m_SelStreamPath = m_HlsOutputFile;
	} else if(m_StreamType == OUTPUT_TYPE_HLS_S3) {
		m_SelStreamPath = m_HlsS3Address;
	} else if(m_StreamType == OUTPUT_TYPE_RTSP) {
		m_SelStreamPath = m_RtspLanAddress;
	} else if(m_StreamType == OUTPUT_TYPE_MP4) {
		m_SelStreamPath = m_Mp4OutputFile;
	}
	OnOK();
}

void COutputInf::OnEnChangeEditStreamPath4()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
