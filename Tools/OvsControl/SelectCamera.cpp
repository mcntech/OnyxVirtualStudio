// SelectCamera.cpp : implementation file
//

#include "stdafx.h"
#include "OvsControl.h"
#include "SelectCamera.h"
#include "DsUtil.h"
#include "DShowCapGraph.h"

// CSelectCamera dialog

IMPLEMENT_DYNAMIC(CSelectCamera, CDialog)

CSelectCamera::CSelectCamera(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectCamera::IDD, pParent)
	, m_CameraName(_T(""))
{

}

CSelectCamera::~CSelectCamera()
{
}

void CSelectCamera::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_COMBO_SELECT_CAMERA, m_CameraName);
	DDX_Control(pDX, IDC_COMBO_SELECT_CAMERA, m_ComboCamera);
}


BEGIN_MESSAGE_MAP(CSelectCamera, CDialog)
END_MESSAGE_MAP()


// CSelectCamera message handlers

BOOL CSelectCamera::OnInitDialog()
{
	CDialog::OnInitDialog();

	CDsUtil DsUtil;
	std::vector<CCapDevice *> mDeviceList;
	DsUtil.GetVidCapDeviceList( mDeviceList);

	m_ComboCamera.ResetContent();
	if(!mDeviceList.empty()){
		for (int i=0; i < mDeviceList.size(); i++){
			CString nameDevice=DEVICE_PREFIX;
			if(mDeviceList[i]->mFriendlyName.compare(CSTR_MCN_VCAP) == 0)
				continue;
			nameDevice += mDeviceList[i]->mFriendlyName.c_str();
			nameDevice += DEVICE_INSTANCE_SEPARATOR;
			nameDevice += mDeviceList[i]->mDevicePath.c_str();
			m_ComboCamera.AddString(nameDevice);
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
