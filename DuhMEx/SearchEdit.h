#if !defined(AFX_SEARCHEDIT_H__997B7C8D_99CC_424B_BE05_016501608CB1__INCLUDED_)
#define AFX_SEARCHEDIT_H__997B7C8D_99CC_424B_BE05_016501608CB1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SearchEdit.h : header file
//
#include <IOSTREAM>
#include <MAP>
#include <vector>
/////////////////////////////////////////////////////////////////////////////
// CSearchEdit window
namespace Duh
{
    class CSearchListWnd;
    class AFX_EXT_CLASS CSearchEdit : public CEdit
    {
        friend class CSearchListWnd;
    public:
        struct RESULTINFO
        {
            CString strDispaly;
            CString strEditText;
            CString strEx;
        };
    public:
        typedef void(*pFunGetDataFunctor)(IN CString strEditText, IN LPARAM lParam, OUT std::vector<RESULTINFO> *vec);
        // Construction
    public:
        CSearchEdit();
        //bComplete = ture: DropDown完成
        BOOL IsDropDown(BOOL bComplete = FALSE);
        enum ALIMENT{ align_left, align_right };
        void SetDropDownStyle(ALIMENT eAlign = align_left, PSIZE pSize = NULL, CFont *pFont = NULL, BOOL bWidenWithScrollBar = FALSE);
        void SetSearchFunc(pFunGetDataFunctor pFun, LPARAM lParam);
        void SetSearchDelay(UINT millisecondsDelay);
        void SetDimText(CString strText);
        void SetDimTextFont(LOGFONT *pLogFont);
        //异步查询
        void AsynSearch(BOOL bAsyn);
        // Attributes

    protected:

        pFunGetDataFunctor m_pFunGetData;
        LPARAM m_lGetDataFuncParam;


        UINT m_nDelay;
        CString m_strDimText;
        ALIMENT m_eWndAlign;
        SIZE *m_pDropDownSize;
        CFont *m_pDropDownFont;
        CFont *m_pDimTextFont;
        BOOL m_bWidenWithScrollBar;
        CSearchListWnd *m_pDropDownWnd;

        void *m_pPictureCtrl;
        int m_nVisableFlags;
        int m_nUpdateFlags;
        HANDLE m_hThread;
        BOOL m_bAsynSearch;

        //m_pVecCache 用于缓存新数据（考虑异步获取数据）
        std::vector<RESULTINFO> *m_pVecCache;
        std::vector<RESULTINFO> *m_pVecData;


        void ExchangeCacheData();
        void UpdateDropDownSize();
        void UpdateDropDownPos();
        void ShowDropDownWnd(BOOL bShow = TRUE);
        LRESULT OnSearchResult(WPARAM wParam, LPARAM lParam);
        int CallBackDataFunc();
        void UpdateMargins();
        virtual void OnEndSelDropDownItem(RESULTINFO info);
        virtual void UpdateRightCtrl(UINT &nRightMargin);


        static UINT UpdateThread(LPVOID pParam);
        static LRESULT CALLBACK MouseHookProc(int code, WPARAM wParam, LPARAM lParam);

    protected:
        struct ANIMATIONINFO
        {
            CRect BeginRect;
            CRect EndRect;
            CRect CacheRect;
            int nType; //1下拉。2收回
        } m_sAMInfo;
        // Operations
    public:

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CSearchEdit)
    public:
        virtual BOOL PreTranslateMessage(MSG* pMsg);
        virtual void PreSubclassWindow();
        virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
        //}}AFX_VIRTUAL

        // Implementation
    public:
        virtual ~CSearchEdit();

        // Generated message map functions
    protected:
        //{{AFX_MSG(CSearchEdit)
        afx_msg BOOL OnChange();
        afx_msg void OnDestroy();
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg void OnMove(int x, int y);
        afx_msg void OnTimer(UINT nIDEvent);
        afx_msg void OnPaint();
        afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        //}}AFX_MSG

        DECLARE_MESSAGE_MAP()
    };





    /////////////////////////////////////////////////////////////////////////////
    // CSearchListWnd window
    class CSearchListWnd : public CWnd
    {
        // Construction
    public:
        CSearchListWnd(CSearchEdit *pWndEdit);

        CListCtrl *m_pWndList;
        int GetCount();
        int GetIndexOffsetPage(int nIndex, int nOffsetPage);
        int GetSel();
        void SetSel(int nIndex, BOOL bEnsureVisible = TRUE);
        void ResetData(int nCount);
        void OnClickItem(int nItem = -1);
        void UpdateColumnWidth();
    protected:
        CSearchEdit *m_pWndEdit;
        CString m_strDispalyBuffer;

        // Attributes
    public:

        // Operations
    public:

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CSearchListWnd)
    protected:
        virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
        virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
        //}}AFX_VIRTUAL

        // Implementation
    public:
        virtual ~CSearchListWnd();

        // Generated message map functions
    protected:
        //{{AFX_MSG(CSearchListWnd)
        afx_msg void OnClickListView(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnRClickListView(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnGetdispinfoListView(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg void OnDestroy();
        afx_msg void OnTimer(UINT nIDEvent);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };
    /////////////////////////////////////////////////////////////////////////////


    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}
#endif // !defined(AFX_SEARCHEDIT_H__997B7C8D_99CC_424B_BE05_016501608CB1__INCLUDED_)


