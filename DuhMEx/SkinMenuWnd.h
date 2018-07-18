#pragma once
#include "afxwin.h"
#include "Subclass.h"
#include <map>

namespace Duh
{
   class CSkinMenuOwnerWnd;
   class CSkinMenuStyle;
   class CSkinMenuWnd : public CSubclassWnd
   {
   public:
      typedef BOOL (*funcOnAttchMenu)(CSkinMenuWnd *pWnd,LPARAM lParam);
      friend class CSkinMenuOwnerWnd;
      CSkinMenuWnd(CSkinMenuStyle *pStyle);
      ~CSkinMenuWnd();

      static BOOL IsMenuWnd(HWND hWnd);
      HMENU       GetMenu  ();
      BOOL        Attach   (HWND hWnd);
      BOOL        Detach   ();
      void        SetCallBackOnAttchMenu(funcOnAttchMenu pFunc, LPARAM lParam);

   protected:
      virtual LRESULT WindowProc(UINT msg, WPARAM wParam, LPARAM lParam);
      void AttchMenu            ();
      void DetachMenu           ();
      void MeasureItem          (LPMEASUREITEMSTRUCT lpMeasureItemStruct);
      void DrawItem             (LPDRAWITEMSTRUCT lpDrawItemStruct);
      void OnWindowPosChanging  (WINDOWPOS* pWindowPos);
      void InitSectionWidth     ();
      void SetNcBgColor         ();
      void RestoreNcBgColor     ();
      void SetOwnerDrawData     ();
      void RestoreOwnerDrawData ();
      void OnNcPaint            (CDC* pDC);
      void ScreenToNoNClient    (LPRECT pRect);
      BOOL IsOwnerDraw          ();
   protected:
      class CItem
      {
      public:
         CItem(HMENU hMenu, UINT uItem, bool fByPosition);
      public:
         CString GetCaption        () const;
         CString GetShortcutKey    () const;
         HMENU   GetPopup          () const;
         UINT    GetID             () const;
         UINT    GetCheckWidth     () const;
         UINT    GetCaptionWidth   (CDC *pDc) const;
         UINT    GetSortcutKeyWidth(CDC *pDc) const;
         UINT    GetHeight         (CDC *pDc, CSkinMenuStyle *pStyle) const;
         UINT    GetWidth          (UINT nCheckWidth, UINT nCaptionWidth, UINT nShortcutKeyWidth, CSkinMenuStyle *pStyle) const;
         bool    IsSeparator       () const;
         bool    IsRadio           () const;
         bool    IsBitMap          () const;
         bool    IsChecked         () const;
         bool    IsDisabled        () const;
         bool    IsDefault         () const;
         bool    IsHilite          () const;
         void    Draw              (CDC *pDc, const CRect &rect, CSkinMenuStyle *pStyle, UINT nCheckWidth, UINT nCaptionWidth) const;
         void    DrawBackGround    (CDC *pDc, const CRect &rect,const CRect &rcBar, CSkinMenuStyle *pStyle) const;
         void    DrawSeparator     (CDC *pDc, const CRect &rect, CSkinMenuStyle *pStyle) const;
         void    DrawCheck         (CDC *pDc, const CRect &rect) const;
         void    DrawCaption       (CDC *pDc, const CRect &rect, CSkinMenuStyle *pStyle) const;
         void    DrawSortcutKey    (CDC *pDc, const CRect &rect, CSkinMenuStyle *pStyle) const;
         void    DrawArrow         (CDC *pDc, const CRect &rect) const;
      protected:
         void ParsingText(HMENU hMenu, UINT uItem, bool fByPosition);
      protected:
         MENUITEMINFO m_mii;
         CString      m_strShortcutKey;
         CString      m_strCaption;
      };
      /************************************************************************/

   protected:
      UINT            m_nCheckWidth;
      UINT            m_nCaptionWidth;
      UINT            m_nSortcutKeyWidth;
      HBRUSH          m_hNcBrush;
      HBRUSH          m_hOldNcBrush;
      BOOL            m_bAttchIngMenu;
      CSkinMenuStyle *m_pStyle;
      HMENU           m_hMenu;
      funcOnAttchMenu m_pOnAttchMenuFunc;
      LPARAM          m_lOnAttchMenuFuncParam;
      BOOL            m_bViald;
      std::map<UINT, MENUITEMINFO> m_mapOldItemInfo;
   };


   class CSkinMenuOwnerWnd : public CSubclassWnd
   {
   public:
      CSkinMenuOwnerWnd();
      ~CSkinMenuOwnerWnd();
      void SetDelegate(CSkinMenuWnd *pMenu);
      BOOL Attch(HWND hwnd, HMENU hMenu);
      BOOL Detach();
      HMENU GetMenu();
   protected:
      virtual LRESULT WindowProc(UINT msg, WPARAM wParam, LPARAM lParam);
      void OnInitMenuPopup(HMENU hPopupMenu, UINT nIndex, BOOL bSysMenu);
   protected:
      CSkinMenuWnd *m_pDelegate;
      HMENU m_hMenu;
   };
}
