// OvsConfigDlg.h : header file
//

#pragma once
#include "afxwin.h"


// COvsConfigDlg dialog
class COvsConfigDlg : public CDialog
{
// Construction
public:
	COvsConfigDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_OVSCONFIG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

public:
	void GetStreamPath(CString &StreamPath);

protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEditStream2();
	CString m_Stream1;
	CString m_Stream2;
	CString m_Stream3;
	CString m_Stream4;
	CString m_Stream5;
	CString m_Stream6;
	CString m_Anchor1;
	CString m_Output1;
	CString m_Output2;
	CString m_Advt1;
	CString m_Advt2;

	BOOL m_ChkStream1;
	BOOL m_ChkStream2;
	BOOL m_ChkStream3;
	BOOL m_ChkStream4;
	BOOL m_ChkStream5;
	BOOL m_ChkStream6;
	BOOL m_ChkAnchor1;
	BOOL m_ChkOutput1;
	BOOL m_ChkOutput2;
	BOOL m_ChkAdvt1;
	BOOL m_ChkAdvt2;

	BOOL m_ChkFlipVStream1;
	BOOL m_ChkFlipVStream2;
	BOOL m_ChkFlipVStream3;
	BOOL m_ChkFlipVStream4;
	BOOL m_ChkFlipVStream5;
	BOOL m_ChkFlipVStream6;
	BOOL m_ChkFlipVAnchor1;
	BOOL m_ChkFlipVAdvt1;
	BOOL m_ChkFlipVAdvt2;

	CString m_ConfigFile;

	afx_msg void OnBnClickedButtonStream1();
	afx_msg void OnBnClickedButtonStream2();
	afx_msg void OnBnClickedButtonStream3();
	afx_msg void OnBnClickedButtonStream4();
	afx_msg void OnBnClickedButtonStream5();
	afx_msg void OnBnClickedButtonStream6();
	afx_msg void OnBnClickedButtonAnchor1();
	afx_msg void OnBnClickedButtonAdvt1();
	afx_msg void OnBnClickedButtonAdvt2();

	BOOL m_ChkAnchor1ColorKey;
	afx_msg void OnBnClickedCheckFlipvAnchor1();
	CString m_EntityStream1;
	CString m_EntityStream2;
	CString m_EntityStream3;
	CString m_EntityStream4;
	CString m_EntityStream5;
	CString m_EntityStream6;
	CString m_EntityAnchor1;
	CString m_MaterialStream1;
	CString m_MaterialStream2;
	CString m_MaterialStream3;
	CString m_MaterialStream4;
	CString m_MaterialStream5;
	CString m_MaterialStream6;
	CString m_MaterialAnchor1;
	CString m_EntityAdv1;
	CString m_EntityAdv2;
	CString m_MaterialAdv1;
	CString m_MaterialAdv2;
};
