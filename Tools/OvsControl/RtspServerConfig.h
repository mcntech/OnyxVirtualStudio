#pragma once


// CRtspServerConfig dialog

class CRtspServerConfig : public CDialog
{
	DECLARE_DYNAMIC(CRtspServerConfig)

public:
	CRtspServerConfig(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRtspServerConfig();

// Dialog Data
	enum { IDD = IDD_DIALOG_EDIT_RTSP_SRV };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_StreamName;
	int m_Port;
};
