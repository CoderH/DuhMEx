#pragma once
#include <gdiplus.h>

namespace Duh
{
   namespace GdiplusEx
   {
      void MakeRoundRectPath(Gdiplus::GraphicsPath &path,const Gdiplus::Rect &rect, int roundWidth, int roundHeight);
      Gdiplus::Rect GetDrawRect(const Gdiplus::Rect &rect,int penWidth);
      Gdiplus::Rect  ConvertRect(const RECT &rect);
      Gdiplus::Color ConvertClr(COLORREF color);
      Gdiplus::Image *LoadImageFromResource(HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType);

   }
   namespace GdiEx
   {
      /************************************************************************/
      class CDCObject
      {
      public:
         CDCObject           (HDC hDC);
         virtual ~CDCObject  ();
         void SetObject      (HGDIOBJ hObj, BOOL bDeleteCur = TRUE);
         virtual void Restore();
      protected:
         HGDIOBJ m_hOrigObj;
         HGDIOBJ m_hCurObj;
         HDC     m_hDC;
      };

      /************************************************************************/
      class CDCFont : public CDCObject
      {
      public:
         CDCFont(HDC hDC);
         CDCFont(HDC hDC, LPLOGFONT lpLogFont);
      public:
         void GetLogFont(LPLOGFONT lpLogFont);

         void SetLogFont(const LPLOGFONT lpLogFont);
         void SetWeight (LONG lfWeight);
         void SetColor  (COLORREF color);
         void SetBkMode (int nMode);

         void Restore   ();
      private:
         COLORREF m_crOrigColor;
         int      m_nOrigMode;
      };
      /************************************************************************/
      class CDCPen : public CDCObject
      {
      public:
         CDCPen(HDC hDC);
         CDCPen(HDC hDC, UINT nPenStyle, UINT nWidth, COLORREF color);
      public:
         void GetLogPen(LPLOGPEN lpLogPen);

         void SetLogPen(const LPLOGPEN lpLogPen);
         void SetStyle (UINT nStyle);
         void SetWidth (UINT nWidth);
         void SetColor (COLORREF color);
      };
      /************************************************************************/
      class CDCBrush : public CDCObject
      {
      public:
         CDCBrush(HDC hDC);
         CDCBrush(HDC hDC, LPLOGBRUSH lpLogBrush);
         CDCBrush(HDC hDC,COLORREF color);                // SolidBrush
         CDCBrush(HDC hDC,int nIndex, COLORREF color);    // HatchBrush
         explicit CDCBrush(HDC hDC,HBITMAP hbm);          // PatternBrush
          
         void GetLogBrush(const LPLOGBRUSH lpLogBrush);
         void SetLogBrush(LPLOGBRUSH lpLogBrush);
         void SetSolidBrush(COLORREF color);
         void SetHatchBrush(int nIndex, COLORREF color);
         void SetPatternBrush(HBITMAP hbm);
      };
   }
}