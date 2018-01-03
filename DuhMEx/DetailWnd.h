// DetailWnd.h: interface for the CDetailWnd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DETAILWND_H__231F1345_27FB_43DC_B526_66D65013678A__INCLUDED_)
#define AFX_DETAILWND_H__231F1345_27FB_43DC_B526_66D65013678A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>
namespace Duh
{
    class AFX_EXT_CLASS CDetailWnd : public CWnd
    {
        DECLARE_DYNAMIC(CDetailWnd)
    public:
        CDetailWnd(BOOL bAutoCleanResource = FALSE);
        virtual ~CDetailWnd();
    public:
        struct ExAttribute
        {
            int nMaxWidth;
            COLORREF clrBk;
            COLORREF clrTitle;
            COLORREF clrContent;
            COLORREF clrBorder;
            PLOGFONT pLfTitle;
            PLOGFONT pLfContent;
            BOOL bHideCloseBtn;
            BOOL bHidePositionTag;
        };
        static void Popup(CWnd *pParent, CString strTitle, CString strContent, CRect position, ExAttribute *pExAttribute = NULL);
        static void Popup(CWnd *pParent, CString strTitle, CString strContent, CPoint position, ExAttribute *pExAttribute = NULL);
    public:
        BOOL Create(CWnd *pParent);
        void SetMaxWidth(int nMaxWidth);
        void SetBkColor(COLORREF clrBk);
        void SetTitleColor(COLORREF clrTitle);
        void SetContentColor(COLORREF clrContent);
        void SetBorderColor(COLORREF clrBorder);
        void SetTitleFont(PLOGFONT pLogFont);
        void setContentFont(PLOGFONT pLogFont);
        void HideCloseBtn(BOOL bHide);
        void HidePositionTag(BOOL bHide);
        void SetUserPointer(CDetailWnd **pUserPtr);
        void Popup(CString strTitle, CString strContent, CRect position);
        void Popup(CString strTitle, CString strContent, CPoint position);

    protected:
        struct RECTINFO
        {
            CRect drawContent;
            CRect drawTitle;
            CRect closeBtn;
            CRect client;
            CRect window;
            CRgn  rgnWindow;
            std::vector<Gdiplus::Point> vecRgnPoint;
        };

        struct OVERFLOWLENGTH
        {
            int left;
            int right;
            int bottom;
            int top;
        };

        RECTINFO m_rectInfo;
        int m_nMaxWidth;
        CString m_strTitle;
        CString m_strContent;
        COLORREF m_clrBk;
        COLORREF m_clrContent;
        COLORREF m_clrTitle;
        COLORREF m_clrBorder;
        DWORD m_dwStyle;
        //CMultiTypeOwnerDrawBtn *m_pBtnClose;
        CDetailWnd **m_pUserPtr;

        CFont m_ftTitle;
        CFont m_ftContent;

    protected:

        void InitAllRect(RECTINFO &rectInfo);
        void OffSetAllRect(RECTINFO &rectInfo, CPoint WndPosition, CPoint TagPosition);
        void GetWindowOverflow(CRect windowRect, OVERFLOWLENGTH &overflowLength);
        void PositionWindow(CRect rcPosition);
        void LoadDefaultFont();
        void Hide();

        static LRESULT CALLBACK KeyboardHookProc(int code, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK MouseHookProc(int code, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK CallWndRetProc(int code, WPARAM wParam, LPARAM lParam);

        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg void OnPaint();
        afx_msg void OnClickCloseBtn();
        virtual void PostNcDestroy();
        virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

        DECLARE_MESSAGE_MAP()
    };
}
#endif // !defined(AFX_DETAILWND_H__231F1345_27FB_43DC_B526_66D65013678A__INCLUDED_)