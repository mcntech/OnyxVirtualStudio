// RtspServerConfig.cpp : implementation file
//

#include "stdafx.h"
#include "OvsControl.h"
#include "RtspServerConfig.h"
#include "afxdialogex.h"


// CRtspServerConfig dialog

IMPLEMENT_DYNAMIC(CRtspServerConfig, CDialog)

CRtspServerConfig::CRtspServerConfig(CWnd* pParent /*=NULL*/)
	: CDialog(CRtspServerConfig::IDD, pParent)
	, m_StreamName(_T(""))
	, m_Port(0)
{

}

CRtspServerConfig::~CRtspServerConfig()
{
}

void CRtspServerConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ADDR, m_StreamName);
	DDX_Text(pDX, IDC_EDIT_PORT, m_Port);
}


BEGIN_MESSAGE_MAP(CRtspServerConfig, CDialog)
END_MESSAGE_MAP()


// CRtspServerConfig message handlers
