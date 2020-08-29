// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "OvsControl.h"
#include "PageInputVideos.h"
#include "DsUtil.h"
#include <vector>
#include "PageSkype.h"
#include "OnyxControl.h"
#include "EditRtsp.h"
#include "EditSkype.h"
#include "SelectCamera.h"
#include "PluginParams.h"
#include "ConfigComn.h"

#define STATE_YES "Yes"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const size_t MAX_TIP_TEXT_LENGTH = 1024;
#define MAX_CROP_PERCENT 30
#define CROP_SLIDER_SCALE     1000

BEGIN_MESSAGE_MAP(CVideoPlane, CStatic)
   ON_WM_PAINT()
   ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CVideoPlane, CStatic)

void CVideoPlane::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect Rect;
	GetClientRect(&Rect);

	CDC MemDC;
	MemDC.CreateCompatibleDC(&dc);
	CBitmap btmap;

	if(m_PlaneId.Compare("display1") == 0) {
		btmap.LoadBitmapA(IDB_BITMAP_DISPLAY1);
	} else if(m_PlaneId.Compare("display2") == 0) {
		btmap.LoadBitmapA(IDB_BITMAP_DISPLAY2);
	}  else if(m_PlaneId.Compare("display3") == 0) {
		btmap.LoadBitmapA(IDB_BITMAP_DISPLAY3);
	}   else if(m_PlaneId.Compare("anchor1") == 0) {
		btmap.LoadBitmapA(IDB_BITMAP_ANCHOR1);
	}   else if(m_PlaneId.Compare("anchor2") == 0) {
		btmap.LoadBitmapA(IDB_BITMAP_ANCHOR2);
	}   else if(m_PlaneId.Compare("ticker1") == 0) {
		btmap.LoadBitmapA(IDB_BITMAP_OVERLAY1);
	} else {
		btmap.LoadBitmapA(IDB_BITMAP_DISPLAY0);
	}

	int SavedDC = MemDC.SaveDC();
	MemDC.SelectObject(&btmap);

   dc.BitBlt(0,0,Rect.Width(),Rect.Height(),&MemDC,0,0,SRCCOPY);
   MemDC.RestoreDC(SavedDC);
}

void CVideoPlane::SetPlane(CString PlaneId) 
{
	m_PlaneId = PlaneId;
	InvalidateRect(NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CPageInputVideos property page

IMPLEMENT_DYNCREATE(CPageInputVideos, CMFCPropertyPage)

CPageInputVideos::CPageInputVideos() : CMFCPropertyPage(CPageInputVideos::IDD)
	, m_SelStreamPath(_T(""))
	, m_EditPlaneId(_T(""))
	, m_fModified(false)
{
	m_DefaultFileFolder = "c:\\\0";
}

CPageInputVideos::~CPageInputVideos()
{
}

void CPageInputVideos::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_COLORKEYENABLE, m_BtnColorKeyEanble);
	DDX_Control(pDX, IDC_BUTTON_UPDATE_VID_LIST, m_BtnUpdate);
	DDX_Control(pDX, IDC_BUTTON_EDIT_VID_INPUT, m_BtnEdit);
	DDX_Control(pDX, IDC_STATIC_IMG_PLANE, m_VideoPlane);
	DDX_Control(pDX, IDC_BUTTON_UPSIDEDOWN, m_BtnUpsideDown);
	DDX_Control(pDX, IDC_SLIDER_GREEN_CONTRAST, m_GreenContrast);
	DDX_Control(pDX, IDC_SLIDER_GREEN_BRIGHT, m_GreenBright);
	DDX_Control(pDX, IDC_BUTTON_RESET_VID_LIST, m_BtnResetVidList);
	DDX_Control(pDX, IDC_COMBOEX_ROTATION, m_ComboexRotation);
	DDX_Control(pDX, IDC_COMBOEX_INPUT_SOURCE, m_ComboexInputSrc);
	DDX_Control(pDX, IDC_SLIDER_CROP_LEFT, m_SliderCropLeft);
	DDX_Control(pDX, IDC_SLIDER_CROP_RIGHT, m_SliderCropRight);
	DDX_Control(pDX, IDC_SLIDER_CROP_TOP, m_SliderCropTop);
	DDX_Control(pDX, IDC_SLIDER_CROP_BOTTOM, m_SliderCropBottom);
}

BEGIN_MESSAGE_MAP(CPageInputVideos, CMFCPropertyPage)
	ON_COMMAND(ID_DIALOG_ABOUT, OnDialogAbout)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_VID_LIST, &CPageInputVideos::OnBnClickedButtonUpdate)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_VID_INPUT, &CPageInputVideos::OnBnClickedButtonEdit)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_INPUT_STREAMS, &CPageInputVideos::OnLvnItemchangedListInputStreams)
	ON_NOTIFY(HDN_ITEMDBLCLICK, 0, &CPageInputVideos::OnHdnItemdblclickListInputStreams)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_INPUT_STREAMS, &CPageInputVideos::OnNMDblclkListInputStreams)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_RESET_VID_LIST, &CPageInputVideos::OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_COLORKEYENABLE, &CPageInputVideos::OnBnClickedButtonColorkeyenable)
	ON_BN_CLICKED(IDC_BUTTON_UPSIDEDOWN, &CPageInputVideos::OnBnClickedButtonUpsidedown)
ON_WM_HSCROLL()
ON_WM_HELPINFO()
ON_CBN_SELCHANGE(IDC_COMBOEX_ROTATION, &CPageInputVideos::OnCbnSelchangeComboexRotation)
ON_CBN_SELCHANGE(IDC_COMBOEX_INPUT_SOURCE, &CPageInputVideos::OnCbnSelchangeComboexInputSource)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_CROP_LEFT, &CPageInputVideos::OnNMCustomdrawSliderCropLeft)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_CROP_RIGHT, &CPageInputVideos::OnNMCustomdrawSliderCropRight)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_CROP_TOP, &CPageInputVideos::OnNMCustomdrawSliderCropTop)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_CROP_BOTTOM, &CPageInputVideos::OnNMCustomdrawSliderCropBottom)
ON_WM_VSCROLL()
END_MESSAGE_MAP()


