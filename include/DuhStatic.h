#pragma once


// CDuhStatic
namespace Duh
{
    class AFX_EXT_CLASS CDuhStatic : public CStatic
    {
        DECLARE_DYNAMIC(CDuhStatic)

    public:
        CDuhStatic();
        virtual ~CDuhStatic();
        void SetBkBrushHighlightColor(COLORREF color);
        void SetTextHighlightColor(COLORREF color);
        void SetBorderNormalColor(COLORREF color);
        void SetBorderHighlightColor(COLORREF color);
        void DisplayBorder(BOOL bEnable = TRUE);
        void SetSystemCursor(HCURSOR hCursor);
    protected:
        COLORREF m_clrBorderNormalColor;
        COLORREF m_clrBorderHighlightColor;
        COLORREF m_clrBkBrushHighlightColor;
        COLORREF m_clrTextHighlightColor;
        HCURSOR  m_hCursor;
        int      m_dwMouseFlags;
    protected:
        void DrawBorder(CDC *pDC);
    protected:
        DECLARE_MESSAGE_MAP()
        afx_msg void OnPaint();
        afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
        afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
        afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam);
        afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
    };
}
