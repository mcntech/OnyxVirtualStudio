// EditRtsp.cpp : implementation file
//

#include "stdafx.h"
#include "OvsControl.h"
#include "EditRtsp.h"


// CEditRtsp dialog

IMPLEMENT_DYNAMIC(CEditRtsp, CDialog)

CEditRtsp::CEditRtsp(CWnd* pParent /*=NULL*/)
	: CDialog(CEditRtsp::IDD, pParent)
	, m_RtspAddr(_T(""))
{

}

CEditRtsp::~CEditRtsp()
{
}

void CEditRtsp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_RTSP_ADDR, m_RtspAddr);
}


BEGIN_MESSAGE_MAP(CEditRtsp, CDialog)
END_MESSAGE_MAP()


// CEditRtsp message handlers

BOOL CEditRtsp::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
