// PageLayout.cpp : implementation file
//

#include "stdafx.h"
#include "OvsControl.h"
#include "PageLayout.h"
#include "afxdialogex.h"

typedef struct _DEF_VIEWS_T
{
	char         szName[64];
	VIEW_STATE_T view_state;
} DEF_VIEWS_T;

DEF_VIEWS_T default_views[] = 
{
	{"3D Plain",    {LAYOUT_3D_PLANE,    0, {500, 500, 525, 500, 500, 500, 500}, {600, 400, 250, 500, 500, 500, 500}}},
	{"3D Curve",    {LAYOUT_3D_CURVE,    0, {500, 500, 525, 500, 500, 500, 500}, {600, 400, 250, 500, 500, 500, 500}}},
	{"3D-Left",     {LAYOUT_3D_PLANE,    0, {253, 500, 355, 500, 500, 500, 500}, {280, 430, 160, 500, 500, 500, 500}}},
	{"3D-Right",    {LAYOUT_3D_PLANE,    0, {747, 500, 355, 500, 500, 500, 500}, {720, 430, 160, 500, 500, 500, 500}}},
	{"2D-D1D2XD3A1",{LAYOUT_2D_D1D2XD3A1,0, {500, 500, 272, 500, 500, 500, 500}, {500, 500, 500, 500, 500, 500, 500}}},
	{"2D-D1D2XA1",  {LAYOUT_2D_D1D2XA1,  0, {500, 500, 272, 500, 500, 500, 500}, {500, 500, 500, 500, 500, 500, 500}}},
	{"2D-D1D2D3 Cliped",  {LAYOUT_2D_D1D2D3,  0, {500, 500, 272, 500, 500, 500, 500}, {500, 500, 500, 500, 500, 500, 500}}},
	{"2D-D1D2",     {LAYOUT_2D_D1D2,     0, {500, 500, 272, 500, 500, 500, 500}, {500, 500, 500, 500, 500, 500, 500}}},
	{"2D-D1A1",     {LAYOUT_2D_D1A1,     0, {500, 500, 272, 500, 500, 500, 500}, {500, 500, 500, 500, 500, 500, 500}}},
	{"2D-D2A1",     {LAYOUT_2D_D2A1,     0, {500, 500, 272, 500, 500, 500, 500}, {500, 500, 500, 500, 500, 500, 500}}},
	{"2D-D3A1",     {LAYOUT_2D_D3A1,     0, {500, 500, 272, 500, 500, 500, 500}, {500, 500, 500, 500, 500, 500, 500}}},
	{"2D-D1",       {LAYOUT_2D_D1,       0, {500, 500, 153, 500, 500, 500, 500}, {500, 500, 500, 500, 500, 500, 500}}},
	{"2D-D2",       {LAYOUT_2D_D2,       0, {500, 500, 153, 500, 500, 500, 500}, {500, 500, 500, 500, 500, 500, 500}}},
	{"2D-D3",       {LAYOUT_2D_D3,       0, {500, 500, 153, 500, 500, 500, 500}, {500, 500, 500, 500, 500, 500, 500}}},
	{"2D-A1",       {LAYOUT_2D_A1,       0, {500, 500, 153, 500, 500, 500, 500}, {500, 500, 500, 500, 500, 500, 500}}}
};

// CPageLayout dialog

IMPLEMENT_DYNCREATE(CPageLayout, CMFCPropertyPage)

CPageLayout::CPageLayout()
	: CMFCPropertyPage(CPageLayout::IDD)
{

}

CPageLayout::~CPageLayout()
{
}

void CPageLayout::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_WALLPAPER, m_ComboexWallpaper);
	DDX_Control(pDX, IDC_COMBO_VIEW, m_ComboexView);
	DDX_Control(pDX, IDC_SLIDER_SCENE_ZOOM,        m_SliderSceneZoom       );
	DDX_Control(pDX, IDC_SLIDER_SCENE_MOVE_HORZ,   m_SliderSceneMoveHorz   );
	DDX_Control(pDX, IDC_SLIDER_SCENE_MOVE_VERT,   m_SliderSceneMoveVert   );
	DDX_Control(pDX, IDC_SLIDER_SCENE_ROTATE_HORZ, m_SliderSceneRotateHorz );
	DDX_Control(pDX, IDC_SLIDER_SCENE_ROTATE_VERT, m_SliderSceneRotateVert );
	
	DDX_Control(pDX, IDC_SLIDER_ANCHOR_ZOOM,       m_SliderAnchorZoom      );
	DDX_Control(pDX, IDC_SLIDER_ANCHOR_MOVE_HORZ,  m_SliderAnchorMoveHorz  );
	DDX_Control(pDX, IDC_SLIDER_ANCHOR_MOVE_VERT,  m_SliderAnchorMoveVert  );
	DDX_Control(pDX, IDC_SLIDER_ANCHOR_MOVE_FRONT, m_SliderAnchorMoveFront );
	DDX_Control(pDX, IDC_SLIDER_ANCHOR_ROTATE_VERT,m_SliderAnchorRotateVert);
}


