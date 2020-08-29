#pragma once


// CEditRtsp dialog

class CEditRtsp : public CDialog
{
	DECLARE_DYNAMIC(CEditRtsp)

public:
	CEditRtsp(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditRtsp();

// Dialog Data
	enum { IDD = IDD_DIALOG_EDIT_RTSP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_RtspAddr;
};
