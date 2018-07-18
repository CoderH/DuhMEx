#include "stdafx.h"
#include "SkinMenuMgr.h"
#include "wclassdefines.h"
#include "SkinMenuWnd.h"
#include "WinClasses.h"

using namespace Duh;

Duh::CSkinMenuStyle::CSkinMenuStyle()
{
   memset(&m_LogFont, 0, sizeof(m_LogFont));
   m_LogFont.lfCharSet = DEFAULT_CHARSET;
   m_LogFont.lfHeight = -12;
   m_LogFont.lfWeight = FW_NORMAL;
   strcpy(m_LogFont.lfFaceName, "微软雅黑");
   m_bViasbleMargin = FALSE;
   m_nItemBorderWidth = 1;
   memset(&m_itemRound, 0, sizeof(m_itemRound));
   SetColor(GC_BACKGROUND, RGB(255, 255, 255));
   SetColor(GC_BORDER, RGB(217, 217, 217));
   SetColor(GC_TEXT, GetSysColor(COLOR_MENUTEXT));
   SetColor(GC_HILIGHT_TEXT, RGB(255, 255, 255));
   SetColor(GC_HILIGHT_BG, RGB(0, 122, 204));
   SetColor(GC_HILIGHT_BORDER, RGB(0, 99, 177));
   SetColor(GC_GRAYED_TEXT, GetSysColor(COLOR_GRAYTEXT));
   SetColor(GC_GRAYED_BG, RGB(217, 217, 217));
   SetColor(GC_GRAYED_BORDER, RGB(217, 217, 217));
   SetColor(GC_SEPARATOR, RGB(183, 195, 204));
   SetColor(GC_CHECKBAR, RGB(237, 238, 238));
}

void Duh::CSkinMenuStyle::SetColor(GSKM_COLOR eIndex, COLORREF color)
{
   m_mapColor[eIndex] = color;
}

COLORREF Duh::CSkinMenuStyle::GetColor(GSKM_COLOR eIndex)
{
   COLORREF clr = RGB(0, 0, 0);
   std::map<GSKM_COLOR, COLORREF>::iterator it = m_mapColor.find(eIndex);
   if (it != m_mapColor.end())
   {
      clr = it->second;
   }
   return clr;
}

void Duh::CSkinMenuStyle::SetItemRound(UINT roundX, UINT roundY)
{
   m_itemRound.cx = roundX;
   m_itemRound.cy = roundY;
}

const SIZE &Duh::CSkinMenuStyle::GetItemRound()
{
   return m_itemRound;
}

Duh::CSkinMenuMgr * Duh::CSkinMenuMgr::Instance()
{
   return &CHookMgr<CSkinMenuMgr>::GetInstance();
}

void Duh::CSkinMenuMgr::Hook()
{
   VERIFY(InitHooks(HM_CALLWNDPROC, NULL));
   InitHooks(HM_CALLWNDPROC, NULL);
}

void Duh::CSkinMenuMgr::UnHook()
{
   ReleaseHooks();
}

void Duh::CSkinMenuMgr::AddFilterMenu(HMENU hMenu, BOOL bAutoRemove, BOOL bContainSubMenu)
{
   ASSERT(::IsMenu(hMenu));
   if (::IsMenu(hMenu))
   {
      FILTERMENUINFO *info;
      FILITERMENUMAP::iterator it = m_pMapFilter->find(hMenu);
      if (it == m_pMapFilter->end())
      {
         info = new FILTERMENUINFO;
         (*m_pMapFilter)[hMenu] = info;
      }
      else
      {
         info = it->second;
      }
      info->bAutoRmove = bAutoRemove;
      info->bContainSubMenu = bContainSubMenu;
   }
}

void Duh::CSkinMenuMgr::RemoveFilterMenu(HMENU hMenu)
{
   FILITERMENUMAP::iterator it = m_pMapFilter->find(hMenu);
   if (it != m_pMapFilter->end())
   {
      delete it->second;
      m_pMapFilter->erase(it);
   }
}

BOOL Duh::CSkinMenuMgr::Filter(HMENU hFilterMenu)
{
   if (m_pMapFilter->find(hFilterMenu) != m_pMapFilter->end())
   {
      return TRUE;
   }
   FILITERMENUMAP::iterator it = m_pMapFilter->begin();
   for (; it != m_pMapFilter->end(); it++)
   {
      if (it->second->bContainSubMenu)
      {
         if (FilterSubMenu(it->first, hFilterMenu))
         {
            return TRUE;
         }
      }
   }
   return FALSE;
}

