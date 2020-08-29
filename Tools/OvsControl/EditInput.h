#pragma once


// CEditInput dialog

class CEditInput : public CDialog
{
	DECLARE_DYNAMIC(CEditInput)

public:
	CEditInput(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditInput();

// Dialog Data
	enum { IDD = IDD_DIALOG_EDIT_STREAM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