void CPageInputVideos::OnDialogAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


//=================================
int CPageInputVideos::OnGetStreamFilePath(char *szFilePath) 
{
    OPENFILENAMEA ofn={0};

    // Fill in standard structure fields
    ofn.lStructSize       = sizeof(OPENFILENAME);
    ofn.hwndOwner         = m_hWnd;
	ofn.lpstrFilter       = "Media Files (*.mp4; *.m4v; *.wmv)\0*.mp4; *.m4v; *.wmv\0\0";

    ofn.lpstrCustomFilter = NULL;
    ofn.nFilterIndex      = 1;
    ofn.nMaxFile          = MAX_PATH;
    ofn.lpstrFile         = szFilePath;
    ofn.lpstrTitle        = "Select Media File...\0";
    ofn.lpstrFileTitle    = NULL;
    ofn.lpstrDefExt       = "wmv\0";
    ofn.Flags             = OFN_OVERWRITEPROMPT;
    ofn.lpstrInitialDir   =  m_DefaultFileFolder;

    BOOL bSuccess = GetOpenFileNameA((LPOPENFILENAMEA)&ofn);
    return bSuccess;
}


// CPageInputVideos message handlers

BOOL InitStreamInf(
			CString &ConfigFile,
			const char *SectionName,
			CString &StreamPath, 
			CString &StreamEnity, 
			CString &StreamMaterial, 
			CString &Type, 
			int     &nRotation,
			bool    &bUpsideDown,
			bool    &ColorKey)
{
	char szValue[MAX_PATH] = {0};
	GetPrivateProfileStringA(SectionName, KEY_NAME_STREAM_VIDEO,"",szValue,MAX_PATH - 1,ConfigFile);
	StreamPath = szValue;
	GetPrivateProfileStringA(SectionName, KEY_NAME_STREAM_ENTITY,"",szValue,MAX_PATH - 1,ConfigFile);
	StreamEnity = szValue;
	GetPrivateProfileStringA(SectionName,KEY_NAME_STREAM_MATERIAL,"",szValue,MAX_PATH - 1,ConfigFile);
	StreamMaterial = szValue;

	GetPrivateProfileStringA(SectionName,KEY_NAME_ROTATION,"",szValue,MAX_PATH - 1,ConfigFile);
	nRotation = atoi(szValue);
	
	GetPrivateProfileStringA(SectionName, KEY_NAME_UPSIDEDOWN,"",szValue,MAX_PATH - 1,ConfigFile);
	bUpsideDown = atoi(szValue);

	GetPrivateProfileStringA(SectionName, KEY_NAME_STREAM_COLORKEY_ENABLE,"",szValue,MAX_PATH - 1,ConfigFile);
	ColorKey = atoi(szValue);

	return TRUE;
}

void CStreamInf::Load(
			const char *ConfigFile,
			const char *SectionName
			)
{
	char szValue[MAX_PATH] = {0};

	GetPrivateProfileStringA(SectionName, KEY_NAME_CROP_LEFT,"",szValue,MAX_PATH - 1,ConfigFile);
	mCropLeft = atoi(szValue);
	GetPrivateProfileStringA(SectionName, KEY_NAME_CROP_RIGHT,"",szValue,MAX_PATH - 1,ConfigFile);
	mCropRight = atoi(szValue);
	GetPrivateProfileStringA(SectionName, KEY_NAME_CROP_TOP,"",szValue,MAX_PATH - 1,ConfigFile);
	mCropTop = atoi(szValue);
	GetPrivateProfileStringA(SectionName, KEY_NAME_CROP_BOTTOM,"",szValue,MAX_PATH - 1,ConfigFile);
	mCropBottom = atoi(szValue);
	mSectionName = SectionName;
	mConfigFile = ConfigFile;
}

BOOL SaveStreamInf(
			CString    &ConfigFile,
			const char *SectionName,
			CString    &StreamPath, 
			CString    &Plane, 
			int        nRotation,
			bool       bUpsideDown,
			bool       ColorKey)
{
	char szValue[MAX_PATH] = {0};

	WritePrivateProfileStringA(SectionName, KEY_NAME_STREAM_VIDEO,StreamPath, ConfigFile);

	WritePrivateProfileStringA(SectionName, KEY_NAME_STREAM_ENTITY,Plane, ConfigFile);
	WritePrivateProfileStringA(SectionName, KEY_NAME_STREAM_MATERIAL,Plane, ConfigFile);

	itoa(nRotation, szValue, 10);
	WritePrivateProfileStringA(SectionName, KEY_NAME_ROTATION,szValue, ConfigFile);

	itoa(bUpsideDown, szValue, 10);
	WritePrivateProfileStringA(SectionName, KEY_NAME_UPSIDEDOWN,szValue, ConfigFile);

	itoa(ColorKey, szValue, 10);
	WritePrivateProfileStringA(SectionName,KEY_NAME_STREAM_COLORKEY_ENABLE,szValue, ConfigFile);

	return TRUE;
}

void CStreamInf::Save()
{
	char szValue[MAX_PATH] = {0};
	const char *SectionName = mSectionName.c_str();
	const char *ConfigFile = mConfigFile.c_str();

	itoa(mCropLeft, szValue, 10);
	WritePrivateProfileStringA(SectionName,KEY_NAME_CROP_LEFT,szValue, ConfigFile);

	itoa(mCropRight, szValue, 10);
	WritePrivateProfileStringA(SectionName,KEY_NAME_CROP_RIGHT,szValue, ConfigFile);

	itoa(mCropTop, szValue, 10);
	WritePrivateProfileStringA(SectionName,KEY_NAME_CROP_TOP,szValue, ConfigFile);

	itoa(mCropBottom, szValue, 10);
	WritePrivateProfileStringA(SectionName,KEY_NAME_CROP_BOTTOM,szValue, ConfigFile);
}

