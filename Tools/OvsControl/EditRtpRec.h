#pragma once
#include "afxwin.h"


// CEditRtpRec dialog

class CEditRtpRec : public CDialog
{
	DECLARE_DYNAMIC(CEditRtpRec)

public:
	CEditRtpRec(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditRtpRec();

// Dialog Data
	enum { IDD = IDD_DIALOG_EDIT_RTP_REC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_HostAddr;
	CString m_UserId;
	CString m_Passwd;
	int		m_RemotePort;
	CString m_StreamName;
};
