#include "stdafx.h"
#include "DuhMenu.h"
#include <algorithm>
using namespace Gdiplus;
using namespace std;

static HHOOK sm_hCallWndProcHook = NULL;
const TCHAR DuhMenu_oldProc[]    = _T("DuhMenu_oldProc");
static vector<HWND> sm_vecHookWnd;
static BOOL sm_bVisableFrame = TRUE;


#define SPACE_TEXT              5
#define SPACE_VER_ARROW         4
#define WIDTH_ARROW             4
#define HEIGHT_ARROW            8

//IMPLEMENT_DYNAMIC(CDuhMenu, CMenu)
void Duh::CDuhMenu::EmtyItemData(DUHMENUITEM *pItem)
{
    if (pItem == NULL) return;
    pItem->strText.Empty();
    pItem->nIDItem = 0;
    pItem->pBmp = NULL;
}

BOOL Duh::CDuhMenu::DeleteItemData(UINT nPosition, UINT nFlags)
{
    BOOL bSuccess = FALSE;
    if (nFlags & MF_BYPOSITION)
    {
        if (nPosition < m_vcItemData.size())
        {
            vector<DUHMENUITEM *>::iterator iter = m_vcItemData.begin() + nPosition;
            delete ((DUHMENUITEM *)(*iter));
            *iter = NULL;
            m_vcItemData.erase(iter);
            bSuccess = TRUE;
        }
    }
    else
    {
        vector<DUHMENUITEM *>::iterator iter = m_vcItemData.begin();
        for (; iter != m_vcItemData.end(); iter++)
        {
            if ((*iter)->nIDItem == nPosition)
            {
                delete ((DUHMENUITEM *)(*iter));
                *iter = NULL;
                m_vcItemData.erase(iter);
                bSuccess = TRUE;
                break;
            }
        }
    }
    return bSuccess;
}

void Duh::CDuhMenu::ResetStaticMenber()
{
    sm_bVisableFrame = TRUE;
}

LRESULT WINAPI Duh::CDuhMenu::MenuProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WNDPROC oldWndProc = (WNDPROC)::GetProp(hWnd, DuhMenu_oldProc);
    LRESULT lResult;
    if (find(sm_vecHookWnd.begin(), sm_vecHookWnd.end(), hWnd) != sm_vecHookWnd.end())
    {
        switch (message)
        {
        case WM_CREATE:
        {
                          lResult = CallWindowProc(oldWndProc, hWnd, message, wParam, lParam);
                          if (!sm_bVisableFrame)
                          {
                              DWORD dwExStyle = ::GetWindowLong(hWnd, GWL_EXSTYLE);
                              ::SetWindowLong(hWnd, GWL_EXSTYLE, (dwExStyle&~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE)));
                          }

        }
            break;
        case WM_WINDOWPOSCHANGING:
        {
                                     if (!sm_bVisableFrame)
                                     {
                                         LPWINDOWPOS lpPos = (LPWINDOWPOS)lParam;
                                         lpPos->cy -= 2 * GetSystemMetrics(SM_CYBORDER) + 2;
                                         lpPos->cx -= 2 * GetSystemMetrics(SM_CYBORDER) + 2;
                                     }
                                     lResult = CallWindowProc(oldWndProc, hWnd, message, wParam, lParam);
        }
            break;
        default:
            lResult = CallWindowProc(oldWndProc, hWnd, message, wParam, lParam);
            break;
        }
    }
    else
    {
        lResult = CallWindowProc(oldWndProc, hWnd, message, wParam, lParam);
    }
    return lResult;
}