int CPageInputVideos::GetStreamPath(CString &StmPath, CString &StmType)
{
	GetDlgItemText(IDC_EDIT_STREAM_PATH,StmPath);
	int nStreamType = m_ComboexInputSrc.GetCurSel();
	if(nStreamType == STREAM_TYPE_CAMERA) {
		StmType = "Camera";
	} else if(nStreamType == STREAM_TYPE_SKYPE) {
		StmType = "Skype";
	} else if(nStreamType == STREAM_TYPE_RTSP) {
		StmType = "IP Cam";
	} else {
		StmType = "File";
	}
	return nStreamType;
}

int StreamTypeFromPath(CString &StreamPath)
{
	int nStrmType = STREAM_TYPE_NOSRC;
	if(!StreamPath.IsEmpty()){
		if(StreamPath.Find(KEY_NAME_NO_VIDEO) != -1) {
			nStrmType = STREAM_TYPE_NOSRC;
		} else if(StreamPath.Find("device:") != -1) {
			nStrmType = STREAM_TYPE_CAMERA;
		} else if(StreamPath.Find("rtsp:") != -1) {
			nStrmType = STREAM_TYPE_RTSP;
		} else if(StreamPath.Find("SkypeVideo") != -1) {
			nStrmType = STREAM_TYPE_SKYPE;
		} else {
			nStrmType = STREAM_TYPE_FILE;
		}
	}
	return nStrmType;
}

void CPageInputVideos::OnEnChangeEditStreamPath()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CPageInputVideos::OnBnClickedButtonBrowsPath()
{
	char szFilePath[MAX_PATH] = {0};
	//GetDlgItemText(IDC_EDIT_STREAM_PATH, szFilePath, MAX_PATH);
	if(OnGetStreamFilePath(szFilePath) == TRUE){
		SetDlgItemText(IDC_EDIT_STREAM_PATH,szFilePath);
	} else {
		RestoreSrcType();
	}
}

const int nMaxStreams = 3;
const int nMaxAnchors = 1;
const int nMaxOverlays = 1;

typedef enum _COLUMN_LOCN_T
{
	COLUMN_IMG,
	COLUMN_PLANE,
	COLUMN_SOURCE,
	COLUMN_ROTATION,
	COLUMN_UPSIDEDOWN,
	COLUMN_COLOURKEY,
	COLUMN_MAX,
} COLUMN_LOCN_T;

#define COL_PLANE	0

void CPageInputVideos::StrmListInsertImage(int row, int nImgId)
{
	LVITEM lvi;

	lvi.mask =  LVIF_IMAGE | LVIF_TEXT;
	lvi.iItem = row;
	lvi.iSubItem = 0;
	lvi.pszText = (LPTSTR)(LPCTSTR)NULL;//(strItem);
	lvi.iImage = nImgId;		// There are 8 images in the image list
	m_ctrlStrmList.InsertItem(&lvi);
}

void CPageInputVideos::StrmListSetImage(int row, int nImgId)
{
	LVITEM lvi;

	lvi.mask =  LVIF_IMAGE | LVIF_TEXT;
	lvi.iItem = row;
	lvi.iSubItem = 0;
	lvi.pszText = (LPTSTR)(LPCTSTR)NULL;//(strItem);
	lvi.iImage = nImgId;		// There are 8 images in the image list
	m_ctrlStrmList.SetItem(&lvi);
}

void CPageInputVideos::InitStreamList(CString ConfigFile, const char *szPrefix, int nCountStreams)
{
	CString streamType;
	CString Stream;
	CString EntityStream;
	CString MaterialStream;

	bool    bColorKey;
	bool    bUpsideDown;
	int     nRotation;

	for (int i = 0; i < nCountStreams; i++)	{
		CStreamInf StreamInf;
		CString Section;
		CString tmpVal;
		tmpVal.Format("%d", i+1);
		Section = szPrefix + tmpVal;
		InitStreamInf(ConfigFile, Section,Stream, EntityStream, MaterialStream, streamType, nRotation, bUpsideDown, bColorKey);
		StreamInf.Load(ConfigFile, Section);
		mStreams[(LPCSTR)MaterialStream] = StreamInf;

		int nStreamType = StreamTypeFromPath(Stream);
		StrmListInsertImage(i, nStreamType);
		m_ctrlStrmList.SetItemText(i, COLUMN_PLANE, MaterialStream);
		m_ctrlStrmList.SetItemText(i, COLUMN_SOURCE, Stream);

		// Rotation
		tmpVal = "0";	
		if(nRotation == 90)  tmpVal = "90";
		else if(nRotation == 180)  tmpVal = "180";
		else if(nRotation == 270)  tmpVal = "270";
		m_ctrlStrmList.SetItemText(i, COLUMN_ROTATION, tmpVal);

		
		if(bUpsideDown) tmpVal = "Yes";	else tmpVal = "No";
		m_ctrlStrmList.SetItemText(i, COLUMN_UPSIDEDOWN, tmpVal);

		if(bColorKey) tmpVal = "Yes";	else tmpVal = "No";
		m_ctrlStrmList.SetItemText(i, COLUMN_COLOURKEY, tmpVal);
	}
}

static int dialogIdInputDetail[] = 
{
	IDC_BUTTON_COLORKEYENABLE,
	IDC_EDIT_STREAM_PATH,
	IDC_BUTTON_UPDATE_VID_LIST,
	IDC_STATIC_IMG_PLANE,
	IDC_STATIC_LABLE_SOURCE_LOCATION,
	IDC_STATIC_LABEL_VIDEO_PLANE,
	IDC_STATIC_LABLE_SOURCE_TYPE,
	IDC_BUTTON_UPSIDEDOWN,
	IDC_SLIDER_GREEN_CONTRAST,
	IDC_SLIDER_GREEN_BRIGHT,
	IDC_COMBOEX_ROTATION,
	IDC_COMBOEX_INPUT_SOURCE,
	IDC_SLIDER_CROP_LEFT,
	IDC_SLIDER_CROP_RIGHT,
	IDC_SLIDER_CROP_TOP,
	IDC_SLIDER_CROP_BOTTOM,
};

static int dialogIdInputList[] = 
{
	IDC_LIST_INPUT_STREAMS,
	IDC_BUTTON_EDIT_VID_INPUT,
	IDC_STATIC_LABEL_VIDEOLANE_LIST,
	IDC_BUTTON_RESET_VID_LIST
};