BEGIN_MESSAGE_MAP(CPageLayout, CPropertyPage)
	ON_CBN_SELCHANGE(IDC_COMBO_WALLPAPER, &CPageLayout::OnCbnSelchangeComboWallpaper)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_VIEW_SAVE, &CPageLayout::OnBnClickedButtonViewSave)
	ON_CBN_SELCHANGE(IDC_COMBO_VIEW, &CPageLayout::OnCbnSelchangeComboView)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_REFRESH, &CPageLayout::OnBnClickedButtonViewRefresh)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK_ANCHOR_LOCK, &CPageLayout::OnBnClickedCheckAnchorLock)
	ON_BN_CLICKED(IDC_CHECK_ANCHOR_HIDE, &CPageLayout::OnBnClickedCheckAnchorHide)
	ON_BN_CLICKED(IDC_CHECK_STITCH_PLANES, &CPageLayout::OnBnClickedCheckStitchPlanes)
END_MESSAGE_MAP()


// CPageLayout message handlers
void CPageLayout::SetComboExItem(CComboBoxEx *pCombo, int row, LPCTSTR itemText)
{
	COMBOBOXEXITEM item;
	memset(&item, 0, sizeof(item));
	//item.mask = CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_TEXT;
	item.mask =  CBEIF_TEXT;

	item.iItem = row;
	item.iSelectedImage = item.iImage = row;
	//item.iIndent = m_lstIndents [i];
	item.pszText = (LPTSTR)(LPCTSTR) itemText;
	item.cchTextMax = strlen(itemText);

	pCombo->InsertItem(&item);
}



void CPageLayout::CreateWallpaperSelectionList()
{
	int i = 0;
	SetComboExItem(&m_ComboexWallpaper, i, "Option 1");
	m_ComboexWallpaper.SetItemData(i, 0);
	i++;
	SetComboExItem(&m_ComboexWallpaper, i, "Option 2");
	m_ComboexWallpaper.SetItemData(i, i);
	i++;
	SetComboExItem(&m_ComboexWallpaper, i, "Option 3");
	m_ComboexWallpaper.SetItemData(i, i);

	m_ComboexWallpaper.SetCurSel(0); 
	m_ComboexWallpaper.ShowWindow(SW_SHOW);
}


void CPageLayout::CreatViewSelectionList()
{
	int i = 0;
	CString ViewName;

	for(int i = 0; i < sizeof(default_views) / sizeof(DEF_VIEWS_T); i++) {
		SetComboExItem(&m_ComboexView, i, default_views[i].szName);
		m_ComboexView.SetItemData(i, i);
	}

	m_ComboexView.SetCurSel(0); 
	m_ComboexView.ShowWindow(SW_SHOW);
}

BOOL CPageLayout::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	for(int i = 0; i < sizeof(default_views) / sizeof(DEF_VIEWS_T); i++)
		m_ViewList.push_back(default_views[i].view_state);

	m_Background.LoadBitmap(IDB_BITMAP_WELCOME); //Load bitmap
	m_pEditBkBrush = new CBrush(RGB(0, 0, 0x50));
	m_pStaticBkBrush = new CBrush(RGB(0xB1, 0xC1, 0xEC));

	CreateWallpaperSelectionList();
	CreatViewSelectionList();


	m_SliderSceneZoom.SetRange(0, 1000);
	m_SliderSceneZoom.SetPos(500);
	m_SliderSceneMoveHorz.SetRange(0, 1000);
	m_SliderSceneMoveHorz.SetPos(500);
	m_SliderSceneMoveVert.SetRange(0, 1000);
	m_SliderSceneMoveVert.SetPos(500);
	m_SliderSceneRotateHorz.SetRange(0, 1000);
	m_SliderSceneRotateHorz.SetPos(500);
	m_SliderSceneRotateVert.SetRange(0, 1000);
	m_SliderSceneRotateVert.SetPos(500);

	m_SliderAnchorZoom.SetRange(0, 1000);
	m_SliderAnchorZoom.SetPos(500);
	m_SliderAnchorMoveHorz.SetRange(0, 1000);
	m_SliderAnchorMoveHorz.SetPos(500);
	m_SliderAnchorMoveVert.SetRange(0, 1000);
	m_SliderAnchorMoveVert.SetPos(500);
	m_SliderAnchorMoveFront.SetRange(0, 1000);
	m_SliderAnchorMoveFront.SetPos(500);
	m_SliderAnchorRotateVert.SetRange(0, 1000);
	m_SliderAnchorRotateVert.SetPos(500);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int CPageLayout::UpdateOnyxWallPaper()
{
	int nSelection = m_ComboexWallpaper.GetCurSel();;
	unsigned long ulWallPaper = m_ComboexWallpaper.GetItemData(nSelection);

	ONYX_MSG_T onyxMsg = {0};
	onyxMsg.Id = ONYX_MSG_UPDATE_WALLPAPER;
	onyxMsg.v.wallpaper.ulWallPaperId = ulWallPaper;

	ocntrlPostMsg(&onyxMsg);
	return 0;
}