LRESULT CALLBACK Duh::CDuhMenu::CallWndHookProc(int code, WPARAM wParam, LPARAM lParam)
{
    CWPSTRUCT* pStruct = (CWPSTRUCT*)lParam;

    while (code == HC_ACTION)
    {
        HWND hWnd = pStruct->hwnd;

        if (pStruct->message != WM_CREATE && pStruct->message != 0x01E2)
        {
            break;
        }

        // 是否为菜单类 ----------------------------------------
        TCHAR strClassName[10];
        int Count = ::GetClassName(hWnd, strClassName, sizeof(strClassName) / sizeof(strClassName[0]));
        if (Count != 6 || _tcscmp(strClassName, _T("#32768")) != 0)
        {
            break;
        }

        // 是否已经被子类化 ------------------------------------
        if (::GetProp(pStruct->hwnd, DuhMenu_oldProc) != NULL)
        {
            break;
        }
        sm_vecHookWnd.push_back(pStruct->hwnd);

        // 取得原来的窗口过程 ----------------------------------
        WNDPROC oldWndProc = (WNDPROC)(long)::GetWindowLong(pStruct->hwnd, GWL_WNDPROC);
        if (oldWndProc == NULL)
        {
            break;
        }

        ASSERT(oldWndProc != MenuProc);
        // 保存到窗口的属性中 ----------------------------------
        if (!SetProp(pStruct->hwnd, DuhMenu_oldProc, oldWndProc))
        {
            break;
        }

        // 子类化 ----------------------------------------------
        if (!SetWindowLong(pStruct->hwnd, GWL_WNDPROC, (DWORD)(ULONG)MenuProc))
        {
            ::RemoveProp(pStruct->hwnd, DuhMenu_oldProc);
            break;
        }
        break;
    }
    return CallNextHookEx(sm_hCallWndProcHook, code, wParam, lParam);
}


Duh::CDuhMenu::CDuhMenu()
: m_nWidth(-1)
, m_nItemHeight(-1)
, m_nLeftWidth(25)
, m_emTextAlign(DUHMENU_ALIGN_LEFT)
, m_clrBackground(GetSysColor(COLOR_WINDOW))
, m_clrNormalText(GetSysColor(COLOR_MENUTEXT))
, m_clrSelectedText(GetSysColor(COLOR_HIGHLIGHTTEXT))
, m_clrDisableText(RGB(128, 138, 145))
, m_clrLeft(RGB(236, 237, 238))
, m_clrSelectedFill(RGB(51, 127, 209)/*GetSysColor(COLOR_HIGHLIGHT)*/)
, m_clrSelectedBorder(RGB(51, 127, 209)/*GetSysColor(COLOR_HIGHLIGHT)*/)
, m_clrSeparator(RGB(183, 195, 204))
{

}


Duh::CDuhMenu::~CDuhMenu()
{
    DestroyMenu();
}

BOOL Duh::CDuhMenu::CreatePopupMenu()
{   
    BOOL bSuccess = CMenu::CreatePopupMenu();
    if (bSuccess)
    {
        MENUINFO cmi;
        memset(&cmi, 0, sizeof(cmi));
        ::GetMenuInfo(m_hMenu,&cmi);
        if (cmi.cbSize == 0) cmi.cbSize = sizeof(cmi);
        cmi.fMask |= MIM_BACKGROUND;
        cmi.hbrBack = CreateSolidBrush(m_clrBackground);
        ::SetMenuInfo(m_hMenu,&cmi);
    }
    return bSuccess;
}

BOOL Duh::CDuhMenu::DestroyMenu()
{
    vector<DUHMENUITEM *>::iterator iter = m_vcItemData.begin();
    for (; iter != m_vcItemData.end(); iter++)
    {
        DUHMENUITEM *pItemData = (DUHMENUITEM *)(*iter);
        delete pItemData;
        *iter = NULL;
    }
    m_vcItemData.clear();
    return CMenu::DestroyMenu();
}

void Duh::CDuhMenu::SetLeftWidth(int nWidth)
{
    m_nLeftWidth = nWidth;
}

void Duh::CDuhMenu::SetWidth(int nWidth)
{
    m_nWidth = nWidth;
}

void Duh::CDuhMenu::SetItemHeight(int nHeight)
{
    m_nItemHeight = nHeight;
}

void Duh::CDuhMenu::SetBackgroundColor(COLORREF color)
{
    m_clrBackground = color;

    MENUINFO cmi;
    memset(&cmi, 0, sizeof(cmi));
    ::GetMenuInfo(m_hMenu,&cmi);
    if (cmi.cbSize == 0) cmi.cbSize = sizeof(cmi);
    cmi.fMask |= MIM_BACKGROUND;
    cmi.hbrBack = CreateSolidBrush(m_clrBackground);
    ::SetMenuInfo(m_hMenu,&cmi);
}

void Duh::CDuhMenu::SetLeftColor(COLORREF color)
{
    m_clrLeft = color;
}

void Duh::CDuhMenu::SetNormalTextColor(COLORREF color)
{
    m_clrNormalText = color;
}

void Duh::CDuhMenu::SetSelectedTextColor(COLORREF color)
{
    m_clrSelectedText = color;
}

void Duh::CDuhMenu::SetDisableTextColor(COLORREF color)
{
    m_clrDisableText = color;
}

