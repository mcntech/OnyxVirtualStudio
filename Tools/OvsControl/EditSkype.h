#pragma once


// CEditSkype dialog

class CEditSkype : public CDialog
{
	DECLARE_DYNAMIC(CEditSkype)

public:
	CEditSkype(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditSkype();

// Dialog Data
	enum { IDD = IDD_DIALOG_EDIT_SKYPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_EditSkypeVideoChannel;
};