void CPageLayout::OnCbnSelchangeComboWallpaper()
{
	UpdateOnyxWallPaper();
}

void CPageLayout::UpdateCamera(int nPosId, int nPosVal)
{

	ONYX_MSG_T onyxMsg = {0};
	onyxMsg.Id = ONYX_MSG_NAVIGATE;
	onyxMsg.v.navigate.ulCtrlId = nPosId;
	onyxMsg.v.navigate.lValue = nPosVal;

	ocntrlPostMsg(&onyxMsg);

}

void CPageLayout::UpdateAnchor(int nPosId, int nPosVal)
{
	ONYX_MSG_T onyxMsg = {0};
	onyxMsg.Id = ONYX_MSG_NAVIGATE;
	onyxMsg.v.navigate.ulCtrlId = nPosId;
	onyxMsg.v.navigate.lValue = nPosVal;

	ocntrlPostMsg(&onyxMsg);
}

void CPageLayout::UpdateStitchPlanes(int nPosId, int nPosVal)
{
	ONYX_MSG_T onyxMsg = {0};
	onyxMsg.Id = ONYX_MSG_NAVIGATE;
	onyxMsg.v.navigate.ulCtrlId = nPosId;
	onyxMsg.v.navigate.lValue = nPosVal;

	ocntrlPostMsg(&onyxMsg);

}

void CPageLayout::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	char szTmp[32];

	CSliderCtrl*pSld = (CSliderCtrl*)pScrollBar;
	int nCrntPos = pSld->GetPos();
	if(*pSld == m_SliderSceneZoom) {
		UpdateCamera(SCENE_MOVE_DEPTH, nCrntPos);
	} else if(*pSld == m_SliderSceneMoveHorz) {
		UpdateCamera(SCENE_MOVE_HORZ, nCrntPos);
	} else  if(*pSld == m_SliderSceneMoveVert) {
		UpdateCamera(SCENE_MOVE_VERT, nCrntPos);
	} else  if(*pSld == m_SliderSceneRotateHorz) {
		UpdateCamera(SCENE_ROTATE_YAW, nCrntPos);
	} else  if(*pSld == m_SliderSceneRotateVert) {
		UpdateCamera(SCENE_ROTATE_PITCH, nCrntPos);
	} else  if(*pSld == m_SliderAnchorZoom) {
		UpdateCamera(ANCHOR_SCALE, nCrntPos);
	} else  if(*pSld == m_SliderAnchorMoveHorz) {
		UpdateCamera(ANCHOR_MOVE_HORZ, nCrntPos);
	} else  if(*pSld == m_SliderAnchorMoveVert) {
		UpdateCamera(ANCHOR_MOVE_VERT, nCrntPos);
	} else  if(*pSld == m_SliderAnchorMoveFront) {
		UpdateCamera(ANCHOR_MOVE_DEPTH, nCrntPos);
	} else  if(*pSld == m_SliderAnchorRotateVert) {
		UpdateCamera(ANCHOR_ROTATE_VERT, nCrntPos);
	}

	CMFCPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CPageLayout::OnBnClickedButtonViewSave()
{
	char szViewName[256] = "View";
	ONYX_STATUS_T onyxStatus = {0};
	VIEW_STATE_T *pViewState = &onyxStatus.v.crntview;
	ocntrlGetStatus(&onyxStatus);
	m_ViewList.push_back(*pViewState);
	int nIndx = m_ViewList.size() - 1;
	GetDlgItemText(IDC_EDIT_VIEW_NAME, szViewName, 255);
	SetComboExItem(&m_ComboexView, nIndx, szViewName);
	m_ComboexView.SetItemData(nIndx, nIndx);
}



