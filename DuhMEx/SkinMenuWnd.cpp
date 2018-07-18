#include "stdafx.h"
#include "SkinMenuWnd.h"
#include "WinClasses.h"
#include "wclassdefines.h"
#include "GdipEx.h"
#include "SkinMenuMgr.h"
#include "Resource.h"
using namespace Gdiplus;
using namespace std;
using namespace Duh;

#define SPACING_ITEM_TOP               2

#define SPACING_ITEM_BOTTOM            SPACING_ITEM_TOP
#define SPACING_BORDER_RIGHT           3
#define SPACING_SEPARATOR_LEF          3
#define SPACING_CHECK_RIGHT            SPACING_BORDER_RIGHT
#define SPACING_CAPTION_LEFT           8
#define SPACING_SORTCUTKEY_LEFT        20
#define SPACING_ARROW_LEFT             2
#define SPACING_BORDER_LEFT            0


#define WIDTH_ARROW  16
#define HEGIHT_SEPARATOR      1
#define MIN_WIDTH_SEPARATOR   10
#define DEFAULT_CHECK_WIDTH   16


Duh::CSkinMenuWnd::CSkinMenuWnd(CSkinMenuStyle *pStyle)
: m_hNcBrush(NULL)
, m_nCheckWidth(0)
, m_nCaptionWidth(0)
, m_nSortcutKeyWidth(0)
, m_hMenu(NULL)
, m_bAttchIngMenu(FALSE)
, m_pStyle(pStyle)
, m_pOnAttchMenuFunc(NULL)
, m_bViald(FALSE)
{
   VERIFY(pStyle);
}


Duh::CSkinMenuWnd::~CSkinMenuWnd()
{
   Detach();
}

BOOL Duh::CSkinMenuWnd::Attach(HWND hWnd)
{
   if (!IsMenuWnd(hWnd))
      return FALSE;

   return HookWindow(hWnd);
}

BOOL Duh::CSkinMenuWnd::Detach()
{
   DetachMenu();
   return HookWindow((HWND)NULL);
}



void Duh::CSkinMenuWnd::SetCallBackOnAttchMenu(funcOnAttchMenu pFunc, LPARAM lParam)
{
   m_pOnAttchMenuFunc = pFunc;
   m_lOnAttchMenuFuncParam = lParam;
}

BOOL Duh::CSkinMenuWnd::IsMenuWnd(HWND hWnd)
{
   return CWinClasses::IsClass(hWnd, WC_MENU);
}



HMENU Duh::CSkinMenuWnd::GetMenu()
{
   return m_hMenu;
}

LRESULT Duh::CSkinMenuWnd::WindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
   HDC hDC;
   LRESULT lr;
   AttchMenu();
   if (m_bViald)
   {
      switch (msg)
      {
      case WM_WINDOWPOSCHANGING:
         OnWindowPosChanging((LPWINDOWPOS)lParam);
         break;
      case WM_PRINT:
         lr = CSubclassWnd::WindowProc(msg, wParam, lParam);
         if ((lParam&PRF_CHECKVISIBLE) && !IsWindowVisible(m_hWnd))
         {
            return lr;
         }
         if (lParam&PRF_NONCLIENT)
         {
            OnNcPaint(CDC::FromHandle((HDC)wParam));
         }
         return lr;
      case WM_NCPAINT:
         hDC = GetWindowDC(m_hWnd);
         OnNcPaint(CDC::FromHandle(hDC));
         ReleaseDC(m_hWnd, hDC);
         return 0;
         break;
      }
   }
   return CSubclassWnd::WindowProc(msg, wParam, lParam);
}

void Duh::CSkinMenuWnd::AttchMenu()
{
   if (!m_bAttchIngMenu)
   {
      m_bAttchIngMenu = TRUE;

      if (m_hMenu == NULL)
      {
         ASSERT(m_hWnd);
         HMENU hMenu = (HMENU)::SendMessage(m_hWnd, MN_GETHMENU, 0, 0);
         if (hMenu)
         {
            m_hMenu = hMenu;
            BOOL bPass = (m_pOnAttchMenuFunc && m_pOnAttchMenuFunc(this, m_lOnAttchMenuFuncParam));
            if (bPass && !IsOwnerDraw())
            {
               m_bViald = TRUE;
            }
         }
         if (m_bViald)
         {
            if (!m_pStyle->m_bViasbleMargin)
            {
               DWORD dwExStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
               ::SetWindowLong(m_hWnd, GWL_EXSTYLE, (dwExStyle&~(WS_EX_DLGMODALFRAME | WS_EX_STATICEDGE)));
            }
            InitSectionWidth();
            SetNcBgColor();
            SetOwnerDrawData();
         }
      }
      m_bAttchIngMenu = FALSE;
   }
}