BOOL CPageInputVideos::HideInuptDeatils()
{
	int nCount = sizeof(dialogIdInputDetail) / sizeof(int);
	for (int i=0; i < nCount ; i++){
		GetDlgItem(dialogIdInputDetail[i])->ShowWindow(SW_HIDE);
	}
	return TRUE;
}

BOOL CPageInputVideos::HideInputList()
{
	int nCount = sizeof(dialogIdInputList) / sizeof(int);
	for (int i=0; i < nCount ; i++){
		GetDlgItem(dialogIdInputList[i])->ShowWindow(SW_HIDE);
	}
	return TRUE;
}

BOOL CPageInputVideos::ShowInputDetails()
{
	int nCount = sizeof(dialogIdInputDetail) / sizeof(int);
	for (int i=0; i < nCount ; i++){
		GetDlgItem(dialogIdInputDetail[i])->ShowWindow(SW_SHOW);
	}
	return TRUE;
}

BOOL CPageInputVideos::ShowInputList()
{
	int nCount = sizeof(dialogIdInputList) / sizeof(int);
	for (int i=0; i < nCount ; i++){
		GetDlgItem(dialogIdInputList[i])->ShowWindow(SW_SHOW);
	}
	return TRUE;
}

void CPageInputVideos::SetComboExItem(CComboBoxEx *pCombo, int row, CString itemText)
{
	COMBOBOXEXITEM item;
	memset(&item, 0, sizeof(item));
	item.mask = CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_TEXT;

	item.iItem = row;
	item.iSelectedImage = item.iImage = row;
	//item.iIndent = m_lstIndents [i];
	item.pszText = (LPTSTR)(LPCTSTR) itemText;
	item.cchTextMax = itemText.GetLength();

	pCombo->InsertItem(&item);
}


void CPageInputVideos::CreateSourceSelectionList()
{
	m_ComboexInputSrc.SetImageList(&m_ImgListSrcTypeSmall);
	SetComboExItem(&m_ComboexInputSrc, STREAM_TYPE_NOSRC, "No Video");
	SetComboExItem(&m_ComboexInputSrc, STREAM_TYPE_CAMERA, "Camera");
	SetComboExItem(&m_ComboexInputSrc, STREAM_TYPE_FILE, "File");
	SetComboExItem(&m_ComboexInputSrc, STREAM_TYPE_RTSP, "IP Cam");
	SetComboExItem(&m_ComboexInputSrc, STREAM_TYPE_SKYPE, "Skype");
}

static int STRMLIST_IMG_SMALL[]=
{
	IDB_BITMAP_NOSRC_SML,
	IDB_BITMAP_CAMERASRC_SML,
	IDB_BITMAP_FILESRC_SML,
	IDB_BITMAP_IPCAMSRC_SML,
	IDB_BITMAP_SKYPESRC_SML,
};

static int STRMLIST_IMG[]=
{
	IDB_BITMAP_NOSRC,
	IDB_BITMAP_CAMERASRC,
	IDB_BITMAP_FILESRC,
	IDB_BITMAP_IPCAMSRC,
	IDB_BITMAP_SKYPESRC,
};

void CPageInputVideos::CreateSourceImageList()
{
	int nCount = sizeof(STRMLIST_IMG_SMALL) / sizeof(int);
	m_ImgListSrcTypeSmall.Create(16,16,ILC_COLOR24, 5, 5);
	m_ImgListSrcType.Create(24,24,ILC_COLOR24, 5, 5);
	CBitmap tmpBmp;
	for (int i=0; i < nCount; i++) {
		tmpBmp.LoadBitmapA(STRMLIST_IMG_SMALL[i]);
		m_ImgListSrcTypeSmall.Add(&tmpBmp,RGB(255,0, 255));
		tmpBmp.DeleteObject();
	}
	nCount = sizeof(STRMLIST_IMG) / sizeof(int);
	for (int i=0; i < nCount; i++) {
		tmpBmp.LoadBitmapA(STRMLIST_IMG[i]);
		m_ImgListSrcType.Add(&tmpBmp,RGB(255,0, 255));
		tmpBmp.DeleteObject();
	}

}


void CPageInputVideos::CreateRotationSelectionList()
{
	m_ImgListRotationType.Create(16,16,ILC_COLOR24, 5, 5);
	CBitmap tmpBmp;
	tmpBmp.LoadBitmapA(IDB_BITMAP_ARROW_UP);
	m_ImgListRotationType.Add(&tmpBmp,RGB(255,0, 255));
	tmpBmp.DeleteObject();

	tmpBmp.LoadBitmapA(IDB_BITMAP_ARROW_RIGHT);
	m_ImgListRotationType.Add(&tmpBmp,RGB(255,0, 255));
	tmpBmp.DeleteObject();

	tmpBmp.LoadBitmapA(IDB_BITMAP_ARROW_DN);
	m_ImgListRotationType.Add(&tmpBmp,RGB(255,0, 255));
	tmpBmp.DeleteObject();

	tmpBmp.LoadBitmapA(IDB_BITMAP_ARROW_LEFT);
	m_ImgListRotationType.Add(&tmpBmp,RGB(255,0, 255));
	tmpBmp.DeleteObject();

	m_ComboexRotation.SetImageList(&m_ImgListRotationType);
	SetComboExItem(&m_ComboexRotation, ROTATION_TYPE_0, "Rotate 0");
	SetComboExItem(&m_ComboexRotation, ROTATION_TYPE_90, "Rotate 90");
	SetComboExItem(&m_ComboexRotation, ROTATION_TYPE_180, "Rotate 180");
	SetComboExItem(&m_ComboexRotation, ROTATION_TYPE_270, "Rotate 270");
}


