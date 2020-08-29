#pragma once
#include "afxwin.h"


// CSelectCamera dialog

class CSelectCamera : public CDialog
{
	DECLARE_DYNAMIC(CSelectCamera)

public:
	CSelectCamera(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectCamera();

// Dialog Data
	enum { IDD = IDD_DIALOG_EDIT_CAMERA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_CameraName;
	virtual BOOL OnInitDialog();
	CComboBox m_ComboCamera;
};
