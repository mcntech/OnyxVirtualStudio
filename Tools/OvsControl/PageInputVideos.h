#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "customproperties.h"
#include <map>

// CStreamInf dialog
typedef enum _STREAM_TYPE_T
{
	STREAM_TYPE_NOSRC,
	STREAM_TYPE_CAMERA,
	STREAM_TYPE_FILE,
	STREAM_TYPE_RTSP,
	STREAM_TYPE_SKYPE,
	STREAM_TYPE_SIP,
} STREAM_TYPE_T;

typedef enum _ROTATION_TYPE_T
{
	ROTATION_TYPE_0,
	ROTATION_TYPE_90,
	ROTATION_TYPE_180,
	ROTATION_TYPE_270,
} ROTATION_TYPE_T;


class CVideoPlane : public CStatic
{
	DECLARE_DYNAMIC(CVideoPlane)

public:
	CVideoPlane(){}
	virtual ~CVideoPlane(){}
	void SetPlane(CString PlaneId);
protected:
   afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
private:
	CString m_PlaneId;
};

class CStreamInf
{
public:
	CStreamInf(){}
	void Save();
	void Load(const char *szConfigFile, const char *szSection);

	int        mCropLeft;
	int        mCropRight;
	int        mCropTop;
	int        mCropBottom;

private:
	std::string mSectionName;
	std::string mConfigFile;
};

/////////////////////////////////////////////////////////////////////////////
// CPageInputVideos dialog

class CPageInputVideos : public CMFCPropertyPage
{
	friend class CStrmListCtrl;

	DECLARE_DYNCREATE(CPageInputVideos)

// Construction
public:
	CPageInputVideos();
	~CPageInputVideos();

// Dialog Data
	enum { IDD = IDD_PAGE_INPUT_VIDEOS };

// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDialogAbout();

	DECLARE_MESSAGE_MAP()

	CStrmListCtrl m_ctrlStrmList;
	
	void ConfigBtn(CMFCButton &Btn, int nToolTipId);
	void UpdateButtons();
	int OnGetStreamFilePath(char *szFile);
	int GetStreamPath(CString &StmPath, CString &StmType);
	int GetRowForPlane(CString Plane);
	void InitStreamList(CString ConfigFile, const char *szPrefix, int nCountStreams);
	void StrmListInsertImage(int row, int nImgId);
	void StrmListSetImage(int row, int nImgId);
	CComboBox m_ComboCamera;
	CButton m_BtnBrowse;
	CMFCButton m_BtnUpdate;
	CMFCButton m_BtnEdit;
	CMFCButton m_BtnResetVidList;

	CMFCButton m_BtnColorKeyEanble;
	CMFCButton m_BtnUpsideDown;
	CImageList m_ImgListSrcType;
	CImageList   m_ImgListRotationType;
	CVideoPlane  m_VideoPlane;
	CImageList m_ImgListSrcTypeSmall;

	bool m_fModified;


	CString m_EditPlaneId;
	CString m_SelStreamPath;

	virtual BOOL OnApply();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();

	void SaveAllStreams();
	void SaveStreamList(CString ConfigFile, const char *szStrmPrefix, const char *szPlanePrefix, int nCountStreams);

	CBitmap m_Background;
	CBrush* m_pEditBkBrush;
	CBrush* m_pStaticBkBrush;

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSelectCameraSource();
	afx_msg void OnLvnItemchangedListInputStreams(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnItemdblclickListInputStreams(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListInputStreams(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonUpdate();
	afx_msg void OnBnClickedButtonEdit();
	afx_msg void OnEnChangeEditStreamPath();
	afx_msg void OnBnClickedButtonBrowsPath();
	afx_msg void OnDestroy();
	afx_msg void OnCbnSelchangeRotation();
	int UpdateOnyx();
	int UpdateOnyxCrop();

	void UpdateInputListItem(CString Plane);
	void OnSelectRtspSource();
	void OnSelectSkypeSource();
	void RestoreSrcType();
	void CreateSourceSelectionList();
	void CreateRotationSelectionList();
	void CreateSourceImageList();
	int IsInputUsed(const char *szInputLocation);
	BOOL HideInuptDeatils();
	BOOL HideInputList();
	BOOL ShowInputDetails();
	BOOL ShowInputList();
	void SetComboExItem(CComboBoxEx *pCombo, int row, CString itemText);
	afx_msg void OnBnClickedButtonReset();
	CString m_DefaultFileFolder;
	afx_msg void OnBnClickedButtonColorkeyenable();
	afx_msg void OnBnClickedButtonUpsidedown();
	CSliderCtrl m_GreenContrast;
	CSliderCtrl m_GreenBright;
	
	CSliderCtrl m_SliderCropLeft;
	CSliderCtrl m_SliderCropRight;
	CSliderCtrl m_SliderCropTop;
	CSliderCtrl m_SliderCropBottom;

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	CComboBoxEx m_ComboexRotation;
	afx_msg void OnCbnSelchangeComboexRotation();
	CComboBoxEx m_ComboexInputSrc;
	afx_msg void OnCbnSelchangeComboexInputSource();
	afx_msg void OnNMCustomdrawSliderCropLeft(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderCropRight(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderCropTop(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderCropBottom(NMHDR *pNMHDR, LRESULT *pResult);

private:
	std::map<std::string, CStreamInf> mStreams;
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};

