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
#include "CustomProperties.h"
#include "PageStatus.h"

#include <memory>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageStatus property page

IMPLEMENT_DYNCREATE(CPageStatus, CMFCPropertyPage)

CPageStatus::CPageStatus() : CMFCPropertyPage(CPageStatus::IDD)
{
}

CPageStatus::~CPageStatus()
{
}

void CPageStatus::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WELCOME_SLIDES, m_WelcomeSlides);
}

BEGIN_MESSAGE_MAP(CPageStatus, CMFCPropertyPage)

	ON_WM_ERASEBKGND()
	ON_STN_CLICKED(IDC_WELCOME_SLIDES, &CPageStatus::OnStnClickedWelcomeSlides)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, &CPageStatus::OnBnClickedButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_PREV, &CPageStatus::OnBnClickedButtonPrev)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageStatus message handlers

const int nColumns = 3;
const int nRows = 50;

BOOL CPageStatus::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();
	m_Background.LoadBitmap(IDB_BITMAP_WELCOME); //Load bitmap

	return TRUE;
}


BOOL CPageStatus::OnEraseBkgnd(CDC* pDC)
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

void CPageStatus::OnStnClickedWelcomeSlides()
{
	// TODO: Add your control notification handler code here
}

BOOL CPageStatus::OnSetActive()
{
	CString Foder = "media\\welcome";
	m_WelcomeSlides.Start(Foder, 0/*4000*/, 10);

	return CMFCPropertyPage::OnSetActive();
}

BOOL CPageStatus::OnKillActive()
{
	m_WelcomeSlides.Stop();
	return CMFCPropertyPage::OnKillActive();
}

void CPageStatus::OnBnClickedButtonNext()
{
	m_WelcomeSlides.NextSlide();
}

void CPageStatus::OnBnClickedButtonPrev()
{
	m_WelcomeSlides.PrevSlide();
}
