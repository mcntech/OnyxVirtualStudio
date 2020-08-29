#pragma once
#include "PluginSkypeIf.h"
#include "afxwin.h"

// CSkypeConfig dialog

class CSkypeConfig : public CDialog
{
	DECLARE_DYNAMIC(CSkypeConfig)

public:
	CSkypeConfig(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSkypeConfig();

// Dialog Data
	enum { IDD = IDD_DIALOG_CONFIG_SKYPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_AudioInput;
	CString m_AudioOutput;
	CSkypeIf *m_pSkypeIf;
	CComboBox m_ComboAudInput;
	CComboBox m_ComboAudOutput;
};
