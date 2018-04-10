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
        CDetailWnd();
        virtual ~CDetailWnd();
    public:
        enum //WndStyle
        {
            WS_CLOSEBTN              = 0x00000001,    //关闭按钮
            WS_POSITIIONTAG          = 0x00000002,    //位置标记
            WS_AUTO_CLEAN_RESOURCE   = 0x00000008     //隐藏时自动清理资源
        };
        struct ExAttribute
        {
            int nMaxWidth;
            COLORREF clrBk;
            COLORREF clrTitle;
            COLORREF clrContent;
            COLORREF clrBorder;
            PLOGFONT pLfTitle;
            PLOGFONT pLfContent;
            DWORD    dwWndStyle;
        };
        static void Popup(CWnd *pParent, CString strTitle, CString strContent, CRect position, ExAttribute *pExAttribute = NULL);
        static void Popup(CWnd *pParent, CString strTitle, CString strContent, CPoint position, ExAttribute *pExAttribute = NULL);
    public:
        BOOL Create(CWnd *pParent);
        void ModifyWndStyle(DWORD dwAdd, DWORD dwRemove);
        void SetMaxWidth(int nMaxWidth);
        void SetBkColor(COLORREF clrBk);
        void SetTitleColor(COLORREF clrTitle);
        void SetContentColor(COLORREF clrContent);
        void SetBorderColor(COLORREF clrBorder);
        void SetTitleFont(PLOGFONT pLogFont);
        void setContentFont(PLOGFONT pLogFont);
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
            std::vector<CPoint> vecRgnPoint;
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
        DWORD m_dwFlags;
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

        void DrawCloseBtn(CDC *pDC);
        void DrawTitle(CDC *pDC);
        void DrawContent(CDC *pDC);

        static LRESULT CALLBACK KeyboardHookProc(int code, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK MouseHookProc(int code, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK CallWndRetProc(int code, WPARAM wParam, LPARAM lParam);

        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        afx_msg void OnPaint();
        afx_msg void OnClickCloseBtn();
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
        afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
        virtual void PostNcDestroy();
        virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

        DECLARE_MESSAGE_MAP()
    };
}
#endif // !defined(AFX_DETAILWND_H__231F1345_27FB_43DC_B526_66D65013678A__INCLUDED_)