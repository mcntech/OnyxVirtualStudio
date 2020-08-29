#pragma once

#include <atlimage.h>

// CSlideShow

class CSlideShow : public CStatic
{
	DECLARE_DYNAMIC(CSlideShow)

public:
	CSlideShow();
	virtual ~CSlideShow();

   void Start(CString Dir,UINT Interval);
   void Stop();
protected:
   afx_msg void OnTimer(UINT nIDEvent);
   afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
private:
   CFileFind   m_Finder;
   BOOL        m_Ret;
   CString     m_Dir;
   Image      *m_CurImage;
};
