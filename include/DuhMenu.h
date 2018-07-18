#pragma once
#include "afxwin.h"
#include <vector>

namespace Duh
{
    class AFX_EXT_CLASS CDuhMenu :public CMenu
    {
        //DECLARE_DYNAMIC(CDuhMenu)
    public:
        enum Alignment
        {
            DUHMENU_ALIGN_LEFT,
            DUHMENU_ALIGN_RIGHT,
            DUHMENU_ALIGN_CENTERNT
        };
    public:
        CDuhMenu();
        virtual ~CDuhMenu();

        BOOL CreatePopupMenu();
        BOOL AppendMenu(UINT nFlags, UINT_PTR nIDNewItem = 0, LPCTSTR lpszNewItem = NULL);
        BOOL AppendMenu(UINT nFlags, UINT_PTR nIDNewItem, const CBitmap* pBmp);
        BOOL InsertMenu(UINT nPosition, UINT nFlags, UINT_PTR nIDNewItem = 0, LPCTSTR lpszNewItem = NULL);
        BOOL InsertMenu(UINT nPosition, UINT nFlags, UINT_PTR nIDNewItem, const CBitmap* pBmp);
        BOOL RemoveMenu(UINT nPosition, UINT nFlags);
        BOOL DeleteMenu(UINT nPosition, UINT nFlags);
        BOOL DestroyMenu();
        // ************************************
        // Qualifier: 设置无边框需调用该重写函数。
        // Parameter: BOOL bVisbaleFrame 同时设置父菜单和子菜单边框显示
        // ************************************
        BOOL TrackPopupMenu(UINT nFlags, int x, int y, CWnd* pWnd, LPCRECT lpRect = 0, BOOL bVisbaleFrame = TRUE);

        void SetLeftWidth(int nWidth);          //-1,自适应图片大小
        void SetWidth(int nWidth);              //-1，自适应大小
        void SetItemHeight(int nHeight);
        void SetBackgroundColor(COLORREF color);
        void SetLeftColor(COLORREF color);
        void SetNormalTextColor(COLORREF color);
        void SetSelectedTextColor(COLORREF color);
        void SetDisableTextColor(COLORREF color);
        void SetSelectedFillColor(COLORREF color);
        void SetSelectedBorderColor(COLORREF color);
        void SetSeraratorColor(COLORREF color);
        void SetTextAlgin(Alignment emAlignment);
        void GetRects(std::vector<CRect> &vecRects);

    protected:
        int       m_nLeftWidth;
        int       m_nWidth;
        int       m_nItemHeight;
        Alignment m_emTextAlign;
        COLORREF  m_clrBackground;
        COLORREF  m_clrLeft;
        COLORREF  m_clrNormalText;
        COLORREF  m_clrSelectedText;
        COLORREF  m_clrDisableText;
        COLORREF  m_clrSelectedFill;
        COLORREF  m_clrSelectedBorder;
        COLORREF  m_clrSeparator;

        typedef struct tagDUHMENUITEM
        {
            CString strText;
            UINT_PTR nIDItem;
            const CBitmap *pBmp;
        }DUHMENUITEM;
        std::vector<DUHMENUITEM *> m_vcItemData;

        void EmtyItemData(DUHMENUITEM *pItem);
        BOOL DeleteItemData(UINT nPosition, UINT nFlags);
        void ResetStaticMenber();


        static LRESULT WINAPI MenuProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK CallWndHookProc(int code, WPARAM wParam, LPARAM lParam);
    public:
        virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
        virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
        void DrawItemBackground(LPDRAWITEMSTRUCT lpDIS);
        void DrawItemSelected(LPDRAWITEMSTRUCT lpDIS);
        void DrawItemImage(LPDRAWITEMSTRUCT lpDIS);
        void DrawItemText(LPDRAWITEMSTRUCT lpDIS);
        void DrawItemArrow(LPDRAWITEMSTRUCT lpDIS, CRect &rcClip);
        void DrawSerarator(LPDRAWITEMSTRUCT lpDIS);
    };
}
