#include "stdafx.h"
#include "GdipEx.h"

using namespace Gdiplus;

void Duh::GdiplusEx::MakeRoundRectPath(GraphicsPath &path, const Rect &rect, int roundWidth, int roundHeight)
{
   path.ClearMarkers();

   int elWid = 2 * roundWidth;
   int elHei = 2 * roundHeight;
     
   int x = rect.X;
   int y = rect.Y;
   int width = rect.Width;
   int height = rect.Height;
  
   path.AddArc(x, y, elWid, elHei, 180, 90); // ×óÉÏ½ÇÔ²»¡  
   path.AddLine(x + roundWidth, y, x + width - roundWidth, y); // ÉÏ±ß  

   path.AddArc(x + width - elWid, y, elWid, elHei, 270, 90); // ÓÒÉÏ½ÇÔ²»¡  
   path.AddLine(x + width, y + roundHeight, x + width, y + height - roundHeight);// ÓÒ±ß  

   path.AddArc(x + width - elWid, y + height - elHei, elWid, elHei, 0, 90); // ÓÒÏÂ½ÇÔ²»¡  
   path.AddLine(x + width - roundWidth, y + height, x + roundWidth, y + height); // ÏÂ±ß  

   path.AddArc(x, y + height - elHei, elWid, elHei, 90, 90);
   path.AddLine(x, y + roundHeight, x, y + height - roundHeight);
   path.CloseFigure();
}

Rect Duh::GdiplusEx::GetDrawRect(const Rect &rect, int penWidth)
{
   Rect rResult;
   int nOnePixel = penWidth % 2;
   penWidth -= nOnePixel;
   int nHalf = penWidth / 2;
   rResult.X = rect.X + nHalf;
   rResult.Y = rect.Y + nHalf;
   rResult.Width = rect.Width - nHalf*2 - nOnePixel;
   rResult.Height = rect.Height - nHalf*2 - nOnePixel;
   return rResult;
}

Image * Duh::GdiplusEx::LoadImageFromResource(HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType)
{
   HRSRC hRes;
   HGLOBAL hResEntry, hResData;
   Image *pImage = NULL;

   if ((hRes = FindResource(hModule, lpName, lpType)) != NULL)
   {
      DWORD dwBytesData = SizeofResource(hModule, hRes);
      hResEntry = LoadResource(hModule, hRes);
      hResData = GlobalAlloc(GPTR, dwBytesData);
      memcpy(hResData, LockResource(hResEntry), dwBytesData);
      UnlockResource(hResEntry);
      IStream* pStream = NULL;
      CreateStreamOnHGlobal(hResData, TRUE, &pStream);
      pImage = Image::FromStream(pStream, FALSE);
      pStream->Release();
   }
   return pImage;
}

Rect Duh::GdiplusEx:: ConvertRect(const RECT &rect)
{
   return Rect(rect.left, rect.top, max(rect.right - rect.left, 0), max(rect.bottom - rect.top, 0));
}

Color Duh::GdiplusEx:: ConvertClr(COLORREF color)
{
   Color clr;
   clr.SetFromCOLORREF(color);
   return clr;
}


/************************************************************************/
/* CDCObject                                                            */
/************************************************************************/
Duh::GdiEx::CDCObject::CDCObject(HDC hDC)
: m_hDC(hDC), m_hCurObj(NULL), m_hOrigObj(NULL)
{
   ASSERT(m_hDC);
}

Duh::GdiEx::CDCObject::~CDCObject()
{
   Restore();
}

void Duh::GdiEx::CDCObject::SetObject(HGDIOBJ hObj, BOOL bDeleteCur /*= TRUE*/)
{
   ASSERT(m_hDC);
   HGDIOBJ hOldObj = ::SelectObject(m_hDC, hObj);
   ASSERT(hOldObj);
   ASSERT(hOldObj != HGDI_ERROR);
   if (hOldObj && (hOldObj != HGDI_ERROR))
   {
      if (m_hCurObj && bDeleteCur) ::DeleteObject(m_hCurObj);
      m_hCurObj = hObj;
      if (m_hOrigObj == NULL)
      {
         m_hOrigObj = hOldObj;
      }
   }
}

void Duh::GdiEx::CDCObject::Restore()
{
   if (m_hDC && m_hCurObj && m_hOrigObj)
   {
      ::SelectObject(m_hDC, m_hOrigObj);
      ::DeleteObject(m_hCurObj);
      m_hOrigObj = m_hCurObj = NULL;
   }
}

/************************************************************************/
/*CDCFont                                                               */
/************************************************************************/
Duh::GdiEx::CDCFont::CDCFont(HDC hDC)
: CDCObject(hDC)
, m_crOrigColor(CLR_NONE)
, m_nOrigMode(0)
{
}

Duh::GdiEx::CDCFont::CDCFont(HDC hDC, LPLOGFONT lpLogFont)
: CDCObject(hDC)
, m_crOrigColor(CLR_NONE)
, m_nOrigMode(0)
{
   SetLogFont(lpLogFont);
}

void Duh::GdiEx::CDCFont::GetLogFont(LPLOGFONT lpLogFont)
{
   ASSERT(lpLogFont);
   if (!lpLogFont) return;
   memset(lpLogFont, 0, sizeof(LOGFONT));
   HGDIOBJ hObj = ::GetCurrentObject(m_hDC, OBJ_FONT);
   ::GetObject(hObj, sizeof(LOGFONT), lpLogFont);
}

void Duh::GdiEx::CDCFont::SetLogFont(const LPLOGFONT lpLogFont)
{
   ASSERT(lpLogFont);
   if (!lpLogFont) return;
   SetObject(::CreateFontIndirect(lpLogFont));
}

