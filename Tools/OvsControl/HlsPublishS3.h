#pragma once


// CHlsPublishS3 dialog

class CHlsPublishS3 : public CDialog
{
	DECLARE_DYNAMIC(CHlsPublishS3)

public:
	CHlsPublishS3(CWnd* pParent = NULL);   // standard constructor
	virtual ~CHlsPublishS3();

// Dialog Data
	enum { IDD = IDD_HLS_PUBLISH_S3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_szFolder;
	CString m_szStream;
	CString m_szHost;
	CString m_szAccessId;
	CString m_szSecurityKey;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedNo();
	BOOL m_fLiveOnly;
	CString m_SegmentDuration;
	CString m_szBucket;
};
