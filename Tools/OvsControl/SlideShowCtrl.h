#pragma once

#include <atlimage.h>
#include <gdiplus.h>
using namespace Gdiplus;

class CSlideShowCtrl : public CStatic
{
	DECLARE_DYNAMIC(CSlideShowCtrl)

public:
	CSlideShowCtrl();
	virtual ~CSlideShowCtrl();

   void Start(CString Dir,UINT Interval, int nSlideCount);
   void Stop();
	void LoadNextImage();
	bool InitIfAnimatedGIF(Image *pImg);
	void NextSlide();
	void PrevSlide();
protected:
   afx_msg void OnTimer(UINT nIDEvent);
   afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
private:
   CFileFind   m_Finder;
   BOOL        m_fInit;
   CString     m_Dir;
   Image      *m_CurImage;

   UINT             m_nSlidePosition;
	UINT            m_nSlideCount;
	UINT			m_nFrameCount;
	UINT			m_nFramePosition;
	PropertyItem*	m_pPropertyItem;
	UINT			m_Interval;
   ULONG_PTR m_gdiplusToken;
};