void Duh::CSkinMenuWnd::DetachMenu()
{
   if (m_bViald)
   {
      RestoreNcBgColor();
      RestoreOwnerDrawData();
   }
   m_hMenu = NULL;
   m_bViald = FALSE;
}

void Duh::CSkinMenuWnd::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
   if (!m_bViald) return;

   CDC *pDc = CDC::FromHandle(lpDrawItemStruct->hDC);
   CItem item(m_hMenu, lpDrawItemStruct->itemID, FALSE);
   item.Draw(pDc, lpDrawItemStruct->rcItem,m_pStyle, m_nCheckWidth, m_nCaptionWidth);
}

void Duh::CSkinMenuWnd::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
   if (!m_bViald) return;
   if (lpMeasureItemStruct->CtlType != ODT_MENU) return;

   CDC *pDC = CDC::FromHandle(::GetDC(m_hWnd));
   GdiEx::CDCFont dcFont(*pDC, &m_pStyle->m_LogFont);
   CItem item(m_hMenu, lpMeasureItemStruct->itemID, FALSE);
   lpMeasureItemStruct->itemWidth = item.GetWidth(m_nCheckWidth,m_nCaptionWidth,m_nSortcutKeyWidth,m_pStyle);
   lpMeasureItemStruct->itemHeight = item.GetHeight(pDC, m_pStyle);
   dcFont.Restore();
   ::ReleaseDC(m_hWnd, *pDC);
}

void Duh::CSkinMenuWnd::OnWindowPosChanging(WINDOWPOS* pWindowPos)
{
   if (!m_pStyle->m_bViasbleMargin)
   {
      pWindowPos->cy -= 4;
      pWindowPos->cx -= 4;
   }
}

void Duh::CSkinMenuWnd::InitSectionWidth()
{
   ASSERT(m_hMenu);
   if (m_hMenu)
   {
      CDC *pDc = CDC::FromHandle(::GetDC(m_hWnd));
      GdiEx::CDCFont dcFont(*pDc, &m_pStyle->m_LogFont);
      int nCount = ::GetMenuItemCount(m_hMenu);
      while (nCount--)
      {
         CItem item(m_hMenu, nCount, TRUE);
         m_nCheckWidth = max(m_nCheckWidth, item.GetCheckWidth());
         m_nCaptionWidth = max(m_nCaptionWidth, item.GetCaptionWidth(pDc));
         m_nSortcutKeyWidth = max(m_nSortcutKeyWidth,item.GetSortcutKeyWidth(pDc));
      }
      dcFont.Restore();
      ::ReleaseDC(m_hWnd, *pDc);
   }
}

void Duh::CSkinMenuWnd::SetNcBgColor()
{
   ASSERT(m_hMenu);
   if (m_hMenu)
   {
      if (m_hNcBrush == NULL)
      {
         m_hNcBrush = ::CreateSolidBrush(m_pStyle->GetColor(CSkinMenuStyle::GC_BACKGROUND));
      }

      MENUINFO cmi;
      memset(&cmi, 0, sizeof(cmi));
      ::GetMenuInfo(m_hMenu, &cmi);
      if (cmi.cbSize == 0) cmi.cbSize = sizeof(cmi);
      cmi.fMask |= MIM_BACKGROUND;
      cmi.hbrBack = m_hNcBrush;
      ::SetMenuInfo(m_hMenu, &cmi);
   }
}

void Duh::CSkinMenuWnd::RestoreNcBgColor()
{
   ASSERT(m_hMenu);
   if (m_hMenu)
   {
      if (m_hNcBrush != NULL)
      {
         ::DeleteObject(m_hNcBrush);
         m_hNcBrush = NULL;
      }

      MENUINFO cmi;
      memset(&cmi, 0, sizeof(cmi));
      ::GetMenuInfo(m_hMenu, &cmi);
      if (cmi.cbSize == 0) cmi.cbSize = sizeof(cmi);
      cmi.fMask |= MIM_BACKGROUND;
      cmi.hbrBack = m_hOldNcBrush;
      ::SetMenuInfo(m_hMenu, &cmi);
   }
}

