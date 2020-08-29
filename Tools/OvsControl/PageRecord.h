#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "DShowCapGraph.h"

typedef enum _CAPTURE_STATE_T
{
	CAPTURE_UNINIT,
	CAPTURE_STOP,
	CAPTURE_PAUSE,
	CAPTURE_RUN
} CAPTURE_STATE_T;

class CCustomEditListBox : public CVSListBox
{
	virtual void OnBrowse()
	{
		int nSel = GetSelItem();

		MessageBox(_T("Browse item..."));

		if (nSel == GetCount()) // New item
		{
			nSel = AddItem(_T("New text"));
			SelectItem(nSel);
		}
		else
		{
			SetItemText(nSel, _T("Updated text"));
		}
	}
};

class CMyBrowseEdit : public CMFCEditBrowseCtrl
{
	virtual void OnBrowse()
	{
		MessageBox(_T("Browse item..."));
		SetWindowText(_T("New value!"));
	}
};

/////////////////////////////////////////////////////////////////////////////
// CPageRecorder dialog
class CConfigRecord;

class CPageRecorder : public CMFCPropertyPage
{
	DECLARE_DYNCREATE(CPageRecorder)

// Construction
public:
	CPageRecorder();
	~CPageRecorder();
	virtual BOOL OnInitDialog();
	void ConfigBtn(CMFCButton &Btn, int nToolTipId);
// Dialog Data
	enum { IDD = IDD_PAGE_RECORD };

// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButonRecord();
	afx_msg void OnBnClickedButonPause();
	afx_msg void OnBnClickedButonResume();
	afx_msg void OnBnClickedButonStop();
	void Updatebuttons();
	void CreateSourceSelectionList();
	void SetComboExItem(CComboBoxEx *pCombo, int row, LPCTSTR itemText);

	CAPTURE_STATE_T    mCapState;
	CString m_EditAudioInput;
	CMFCButton    m_BtnRecord;
	CMFCButton    m_BtnPause;
	CMFCButton    m_BtnResume;
	CMFCButton    m_BtnStop;

	CSliderCtrl m_SliderAudBitrate;
	CSliderCtrl m_SliderVidBitrate;
	CSliderCtrl m_SliderGoplen;


	CString m_FileName;
	CString m_HlsFileName;
	void SaveConfig();
	void UpdateFileFormatUi(int nDestType);

	bool m_fModified;
	virtual BOOL OnApply();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	CBitmap m_Background;
	CBrush* m_pEditBkBrush;
	CBrush* m_pStaticBkBrush;

//	afx_msg void OnBnClickedButtonBrowseOut();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	CComboBoxEx m_ComboexSource;
	afx_msg void OnCbnSelchangeComboexDestFmt();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);

	void UpdateBitrateRanges();

protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	CCaptureGraph      *m_pCap;
	CConfigRecord    *m_pCfg;
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCbnSelchangeComboVideoInput();
	afx_msg void OnBnClickedButtonSetDefaults();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	int          m_StatusUpdateInterval;
};
