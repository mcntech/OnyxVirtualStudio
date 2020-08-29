#pragma once


// CEditUdpOut dialog

class CEditUdpOut : public CDialog
{
	DECLARE_DYNAMIC(CEditUdpOut)

public:
	CEditUdpOut(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditUdpOut();

// Dialog Data
	enum { IDD = IDD_DIALOG_EDIT_UDP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString  m_HostAddr;
	int      m_RemotePort;
};
