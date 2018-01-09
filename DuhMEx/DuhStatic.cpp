// DuhStatic.cpp : 实现文件
//

#include "stdafx.h"
#include "DuhStatic.h"

#define  FS_HOVER 0x00000001
#define  FS_TRACE 0x00000002
// CDuhStatic

IMPLEMENT_DYNAMIC(CDuhStatic, CStatic)

Duh::CDuhStatic::CDuhStatic()
: m_clrBorderNormalColor(CLR_NONE)
, m_clrBorderHighlightColor(CLR_NONE)
, m_clrBkBrushHighlightColor(CLR_NONE)
, m_clrTextHighlightColor(CLR_NONE)
, m_dwMouseFlags(0)
{
    
}

Duh::CDuhStatic::~CDuhStatic()
{
}


void Duh::CDuhStatic::SetBkBrushHighlightColor(COLORREF color)
{
    m_clrBkBrushHighlightColor = color;
    Invalidate(FALSE);
}

void Duh::CDuhStatic::SetTextHighlightColor(COLORREF color)
{
    m_clrTextHighlightColor = color;
    Invalidate(FALSE);
}

void Duh::CDuhStatic::SetBorderNormalColor(COLORREF color)
{
    m_clrBorderNormalColor = color;
    Invalidate(FALSE);
}

void Duh::CDuhStatic::SetBorderHighlightColor(COLORREF color)
{
    m_clrBorderHighlightColor = color;
    Invalidate(FALSE);
}

void Duh::CDuhStatic::DisplayBorder(BOOL bEnable /*= TRUE*/)
{
    long lStyle = GetWindowLong(m_hWnd, GWL_STYLE);
    if (((lStyle&WS_BORDER) != 0) != bEnable)
    {
        CRect crItem;
        CWnd *pParent = GetParent();
        if (pParent != NULL)
        {
            if (bEnable) ModifyStyle(0, WS_BORDER);
            else ModifyStyle(WS_BORDER, 0);
            GetWindowRect(crItem);
            pParent->ScreenToClient(crItem);
            crItem.InflateRect(1, 0);
            MoveWindow(crItem);
            crItem.DeflateRect(1, 0);
            MoveWindow(crItem);
        }
    }
}

void Duh::CDuhStatic::SetSystemCursor(HCURSOR hCursor)
{ 
    m_hCursor = hCursor;
}
void Duh::CDuhStatic::DrawBorder(CDC *pDC)
{
    HBRUSH hBrush = (m_clrBorderNormalColor != CLR_NONE) ? CreateSolidBrush(m_clrBorderNormalColor) : GetSysColorBrush(COLOR_WINDOWFRAME);
    if (((m_dwMouseFlags&FS_HOVER) != 0) && (m_clrBorderHighlightColor != CLR_NONE)) hBrush = CreateSolidBrush(m_clrBorderHighlightColor);
    CRect rect;
    GetWindowRect(&rect);
    ScreenToClient(rect);
    pDC->FrameRect(rect, CBrush::FromHandle(hBrush));
}

BEGIN_MESSAGE_MAP(CDuhStatic, CStatic)
    ON_WM_PAINT()
    ON_WM_CTLCOLOR_REFLECT()
    ON_WM_SETCURSOR()
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
    ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
END_MESSAGE_MAP()



// CDuhStatic 消息处理程序
HBRUSH Duh::CDuhStatic::CtlColor(CDC* pDC, UINT nCtlColor)
{
    HBRUSH hbr = NULL;
    HWND hWnd = ::GetParent(m_hWnd);
    HDC hDC = ::GetDC(hWnd);
    hbr = (HBRUSH)::SendMessage(hWnd, WM_CTLCOLORDLG, (WPARAM)hDC, (LPARAM)hWnd);
    ::ReleaseDC(hWnd, hDC);

    if ((m_dwMouseFlags&FS_HOVER) != 0)
    {
        if (m_clrBkBrushHighlightColor != CLR_NONE) hbr = CreateSolidBrush(m_clrBkBrushHighlightColor);
        if (m_clrTextHighlightColor != CLR_NONE) pDC->SetTextColor(m_clrTextHighlightColor);
    }
    pDC->SetBkMode(TRANSPARENT);
    return hbr;
}
void Duh::CDuhStatic::OnPaint()
{
    CStatic::OnPaint();
    CDC *pDc = GetDC();
    if ((GetWindowLong(m_hWnd, GWL_STYLE)&WS_BORDER) != 0)
    {
        DrawBorder(pDc);
    }
    ReleaseDC(pDc);
}


BOOL Duh::CDuhStatic::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    if (m_hCursor != NULL)
    {
        ::SetCursor(m_hCursor);
        return TRUE;
    }
    return CStatic::OnSetCursor(pWnd, nHitTest, message);
}


LRESULT Duh::CDuhStatic::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
    m_dwMouseFlags &= ~(FS_HOVER | FS_TRACE);
    Invalidate();
    return TRUE;
}

void Duh::CDuhStatic::OnMouseMove(UINT nFlags, CPoint point)
{
    TRACKMOUSEEVENT trackmouseevent;
    if (((m_dwMouseFlags&FS_TRACE) == 0) && (GetCapture() == NULL))
    {
        trackmouseevent.cbSize = sizeof(trackmouseevent);
        trackmouseevent.hwndTrack = m_hWnd;
        trackmouseevent.dwFlags = TME_LEAVE | TME_HOVER;
        trackmouseevent.dwHoverTime = 1;
        _TrackMouseEvent(&trackmouseevent);
        m_dwMouseFlags |= FS_TRACE;
    }
    CStatic::OnMouseMove(nFlags, point);
}

LRESULT Duh::CDuhStatic::OnMouseHover(WPARAM wparam, LPARAM lparam)
{
    m_dwMouseFlags |= FS_HOVER;
    Invalidate();
    return TRUE;
}