void CPageInputVideos::ConfigBtn(CMFCButton &Btn, int nToolTipId)
{
	CString Tooltip;

	Btn.SetFaceColor(PANEL_BCKGND,  TRUE );
	Btn.m_nFlatStyle = CMFCButton::BUTTONSTYLE_NOBORDERS;
	Btn. m_bDrawFocus = FALSE;
	Btn.SizeToContent();

	if(Tooltip.LoadStringA(nToolTipId)) {
		Btn.SetTooltip(Tooltip);
	}
}
BOOL CPageInputVideos::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();
	CString Tooltip;
	m_psp.dwFlags |= PSP_HASHELP;

	m_Background.LoadBitmap(IDB_BITMAP_WELCOME); //Load bitmap
	m_pEditBkBrush = new CBrush(RGB(0, 0, 0x50));
	m_pStaticBkBrush = new CBrush(RGB(0xB1, 0xC1, 0xEC));

	CreateSourceImageList();

	m_BtnColorKeyEanble.SetImage(IDB_BITMAP_GEN_DISABLEU, IDB_BITMAP_GEN_DISABLED);
	m_BtnColorKeyEanble.SetCheckedImage(IDB_BITMAP_GEN_ENABLEU, IDB_BITMAP_GEN_ENABLED);
	ConfigBtn(m_BtnColorKeyEanble, IDS_TTIP_COLORKEYENABLE);

	m_BtnUpsideDown.SetImage(IDB_BITMAP_GEN_DISABLEU, IDB_BITMAP_GEN_DISABLED);
	m_BtnUpsideDown.SetCheckedImage(IDB_BITMAP_GEN_ENABLEU, IDB_BITMAP_GEN_ENABLED);
	ConfigBtn(m_BtnUpsideDown, IDS_TTIP_UPSIDEDOWN);


	m_BtnUpdate.SetImage(IDB_BITMAP_RETURNLISTU,IDB_BITMAP_RETURNLISTD,IDB_BITMAP_RETURNLISTX);
	m_BtnUpdate.m_bRightImage = TRUE;
	ConfigBtn(m_BtnUpdate, IDS_TTIP_UPDATE_VID_LIST);

	m_BtnEdit.SetImage(IDB_BITMAP_ITEMEDITU, IDB_BITMAP_ITEMEDITD, IDB_BITMAP_ITEMEDITX);
	m_BtnEdit.m_bRightImage = TRUE;
	ConfigBtn(m_BtnEdit, IDS_TTIP_EDIT_VID_INPUT);

	m_BtnResetVidList.SetImage(IDB_BITMAP_RESETU, IDB_BITMAP_RESETD, IDB_BITMAP_RESETX);
	ConfigBtn(m_BtnResetVidList, IDS_TTIP_RESET_VID_LIST);

	RECT rectDlg;
	::GetWindowRect(m_hWnd, &rectDlg);
	RECT rectList = {10, 65, 508, 185};
	rectList.right = 10 + (rectDlg.right - rectDlg.left - 20); 
	rectList.bottom = 65 + (rectDlg.bottom - rectDlg.top - 75); 

	m_ctrlStrmList.Create(LVS_REPORT | LVS_NOSORTHEADER |  LVS_SINGLESEL | WS_BORDER | WS_TABSTOP,rectList,this,IDC_LIST_INPUT_STREAMS);
	m_ctrlStrmList.ShowWindow(SW_SHOW);
	m_ctrlStrmList.InsertColumn(COLUMN_IMG,    _T(""),    LVCFMT_LEFT, 20);
	m_ctrlStrmList.InsertColumn(COLUMN_PLANE,    _T("Plane"),    LVCFMT_LEFT, 60);
	m_ctrlStrmList.InsertColumn(COLUMN_SOURCE,   _T("Source Location"),   LVCFMT_LEFT, 228);
	m_ctrlStrmList.InsertColumn(COLUMN_ROTATION,    _T("Rotation"),    LVCFMT_LEFT, 60);
	m_ctrlStrmList.InsertColumn(COLUMN_UPSIDEDOWN,_T("UpsideDown"), LVCFMT_LEFT, 60);
	m_ctrlStrmList.InsertColumn(COLUMN_COLOURKEY,_T("ColorKey"), LVCFMT_LEFT, 60);

	m_ctrlStrmList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES);
	//m_ctrlStrmList.EnableMarkSortedColumn();
	//CMFCHeaderCtrl& HeaderCtrl = m_ctrlStrmList.GetHeaderCtrl();
	//HeaderCtrl.ModifyStyle(HDS_BUTTONS, 0);    // disable the sorting.

	m_ctrlStrmList.SetImageList(&m_ImgListSrcTypeSmall, LVSIL_SMALL);

	CString ConfigFile;
	GetConfigFoderPath(ConfigFile);
	ConfigFile += ONYX_CONFIG_FILE;

	CString Stream;

	InitStreamList(ConfigFile, "Stream",nMaxStreams);
	InitStreamList(ConfigFile, "Anchor",nMaxAnchors);
	InitStreamList(ConfigFile, "Advertisement",nMaxOverlays);

	CreateSourceSelectionList();
	CreateRotationSelectionList();

	m_GreenContrast.SetRange(33,100);
	m_GreenBright.SetRange(1,255);

	m_SliderCropLeft.SetRange(-CROP_SLIDER_SCALE/2, CROP_SLIDER_SCALE/2 , TRUE);
	m_SliderCropRight.SetRange(-CROP_SLIDER_SCALE/2, CROP_SLIDER_SCALE/2, TRUE);
	m_SliderCropTop.SetRange(-CROP_SLIDER_SCALE/2, CROP_SLIDER_SCALE/2, TRUE);
	m_SliderCropBottom.SetRange(-CROP_SLIDER_SCALE/2, CROP_SLIDER_SCALE/2, TRUE);

	HideInuptDeatils();

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


int CPageInputVideos::GetRowForPlane(CString Plane)
{
	int rows = m_ctrlStrmList.GetItemCount();
	for(int i=0; i < rows;i++) {
		CString tmp = m_ctrlStrmList.GetItemText(i, COLUMN_PLANE);
		if(tmp.Compare(Plane) == 0)
			return i;
	}
	return -1;
}

