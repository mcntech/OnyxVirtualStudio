// OvsConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OvsConfig.h"
#include "OvsConfigDlg.h"
#include <Shlobj.h>
#include "StreamInf.h"
#include "OutputInf.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// COvsConfigDlg dialog




COvsConfigDlg::COvsConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COvsConfigDlg::IDD, pParent)
	, m_Stream1(_T(""))
	, m_Stream2(_T(""))
	, m_Stream3(_T(""))
	, m_Stream4(_T(""))
	, m_Stream5(_T(""))
	, m_Stream6(_T(""))
	, m_Anchor1(_T(""))
	, m_ChkStream1(FALSE)
	, m_ChkStream2(FALSE)
	, m_ChkStream3(FALSE)
	, m_ChkStream4(FALSE)
	, m_ChkStream5(FALSE)
	, m_ChkStream6(FALSE)
	, m_ChkAnchor1(FALSE)
	, m_ChkAdvt1(FALSE)
	, m_ChkAdvt2(FALSE)
	, m_ChkFlipVStream1(FALSE)
	, m_ChkFlipVStream2(FALSE)
	, m_ChkFlipVStream3(FALSE)
	, m_ChkFlipVStream4(FALSE)
	, m_ChkFlipVStream5(FALSE)
	, m_ChkFlipVStream6(FALSE)
	, m_ChkFlipVAnchor1(FALSE)
	, m_ChkFlipVAdvt1(FALSE)
	, m_ChkFlipVAdvt2(FALSE)

	, m_ConfigFile(_T(""))
	, m_Advt1(_T(""))
	, m_Advt2(_T(""))
	, m_ChkAnchor1ColorKey(FALSE)
	, m_EntityStream1(_T(""))
	, m_EntityStream2(_T(""))
	, m_EntityStream3(_T(""))
	, m_EntityStream4(_T(""))
	, m_EntityStream5(_T(""))
	, m_EntityStream6(_T(""))
	, m_EntityAnchor1(_T(""))
	, m_MaterialStream1(_T(""))
	, m_MaterialStream2(_T(""))
	, m_MaterialStream3(_T(""))
	, m_MaterialStream4(_T(""))
	, m_MaterialStream5(_T(""))
	, m_MaterialStream6(_T(""))
	, m_MaterialAnchor1(_T(""))
	, m_EntityAdv1(_T(""))
	, m_EntityAdv2(_T(""))
	, m_MaterialAdv1(_T(""))
	, m_MaterialAdv2(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COvsConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_STREAM2, m_Stream2);
	DDX_Text(pDX, IDC_EDIT_STREAM3, m_Stream3);
	DDV_MaxChars(pDX, m_Stream3, 256);
	DDX_Text(pDX, IDC_EDIT_STREAM4, m_Stream4);
	DDV_MaxChars(pDX, m_Stream4, 256);
	DDX_Text(pDX, IDC_EDIT_STREAM6, m_Stream6);
	DDV_MaxChars(pDX, m_Stream6, 256);
	DDX_Text(pDX, IDC_EDIT_ANCHOR1, m_Anchor1);
	DDV_MaxChars(pDX, m_Anchor1, 256);
	DDX_Text(pDX, IDC_EDIT_STREAM1, m_Stream1);
	DDV_MaxChars(pDX, m_Stream1, 256);
	DDX_Text(pDX, IDC_EDIT_STREAM5, m_Stream5);
	DDV_MaxChars(pDX, m_Stream5, 256);
	DDX_Check(pDX, IDC_CHECK_STREAM1, m_ChkStream1);
	DDX_Check(pDX, IDC_CHECK_STREAM2, m_ChkStream2);
	DDX_Check(pDX, IDC_CHECK_STREAM3, m_ChkStream3);
	DDX_Check(pDX, IDC_CHECK_STREAM4, m_ChkStream4);
	DDX_Check(pDX, IDC_CHECK_STREAM5, m_ChkStream5);
	DDX_Check(pDX, IDC_CHECK_STREAM6, m_ChkStream6);
	DDX_Check(pDX, IDC_CHECK_ANCHOR1, m_ChkAnchor1);
	DDX_Check(pDX, IDC_CHECK_ADVT1, m_ChkAdvt1);
	DDX_Check(pDX, IDC_CHECK_ADVT2, m_ChkAdvt2);
	DDX_Check(pDX, IDC_CHECK_FLIPV_STREAM1, m_ChkFlipVStream1);
	DDX_Check(pDX, IDC_CHECK_FLIPV_STREAM2, m_ChkFlipVStream2);
	DDX_Check(pDX, IDC_CHECK_FLIPV_STREAM3, m_ChkFlipVStream3);
	DDX_Check(pDX, IDC_CHECK_FLIPV_STREAM4, m_ChkFlipVStream4);
	DDX_Check(pDX, IDC_CHECK_FLIPV_STREAM5, m_ChkFlipVStream5);
	DDX_Check(pDX, IDC_CHECK_FLIPV_STREAM6, m_ChkFlipVStream6);
	DDX_Check(pDX, IDC_CHECK_FLIPV_ANCHOR1, m_ChkFlipVAnchor1);
	DDX_Check(pDX, IDC_CHECK_FLIPV_ADVT1, m_ChkFlipVAdvt1);
	DDX_Check(pDX, IDC_CHECK_FLIPV_ADVT2, m_ChkFlipVAdvt2);

	DDX_Check(pDX, IDC_CHECK_ANCHOR1_COLORKEY, m_ChkAnchor1ColorKey);

	DDX_Text(pDX, IDC_EDIT_ADVT1, m_Advt1);
	DDX_Text(pDX, IDC_EDIT_ADVT2, m_Advt2);
	DDX_Text(pDX, IDC_EDIT_CONFIGFILE, m_ConfigFile);



	DDX_Text(pDX, IDC_EDIT_3D_ENTITY_STRAEM_1, m_EntityStream1);
	DDX_Text(pDX, IDC_EDIT_3D_ENTITY_STRAEM_2, m_EntityStream2);
	DDX_Text(pDX, IDC_EDIT_3D_ENTITY_STRAEM_3, m_EntityStream3);
	DDX_Text(pDX, IDC_EDIT_3D_ENTITY_STRAEM_4, m_EntityStream4);
	DDX_Text(pDX, IDC_EDIT_3D_ENTITY_STRAEM_5, m_EntityStream5);
	DDX_Text(pDX, IDC_EDIT_3D_ENTITY_STRAEM_6, m_EntityStream6);
	DDX_Text(pDX, IDC_EDIT_3D_ENTITY_ANCHOR1, m_EntityAnchor1);
	DDX_Text(pDX, IDC_EDIT_3D_MATERIAL_STREAM1, m_MaterialStream1);
	DDX_Text(pDX, IDC_EDIT_3D_MATERIAL_STREAM2, m_MaterialStream2);
	DDX_Text(pDX, IDC_EDIT_3D_MATERIAL_STREAM3, m_MaterialStream3);
	DDX_Text(pDX, IDC_EDIT_3D_MATERIAL_STREAM4, m_MaterialStream4);
	DDX_Text(pDX, IDC_EDIT_3D_MATERIAL_STREAM5, m_MaterialStream5);
	DDX_Text(pDX, IDC_EDIT_3D_MATERIAL_STREAM6, m_MaterialStream6);
	DDX_Text(pDX, IDC_EDIT_3D_MATERIAL_ANCHOR1, m_MaterialAnchor1);
	DDX_Text(pDX, IDC_EDIT_3D_ENTITY_ADV1, m_EntityAdv1);
	DDX_Text(pDX, IDC_EDIT_3D_ENTITY_ADV2, m_EntityAdv2);
	DDX_Text(pDX, IDC_EDIT_3D_MATERIAL_ADV1, m_MaterialAdv1);
	DDX_Text(pDX, IDC_EDIT_3D_MATERIAL_ADV2, m_MaterialAdv2);
}

