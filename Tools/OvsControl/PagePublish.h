// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CPagepublish dialog

class CPagepublish : public CMFCPropertyPage
{
	DECLARE_DYNCREATE(CPagepublish)

// Construction
public:
	CPagepublish();
	~CPagepublish();

// Dialog Data
	enum { IDD = IDD_PAGE_PUBLISH };

// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CString m_ConfigFile;
	BOOL InitParams();
	BOOL SaveParams();
	static UINT StartThread(LPVOID pParam);
	void threadUploadFolder();
	void StopThread();
	CWinThread *m_Thread; 
	bool m_fRunUpload;
protected:
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CString m_S3HostName;
	CString m_S3SecurityKey;
	CString m_S3BucketName;
	CString m_S3AccessId;
	CString m_VideoChannel;
	CString m_LocalFolder;
	afx_msg void OnBnClickedButtonStartUpload();
	afx_msg void OnBnClickedButtonCancelUpload();
	afx_msg void OnBnClickedButtonClearChannel();

	afx_msg void OnBnClickedButtonBrowseFolder();
	CProgressCtrl m_ProgressCrntFile;
	CProgressCtrl m_CtrlProgressTotal;
	CEdit m_CtrlStatus;

	afx_msg void OnEnChangeEditS3AccessId();
	afx_msg void OnEnChangeEditS3SecurityKey();
	afx_msg void OnEnChangeEditS3Bucket();
	afx_msg void OnEnChangeEditS3Folder();
	afx_msg void OnEnChangeEditLocalFolder();
	virtual BOOL OnApply();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	CBitmap m_Background;
	CBrush* m_pEditBkBrush;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
};

