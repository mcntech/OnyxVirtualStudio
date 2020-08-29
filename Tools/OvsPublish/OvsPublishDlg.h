// OvsPublishDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// COvsPublishDlg dialog
class COvsPublishDlg : public CDialog
{
// Construction
public:
	COvsPublishDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_OVSPUBLISH_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	
	CString m_ConfigFile;
	BOOL InitParams();
	BOOL SaveParams();
	static UINT StartThread(LPVOID pParam);
	void threadUploadFolder();
	void StopThread();
	CWinThread *m_Thread; 
	bool m_fRunUpload;
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
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
	afx_msg void OnBnClickedOk();

	afx_msg void OnBnClickedButtonBrowseFolder();
	CProgressCtrl m_ProgressCrntFile;
	CProgressCtrl m_CtrlProgressTotal;
	CEdit m_CtrlStatus;
};