void Duh::CSkinMenuWnd::SetOwnerDrawData()
{
   int nCount = GetMenuItemCount(m_hMenu);
   m_mapOldItemInfo.clear();
   while (nCount--)
   {
      MENUITEMINFO mii;
      memset(&mii, 0, sizeof(mii));
      mii.cbSize = sizeof(mii);
      mii.fMask = MIIM_FTYPE | MIIM_DATA | MIIM_ID;
      ::GetMenuItemInfo(m_hMenu, nCount, TRUE, &mii);
      m_mapOldItemInfo[mii.wID] = mii;
      mii.fType |= MFT_OWNERDRAW;
      mii.dwItemData = (ULONG_PTR)m_hMenu;
      ::SetMenuItemInfo(m_hMenu, nCount, TRUE, &mii);
   }
}

void Duh::CSkinMenuWnd::RestoreOwnerDrawData()
{
   std::map<UINT, MENUITEMINFO>::iterator it = m_mapOldItemInfo.begin();
   for (; it != m_mapOldItemInfo.end();it ++)
   {
      ::SetMenuItemInfo(m_hMenu, it->second.wID, FALSE, &it->second);
   }
}

void Duh::CSkinMenuWnd::OnNcPaint(CDC* pDC)
{
   CRect rcWnd,rcClient;
   GetWindowRect(&rcWnd);
   ScreenToNoNClient(&rcWnd);
   GetClientRect(&rcClient);
   ClientToScreen(&rcClient);
   ScreenToNoNClient(&rcClient);

   if (!m_pStyle->m_bViasbleMargin && (m_pStyle->GetColor(CSkinMenuStyle::GC_CHECKBAR) != m_pStyle->GetColor(CSkinMenuStyle::GC_BACKGROUND)))
   {
      CRect rcBarEx;
      CBrush brush;
      brush.CreateSolidBrush(m_pStyle->GetColor(CSkinMenuStyle::GC_CHECKBAR));
      rcBarEx = rcWnd;
      rcBarEx.right = rcClient.left + 2;
      rcBarEx.top = rcClient.top - 1;
      rcBarEx.bottom = rcClient.bottom + 1;
      pDC->FillRect(&rcBarEx, &brush);
      rcBarEx = rcWnd;
      rcBarEx.top = rcClient.top - 1;
      rcBarEx.bottom = rcClient.top + 2;
      rcBarEx.right = rcClient.left + m_pStyle->m_nItemBorderWidth + SPACING_BORDER_RIGHT + m_nCheckWidth + SPACING_CHECK_RIGHT;
      pDC->FillRect(&rcBarEx, &brush);
      rcBarEx = rcWnd;
      rcBarEx.top = rcClient.bottom - 2;
      rcBarEx.bottom = rcClient.bottom + 1;
      rcBarEx.right = rcClient.left + m_pStyle->m_nItemBorderWidth + SPACING_BORDER_RIGHT + m_nCheckWidth + SPACING_CHECK_RIGHT;
      pDC->FillRect(&rcBarEx, &brush);
   }
   if (m_pStyle->GetColor(CSkinMenuStyle::GC_BORDER) != m_pStyle->GetColor(CSkinMenuStyle::GC_BACKGROUND))
   {
      CBrush brush;
      brush.CreateSolidBrush(m_pStyle->GetColor(CSkinMenuStyle::GC_BORDER));
      pDC->FrameRect(rcWnd, &brush);
   }
}

void Duh::CSkinMenuWnd::ScreenToNoNClient(LPRECT pRect)
{
   if (pRect)
   {
      CRect rWindow;
      GetWindowRect(rWindow);
      ::OffsetRect(pRect, -rWindow.left, -rWindow.top);
   }
}

BOOL Duh::CSkinMenuWnd::IsOwnerDraw()
{
   int nCount = GetMenuItemCount(m_hMenu);
   while (nCount--)
   {
      MENUITEMINFO mii;
      memset(&mii, 0, sizeof(mii));
      mii.cbSize = sizeof(mii);
      mii.fMask = MIIM_FTYPE;
      ::GetMenuItemInfo(m_hMenu, nCount, TRUE, &mii);
      if ((mii.fType&MFT_OWNERDRAW ) == MFT_OWNERDRAW)
      {
         return TRUE;
      }
   }
   return FALSE;
}

/************************************************************************/
//Class CItem
/************************************************************************/
Duh::CSkinMenuWnd::CItem::CItem(HMENU hMenu, UINT uItem, bool fByPosition)
{
   memset(&m_mii, 0, sizeof(MENUITEMINFO));
   m_mii.cbSize = sizeof(MENUITEMINFO);
   m_mii.fMask = (MIIM_BITMAP | MIIM_CHECKMARKS | MIIM_DATA | MIIM_FTYPE | MIIM_ID | MIIM_STATE | MIIM_SUBMENU);
   ::GetMenuItemInfo(hMenu, uItem, fByPosition, &m_mii);
   ParsingText(hMenu, uItem, fByPosition);
}