int CPageInputVideos::UpdateOnyx()
{
	int row = GetRowForPlane(m_EditPlaneId);

	if(row == -1) {
		return 0;
	}
	CString StrmPath, StrmType;
	GetStreamPath(StrmPath, StrmType);
	int nStreamType = m_ComboexInputSrc.GetCurSel();
	int nRotation = m_ComboexRotation.GetCurSel();
	int fChkUpsideDown = m_BtnUpsideDown.IsChecked();
	int fChkColorKey = m_BtnColorKeyEanble.IsChecked();

	int nColorKeyThresh = m_GreenBright.GetPos();
	int nColorKeyContrast = m_GreenContrast.GetPos();

	
	ONYX_MSG_T onyxMsg = {0};
	onyxMsg.Id = ONYX_MSG_UPDATE_SOURCE;
	UPDATE_SOURCE_T *ctrlMsgUpdate = &onyxMsg.v.update;
	strcpy(ctrlMsgUpdate->szDisplay, m_EditPlaneId);
	strcpy(ctrlMsgUpdate->szSource, StrmPath);
	ctrlMsgUpdate->fEnableColorKey = fChkColorKey;
	ctrlMsgUpdate->nRotation = nRotation * 90;
	ctrlMsgUpdate->fUpsideDown = fChkUpsideDown;
	ctrlMsgUpdate->ulColorKeyThresh = (unsigned long)nColorKeyThresh  << 8;
	ctrlMsgUpdate->ulColorKeyRatio = nColorKeyContrast;

	ocntrlPostMsg(&onyxMsg);
	return 0;
}

int CPageInputVideos::UpdateOnyxCrop()
{
	int row = GetRowForPlane(m_EditPlaneId);

	if(row == -1) {
		return 0;
	}
	CString StrmPath, StrmType;
	GetStreamPath(StrmPath, StrmType);
	
	CStreamInf &StramInf = mStreams[(LPCSTR)m_EditPlaneId];
	StramInf.mCropLeft = m_SliderCropLeft.GetPos();
	StramInf.mCropRight = m_SliderCropRight.GetPos();
	StramInf.mCropTop = m_SliderCropTop.GetPos();
	StramInf.mCropBottom = m_SliderCropBottom.GetPos();

	ONYX_MSG_T onyxMsg = {0};
	onyxMsg.Id = ONYX_MSG_UPDATE_CROP;
	UPDATE_CROP_T *ctrlMsgCrop = &onyxMsg.v.crop;
	strcpy(ctrlMsgCrop->szDisplay, m_EditPlaneId);
	ctrlMsgCrop->ulCropLeft = StramInf.mCropLeft;
	ctrlMsgCrop->ulCropRight = StramInf.mCropRight;
	ctrlMsgCrop->ulCropTop = StramInf.mCropTop;
	ctrlMsgCrop->ulCropBottom = StramInf.mCropBottom;

	ocntrlPostMsg(&onyxMsg);

	return 0;
}

void CPageInputVideos::UpdateInputListItem(CString Plane)
{
	CString str;
	int row = GetRowForPlane(Plane);

	if(row == -1) {
		// Error
		return;
	}
	CString StrmPath, StrmType;
	GetStreamPath(StrmPath, StrmType);
	int nStreamType = m_ComboexInputSrc.GetCurSel();
	
	StrmListSetImage(row, nStreamType);
	m_ctrlStrmList.SetItemText(row, COLUMN_SOURCE, StrmPath);

	int nRotation = m_ComboexRotation.GetCurSel();
	CString strRotation;
	strRotation.Format("%d",nRotation * 90);
	m_ctrlStrmList.SetItemText(row, COLUMN_ROTATION, strRotation);

	int fChkUpsideDown = m_BtnUpsideDown.IsChecked();
	if(fChkUpsideDown) str = "Yes";	else str = "No";
	m_ctrlStrmList.SetItemText(row, COLUMN_UPSIDEDOWN, str);


	int fChkColorKey = m_BtnColorKeyEanble.IsChecked();
	if(fChkColorKey) str = "Yes";	else str = "No";
	m_ctrlStrmList.SetItemText(row, COLUMN_COLOURKEY, str);
	m_fModified = true;
	SetModified(1);

}

void CPageInputVideos::OnBnClickedButtonUpdate()
{
	HideInuptDeatils();
	ShowInputList();
	InvalidateRect(NULL, TRUE);
}

void CPageInputVideos::OnBnClickedButtonEdit()
{
	CString strTmp;
	POSITION pos = m_ctrlStrmList.GetFirstSelectedItemPosition();
	if (pos != NULL) {
		int nItem = m_ctrlStrmList.GetNextSelectedItem(pos);
		m_SelStreamPath = m_ctrlStrmList.GetItemText(nItem, COLUMN_SOURCE);

		int nStreamType = STREAM_TYPE_NOSRC; // Default
		if(!m_SelStreamPath.IsEmpty()){
			if(m_SelStreamPath.Find(KEY_NAME_NO_VIDEO) != -1) {
				nStreamType = STREAM_TYPE_NOSRC;
			} else if(m_SelStreamPath.Find("device:") != -1){
				nStreamType = STREAM_TYPE_CAMERA;
							} else if(m_SelStreamPath.Find("rtsp:") != -1) {
				nStreamType = STREAM_TYPE_RTSP;

			} else if(m_SelStreamPath.Find(KEY_NAME_SKYPE_VIDEO) != -1) {
				nStreamType = STREAM_TYPE_SKYPE;
			} else {
				nStreamType = STREAM_TYPE_FILE;
			}
		}
		
		m_EditPlaneId = m_ctrlStrmList.GetItemText(nItem, COLUMN_PLANE);
		CStreamInf &StreamInf = mStreams[(LPCSTR)m_EditPlaneId];
		m_SliderCropLeft.SetPos(StreamInf.mCropLeft);
		m_SliderCropRight.SetPos(StreamInf.mCropRight);
		m_SliderCropTop.SetPos(StreamInf.mCropTop);
		m_SliderCropBottom.SetPos(StreamInf.mCropBottom);

		m_VideoPlane.SetPlane(m_EditPlaneId);

		strTmp = m_ctrlStrmList.GetItemText(nItem, COLUMN_UPSIDEDOWN);
		int fChkUpsideDown = (strTmp.Compare(STATE_YES) == 0);
		m_BtnUpsideDown.SetCheck(fChkUpsideDown);

		strTmp = m_ctrlStrmList.GetItemText(nItem, COLUMN_COLOURKEY);
		int fChkColorKey = (strTmp.Compare(STATE_YES) == 0);
		m_BtnColorKeyEanble.SetCheck(fChkColorKey);

		strTmp = m_ctrlStrmList.GetItemText(nItem, COLUMN_ROTATION);
		int nRotation = ROTATION_TYPE_0;
		if(strTmp.Compare("0") == 0);
		else if(strTmp.Compare("90") == 0) nRotation = ROTATION_TYPE_90;
		else if(strTmp.Compare("180") == 0) nRotation = ROTATION_TYPE_180;
		else if(strTmp.Compare("270") == 0) nRotation = ROTATION_TYPE_270;
		
		int nColorKeyRatio = 33;
		int nColorKeyThresh = 25;
		m_GreenContrast.SetPos(nColorKeyRatio);
		m_GreenBright.SetPos(nColorKeyThresh);

		HideInputList();
		ShowInputDetails();
		m_ComboexInputSrc.SetCurSel(nStreamType);
		m_ComboexRotation.SetCurSel(nRotation);
		SetDlgItemText(IDC_EDIT_STREAM_PATH,m_SelStreamPath);
		InvalidateRect(NULL, TRUE);
	} else {
		MessageBox("Select an item from the list for editing","Onyx Prompt");
	}
}

