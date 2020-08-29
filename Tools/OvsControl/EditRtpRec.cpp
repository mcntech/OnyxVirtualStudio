// EditRtpRec.cpp : implementation file
//

#include "stdafx.h"
//#include "OnyxLiveCast.h"
#include "OvsControl.h"
#include "EditRtpRec.h"


// CEditRtpRec dialog

IMPLEMENT_DYNAMIC(CEditRtpRec, CDialog)

CEditRtpRec::CEditRtpRec(CWnd* pParent /*=NULL*/)
	: CDialog(CEditRtpRec::IDD, pParent)
	, m_HostAddr(_T(""))
	, m_UserId(_T(""))
	, m_Passwd(_T(""))
	, m_RemotePort(0)
	, m_StreamName(_T(""))
{

}

CEditRtpRec::~CEditRtpRec()
{
}

void CEditRtpRec::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_RTPREC_HOST_ADDR, m_HostAddr);
	DDX_Text(pDX, IDC_EDIT_RTPREC_USER_ID, m_UserId);
	DDX_Text(pDX, IDC_EDIT_RTPREC_PASSWD, m_Passwd);
	DDX_Text(pDX, IDC_EDIT_RTPREC_HOST_PORT, m_RemotePort);
	DDX_Text(pDX, IDC_EDIT_RTPREC_STREAM_NAME, m_StreamName);
}


BEGIN_MESSAGE_MAP(CEditRtpRec, CDialog)
END_MESSAGE_MAP()


// CEditRtpRec message handlers