CString Duh::CSkinMenuWnd::CItem::GetCaption() const
{
   return m_strCaption;
}

CString Duh::CSkinMenuWnd::CItem::GetShortcutKey() const
{
   return m_strShortcutKey;
}

bool Duh::CSkinMenuWnd::CItem::IsSeparator() const
{
   ASSERT(m_mii.fMask & MIIM_FTYPE);
   return (m_mii.fType & MFT_SEPARATOR) == MFT_SEPARATOR;
}
bool Duh::CSkinMenuWnd::CItem::IsRadio() const
{
   ASSERT(m_mii.fMask & MIIM_FTYPE);
   return (m_mii.fType & MFT_RADIOCHECK) == MFT_RADIOCHECK;
}

bool Duh::CSkinMenuWnd::CItem::IsBitMap() const
{
   ASSERT(m_mii.fMask & MIIM_BITMAP);
   return (m_mii.fType & MFT_BITMAP) == MFT_BITMAP;
}

bool Duh::CSkinMenuWnd::CItem::IsChecked() const
{
   ASSERT(m_mii.fMask & MIIM_STATE);
   return (m_mii.fState & MFS_CHECKED) == MFS_CHECKED;
}

bool Duh::CSkinMenuWnd::CItem::IsDisabled() const
{
   ASSERT(m_mii.fMask & MIIM_STATE);
   return (m_mii.fState & MFS_GRAYED) == MFS_GRAYED;
}

bool Duh::CSkinMenuWnd::CItem::IsDefault() const
{
   ASSERT(m_mii.fMask & MIIM_STATE);
   return (m_mii.fState & MFS_DEFAULT) == MFS_DEFAULT;
}
bool Duh::CSkinMenuWnd::CItem::IsHilite() const
{
   ASSERT(m_mii.fMask & MIIM_STATE);
   return (m_mii.fState & MFS_HILITE) == MFS_HILITE;
}

void Duh::CSkinMenuWnd::CItem::Draw(CDC *pDc, const CRect &rect, CSkinMenuStyle *pStyle, UINT nCheckWidth, UINT nCaptionWidth) const
{
   int nSveDc = pDc->SaveDC();

   CRgn rgn;
   CRect rcContent,rcCheckBar, rcCheck, rcCaption, rcSortCutKey, rcArrow, rcSeparator;

   rcContent = rcCheckBar = rect;
   pStyle->m_nItemBorderWidth = 1;

   rcContent.DeflateRect(pStyle->m_nItemBorderWidth, pStyle->m_nItemBorderWidth, pStyle->m_nItemBorderWidth, pStyle->m_nItemBorderWidth);
   rcContent.DeflateRect(SPACING_BORDER_RIGHT, SPACING_ITEM_TOP, SPACING_BORDER_LEFT, SPACING_ITEM_BOTTOM);
   rcCheck = rcCaption = rcSortCutKey = rcArrow = rcSeparator = rcContent;
   rcCheck.right = rcCheck.left + nCheckWidth;
   rcCheckBar.right = rcCheck.right + SPACING_CHECK_RIGHT;

   DrawBackGround(pDc, rect,rcCheckBar,pStyle);
   if (IsSeparator())
   {
      rcSeparator.left = rcCheckBar.right + SPACING_SEPARATOR_LEF;
      DrawSeparator(pDc, rcSeparator,pStyle);
   }
   else
   {
      rcCaption.left = rcCheckBar.right + SPACING_CAPTION_LEFT;
      rcCaption.right = rcCaption.left + nCaptionWidth;
      rcArrow.left = rcArrow.right - WIDTH_ARROW;
      rcSortCutKey.left = rcCaption.right;
      rcSortCutKey.right = rcArrow.left - SPACING_ARROW_LEFT;

      DrawCheck(pDc, rcCheck);
      DrawCaption(pDc, rcCaption,pStyle);
      DrawSortcutKey(pDc, rcSortCutKey,pStyle);
      DrawArrow(pDc, rcArrow);
   }
   pDc->RestoreDC(nSveDc);
   ::ExcludeClipRect(*pDc, rect.left, rect.top, rect.right, rect.bottom);
}