BEGIN_MESSAGE_MAP(COvsConfigDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_EDIT_STREAM2, &COvsConfigDlg::OnEnChangeEditStream2)
	ON_BN_CLICKED(IDC_BUTTON_STREAM1, &COvsConfigDlg::OnBnClickedButtonStream1)
	ON_BN_CLICKED(IDC_BUTTON_STREAM2, &COvsConfigDlg::OnBnClickedButtonStream2)
	ON_BN_CLICKED(IDC_BUTTON_STREAM3, &COvsConfigDlg::OnBnClickedButtonStream3)
	ON_BN_CLICKED(IDC_BUTTON_STREAM4, &COvsConfigDlg::OnBnClickedButtonStream4)
	ON_BN_CLICKED(IDC_BUTTON_STREAM5, &COvsConfigDlg::OnBnClickedButtonStream5)
	ON_BN_CLICKED(IDC_BUTTON_STREAM6, &COvsConfigDlg::OnBnClickedButtonStream6)
	ON_BN_CLICKED(IDC_BUTTON_ANCHOR1, &COvsConfigDlg::OnBnClickedButtonAnchor1)
	ON_BN_CLICKED(IDC_BUTTON_ADVT1, &COvsConfigDlg::OnBnClickedButtonAdvt1)
ON_BN_CLICKED(IDC_BUTTON_ADVT2, &COvsConfigDlg::OnBnClickedButtonAdvt2)
ON_BN_CLICKED(IDC_CHECK_FLIPV_ANCHOR1, &COvsConfigDlg::OnBnClickedCheckFlipvAnchor1)
END_MESSAGE_MAP()


// COvsConfigDlg message handlers

BOOL COvsConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void COvsConfigDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void COvsConfigDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR COvsConfigDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void COvsConfigDlg::OnEnChangeEditStream2()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void COvsConfigDlg::GetStreamPath(CString &StreamPath)
{
	CStreamInf dlg;
	UpdateData(true);
	dlg.m_SelStreamPath = StreamPath;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)	{
		StreamPath = dlg.m_SelStreamPath;
		UpdateData(false);
	}
}

void COvsConfigDlg::OnBnClickedButtonStream1()
{
	GetStreamPath(m_Stream1);
}

void COvsConfigDlg::OnBnClickedButtonStream2()
{
		GetStreamPath(m_Stream2);
}

void COvsConfigDlg::OnBnClickedButtonStream3()
{
	GetStreamPath(m_Stream3);
}

void COvsConfigDlg::OnBnClickedButtonStream4()
{
	GetStreamPath(m_Stream4);
}

void COvsConfigDlg::OnBnClickedButtonStream5()
{
	GetStreamPath(m_Stream5);
}

void COvsConfigDlg::OnBnClickedButtonStream6()
{
	GetStreamPath(m_Stream6);
}

void COvsConfigDlg::OnBnClickedButtonAnchor1()
{
	GetStreamPath(m_Anchor1);
}


void COvsConfigDlg::OnBnClickedButtonAdvt1()
{
	GetStreamPath(m_Advt1);
}

void COvsConfigDlg::OnBnClickedButtonAdvt2()
{
	GetStreamPath(m_Advt2);
}

void COvsConfigDlg::OnBnClickedCheckFlipvAnchor1()
{
	// TODO: Add your control notification handler code here
}
