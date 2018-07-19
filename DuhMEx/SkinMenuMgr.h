#pragma once
#include <map>
#include <vector>
#include <set>
#include "HookMgr.h"

namespace Duh
{
   class AFX_EXT_CLASS CSkinMenuStyle
   {
   public:
      enum GSKM_COLOR
      {
         GC_BACKGROUND,       //����ɫ
         GC_BORDER,           //�߿�
         GC_TEXT,             //�˵���ı� 
         GC_HILIGHT_TEXT,     //�˵��ͻ����ʾʱ    �ı�
         GC_HILIGHT_BG,       //�˵��ͻ����ʾʱ    ����ɫ
         GC_HILIGHT_BORDER,   //�˵��ͻ����ʾʱ    �߿�
         GC_GRAYED_TEXT,      //�˵���һ�/����ʱ   �ı�
         GC_GRAYED_BG,        //�˵��ͻ����ʾʱ    ����ɫ
         GC_GRAYED_BORDER,    //�˵���һ�/����ʱ   �߿�
         GC_SEPARATOR,        //�ָ���
         GC_CHECKBAR,         //λͼ����
      };
   public:
      UINT    m_nItemBorderWidth;
      BOOL    m_bViasbleMargin;
      LOGFONT m_LogFont;
   public:
      COLORREF    GetColor    (GSKM_COLOR eIndex);
      void        SetColor    (GSKM_COLOR eIndex, COLORREF color);
      const SIZE &GetItemRound();
      void        SetItemRound(UINT roundX, UINT roundY);
   public:
      CSkinMenuStyle();
   protected:
      std::map<GSKM_COLOR, COLORREF> m_mapColor;
      SIZE                           m_itemRound;
   };


   class CSkinMenuWnd;
   class CSkinMenuOwnerWnd;
   class AFX_EXT_CLASS CSkinMenuMgr :protected CHookMgr<CSkinMenuMgr>
   {
      friend class CHookMgr<CSkinMenuMgr>;
   public:
      virtual ~CSkinMenuMgr();
      static CSkinMenuMgr *Instance();
      void            Hook            ();
      void            UnHook          ();

      //************************************
      // Parameter: HMENU hMenu
      // Parameter: BOOL bAutoRemove      �˵����ڹرպ��Զ��ӹ����б����Ƴ�
      // Parameter: BOOL bContainSubMenu  �Ƿ�����Ӳ˵�
      //************************************
      void            AddFilterMenu   (HMENU hMenu,BOOL bAutoRemove = TRUE,BOOL bContainSubMenu = TRUE);
      void            RemoveFilterMenu(HMENU hMenu);
      CSkinMenuStyle *GetStyle        ();
   protected:
      typedef std::map<HWND, CSkinMenuWnd*> SKINMENUMAP;
      typedef std::multimap<HMENU, CSkinMenuOwnerWnd*> SKINMENUOWNERMAP;
      typedef std::set<HMENU> MENUFILTERSET;
      typedef struct tagFILTERMENUINFO
      {
         BOOL  bAutoRmove;
         BOOL  bContainSubMenu;
      }FILTERMENUINFO;
      typedef std::map<HMENU, FILTERMENUINFO *> FILITERMENUMAP;


      SKINMENUOWNERMAP    *m_pMapMenuOwner;
      SKINMENUMAP         *m_pMapMenus;
      CSkinMenuStyle      *m_pStyle;
      FILITERMENUMAP      *m_pMapFilter;
   protected:
      CSkinMenuMgr();

      virtual void OnCallWndProc     (const MSG& msg);
      static BOOL CallBackOnAttchMenu(CSkinMenuWnd *pWnd, LPARAM lParam);

      BOOL Filter                    (HMENU hFilterMenu);
      BOOL FilterSubMenu             (HMENU hMenu, HMENU hFilterMenu);
      BOOL AutoRemoveFilterMenu      (HMENU hMenu);
      BOOL SubclassMenu              (HWND hwnd);
      BOOL UnSubclassMenu            (HWND hwnd);
      BOOL SubclassMenuOwner         (HMENU hMenu,HWND hwnd);
      BOOL UnSubclassMenuOwner       (HMENU hMenu);
      void UnSubclassAllMenuOwner    ();
      BOOL MatchMenuOwner            (CSkinMenuWnd *pDelegate);
      void OnInitMenuPopup           (HWND hWnd,HMENU hPopupMenu, UINT nIndex, BOOL bSysMenu);
   };
}