void Duh::CSkinMenuWnd::CItem::DrawBackGround(CDC *pDc, const CRect &rect, const CRect &rcBar, CSkinMenuStyle *pStyle) const
{
   Gdiplus::Graphics graphics(pDc->GetSafeHdc());
   CRect rcItem = rect;
   //背景
   if (IsHilite())
   {
      CSkinMenuStyle::GSKM_COLOR clrBg = (IsDisabled()) ? CSkinMenuStyle::GC_GRAYED_BG : CSkinMenuStyle::GC_HILIGHT_BG;
      CSkinMenuStyle::GSKM_COLOR clrBorder = (IsDisabled()) ? CSkinMenuStyle::GC_GRAYED_BORDER : CSkinMenuStyle::GC_HILIGHT_BORDER;
      if (pStyle->GetItemRound().cx > 0 || pStyle->GetItemRound().cy > 0)
      {
         graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
         rcItem.DeflateRect(1, 1); //反走样模式下多留1像素用于渲染
      }
      Gdiplus::GraphicsPath path;
      GdiplusEx::MakeRoundRectPath(path, GdiplusEx::ConvertRect(rcItem), pStyle->GetItemRound().cx, pStyle->GetItemRound().cy);
      Gdiplus::SolidBrush brush(GdiplusEx::ConvertClr(pStyle->GetColor(clrBg)));
      graphics.FillPath(&brush, &path);
      if (pStyle->m_nItemBorderWidth > 0)
      {
         path.Reset();
         Rect rDrawBorade = GdiplusEx::GetDrawRect(GdiplusEx::ConvertRect(rcItem), pStyle->m_nItemBorderWidth);
         GdiplusEx::MakeRoundRectPath(path, rDrawBorade, pStyle->GetItemRound().cx, pStyle->GetItemRound().cy);
         Gdiplus::Pen pen(GdiplusEx::ConvertClr(pStyle->GetColor(clrBorder)), (REAL)pStyle->m_nItemBorderWidth);
         graphics.DrawPath(&pen, &path);
      }
   }
   else
   {
      Gdiplus::SolidBrush brush(GdiplusEx::ConvertClr(pStyle->GetColor(CSkinMenuStyle::GC_BACKGROUND)));
      graphics.FillRectangle(&brush, GdiplusEx::ConvertRect(rcItem));
      brush.SetColor(GdiplusEx::ConvertClr(pStyle->GetColor(CSkinMenuStyle::GC_CHECKBAR)));
      graphics.FillRectangle(&brush, GdiplusEx::ConvertRect(rcBar));
   }
}

void Duh::CSkinMenuWnd::CItem::DrawSeparator(CDC *pDc, const CRect &rect, CSkinMenuStyle *pStyle) const
{
   if (IsSeparator())
   {
      GdiEx::CDCPen dcPen(*pDc, PS_SOLID, 1, pStyle->GetColor(CSkinMenuStyle::GC_SEPARATOR));
      pDc->MoveTo(rect.left, rect.top + rect.Height() / 2);
      pDc->LineTo(rect.right, rect.top + rect.Height() / 2);
   }
}

void Duh::CSkinMenuWnd::CItem::DrawCheck(CDC *pDc, const CRect &rect) const
{
   Gdiplus::Graphics graphics(pDc->m_hDC);
   Gdiplus::Image *pImage = NULL;

   if (IsChecked())
   {
      if (m_mii.hbmpChecked)
      {
         pImage = Bitmap::FromHBITMAP(m_mii.hbmpChecked, NULL);
      }
      else
      {
         if (IsRadio())
         {
            if (IsHilite())pImage = GdiplusEx::LoadImageFromResource(g_hModuleInstance, MAKEINTRESOURCE(IDB_PNG_RADIO_HILITE_16), "PNG");
            else pImage = GdiplusEx::LoadImageFromResource(g_hModuleInstance, MAKEINTRESOURCE(IDB_PNG_RADIO_16), "PNG");
         }
         else
         {
            if (IsHilite()) pImage = GdiplusEx::LoadImageFromResource(g_hModuleInstance, MAKEINTRESOURCE(IDB_PNG_CHECK_HILITE16), "PNG");
            else pImage = GdiplusEx::LoadImageFromResource(g_hModuleInstance, MAKEINTRESOURCE(IDB_PNG_CHECK16), "PNG");
         }
      }
   }
   else if (m_mii.hbmpUnchecked)
   {
      pImage = Bitmap::FromHBITMAP(m_mii.hbmpUnchecked, NULL);
   }
   //句柄已无效
   /* else  if ((lpMii->hbmpItem == HBMMENU_POPUP_CLOSE) ||
       (lpMii->hbmpItem == HBMMENU_POPUP_MAXIMIZE) ||
       (lpMii->hbmpItem == HBMMENU_POPUP_MINIMIZE) ||
       (lpMii->hbmpItem == HBMMENU_POPUP_RESTORE) ||
       (lpMii->hbmpItem == HBMMENU_SYSTEM))
       {;
       pBitmap = Bitmap::FromHBITMAP(lpMii->hbmpItem, NULL);
       }*/
   if (pImage)
   {
      int x = max(0, rect.right - pImage->GetWidth());
      int y = rect.top + max(0, (max(0, rect.Height() - (int)pImage->GetHeight())) / 2);
      graphics.DrawImage(pImage, x, y, pImage->GetWidth(), pImage->GetHeight());
      
      delete pImage;
      pImage = NULL;
   }
}

