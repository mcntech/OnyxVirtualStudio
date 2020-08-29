// HlsPublishS3.cpp : implementation file
//

#include "stdafx.h"
#include "OvsControl.h"
#include "HlsPublishS3.h"
#include "afxdialogex.h"


// CHlsPublishS3 dialog

IMPLEMENT_DYNAMIC(CHlsPublishS3, CDialog)

CHlsPublishS3::CHlsPublishS3(CWnd* pParent /*=NULL*/)
	: CDialog(CHlsPublishS3::IDD, pParent)
	, m_szFolder(_T(""))
	, m_szStream(_T(""))
	, m_szHost(_T(""))
	, m_szAccessId(_T(""))
	, m_szSecurityKey(_T(""))
	, m_fLiveOnly(FALSE)
	, m_SegmentDuration(_T(""))
	, m_szBucket(_T(""))
{

}

CHlsPublishS3::~CHlsPublishS3()
{
}

void CHlsPublishS3::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FOLDER, m_szFolder);
	DDX_Text(pDX, IDC_EDIT_STREAM, m_szStream);
	DDX_Text(pDX, IDC_EDIT_HOST_NAME, m_szHost);
	DDX_Text(pDX, IDC_EDIT_ACCESS_ID, m_szAccessId);
	DDX_Text(pDX, IDC_EDIT_SECURITY_KEY, m_szSecurityKey);
	DDX_Check(pDX, IDC_CHECK_LIVEONLY, m_fLiveOnly);
	DDX_Text(pDX, IDC_EDIT_SEGMENT_DURATION, m_SegmentDuration);
	DDX_Text(pDX, IDC_EDIT_BUCKET_NAME, m_szBucket);
}


BEGIN_MESSAGE_MAP(CHlsPublishS3, CDialog)
	ON_BN_CLICKED(IDOK, &CHlsPublishS3::OnBnClickedOk)
	ON_BN_CLICKED(IDNO, &CHlsPublishS3::OnBnClickedNo)
END_MESSAGE_MAP()


// CHlsPublishS3 message handlers


void CHlsPublishS3::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}


void CHlsPublishS3::OnBnClickedNo()
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}