void Duh::CDuhMenu::SetSelectedFillColor(COLORREF color)
{
    m_clrSelectedFill = color;
}

void Duh::CDuhMenu::SetSelectedBorderColor(COLORREF color)
{
    m_clrSelectedBorder = color;
}

void Duh::CDuhMenu::SetSeraratorColor(COLORREF color)
{
    m_clrSeparator = color;
}


BOOL Duh::CDuhMenu::TrackPopupMenu(UINT nFlags, int x, int y, CWnd* pWnd, LPCRECT lpRect /*= 0*/, BOOL bVisbaleFrame /*= TRUE*/)
{
    AFX_MANAGE_STATE(AfxGetAppModuleState());
    sm_bVisableFrame = bVisbaleFrame;
    sm_hCallWndProcHook = ::SetWindowsHookEx(WH_CALLWNDPROC, CallWndHookProc,AfxGetInstanceHandle(), ::GetCurrentThreadId());
    BOOL bResult = CMenu::TrackPopupMenu(nFlags, x, y, pWnd, lpRect);
    ::UnhookWindowsHookEx(sm_hCallWndProcHook);
    ResetStaticMenber();
    return bResult;
}

BOOL Duh::CDuhMenu::AppendMenu(UINT nFlags, UINT_PTR nIDNewItem /*= 0*/, LPCTSTR lpszNewItem /*= NULL*/)
{
    int nCount = GetMenuItemCount();
    return InsertMenu(nCount, nFlags | MF_BYPOSITION, nIDNewItem, lpszNewItem);
}

BOOL Duh::CDuhMenu::AppendMenu(UINT nFlags, UINT_PTR nIDNewItem, const CBitmap* pBmp)
{
    int nCount = GetMenuItemCount();
    return InsertMenu(nCount, nFlags | MF_BYPOSITION, nIDNewItem, pBmp);
}


BOOL Duh::CDuhMenu::InsertMenu(UINT nPosition, UINT nFlags, UINT_PTR nIDNewItem /*= 0*/, LPCTSTR lpszNewItem /*= NULL*/)
{
    nFlags |= MF_OWNERDRAW;
    DUHMENUITEM *pMenuItem = new DUHMENUITEM;
    EmtyItemData(pMenuItem);
    pMenuItem->nIDItem = nIDNewItem;
    if ((nFlags&MF_SEPARATOR) == 0)
    {
        pMenuItem->strText = lpszNewItem;
    }
    BOOL bSuccess = CMenu::InsertMenu(nPosition, nFlags, nIDNewItem, (LPCTSTR)pMenuItem);
    if (bSuccess) m_vcItemData.push_back(pMenuItem);
    return bSuccess;
}

BOOL Duh::CDuhMenu::InsertMenu(UINT nPosition, UINT nFlags, UINT_PTR nIDNewItem, const CBitmap* pBmp)
{
    DUHMENUITEM *pMenuItem = new DUHMENUITEM;
    EmtyItemData(pMenuItem);
    pMenuItem->nIDItem = nIDNewItem;
    pMenuItem->pBmp = pBmp;
    BOOL bSuccess = CMenu::InsertMenu(nPosition, nFlags | MF_OWNERDRAW, nIDNewItem, (LPCTSTR)pMenuItem);
    if (bSuccess) m_vcItemData.push_back(pMenuItem);
    return bSuccess;
}

BOOL Duh::CDuhMenu::RemoveMenu(UINT nPosition, UINT nFlags)
{
    if (DeleteItemData(nPosition, nFlags))
    {
        return CMenu::RemoveMenu(nPosition, nFlags);
    }
    else
    {
        return FALSE;
    }
}

BOOL Duh::CDuhMenu::DeleteMenu(UINT nPosition, UINT nFlags)
{
    if (DeleteItemData(nPosition, nFlags))
    {
        return CMenu::DeleteMenu(nPosition, nFlags);
    }
    else
    {
        return FALSE;
    }
}

void Duh::CDuhMenu::SetTextAlgin(Alignment emAlignment)
{
    m_emTextAlign = emAlignment;
}

void Duh::CDuhMenu::GetRects(std::vector<CRect> &vecRects)
{
    vector<HWND>::iterator iter = sm_vecHookWnd.begin();
    for (iter; iter < sm_vecHookWnd.end(); iter++)
    {
        HWND hWnd = *iter;
        if (IsWindow(hWnd) && IsWindowVisible(hWnd))
        {
            CRect rect;
            GetWindowRect(hWnd, &rect);
            vecRects.push_back(rect);
        }
    }
}

