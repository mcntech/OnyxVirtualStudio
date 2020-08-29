// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#pragma once
#include "afxwin.h"
#include "SlideShowCtrl.h"
class CMyListCtrl : public CMFCListCtrl
{
	virtual COLORREF OnGetCellTextColor(int nRow, int nColum);
	virtual COLORREF OnGetCellBkColor(int nRow, int nColum);
	virtual HFONT OnGetCellFont(int nRow, int nColum, DWORD dwData = 0);

	virtual int OnCompareItems(LPARAM lParam1, LPARAM lParam2, int iColumn);

public:
	BOOL m_bColor;
	BOOL m_bModifyFont;
};

/////////////////////////////////////////////////////////////////////////////
// CPageStatus dialog

class CPageStatus : public CMFCPropertyPage
{
	friend class CMyListCtrl;

	DECLARE_DYNCREATE(CPageStatus)

// Construction
public:
	CPageStatus();
	~CPageStatus();

// Dialog Data
	enum { IDD = IDD_PAGE_HOME };

// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	virtual BOOL OnInitDialog();


	DECLARE_MESSAGE_MAP()

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);


	CBitmap m_Background;
	afx_msg void OnStnClickedWelcomeSlides();
	CSlideShowCtrl m_WelcomeSlides;
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	afx_msg void OnBnClickedButtonNext();
	afx_msg void OnBnClickedButtonPrev();
};

