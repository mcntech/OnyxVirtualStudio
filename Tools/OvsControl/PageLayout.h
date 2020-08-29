#pragma once

#include <vector>
#include "OnyxControl.h"

// CPageLayout dialog

class CPageLayout : public CMFCPropertyPage
{
	DECLARE_DYNCREATE(CPageLayout)

public:
	CPageLayout();
	virtual ~CPageLayout();
	CComboBoxEx m_ComboexLayout;
	CComboBoxEx m_ComboexWallpaper;
	CComboBoxEx m_ComboexView;

	CSliderCtrl m_SliderSceneZoom;       
	CSliderCtrl m_SliderSceneMoveHorz;
	CSliderCtrl m_SliderSceneMoveVert;   
	CSliderCtrl m_SliderSceneRotateHorz; 
	CSliderCtrl m_SliderSceneRotateVert; 
	
	CSliderCtrl m_SliderAnchorZoom;      
	CSliderCtrl m_SliderAnchorMoveHorz;  
	CSliderCtrl m_SliderAnchorMoveVert;  
	CSliderCtrl m_SliderAnchorMoveFront; 
	CSliderCtrl m_SliderAnchorRotateVert;
	
	CBitmap m_Background;
	CBrush* m_pEditBkBrush;
	CBrush* m_pStaticBkBrush;

// Dialog Data
	enum { IDD = IDD_PAGE_LAYOUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	int UpdateOnyxWallPaper();
	void UpdateCamera(int nPosId, int nPosVal);
	void UpdateAnchor(int nPosId, int nPosVal);
	void UpdateStitchPlanes(int nPosId, int nPosVal);

	void SetComboExItem(CComboBoxEx *pCombo, int row, LPCTSTR itemText);
	void CreateLayoutSelectionList();
	void CreateWallpaperSelectionList();
	void CreatViewSelectionList();

public:
	afx_msg void OnCbnSelchangeComboLayout();
	afx_msg void OnCbnSelchangeComboWallpaper();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButtonViewSave();
	afx_msg void OnBnClickedButtonViewClear();
	afx_msg void OnCbnSelchangeComboView();
	afx_msg void OnBnClickedButtonViewRefresh();

	void UpdateControlsFromOnyxStatus();

	std::vector<VIEW_STATE_T>     m_ViewList;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedCheckAnchorLock();
	afx_msg void OnBnClickedCheckAnchorHide();
	afx_msg void OnBnClickedCheckStitchPlanes();
};