void Duh::CDuhMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
    MENUITEMINFO itemInfo;
    ZeroMemory(&itemInfo, sizeof(itemInfo));
    itemInfo.cbSize = sizeof(itemInfo);
    itemInfo.fMask |= MIIM_TYPE | MIIM_SUBMENU;
    GetMenuItemInfo(lpMeasureItemStruct->itemID, &itemInfo, FALSE);
    if ((itemInfo.fType&MFT_SEPARATOR) == 0)
    {
        CDC *pDC = AfxGetMainWnd()->GetDC();
        DUHMENUITEM *pItemData = (DUHMENUITEM *)(lpMeasureItemStruct->itemData);
        CString strText;
        if (pItemData != NULL) strText = pItemData->strText;
        CSize TextSize = pDC->GetTextExtent(strText).cx;
        int nAutoWidth = m_nLeftWidth + SPACE_TEXT + TextSize.cx + SPACE_VER_ARROW + WIDTH_ARROW + SPACE_VER_ARROW;
        lpMeasureItemStruct->itemWidth = (m_nWidth == -1) ? nAutoWidth : m_nWidth;
        lpMeasureItemStruct->itemHeight = (m_nItemHeight == -1) ? GetSystemMetrics(SM_CYMENU) : m_nItemHeight;
        AfxGetMainWnd()->ReleaseDC(pDC);
    }
    else
    {
        lpMeasureItemStruct->itemWidth = 0;
        lpMeasureItemStruct->itemHeight = GetSystemMetrics(SM_CYMENU) / 2;
    }
    lpMeasureItemStruct->itemWidth -= GetSystemMetrics(SM_CXMENUCHECK);
}

void Duh::CDuhMenu::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CRect rcClip;
    if ((lpDrawItemStruct->CtlType == ODT_MENU) && (lpDrawItemStruct->itemID != NULL))
    {
        DRAWITEMSTRUCT dis = *lpDrawItemStruct;
        MENUITEMINFO itemInfo;
        ZeroMemory(&itemInfo, sizeof(itemInfo));
        itemInfo.cbSize = sizeof(itemInfo);
        itemInfo.fMask |= MIIM_TYPE | MIIM_SUBMENU;
        GetMenuItemInfo(dis.itemID, &itemInfo, FALSE);
        DrawItemBackground(&dis);
        if ((itemInfo.fType&MFT_SEPARATOR) == 0) DrawItemSelected(&dis);
        DrawItemImage(&dis);
        if ((itemInfo.fType&MFT_SEPARATOR) == 0) DrawItemText(&dis);
        else DrawSerarator(&dis);
        if (itemInfo.hSubMenu != NULL) DrawItemArrow(&dis,rcClip);

        if (!rcClip.IsRectEmpty()) ExcludeClipRect(dis.hDC, rcClip.left, rcClip.top, rcClip.right, rcClip.bottom);
    }
}
void Duh::CDuhMenu::DrawItemBackground(LPDRAWITEMSTRUCT lpDIS)
{
    CBrush BGBrush(m_clrBackground);
    CBrush LeftBrush(m_clrLeft);
    CRect rcLeft = lpDIS->rcItem;
    rcLeft.right = rcLeft.left + m_nLeftWidth;
    FillRect(lpDIS->hDC, &(lpDIS->rcItem), BGBrush);
    FillRect(lpDIS->hDC, &rcLeft, LeftBrush);
}
void Duh::CDuhMenu::DrawItemSelected(LPDRAWITEMSTRUCT lpDIS)
{
    if ((lpDIS->itemState & ODS_GRAYED) || (lpDIS->itemState & ODS_DISABLED))
        return;
    if (lpDIS->itemState & ODS_SELECTED)
    {
        CDC *pDC = CDC::FromHandle(lpDIS->hDC);
        if (pDC != NULL)
        {
            CPen BorderPen(PS_SOLID, 1, m_clrSelectedBorder);
            CBrush FillBrush(m_clrSelectedFill);
            CPen* pOldPen = pDC->SelectObject(&BorderPen);
            CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
            pDC->Rectangle(&(lpDIS->rcItem));

            pDC->SelectObject(pOldPen);
            pDC->SelectObject(pOldBrush);
        }
    }
}

