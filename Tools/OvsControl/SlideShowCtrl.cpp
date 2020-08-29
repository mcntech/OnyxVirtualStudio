// SlideShowCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "SlideShowCtrl.h"


// CSlideShowCtrl

IMPLEMENT_DYNAMIC(CSlideShowCtrl, CStatic)
CSlideShowCtrl::CSlideShowCtrl()
	: m_CurImage(NULL)
	,m_fInit(FALSE)
	,m_pPropertyItem(NULL)
	,m_nFrameCount(0)
	,m_nFramePosition(0)
	, m_nSlidePosition(1)
	, m_nSlideCount(1)
{
   GdiplusStartupInput gdiplusStartupInput;
   GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
}

CSlideShowCtrl::~CSlideShowCtrl()
{
   GdiplusShutdown(m_gdiplusToken);
}


BEGIN_MESSAGE_MAP(CSlideShowCtrl, CStatic)
   ON_WM_TIMER()
   ON_WM_PAINT()
   ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


bool CSlideShowCtrl::InitIfAnimatedGIF(Image *pImg)
{
   UINT count = 0;
   count = pImg->GetFrameDimensionsCount();
   GUID* pDimensionIDs = new GUID[count];

   pImg->GetFrameDimensionsList(pDimensionIDs, count);
   m_nFrameCount = pImg->GetFrameCount(&pDimensionIDs[0]);
   int nSize = pImg->GetPropertyItemSize(PropertyTagFrameDelay);

   // Allocate a buffer to receive the property item.
   m_pPropertyItem = (PropertyItem*) malloc(nSize);
   pImg->GetPropertyItem(PropertyTagFrameDelay, nSize, m_pPropertyItem);

   delete  pDimensionIDs;
   m_nFramePosition = 0;
   return m_nFrameCount > 1;
}

void CSlideShowCtrl::LoadNextImage()
{
#if 0
	if(!m_fInit){
		m_fInit = m_Finder.FindFile(m_Dir+_T("\\*.*"));
	}
   while (m_fInit)  
#endif
   {
#if 0
      m_fInit = m_Finder.FindNextFile();
      if (!m_Finder.IsDirectory())  
#endif
	  {
         Image *TempImage;
#if 0
		 CString FilePath = m_Finder.GetFilePath();
#else
		CString FilePath;
		FilePath.Format("%s\\onyx_scene%03d.jpg", m_Dir, m_nSlidePosition);
#endif		
		const int MAX_STRINGZ=500;
		WCHAR wtext[MAX_STRINGZ+2];
		LPTSTR lpStr = FilePath.GetBuffer( FilePath.GetLength() );
		MultiByteToWideChar(CP_ACP, 0, lpStr, -1, wtext, MAX_STRINGZ);

         TempImage = Image::FromFile(wtext);
         if (TempImage && TempImage->GetLastStatus() == Ok)  {
            delete m_CurImage;
			if(m_pPropertyItem){
				delete m_pPropertyItem;
				m_pPropertyItem = NULL;
			}
            m_CurImage = TempImage;
			InitIfAnimatedGIF(m_CurImage);

            Invalidate();
            UpdateWindow();
            //break;
         } else if (TempImage) {
            delete TempImage;
         }
      }
   }
}

// CSlideShowCtrl message handlers
void CSlideShowCtrl::Start(CString Dir,UINT Interval, int nSlideCount)
{
	m_nSlidePosition = 1;
	m_nSlideCount = nSlideCount;

	m_Dir = Dir;
	LoadNextImage();
	m_Interval = Interval;
	if(m_Interval > 0) {
		SetTimer(100,Interval,NULL);
	}
}

void CSlideShowCtrl::Stop()
{
   KillTimer(100);
}

void CSlideShowCtrl::NextSlide()
{
	m_nSlidePosition++;
	if(m_nSlidePosition > m_nSlideCount)
		m_nSlidePosition = 1;
	LoadNextImage();
}

void CSlideShowCtrl::PrevSlide()
{
	m_nSlidePosition--;
	if(m_nSlidePosition <= 0){
		m_nSlidePosition = m_nSlideCount;
	}
	LoadNextImage();
}

void CSlideShowCtrl::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == 100)  {
		if (m_nFrameCount > 1 && m_nFramePosition < m_nFrameCount){
			m_nFramePosition++;
	        Invalidate();
		    UpdateWindow();
		} else {
			NextSlide();
		   //LoadNextImage();
		}
   }
   CStatic::OnTimer(nIDEvent);
}

void CSlideShowCtrl::OnPaint()
{
   CPaintDC dc(this); // device context for painting

   CRect Rect;
   GetClientRect(&Rect);
   
   CDC MemDC;
   MemDC.CreateCompatibleDC(&dc);
   CBitmap Bmp;
   Bmp.CreateCompatibleBitmap(&dc,Rect.Width(),Rect.Height());
   int SavedDC = MemDC.SaveDC();
   MemDC.SelectObject(&Bmp);

   //MemDC.FillSolidRect(Rect,RGB(127,127,127));
   MemDC.FillSolidRect(Rect,RGB(0x00, 0xA2, 0xE8));
   
   if (m_CurImage != NULL)  {
		Graphics TheDC(MemDC.GetSafeHdc());
		TheDC.DrawImage(m_CurImage,0,0,Rect.Width(),Rect.Height());

		/* Set Next Frame interval */
		if (m_nFrameCount > 1 && m_nFramePosition < m_nFrameCount){
			GUID   pageGuid = FrameDimensionTime;
			m_CurImage->SelectActiveFrame(&pageGuid, m_nFramePosition);		
			long lPause = ((long*) m_pPropertyItem->value)[m_nFramePosition] * 10;
			SetTimer(100,lPause,NULL);
		} else {
			if(m_Interval > 0) {
				SetTimer(100,m_Interval,NULL);
			}
		}
   }

   dc.BitBlt(0,0,Rect.Width(),Rect.Height(),&MemDC,0,0,SRCCOPY);
   MemDC.RestoreDC(SavedDC);
}