void Duh::GdiEx::CDCFont::SetWeight(LONG lfWeight)
{
   LOGFONT lf;
   GetLogFont(&lf);
   lf.lfWeight = lfWeight;
   SetLogFont(&lf);
}

void Duh::GdiEx::CDCFont::SetColor(COLORREF color)
{
   if (color != CLR_NONE)
   {
      COLORREF oldClr = ::SetTextColor(m_hDC, color);
      if (m_crOrigColor == CLR_NONE)
      {
         m_crOrigColor = oldClr;
      }
   }
}

void Duh::GdiEx::CDCFont::SetBkMode(int nMode)
{
   int nOldMode = ::SetBkMode(m_hDC, nMode);
   if ((nOldMode != 0) && (m_nOrigMode == 0))
   {
      m_nOrigMode = nOldMode;
   }
}

void Duh::GdiEx::CDCFont::Restore()
{
   CDCObject::Restore();
   if (m_crOrigColor != CLR_NONE)
   {
      ::SetTextColor(m_hDC, m_crOrigColor);
      m_crOrigColor = CLR_NONE;
   }
   if (m_nOrigMode != 0)
   {
      ::SetBkMode(m_hDC, m_nOrigMode);
      m_nOrigMode = 0;
   }
}

/************************************************************************/
/*CDCPen                                                                */
/************************************************************************/
Duh::GdiEx::CDCPen::CDCPen(HDC hDC)
: CDCObject(hDC)
{

}

Duh::GdiEx::CDCPen::CDCPen(HDC hDC, UINT nPenStyle, UINT nWidth, COLORREF color)
: CDCObject(hDC)
{
   LOGPEN logPen;
   memset(&logPen, 0, sizeof(LOGPEN));
   logPen.lopnStyle = nPenStyle;
   logPen.lopnWidth.x = nWidth;
   logPen.lopnColor = color;
   SetLogPen(&logPen);
}

void Duh::GdiEx::CDCPen::GetLogPen(LPLOGPEN lpLogPen)
{
   ASSERT(lpLogPen);
   if (!lpLogPen) return;
   memset(lpLogPen, 0, sizeof(LOGPEN));
   HGDIOBJ hObj = ::GetCurrentObject(m_hDC, OBJ_PEN);
   ::GetObject(hObj, sizeof(LOGPEN), lpLogPen);
}

void Duh::GdiEx::CDCPen::SetLogPen(const LPLOGPEN lpLogPen)
{
   ASSERT(lpLogPen);
   if (!lpLogPen) return;
   SetObject(::CreatePenIndirect(lpLogPen));
}


void Duh::GdiEx::CDCPen::SetStyle(UINT nStyle)
{
   LOGPEN logPen;
   GetLogPen(&logPen);
   logPen.lopnStyle = nStyle;
   SetLogPen(&logPen);
}

void Duh::GdiEx::CDCPen::SetWidth(UINT nWidth)
{
   LOGPEN logPen;
   GetLogPen(&logPen);
   logPen.lopnWidth.x = nWidth;
   SetLogPen(&logPen);
}

void Duh::GdiEx::CDCPen::SetColor(COLORREF color)
{
   LOGPEN logPen;
   GetLogPen(&logPen);
   logPen.lopnColor = color;
   SetLogPen(&logPen);
}

/************************************************************************/
/* CDCBrush                                                             */
/************************************************************************/
Duh::GdiEx::CDCBrush::CDCBrush(HDC hDC)
:CDCObject(hDC)
{
   
}

Duh::GdiEx::CDCBrush::CDCBrush(HDC hDC, HBITMAP hbm)
: CDCObject(hDC)
{
   SetPatternBrush(hbm);
}

Duh::GdiEx::CDCBrush::CDCBrush(HDC hDC, int nIndex, COLORREF color)
: CDCObject(hDC)
{
   SetHatchBrush(nIndex, color);
}

Duh::GdiEx::CDCBrush::CDCBrush(HDC hDC, COLORREF color)
: CDCObject(hDC)
{
   SetSolidBrush(color);
}

Duh::GdiEx::CDCBrush::CDCBrush(HDC hDC, LPLOGBRUSH lpLogBrush)
: CDCObject(hDC)
{
   SetLogBrush(lpLogBrush);
}

void Duh::GdiEx::CDCBrush::GetLogBrush(const LPLOGBRUSH lpLogBrush)
{
   ASSERT(lpLogBrush);
   if (!lpLogBrush) return;
   memset(lpLogBrush, 0, sizeof(LPLOGBRUSH));
   HGDIOBJ hObj = ::GetCurrentObject(m_hDC, OBJ_BRUSH);
   ::GetObject(hObj, sizeof(LPLOGBRUSH), lpLogBrush);
}

void Duh::GdiEx::CDCBrush::SetLogBrush(LPLOGBRUSH lpLogBrush)
{
   ASSERT(lpLogBrush);
   if (!lpLogBrush) return;
   SetObject(::CreateBrushIndirect(lpLogBrush));
}

void Duh::GdiEx::CDCBrush::SetSolidBrush(COLORREF color)
{
   SetObject(::CreateSolidBrush(color));
}

void Duh::GdiEx::CDCBrush::SetHatchBrush(int nIndex, COLORREF color)
{
   SetObject(::CreateHatchBrush(nIndex,color));
}

void Duh::GdiEx::CDCBrush::SetPatternBrush(HBITMAP hbm)
{
   SetObject(::CreatePatternBrush(hbm));
}

