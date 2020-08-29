#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "DshowCapGraph.h"
#include <string>
#include "ConfigLiveCast.h"


/////////////////////////////////////////////////////////////////////////////
// CPageLivecast dialog

class CPageLivecast : public CMFCPropertyPage
{
	DECLARE_DYNCREATE(CPageLivecast)

// Construction
public:
	CPageLivecast();
	~CPageLivecast();
	virtual BOOL OnInitDialog();
	void ConfigBtn(CMFCButton &Btn, int nToolTipId);
// Dialog Data
	enum { IDD = IDD_PAGE_LIVECAST };

	static int CreatePropPages(CPageLivecast *pPages[], int nMaxPages);
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

	CImageList m_ImgListSrcType;
	void SetComboExItem(CComboBoxEx *pCombo, int row, CString itemText);

	int           mCapState;
	CString       m_EditAudioInput;
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


	long m_VidBitrate;
	long m_AudBitrate;
	long m_GopLength;
	bool m_fModified;
	virtual BOOL OnApply();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	CBitmap m_Background;
	CBrush* m_pEditBkBrush;
	CBrush* m_pStaticBkBrush;
	CConfigLiveCast    *m_pCfg;
	CCaptureGraph *m_pCap;
//	afx_msg void OnBnClickedButtonBrowseOut();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	CComboBoxEx m_ComboexVidSize;
	afx_msg void OnCbnSelchangeComboexInputSource();
	afx_msg void OnCbnSelchangeComboexVidOutSize();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	std::string mStreamName;

	void UpdateEncodeParams();

protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	afx_msg void OnBnClickedButtonConfigure();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	int          m_StatusUpdateInterval;
	void UpdateBitrateRanges();
	void CreateDestSelectionList();
	void SelectDestinationFormat(int nSel);

	std::vector<CVidDestFmtInf *>  mDestFormats;
	int                            mCrntDestFmt;
	CAVSrcInf    mAVSrcInf;
	afx_msg void OnBnClickedButtonSetDefaults();
	CComboBoxEx m_ComboExDestForamt;
	afx_msg void OnCbnSelchangeComboexDestFormat();
};
