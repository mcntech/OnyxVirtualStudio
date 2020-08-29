#pragma once


// CHlsOutConfigure dialog

class CHlsOutConfigure : public CDialog
{
	DECLARE_DYNAMIC(CHlsOutConfigure)

public:
	CHlsOutConfigure(CWnd* pParent = NULL);   // standard constructor
	virtual ~CHlsOutConfigure();

// Dialog Data
	enum { IDD = IDD_DIALOG_EDIT_HLS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_StreamName;
	int m_Port;
	CString m_szFolder;
	CString m_szSrvRoot;
	int m_SegmentDuration;
	BOOL m_fLiveOnly;
};