BOOL Duh::CSkinMenuMgr::FilterSubMenu(HMENU hMenu, HMENU hFilterMenu)
{
   int nCount = ::GetMenuItemCount(hMenu);
   while (nCount--)
   {
      MENUITEMINFO mii;
      memset(&mii, 0, sizeof(mii));
      mii.cbSize = sizeof(mii);
      mii.fMask = MIIM_SUBMENU;
      ::GetMenuItemInfo(hMenu, nCount, TRUE, &mii);
      if (mii.hSubMenu)
      {
         if (mii.hSubMenu == hFilterMenu)
         {
            return TRUE;
         }
         else
         {
            return FilterSubMenu(mii.hSubMenu, hFilterMenu);
         }
      }
   }
   return FALSE;
}

BOOL Duh::CSkinMenuMgr::AutoRemoveFilterMenu(HMENU hMenu)
{
   FILITERMENUMAP::iterator it = m_pMapFilter->find(hMenu);
   if (it != m_pMapFilter->end() && it->second->bAutoRmove)
   {
      delete it->second;
      m_pMapFilter->erase(it);
      return TRUE;
   }
   return FALSE;
}

Duh::CSkinMenuStyle * Duh::CSkinMenuMgr::GetStyle()
{
   return m_pStyle;
}

Duh::CSkinMenuMgr::CSkinMenuMgr()
{
   m_pStyle = new CSkinMenuStyle();
   m_pMapMenus = new SKINMENUMAP;
   m_pMapMenuOwner = new SKINMENUOWNERMAP;
   m_pMapFilter = new FILITERMENUMAP;
}

Duh::CSkinMenuMgr::~CSkinMenuMgr()
{
   if (m_pMapMenus)
   {
      SKINMENUMAP::iterator it = m_pMapMenus->begin();
      while (it != m_pMapMenus->end())
      {
         SKINMENUMAP::iterator itErase = it;
         it++;
         UnSubclassMenu(itErase->first);
      }
      delete m_pMapMenus;
      m_pMapMenus = NULL;
   }

   if (m_pMapMenuOwner)
   {
      UnSubclassAllMenuOwner();
      delete m_pMapMenuOwner;
      m_pMapMenuOwner = NULL;
   }

   if (m_pMapFilter)
   {
      FILITERMENUMAP::iterator itFilter = m_pMapFilter->begin();
      while (itFilter != m_pMapFilter->end())
      {
         delete itFilter->second;
         itFilter->second = NULL;
         itFilter++;
      }
      m_pMapFilter->clear();
      delete m_pMapFilter;
      m_pMapFilter = NULL;
   }
   
   if (m_pStyle)
   {
      delete m_pStyle;
      m_pStyle = NULL;
   }
}

BOOL Duh::CSkinMenuMgr::SubclassMenu(HWND hwnd)
{
   ASSERT(CWinClasses::IsClass(hwnd, WC_MENU));
   if (m_pMapMenus->find(hwnd) != m_pMapMenus->end())
   {
      return TRUE;
   }
   CSkinMenuWnd *pMenu = new CSkinMenuWnd(m_pStyle);
   if (pMenu->Attach(hwnd))
   {
      pMenu->SetCallBackOnAttchMenu(CallBackOnAttchMenu, (LPARAM)this);
      (*m_pMapMenus)[hwnd] = pMenu;
      return TRUE;
   }
   else
   {
      delete pMenu;
      pMenu = NULL;
   }
   return FALSE;
}

BOOL Duh::CSkinMenuMgr::UnSubclassMenu(HWND hwnd)
{
   ASSERT(CWinClasses::IsClass(hwnd, WC_MENU));
   SKINMENUMAP::iterator it = m_pMapMenus->find(hwnd);
   if (it != m_pMapMenus->end())
   {
      CSkinMenuWnd *pMenu = it->second;
      AutoRemoveFilterMenu(pMenu->GetMenu());
      UnSubclassMenuOwner(pMenu->GetMenu());
      delete pMenu;
      m_pMapMenus->erase(hwnd);
   }
   if (m_pMapMenus->size() == 0)
   {
      UnSubclassAllMenuOwner();
   }

   return TRUE;
}

