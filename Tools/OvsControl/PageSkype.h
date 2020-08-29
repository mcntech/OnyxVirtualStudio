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
// CPageSkype dialog
#include "PluginSkypeIf.h"
#include "afxwin.h"
#include "customproperties.h"

class CStatusLed : public CStatic
{
	DECLARE_DYNAMIC(CStatusLed)

public:
	CStatusLed(){m_fState = false;}
	virtual ~CStatusLed(){}
	void SetState(bool fOn);
protected:
   afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
private:
	bool m_fState;
};

class CPageSkype : public CMFCPropertyPage, public CSkypeCallback
{
	friend class CStrmListCtrl;
	DECLARE_DYNCREATE(CPageSkype)

// Construction
public:
	CPageSkype();
	~CPageSkype();

// Dialog Data
	enum { IDD = IDD_PAGE_SKYPE };

// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	virtual BOOL OnInitDialog();
	BOOL HideDialogDeatils();
	BOOL HideLoginDeatils();
	BOOL ShowDialogDetails();
	BOOL ShowLoginDetails();
	void ConfigBtn(CMFCButton &Btn, int nToolTipId);

	DECLARE_MESSAGE_MAP()

public:
	CString m_SkypeId;
	CString m_Passwd;
	CString mOnyxShareMemName;
	afx_msg void OnBnClickedButtonLogin();
	afx_msg void OnBnClickedButtonLogout();
	bool m_LoginOk;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	CBitmap m_Background;
	CBrush* m_pEditBkBrush;


	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	CSkypeIf   *m_pSkypeIf;
	CString m_SkypeStatus;
	afx_msg void OnDestroy();
	int LoginStatus(int StatusId, void *pInfo);
	int VideoStatus(int StatusId, void *pInfo);
	void ResetVideoStatus();
	int ConversationStatus(int StatusId, void *pInfo);
	int ParticipantStatus(int StatusId, void *pInfo);
	void LiveSessionClosed();
	void OtherSessionClosed();
	void AddContact(SKYPE_CONTACT_INF_T *pContactInf);
	int PaintImage(const char *pFrameBuff, int nLen, CImageList &ImgListSkype);
	CStrmListCtrl m_SkypeContacts;
	CImageList m_ImgListSkype;

	afx_msg void OnBnClickedButtonSkypeCall();
	afx_msg void OnBnClickedButtonSkypeHangup();
	CMFCButton m_AutoReply;
	CMFCButton m_BtnAccept;
	CMFCButton m_BtnReject;

	afx_msg void OnBnClickedAutoReply();
	bool m_fRingingForMe;
	bool m_fCalling;
	afx_msg void OnBnClickedButtonAddAutoreply();
	afx_msg void OnBnClickedButtonDelAutoreply();
	afx_msg void OnNMDblclkListSkypeAutoreply(NMHDR *pNMHDR, LRESULT *pResult);
	CString m_strAutoReply;
	afx_msg void OnEnKillfocusEditAutoReply();
	CStatusLed m_VideoInStatus;
	CStatusLed m_VideoOutStatus;
	afx_msg void OnEnChangeEditAutoReply();
	virtual void OnOK();

	afx_msg void OnBnClickedButtonSkypeConfig();
	CString m_AudioInput;
	CString m_AudioOutput;
	CMFCButton m_BtnLogin;
	CMFCButton m_BtnLogout;
	CMFCButton m_BtnSkypeConfig;
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};

class CSkypeInf
{
public:
	CSkypeInf() : m_SkypeId(_T(""))
		,m_Passwd(_T(""))
		,m_LoginOk(false)
	{
	}
	~CSkypeInf(){}
	CString m_SkypeId;
	CString m_Passwd;
	bool m_LoginOk;
	static CSkypeInf *mInstance;
	static CSkypeInf *Instance();
};