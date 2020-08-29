// EditSkype.cpp : implementation file
//

#include "stdafx.h"
#include "OvsControl.h"
#include "EditSkype.h"


// CEditSkype dialog

IMPLEMENT_DYNAMIC(CEditSkype, CDialog)

CEditSkype::CEditSkype(CWnd* pParent /*=NULL*/)
	: CDialog(CEditSkype::IDD, pParent)
	, m_EditSkypeVideoChannel(_T(""))
{

}

CEditSkype::~CEditSkype()
{
}

void CEditSkype::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SKYPE_VIDEO_CHANNEL, m_EditSkypeVideoChannel);
}


BEGIN_MESSAGE_MAP(CEditSkype, CDialog)
END_MESSAGE_MAP()


// CEditSkype message handlers

BOOL CEditSkype::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