void Duh::CSkinMenuWnd::CItem::DrawCaption(CDC *pDc, const CRect &rect, CSkinMenuStyle *pStyle) const
{
   CRect rcText = rect;
   if (IsBitMap() && m_mii.hbmpItem)
   {
      Gdiplus::Graphics graphics(pDc->m_hDC);
      Gdiplus::Bitmap *pBitmap = Gdiplus::Bitmap::FromHBITMAP(m_mii.hbmpItem, NULL);
      if (IsDisabled())
      {
         ColorMatrix cm =
         {
            0.299f, 0.299f, 0.299f, 0.0f, 0.0f,
            0.587f, 0.587f, 0.587f, 0.0f, 0.0f,
            0.144f, 0.144f, 0.144f, 0.0f, 0.0f,
            0.000f, 0.000f, 0.000f, 1.0f, 0.0f,
            0.000f, 0.000f, 0.000f, 0.0f, 1.0f
         };
         ImageAttributes pimageattributes;
         pimageattributes.SetColorMatrix(&cm, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);
         //转换成灰度并绘制在新的bmp上
         graphics.DrawImage(pBitmap, GdiplusEx::ConvertRect(rect), 0, 0, pBitmap->GetWidth(), pBitmap->GetHeight(), UnitPixel, &pimageattributes);
      }
      else
      {
         graphics.DrawImage(pBitmap, GdiplusEx::ConvertRect(rect));
      }
      delete pBitmap;
   }
   else
   {
      COLORREF clr = pStyle->GetColor(CSkinMenuStyle::GC_TEXT);
      GdiEx::CDCFont dcFont(*pDc,&pStyle->m_LogFont);
      if (IsDisabled())
      {
         clr = pStyle->GetColor(CSkinMenuStyle::GC_GRAYED_TEXT);
      }
      else
      {
         if (IsHilite()) clr = pStyle->GetColor(CSkinMenuStyle::GC_HILIGHT_TEXT);
         if (IsDefault()) dcFont.SetWeight(FW_BOLD);
      }
      dcFont.SetColor(clr);
      dcFont.SetBkMode(TRANSPARENT);
      pDc->DrawText(m_strCaption, &rcText, DT_VCENTER | DT_SINGLELINE);
   }
}