void Duh::CDuhMenu::DrawItemImage(LPDRAWITEMSTRUCT lpDIS)
{

}

void Duh::CDuhMenu::DrawItemText(LPDRAWITEMSTRUCT lpDIS)
{
    CRect rect = lpDIS->rcItem;
    rect.right -= (SPACE_VER_ARROW*2 + WIDTH_ARROW);
    rect.left += m_nLeftWidth;
    rect.left += SPACE_TEXT;

    CDC *pDC = CDC::FromHandle(lpDIS->hDC);
    if (pDC != NULL)
    {
        COLORREF clrText = m_clrNormalText;
        if ((lpDIS->itemState & ODS_SELECTED) != 0) clrText = m_clrSelectedText;
        if ((lpDIS->itemState & ODS_DISABLED) != 0) clrText = m_clrDisableText;
        if ((lpDIS->itemState & ODS_GRAYED) != 0) clrText = GetSysColor(COLOR_GRAYTEXT);

        if (rect.right < rect.left) rect.left = rect.right;

        DUHMENUITEM *pItemData = (DUHMENUITEM *)(lpDIS->itemData);
        if (pItemData != NULL)
        {
            UINT unFormat = DT_SINGLELINE | DT_VCENTER;
            if (m_emTextAlign == DUHMENU_ALIGN_LEFT) unFormat |= DT_LEFT;
            else if (m_emTextAlign == DUHMENU_ALIGN_RIGHT) unFormat |= DT_RIGHT;
            else unFormat |= DT_CENTER;
            int nOldMode = pDC->SetBkMode(TRANSPARENT);
            COLORREF clrOldText = pDC->SetTextColor(clrText);
            pDC->DrawText(pItemData->strText, &rect, unFormat);
            pDC->SetTextColor(clrOldText);
            if (nOldMode > 0) pDC->SetBkMode(nOldMode);
        }
    }
}
void Duh::CDuhMenu::DrawItemArrow(LPDRAWITEMSTRUCT lpDIS, CRect &rcClip)
{
    //禁止系统画Arrow
    CRect rcSysArrow = lpDIS->rcItem;
    rcSysArrow.left = rcSysArrow.right - 15;
    rcClip.UnionRect(&rcClip, &rcSysArrow);

    //绘制Arrow
    CDC *pDC = CDC::FromHandle(lpDIS->hDC);
    if (pDC != NULL)
    {
        Gdiplus::Color  clrText;
        clrText.SetFromCOLORREF(m_clrNormalText);
        if ((lpDIS->itemState & ODS_SELECTED) != 0) clrText.SetFromCOLORREF(m_clrSelectedText);
        if ((lpDIS->itemState & ODS_DISABLED) != 0) clrText.SetFromCOLORREF(m_clrDisableText);
        if ((lpDIS->itemState & ODS_GRAYED) != 0) clrText.SetFromCOLORREF(GetSysColor(COLOR_GRAYTEXT));

        CRect rect = lpDIS->rcItem;
        rect.right = rect.right - SPACE_VER_ARROW;
        rect.left = rect.right - WIDTH_ARROW;
        rect.top = rect.top + (rect.Height() - HEIGHT_ARROW) / 2;
        rect.bottom = rect.top + HEIGHT_ARROW;
        Gdiplus::Graphics graphics(lpDIS->hDC);
        graphics.SetSmoothingMode(SmoothingModeAntiAlias);
        Gdiplus::Point point[3] = {
            Gdiplus::Point(rect.left, rect.top),
            Gdiplus::Point(rect.right, rect.top+rect.Height()/2),
            Gdiplus::Point(rect.left, rect.bottom)
        };
        graphics.FillPolygon(&SolidBrush(clrText), point, 3);
    }
}

void Duh::CDuhMenu::DrawSerarator(LPDRAWITEMSTRUCT lpDIS)
{
    CDC *pDC = CDC::FromHandle(lpDIS->hDC);
    if (pDC != NULL)
    {
        CRect rect = lpDIS->rcItem;
        rect.left += m_nLeftWidth;
        rect.left += SPACE_TEXT;
        CPen pen(PS_SOLID, 1, m_clrSeparator);
        CPen *pOldPen = pDC->SelectObject(&pen);
        pDC->MoveTo(rect.left, rect.top + rect.Height() / 2);
        pDC->LineTo(rect.right, rect.top + rect.Height() / 2);
        pDC->SelectObject(pOldPen);
    }
}
