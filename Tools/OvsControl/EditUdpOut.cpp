// EditUdpOut.cpp : implementation file
//

#include "stdafx.h"
#include "OvsControl.h"
#include "EditUdpOut.h"
#include "afxdialogex.h"


// CEditUdpOut dialog

IMPLEMENT_DYNAMIC(CEditUdpOut, CDialog)

CEditUdpOut::CEditUdpOut(CWnd* pParent /*=NULL*/)
	: CDialog(CEditUdpOut::IDD, pParent)
	, m_HostAddr(_T(""))
	, m_RemotePort(0)
{

}

CEditUdpOut::~CEditUdpOut()
{
}

void CEditUdpOut::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ADDR, m_HostAddr);
	DDX_Text(pDX, IDC_EDIT_PORT, m_RemotePort);
}


BEGIN_MESSAGE_MAP(CEditUdpOut, CDialog)
END_MESSAGE_MAP()


// CEditUdpOut message handlers
