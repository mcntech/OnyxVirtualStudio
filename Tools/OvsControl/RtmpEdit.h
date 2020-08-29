#pragma once


// CRtmpEdit dialog

class CRtmpEdit : public CDialog
{
	DECLARE_DYNAMIC(CRtmpEdit)

public:
	CRtmpEdit(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRtmpEdit();

// Dialog Data
	enum { IDD = IDD_DIALOG_EDIT_RTMP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_EditRtmpAddr;
	CString m_EditRtmpAddrBackup;
	BOOL m_fRecordPrimaryServer;
	BOOL m_fRecordSecondServer;
};
