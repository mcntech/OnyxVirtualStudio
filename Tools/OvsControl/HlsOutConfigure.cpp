// HlsOutConfigure.cpp : implementation file
//

#include "stdafx.h"
#include "OvsControl.h"
#include "HlsOutConfigure.h"
#include "afxdialogex.h"


// CHlsOutConfigure dialog

IMPLEMENT_DYNAMIC(CHlsOutConfigure, CDialog)

CHlsOutConfigure::CHlsOutConfigure(CWnd* pParent /*=NULL*/)
	: CDialog(CHlsOutConfigure::IDD, pParent)
	, m_StreamName(_T(""))
	, m_Port(0)
	, m_szFolder(_T(""))
	, m_szSrvRoot(_T(""))
	, m_SegmentDuration(0)
	, m_fLiveOnly(FALSE)
{

}

CHlsOutConfigure::~CHlsOutConfigure()
{
}

void CHlsOutConfigure::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ADDR, m_StreamName);
	DDX_Text(pDX, IDC_EDIT_PORT, m_Port);
	DDX_Text(pDX, IDC_EDIT_FOLDER, m_szFolder);
	DDX_Text(pDX, IDC_EDIT_SRV_ROOT, m_szSrvRoot);
	DDX_Text(pDX, IDC_EDIT_SEGMENT_DURATION, m_SegmentDuration);
	DDX_Check(pDX, IDC_CHECK_LIVEONLY, m_fLiveOnly);
}


BEGIN_MESSAGE_MAP(CHlsOutConfigure, CDialog)
END_MESSAGE_MAP()


// CHlsOutConfigure message handlers
