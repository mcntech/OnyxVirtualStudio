// RtmpEdit.cpp : implementation file
//

#include "stdafx.h"
//#include "OnyxLiveCast.h"
#include "OvsControl.h"
#include "RtmpEdit.h"


// CRtmpEdit dialog

IMPLEMENT_DYNAMIC(CRtmpEdit, CDialog)

CRtmpEdit::CRtmpEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CRtmpEdit::IDD, pParent)
	, m_EditRtmpAddr(_T(""))
	, m_EditRtmpAddrBackup(_T(""))
	, m_fRecordPrimaryServer(FALSE)
	, m_fRecordSecondServer(FALSE)
{

}

CRtmpEdit::~CRtmpEdit()
{
}

void CRtmpEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_RTMP_ADDR, m_EditRtmpAddr);
	DDX_Text(pDX, IDC_EDIT_RTMP_ADDR_BACKUP, m_EditRtmpAddrBackup);
	DDX_Check(pDX, IDC_CHECK_SERVER1_RECORD, m_fRecordPrimaryServer);
	DDX_Check(pDX, IDC_CHECK_SERVER2_RECORD, m_fRecordSecondServer);
}


BEGIN_MESSAGE_MAP(CRtmpEdit, CDialog)
END_MESSAGE_MAP()


// CRtmpEdit message handlers