void Duh::CSkinMenuMgr::OnCallWndProc(const MSG& msg)
{
   switch (msg.message)
   {
   case WM_CREATE:
      if (CSkinMenuWnd::IsMenuWnd(msg.hwnd))
      {
         BOOL bRes = SubclassMenu(msg.hwnd);
      }
      break;
   case WM_WINDOWPOSCHANGING:
      if (CSkinMenuWnd::IsMenuWnd(msg.hwnd) && msg.lParam)
      {
         WINDOWPOS* pWPos = (WINDOWPOS*)msg.lParam;

         if (pWPos->flags & SWP_SHOWWINDOW)
         {
            BOOL bRes = SubclassMenu(msg.hwnd);
         }
         else if (pWPos->flags & SWP_HIDEWINDOW)
         {
            BOOL bRes = UnSubclassMenu(msg.hwnd);
         }
      }
      break;
   case 0x1e2:
      if (CSkinMenuWnd::IsMenuWnd(msg.hwnd) && msg.wParam)
      {
         BOOL bRes = SubclassMenu(msg.hwnd);

         if (!bRes)
            TRACE("Skin failed on 0x1e2");
      }
      break;

   case WM_SHOWWINDOW:
      if (CSkinMenuWnd::IsMenuWnd(msg.hwnd))
      {
         if (msg.wParam)
         {
            BOOL bRes = SubclassMenu(msg.hwnd);
         }
         else // if (!msg.wParam)
         {
            BOOL bRes = UnSubclassMenu(msg.hwnd);
         }
      }
      break;

   case WM_DESTROY:
   case WM_NCDESTROY:
      if (CSkinMenuWnd::IsMenuWnd(msg.hwnd))
      {
         BOOL bRes = UnSubclassMenu(msg.hwnd);
      }
      break;
   case WM_INITMENUPOPUP:
      OnInitMenuPopup(msg.hwnd,(HMENU)msg.wParam, (UINT)LOWORD(msg.lParam), (BOOL)HIWORD(msg.lParam));
      break;
   default:
      break;
   }
}


BOOL Duh::CSkinMenuMgr::SubclassMenuOwner(HMENU hMenu,HWND hwnd)
{
   std::pair<SKINMENUOWNERMAP::iterator, SKINMENUOWNERMAP::iterator> range = m_pMapMenuOwner->equal_range(hMenu);
   SKINMENUOWNERMAP::iterator it = range.first;
   for (; it != range.second; it ++)
   {
      CSkinMenuOwnerWnd *pOwnerWnd = it->second;
      if (it->second->GetSafeHwnd() == hwnd)
      {
         delete it->second;
         m_pMapMenuOwner->erase(it);
         break;
      }
   }
   CSkinMenuOwnerWnd *pWnd = new CSkinMenuOwnerWnd;
   if (pWnd->Attch(hwnd, hMenu))
   {
      m_pMapMenuOwner->insert(std::pair<HMENU,CSkinMenuOwnerWnd*>(hMenu, pWnd));
      return TRUE;
   }
   else
   {
      delete pWnd;
      pWnd = NULL;
   }
   return FALSE;
}

BOOL Duh::CSkinMenuMgr::UnSubclassMenuOwner(HMENU hMenu)
{
   std::pair<SKINMENUOWNERMAP::iterator, SKINMENUOWNERMAP::iterator> range = m_pMapMenuOwner->equal_range(hMenu);
   SKINMENUOWNERMAP::iterator it = range.first;
   BOOL bRelust = (range.first != range.second);
   while (it != range.second)
   {
      delete it->second;
      SKINMENUOWNERMAP::iterator itErase = it;
      it++;
      m_pMapMenuOwner->erase(itErase);
   }
   return bRelust;
}

void Duh::CSkinMenuMgr::UnSubclassAllMenuOwner()
{
   SKINMENUOWNERMAP::iterator itOwner = m_pMapMenuOwner->begin();
   while (itOwner != m_pMapMenuOwner->end())
   {
      SKINMENUOWNERMAP::iterator itErase = itOwner;
      itOwner++;
      UnSubclassMenuOwner(itErase->first);
   }
}

BOOL Duh::CSkinMenuMgr::MatchMenuOwner(CSkinMenuWnd *pDelegate)
{
   BOOL bResult = FALSE;
   std::pair<SKINMENUOWNERMAP::iterator, SKINMENUOWNERMAP::iterator> range = m_pMapMenuOwner->equal_range(pDelegate->GetMenu());
   SKINMENUOWNERMAP::iterator it = range.first;
   for (; it != range.second; it++)
   {
      it->second->SetDelegate(pDelegate);
      bResult = TRUE;
   }
   return bResult;
}


void Duh::CSkinMenuMgr::OnInitMenuPopup(HWND hWnd, HMENU hPopupMenu, UINT nIndex, BOOL bSysMenu)
{
   if (bSysMenu)
   {
      AddFilterMenu(hPopupMenu,TRUE,FALSE);
   }
   else
   {
      //认定接收WM_INITMENUPOPUP窗口为拥有者。
      //其中有可能包含程序手动发送的WM_INITMENUPOPUP接收者，一并子类化。其中一个是正真拥有者。
      SubclassMenuOwner(hPopupMenu,hWnd);
   }

}

BOOL Duh::CSkinMenuMgr::CallBackOnAttchMenu(CSkinMenuWnd *pWnd, LPARAM lParam)
{
   CSkinMenuMgr *pMgr = (CSkinMenuMgr *)lParam;
   if (!pMgr->Filter(pWnd->GetMenu()))
   {
      return pMgr->MatchMenuOwner(pWnd);
   }
   return FALSE;
}
