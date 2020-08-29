#pragma once
#include "afxwin.h"

// CStreamInf dialog
typedef enum _OUTPUT_TYPE_T
{
	OUTPUT_TYPE_HLS_NET,
	OUTPUT_TYPE_HLS_HD,
	OUTPUT_TYPE_HLS_S3,
	OUTPUT_TYPE_RTSP,
	OUTPUT_TYPE_MP4,
} OUTPUT_TYPE_T;

// COutputInf dialog

class COutputInf : public CDialog
{
	DECLARE_DYNAMIC(COutputInf)

public:
	COutputInf(CWnd* pParent = NULL);   // standard constructor
	virtual ~COutputInf();

	int     m_StreamType;
	CString m_SelStreamPath;
// Dialog Data
	enum { IDD = IDD_DIALOG_OUTPUTINF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_HlsLanAddress;
	CString m_HlsS3Address;
	CString m_RtspLanAddress;
	CString m_Mp4OutputFile;
	CString m_HlsOutputFile;
	afx_msg void OnBnClickedRadioOutputHlsNet();
	afx_msg void OnBnClickedRadioOutputHlsS3();
	afx_msg void OnBnClickedRadioOutputRtsp();
	afx_msg void OnBnClickedRadioOutputHlsFile();
	afx_msg void OnBnClickedButtonConfigureS3();
	afx_msg void OnBnClickedButtonBrowsOutputMp4();
	afx_msg void OnBnClickedButtonBrowsOutputHls();
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeEditStreamPath4();
	afx_msg void OnBnClickedRadioOutputMp4();
	CString m_AudioInput;
	CComboBox m_ComboAudioInput;
};
