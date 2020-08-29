// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#pragma once

#include "AppFeatureConfig.h"
#include "PageInputVideos.h"
#include "PageSkype.h"
#include "PageRecord.h"
#include "PagePublish.h"
#include "PageStatus.h"
#include "PageLiveCast.h"
#include "PageLayout.h"

#ifdef EN_APPUP
#include "adpcppf.h"
#include "OvsControlCrashReport.h"
#endif
#define MAX_LIVECAST_PAGES     4

/////////////////////////////////////////////////////////////////////////////
// COvsControlPropSheet

class COvsControlPropSheet : public CMFCPropertySheet
{
	DECLARE_DYNAMIC(COvsControlPropSheet)

// Construction
public:
	COvsControlPropSheet(CWnd* pParentWnd = NULL);

// Attributes
public:
	CPageInputVideos m_PageInputVideos;
	CPageSkype m_PageSkype;
	CPageRecorder m_PageRecorder;
	CPagepublish m_PagePublish;
	CPageStatus m_PageWelcome;
	CPageLayout m_PageLayout;

	CPageLivecast *m_pPageLivecast[MAX_LIVECAST_PAGES];
	HICON m_hIcon;
	CButton m_BtnLaunchOnyx;
	bool    m_fEvalMode;
// Overrides
public:
	virtual BOOL OnInitDialog();
	
	void AddTabIcon(int nResId);

// Implementation
public:
	virtual ~COvsControlPropSheet();

protected:
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
	
	void OnBtnLaunchOnyx();
	void OnOK();
	PROCESS_INFORMATION mProcessInformation;
	STARTUPINFOA mStartupInfo;

public:
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
#ifdef EN_APPUP
	Intel::Adp::Application *m_pInAppUnlockingSampleApp;
	Intel::Adp::AppItemIF *m_pItemIf;
	COvsControlCrashReport *m_pCrashReport;
#endif

	CImageList  m_ImgList;
public:
	afx_msg void OnDestroy();
};

