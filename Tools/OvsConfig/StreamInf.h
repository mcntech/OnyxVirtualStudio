#pragma once
#include "afxwin.h"


// CStreamInf dialog
typedef enum _STREAM_TYPE_T
{
	STREAM_TYPE_FILE,
	STREAM_TYPE_CAMERA,
	STREAM_TYPE_RTSP,
	STREAM_TYPE_SKYPE,
	STREAM_TYPE_SIP,
} STREAM_TYPE_T;

class CStreamInf : public CDialog
{
	DECLARE_DYNAMIC(CStreamInf)

public:
	CStreamInf(CWnd* pParent = NULL);   // standard constructor
	virtual ~CStreamInf();
	int m_StreamType;

// Dialog Data
	enum { IDD = IDD_DIALOG_STREAMINF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEditStreamPath();

	afx_msg void OnBnClickedButtonBrowsPath();
	afx_msg void OnBnClickedRadioStreamFile();
	afx_msg void OnBnClickedRadioStreamCamera();
	afx_msg void OnBnClickedRadioStreamRtsp();
	afx_msg void OnBnClickedRadioStreamSkype();
	afx_msg void OnBnClickedRadioStreamSip();
	CComboBox m_ComboCamera;
	CString m_PathCamera;
	CString m_StreamPath;
	CButton m_BtnBrowse;
	CEdit   m_EditStreamPath;

	CString m_RtspPath;
	CString m_SipPath;
	CString m_SKypePath;
	CString m_FilePath;

	CString m_SelStreamPath;
public:
	void UpdateButtons();
	CEdit m_EditExample;
	CString m_Example;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelendokComboCamera();
	CString mSkypeId;
	CString m_LablePath;
	CEdit m_CtrlPasswd;
	CString m_Passwd;
	CStatic m_LabelPasswd;
	afx_msg void OnStnClickedStaticPasswd2();
};