void CPageInputVideos::SaveStreamList(CString ConfigFile, const char *szStrmPrefix, const char *szPlanePrefix, int nCountStreams)
{
	CString strPath;
	CString strPanel;
	CString strTmp;
	bool bColorKey;
	bool bUpsideDown;
	int nRotation;

	for (int i=0; i < nCountStreams; i++){

		CString SectionName;
		CString Plane;
		CString tmpVal;
		tmpVal.Format("%d", i+1);
		SectionName = szStrmPrefix + tmpVal;
		Plane = szPlanePrefix + tmpVal;
		int nItem = GetRowForPlane(Plane); 
		if(nItem != -1) {
			strPath = m_ctrlStrmList.GetItemText(nItem, COLUMN_SOURCE);
			strPanel = m_ctrlStrmList.GetItemText(nItem, COLUMN_PLANE);

			strTmp = m_ctrlStrmList.GetItemText(nItem, COLUMN_UPSIDEDOWN);
			bUpsideDown = (strTmp.Compare(STATE_YES) == 0);

			strTmp = m_ctrlStrmList.GetItemText(nItem, COLUMN_COLOURKEY);
			bColorKey = (strTmp.Compare(STATE_YES) == 0);

			strTmp = m_ctrlStrmList.GetItemText(nItem, COLUMN_ROTATION);
			nRotation = atoi(strTmp);

			SaveStreamInf(ConfigFile, SectionName, strPath,strPanel,nRotation,bUpsideDown, bColorKey);
			std::map<std::string, CStreamInf>::iterator it;
			if(mStreams.find((LPCSTR)Plane) != mStreams.end()) {
				CStreamInf StreamInf = mStreams[(LPCSTR)Plane];
				StreamInf.Save();
			}
		}
	}
}


void CPageInputVideos::OnLvnItemchangedListInputStreams(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CPageInputVideos::OnHdnItemdblclickListInputStreams(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	OnBnClickedButtonEdit();
	*pResult = 0;
}

void CPageInputVideos::OnNMDblclkListInputStreams(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	OnBnClickedButtonEdit();
	*pResult = 0;
}

void CPageInputVideos::SaveAllStreams()
{
	CString ConfigFile;
	GetConfigFoderPath(ConfigFile);
	ConfigFile += ONYX_CONFIG_FILE;

	SaveStreamList(ConfigFile, "Stream", "display", nMaxStreams);
	SaveStreamList(ConfigFile, "Anchor","anchor", nMaxAnchors);
	SaveStreamList(ConfigFile, "Advertisement","ticker", nMaxOverlays);
}
BOOL CPageInputVideos::OnApply()
{
	SaveAllStreams();
	m_fModified = false;
	return CMFCPropertyPage::OnApply();
}

void CPageInputVideos::OnOK()
{
	if(m_fModified) {
		SaveAllStreams();
		m_fModified = false;
	}
	CMFCPropertyPage::OnOK();
}

void CPageInputVideos::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CMFCPropertyPage::OnCancel();
}

BOOL CPageInputVideos::OnEraseBkgnd(CDC* pDC)
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