void CPageLayout::OnCbnSelchangeComboView()
{
	ONYX_MSG_T onyxMsg = {0};
	onyxMsg.Id = SET_VIEW;
	int nSelection = m_ComboexView.GetCurSel();
	int nViewId = m_ComboexView.GetItemData(nSelection);

	if(nViewId < m_ViewList.size()){
		onyxMsg.v.setview = m_ViewList[nViewId];
		ocntrlPostMsg(&onyxMsg);
		Sleep(1000);
		UpdateControlsFromOnyxStatus();
	}
}

void CPageLayout::UpdateControlsFromOnyxStatus()
{
	ONYX_STATUS_T onyxStatus = {0};
	ocntrlGetStatus(&onyxStatus);
	LOCATION_T *pCam = &onyxStatus.v.crntview.Camera;
	LOCATION_T *pAnchor = &onyxStatus.v.crntview.Anchor;

	m_ComboexWallpaper.SetCurSel(onyxStatus.v.crntview.lWallpaper);
	m_SliderSceneMoveHorz.SetPos(pCam->lPosX);
	m_SliderSceneMoveVert.SetPos(pCam->lPosY);
	m_SliderSceneZoom.SetPos(pCam->lPosZ);
	m_SliderSceneRotateVert.SetPos(pCam->lPitch);
	m_SliderSceneRotateHorz.SetPos(pCam->lYaw);

	m_SliderAnchorMoveHorz.SetPos(pAnchor->lPosX);
	m_SliderAnchorMoveVert.SetPos(pAnchor->lPosY);
	m_SliderAnchorMoveFront.SetPos(pAnchor->lPosZ);
	m_SliderAnchorZoom.SetPos(pAnchor->lScale);
	m_SliderAnchorRotateVert.SetPos(pAnchor->lYaw);
}

void CPageLayout::OnBnClickedButtonViewRefresh()
{
	UpdateControlsFromOnyxStatus();
}


BOOL CPageLayout::OnEraseBkgnd(CDC* pDC)
{
	CMFCPropertyPage::OnEraseBkgnd(pDC);
	if(m_Background.m_hObject){

		CRect rect;
		GetClientRect(&rect);
		CDC dc;
		dc.CreateCompatibleDC(pDC);
		CBitmap* pOldBitmap = dc.SelectObject(&m_Background);

		BITMAP bmap;
		m_Background.GetBitmap(&bmap);
		pDC->StretchBlt(0, 0, rect.Width(),rect.Height(), &dc,0, 0,bmap.bmWidth,bmap.bmHeight, SRCCOPY);

		dc.SelectObject(pOldBitmap);
	}
	return TRUE;
}


HBRUSH CPageLayout::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (CTLCOLOR_STATIC == nCtlColor)  {
		pDC->SetBkMode(TRANSPARENT);
		if(pWnd->GetDlgCtrlID() == IDC_EDIT_STREAM_PATH){
			pDC->SetTextColor(RGB(0, 255, 0));
			pDC->SetBkColor(RGB(0, 0, 0x80));
			return (HBRUSH)(m_pEditBkBrush->GetSafeHandle());
		} else {
			return (HBRUSH)(m_pStaticBkBrush->GetSafeHandle());
		}
	} else if ( nCtlColor == CTLCOLOR_EDIT ) {
		pDC->SetTextColor(RGB(0, 255, 0));
		pDC->SetBkColor(RGB(0, 0, 0x80));
		return (HBRUSH)(m_pEditBkBrush->GetSafeHandle());
	}

	HBRUSH hbr = CMFCPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}


void CPageLayout::OnDestroy()
{
	CMFCPropertyPage::OnDestroy();

	delete m_pEditBkBrush;
	delete m_pStaticBkBrush;
}


void CPageLayout::OnBnClickedCheckAnchorLock()
{
	int nCheck = SendDlgItemMessage(IDC_CHECK_ANCHOR_LOCK, BM_GETCHECK, 0, 0);
	UpdateAnchor(ANCHOR_LOCK, nCheck == BST_CHECKED);
}


void CPageLayout::OnBnClickedCheckAnchorHide()
{
	int nCheck = SendDlgItemMessage(IDC_CHECK_ANCHOR_HIDE, BM_GETCHECK, 0, 0);
	UpdateAnchor(ANCHOR_HIDE, nCheck == BST_CHECKED);

}


void CPageLayout::OnBnClickedCheckStitchPlanes()
{
	int nCheck = SendDlgItemMessage(IDC_CHECK_STITCH_PLANES, BM_GETCHECK, 0, 0);
	UpdateStitchPlanes(LAOYUT_STITCH_PLANE, nCheck == BST_CHECKED);
}