void Duh::CSkinMenuWnd::CItem::DrawSortcutKey(CDC *pDc, const CRect &rect, CSkinMenuStyle *pStyle) const
{
   CRect rcText = rect;
   COLORREF clr = pStyle->GetColor(CSkinMenuStyle::GC_TEXT);
   GdiEx::CDCFont dcFont(*pDc,&pStyle->m_LogFont);
   if (IsDisabled())
   {
      clr = pStyle->GetColor(CSkinMenuStyle::GC_GRAYED_TEXT);
   }
   else
   {
      if (IsHilite()) clr = pStyle->GetColor(CSkinMenuStyle::GC_HILIGHT_TEXT);
      if (IsDefault())
      {
         dcFont.SetWeight(FW_BOLD);
      }
   }
   dcFont.SetColor(clr);
   dcFont.SetBkMode(TRANSPARENT);
   pDc->DrawText(m_strShortcutKey, &rcText, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
}

void Duh::CSkinMenuWnd::CItem::DrawArrow(CDC *pDc, const CRect &rect) const
{
   Gdiplus::Graphics graphics(pDc->m_hDC);
   if (GetPopup())
   {
      Gdiplus::Image *pImage = NULL;
      if (IsHilite()) pImage = GdiplusEx::LoadImageFromResource(g_hModuleInstance, MAKEINTRESOURCE(IDB_PNG_ARROW_RIGHT_SM_HIKITE_16), "PNG");
      else pImage = GdiplusEx::LoadImageFromResource(g_hModuleInstance, MAKEINTRESOURCE(IDB_PNG_ARROW_RIGHT_SM_16), "PNG");
      int x = rect.right - pImage->GetWidth();
      int y = rect.top + max(0, max(0,rect.Height() - (int)pImage->GetHeight()) / 2);
      graphics.DrawImage(pImage, x, y, pImage->GetWidth(), pImage->GetHeight());
      delete pImage;
   }
}

HMENU Duh::CSkinMenuWnd::CItem::GetPopup() const
{
   ASSERT(m_mii.fMask & MIIM_SUBMENU);
   return m_mii.hSubMenu;
}

UINT Duh::CSkinMenuWnd::CItem::GetID() const
{
   ASSERT(m_mii.fMask & MIIM_ID);
   return m_mii.wID;
}


UINT Duh::CSkinMenuWnd::CItem::GetCheckWidth() const
{
   BITMAP bm;
   UINT nWidth = DEFAULT_CHECK_WIDTH;
   if (IsChecked() && m_mii.hbmpChecked)
   {
      CBitmap::FromHandle(m_mii.hbmpChecked)->GetBitmap(&bm);
      nWidth = bm.bmWidth;
   }
   else if (!IsChecked() && m_mii.hbmpUnchecked)
   {
      CBitmap::FromHandle(m_mii.hbmpUnchecked)->GetBitmap(&bm);
      nWidth = bm.bmWidth;
   }
   return nWidth;
}

UINT Duh::CSkinMenuWnd::CItem::GetCaptionWidth(CDC *pDc) const
{
   BITMAP bm;
   ASSERT(pDc);
   if (!pDc) return 0;
   int nWidth = 0;
   if (IsBitMap() && m_mii.hbmpItem)
   {
      CBitmap::FromHandle(m_mii.hbmpItem)->GetBitmap(&bm);
      nWidth = bm.bmWidth;
   }
   else if (!IsSeparator())
   {
      GdiEx::CDCFont dcFont(*pDc);
      if (IsDefault()) dcFont.SetWeight(FW_BOLD);
      nWidth = pDc->GetTextExtent(m_strCaption).cx;
   }
   return nWidth;
}


UINT Duh::CSkinMenuWnd::CItem::GetSortcutKeyWidth(CDC *pDc) const
{
   ASSERT(pDc);
   if (!pDc) return 0;
   int nWidth = 0;

   GdiEx::CDCFont dcFont(*pDc);
   if (IsDefault())
   {
      dcFont.SetWeight(FW_BOLD);
   }
   nWidth = pDc->GetTextExtent(m_strShortcutKey).cx;
   return nWidth;
}

UINT Duh::CSkinMenuWnd::CItem::GetHeight(CDC *pDc, CSkinMenuStyle *pStyle) const
{
   ASSERT(pDc);
   BITMAP bm;
   SIZE TextSize;
   int nHeight = 0;
   if (IsSeparator())
   {
      nHeight = SPACING_ITEM_TOP + HEGIHT_SEPARATOR + SPACING_ITEM_BOTTOM;
   }
   else
   {
      if (IsChecked() && m_mii.hbmpChecked)
      {
         CBitmap::FromHandle(m_mii.hbmpChecked)->GetBitmap(&bm);
         nHeight= max(nHeight, bm.bmHeight);
      }
      else if (!IsChecked() && m_mii.hbmpUnchecked)
      {
         CBitmap::FromHandle(m_mii.hbmpUnchecked)->GetBitmap(&bm);
         nHeight = max(nHeight, bm.bmHeight);
      }
      if (IsBitMap() && m_mii.hbmpItem)
      {
         CBitmap::FromHandle(m_mii.hbmpItem)->GetBitmap(&bm);
         nHeight = max(nHeight, bm.bmHeight);
      }
      else
      {
         GdiEx::CDCFont dcFont(*pDc);
         if (IsDefault()) dcFont.SetWeight(FW_BOLD);
         TextSize = pDc->GetTextExtent(m_strCaption);
         nHeight = max(nHeight, TextSize.cy);
         TextSize = pDc->GetTextExtent(m_strShortcutKey);
         nHeight = max(nHeight, TextSize.cy);
      }
      nHeight += (SPACING_ITEM_TOP + SPACING_ITEM_BOTTOM);
      nHeight += pStyle->m_nItemBorderWidth * 2;
   }
   return nHeight;
}

UINT Duh::CSkinMenuWnd::CItem::GetWidth(UINT nCheckWidth, UINT nCaptionWidth, UINT nShortcutKeyWidth, CSkinMenuStyle *pStyle) const
{
   UINT nWidth = 0;
   if (IsSeparator())
   {
      nWidth = MIN_WIDTH_SEPARATOR;
   }
   else
   {
      nWidth += (SPACING_BORDER_RIGHT + nCheckWidth + SPACING_CHECK_RIGHT);
      nWidth += (SPACING_CAPTION_LEFT + nCaptionWidth);
      nWidth += (SPACING_SORTCUTKEY_LEFT + nShortcutKeyWidth);
      nWidth += (SPACING_ARROW_LEFT + WIDTH_ARROW + SPACING_BORDER_LEFT);
      nWidth += pStyle->m_nItemBorderWidth * 2;
   }
   return nWidth;
}

void Duh::CSkinMenuWnd::CItem::ParsingText(HMENU hMenu, UINT uItem, bool fByPosition)
{
   m_strCaption.Empty();
   m_strShortcutKey.Empty();
   MENUITEMINFO mii;
   memset(&mii, 0, sizeof(mii));
   mii.cbSize = sizeof(MENUITEMINFO);
   mii.fMask = MIIM_STRING;
   mii.dwTypeData = NULL;
   ::GetMenuItemInfo(hMenu, uItem, fByPosition, &mii);
   if (mii.cch > 0)
   {
      char *pSortCutKeyEnty;
      int nBufLen = mii.cch + 1;
      char *buf = new char[nBufLen];
      char *temp = new char[nBufLen];
      memset(buf, 0, nBufLen);
      memset(temp, 0, nBufLen);
      mii.dwTypeData = buf;
      mii.cch = nBufLen;
      ::GetMenuItemInfo(hMenu, uItem, fByPosition, &mii);
      if (buf)
      {
         strcpy(temp, buf);
         pSortCutKeyEnty = strstr(temp, "\t");
         if (pSortCutKeyEnty) *pSortCutKeyEnty = 0;
         m_strCaption = temp;
         m_strCaption.Remove('&');

         strcpy(temp, buf);
         pSortCutKeyEnty = strstr(temp, "\t");
         while (pSortCutKeyEnty)
         {
            pSortCutKeyEnty += strlen("\t");
            m_strShortcutKey = pSortCutKeyEnty;
            pSortCutKeyEnty = strstr(pSortCutKeyEnty, "\t");
         }
      }
      delete[] buf;
      delete[] temp;
   }
}

Duh::CSkinMenuOwnerWnd::CSkinMenuOwnerWnd()
: m_pDelegate(NULL)
, m_hMenu(NULL)
{

}

Duh::CSkinMenuOwnerWnd::~CSkinMenuOwnerWnd()
{

}

void Duh::CSkinMenuOwnerWnd::SetDelegate(CSkinMenuWnd *pMenu)
{
   m_pDelegate = pMenu;
}

BOOL Duh::CSkinMenuOwnerWnd::Attch(HWND hwnd, HMENU hMenu)
{
   if (HookWindow(hwnd))
   {
      m_hMenu = hMenu;
      return TRUE;
   }
   return FALSE;
}

BOOL Duh::CSkinMenuOwnerWnd::Detach()
{
   if (HookWindow((HWND)NULL))
   {
      m_hMenu = NULL;
      return TRUE;
   }
   return FALSE;
}

HMENU Duh::CSkinMenuOwnerWnd::GetMenu()
{
   return m_hMenu;
}

LRESULT Duh::CSkinMenuOwnerWnd::WindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
   LRESULT lr;
   switch (msg)
   {
   case WM_INITMENUPOPUP:
      lr = CSubclassWnd::WindowProc(msg, wParam, lParam);
      OnInitMenuPopup((HMENU)wParam, (UINT)LOWORD(lParam), (BOOL)HIWORD(lParam));
      return lr;
   case WM_MEASUREITEM:
      if (m_pDelegate)
      {
         LPMEASUREITEMSTRUCT lpMs = (LPMEASUREITEMSTRUCT)lParam;
         if ((lpMs->CtlType == ODT_MENU) && ((HMENU)(lpMs->itemData) == m_pDelegate->GetMenu()))
         {
            m_pDelegate->MeasureItem(lpMs);
            return TRUE;
         }
      }
      else if (m_pNext == NULL)
      {
         return 0;
      }
      break;
   case WM_DRAWITEM:
      if (m_pDelegate)
      {
         LPDRAWITEMSTRUCT lpDs = (LPDRAWITEMSTRUCT)lParam;
         if ((lpDs->CtlType == ODT_MENU) && ((HMENU)(lpDs->itemData) == m_pDelegate->GetMenu()))
         {
            m_pDelegate->DrawItem(lpDs);
            return TRUE;
         }
      }
      else if (m_pNext == NULL)
      {
         return 0;
      }
      break;
   default:
      break;
   }
   return CSubclassWnd::WindowProc(msg, wParam, lParam);
}

void Duh::CSkinMenuOwnerWnd::OnInitMenuPopup(HMENU hPopupMenu, UINT nIndex, BOOL bSysMenu)
{

}