HBRUSH CPageInputVideos::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (CTLCOLOR_STATIC == nCtlColor)  {
		pDC->SetBkMode(TRANSPARENT);
		//return (HBRUSH)GetStockObject(NULL_BRUSH);
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

void CPageInputVideos::RestoreSrcType()
{
	CString szPath;
	GetDlgItemText(IDC_EDIT_STREAM_PATH, szPath);
	int nStreamType = StreamTypeFromPath(szPath);
	m_ComboexInputSrc.SetCurSel(nStreamType);
}

int CPageInputVideos::IsInputUsed(const char *szCamera)
{
	int rows = m_ctrlStrmList.GetItemCount();
	for(int i=0; i < rows;i++) {
		CString tmp = m_ctrlStrmList.GetItemText(i, COLUMN_SOURCE);
		if(tmp.Compare(szCamera) == 0)
			return i;
	}
	return -1;

}
void CPageInputVideos::OnSelectCameraSource()
{
	CSelectCamera dlg;
	if(dlg.DoModal() == IDOK) {
		CString strCamera = dlg.m_CameraName;
		int nPlane = IsInputUsed(dlg.m_CameraName);
		int nCrntPlane = GetRowForPlane(m_EditPlaneId);
		if(nPlane >= 0 && nPlane != nCrntPlane){
			CString strPlaneAssigend =  m_ctrlStrmList.GetItemText(nPlane, COLUMN_PLANE);
			CString strError;
			strError.Format("The Camera %s is already mapped to plane %s", dlg.m_CameraName, strPlaneAssigend);
			MessageBox(strError,"Option ignored", IDOK);
			RestoreSrcType();
		} else {
			SetDlgItemText(IDC_EDIT_STREAM_PATH,dlg.m_CameraName);
		}
	} else {
		RestoreSrcType();
	}
}

void CPageInputVideos::OnSelectRtspSource()
{
	CEditRtsp dlg;
	if(dlg.DoModal() == IDOK) {
		SetDlgItemText(IDC_EDIT_STREAM_PATH,dlg.m_RtspAddr);
	} else {
		RestoreSrcType();
	}
}

void CPageInputVideos::OnSelectSkypeSource()
{
	CEditSkype dlg;
	dlg.m_EditSkypeVideoChannel = KEY_NAME_SKYPE_VIDEO_CHAN1;
	if(dlg.DoModal() == IDOK) {
		int nPlane = IsInputUsed(dlg.m_EditSkypeVideoChannel);
		int nCrntPlane = GetRowForPlane(m_EditPlaneId);
		if(nPlane >= 0 && nPlane != nCrntPlane){
			CString strError;
			CString strPlaneAssigend =  m_ctrlStrmList.GetItemText(nPlane, COLUMN_PLANE);
			strError.Format("In the present version only one Skype video call is supported. It is already mapped to plane %s", strPlaneAssigend);
			MessageBox(strError,"Option ignored", IDOK);
			RestoreSrcType();
		} else {
			SetDlgItemText(IDC_EDIT_STREAM_PATH,KEY_NAME_SKYPE_VIDEO_CHAN1);
		}
	} else {
		RestoreSrcType();
	}
}

void CPageInputVideos::OnDestroy()
{
	CMFCPropertyPage::OnDestroy();
	delete m_pEditBkBrush;
	delete m_pStaticBkBrush;
}

BOOL CPageInputVideos::OnKillActive()
{
	// TODO: Add your specialized code here and/or call the base class

	return CMFCPropertyPage::OnKillActive();
}

BOOL CPageInputVideos::OnSetActive()
{
	// TODO: Add your specialized code here and/or call the base class

	return CMFCPropertyPage::OnSetActive();
}

void CPageInputVideos::OnCbnSelchangeRotation()
{
	UpdateInputListItem(m_EditPlaneId);
	UpdateOnyx();
}

void CPageInputVideos::OnBnClickedButtonReset()
{
	CString ConfigPath;
	CString ConfigFile;
	CString ConfigFileDefault;
	GetConfigFoderPath(ConfigPath);
	ConfigFile = ConfigPath + ONYX_CONFIG_FILE;
	ConfigFileDefault = ConfigPath + "\\ovsdefaults.cfg";
	CString Stream;

	if (CopyFile(ConfigFileDefault, ConfigFile, FALSE)){
		m_ctrlStrmList.DeleteAllItems();
		InitStreamList(ConfigFile, "Stream",nMaxStreams);
		InitStreamList(ConfigFile, "Anchor",nMaxAnchors);
		InitStreamList(ConfigFile, "Advertisement",nMaxOverlays);
		m_fModified = true;
		SetModified(1);
	} else {
		MessageBox("Can not find defaults file.","Failed to reset");
	}
}

void CPageInputVideos::OnBnClickedButtonColorkeyenable()
{
	UpdateInputListItem(m_EditPlaneId);
	UpdateOnyx();
}

void CPageInputVideos::OnBnClickedButtonUpsidedown()
{
	UpdateInputListItem(m_EditPlaneId);
	UpdateOnyx();
}


void CPageInputVideos::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl*pSld = (CSliderCtrl*)pScrollBar;
	int nId = pSld->GetDlgCtrlID();
	if(nId == IDC_SLIDER_CROP_LEFT || nId == IDC_SLIDER_CROP_RIGHT || nId == IDC_SLIDER_CROP_TOP ||  nId == IDC_SLIDER_CROP_BOTTOM) {
		UpdateOnyxCrop();
	} else if(nId == IDC_SLIDER_GREEN_BRIGHT || nId == IDC_SLIDER_GREEN_CONTRAST) {
		UpdateOnyx();
	}
	CMFCPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPageInputVideos::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CMFCPropertyPage::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CPageInputVideos::OnHelpInfo(HELPINFO* pHelpInfo)
{
	return CMFCPropertyPage::OnHelpInfo(pHelpInfo);
}

BOOL CPageInputVideos::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR	*lpnm = (NMHDR *) lParam;
	if(lpnm->code == PSN_HELP) {
		CString cstrHelpFile = "OnyxVirtualStudio.chm";
		::HtmlHelp(this->m_hWnd, cstrHelpFile, HH_HELP_FINDER, 0);
		return TRUE;
	}

	return CMFCPropertyPage::OnNotify(wParam, lParam, pResult);
}

void CPageInputVideos::OnCbnSelchangeComboexRotation()
{
	UpdateInputListItem(m_EditPlaneId);
	UpdateOnyx();
}

void CPageInputVideos::OnCbnSelchangeComboexInputSource()
{
	int nSel = m_ComboexInputSrc.GetCurSel();
	switch(nSel)
	{
		case STREAM_TYPE_NOSRC:
			SetDlgItemText(IDC_EDIT_STREAM_PATH,KEY_NAME_NO_VIDEO);
		break;
		case STREAM_TYPE_FILE:
			OnBnClickedButtonBrowsPath();
		break;
		case STREAM_TYPE_CAMERA:
			OnSelectCameraSource();
		break;
		case STREAM_TYPE_RTSP:
			OnSelectRtspSource();
		break;
		case STREAM_TYPE_SKYPE:
			OnSelectSkypeSource();
		break;
	}
	UpdateInputListItem(m_EditPlaneId);
	UpdateOnyx();
}


void CPageInputVideos::OnNMCustomdrawSliderCropLeft(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void CPageInputVideos::OnNMCustomdrawSliderCropRight(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void CPageInputVideos::OnNMCustomdrawSliderCropTop(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void CPageInputVideos::OnNMCustomdrawSliderCropBottom(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


