// SearchEdit.cpp : implementation file
//

#include "stdafx.h"
#include "SearchEdit.h"
#include "resource.h"
#include "PictureEx.h"
using namespace std;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//updateFlags:
#define UPDATE_FLAG_NEWREQUEST   0x00000001     //新的请求
//custom message:
#define WM_CC_SEARCHRESULT        (WM_USER+350)
//timer
#define TIMER_DELAY              1  //搜索延迟
#define TIMER_DROPDOWN           2  //下拉框动效
//VisableFlags:
#define VISABLE_FLAG_LOADING     0x00000001
//control id
#define IDW_LISTVIEW             102


#define DEFUAL_DROPDOWN_HEIGHT  150
#define LISTCTRL_EXTRA_WIDTH    3

static HHOOK g_hMouseHook;
CPtrArray g_arMouseHookUsers;
static int FindMouseHookUser(void *ptr)
{
   int nCount = g_arMouseHookUsers.GetSize();
   int nIndex = -1;
   for (int i = 0; i < nCount; i++)
   {
      if (g_arMouseHookUsers.GetAt(i) == ptr)
      {
         nIndex = i;
         break;
      }
   }
   return nIndex;
}
static void RemoveMouseHookUser(void *ptr)
{
   int nIndex = FindMouseHookUser(ptr);
   if (nIndex >= 0) g_arMouseHookUsers.RemoveAt(nIndex);
}
static void AddMouseHookUser(void *ptr)
{
   int nIndex = FindMouseHookUser(ptr);
   if (nIndex < 0) g_arMouseHookUsers.Add(ptr);
}

/////////////////////////////////////////////////////////////////////////////
// CSearchEdit
CSearchEdit::CSearchEdit()
: m_pDropDownWnd(NULL)
, m_pFunGetData(NULL)
, m_eWndAlign(align_left)
, m_pDropDownSize(NULL)
, m_pDropDownFont(NULL)
, m_pDimTextFont(NULL)
, m_bWidenWithScrollBar(FALSE)
, m_nDelay(150)
, m_bAsynSearch(FALSE)
, m_nUpdateFlags(0)
, m_hThread(NULL)
, m_pPictureCtrl(NULL)
, m_nVisableFlags(0)
{
   m_pVecData = new vector<RESULTINFO>;
   m_pVecCache = new vector<RESULTINFO>;
}


CSearchEdit::~CSearchEdit()
{
   delete m_pVecCache;
   m_pVecCache = NULL;
   delete m_pVecData;
   m_pVecData = NULL;
   if (m_pDropDownSize != NULL)
   {
      delete m_pDropDownSize;
      m_pDropDownSize = NULL;
   }
}

void CSearchEdit::OnDestroy() 
{
   if (m_hThread != NULL) MsgWaitForMultipleObjects(1,&m_hThread,TRUE,700,QS_ALLINPUT);
   if (m_hThread != NULL)  ::TerminateThread(m_hThread,-1);
   

   RemoveMouseHookUser(this);
   if ((g_arMouseHookUsers.GetSize() == 0) && (g_hMouseHook != NULL))
   {
      UnhookWindowsHookEx(g_hMouseHook);
   }
   
   if (m_pDropDownWnd != NULL)
   {
      if (::IsWindow(m_pDropDownWnd->m_hWnd)) m_pDropDownWnd->DestroyWindow();
      delete m_pDropDownWnd;
      m_pDropDownWnd = NULL;
   }
   if (m_pDropDownFont != NULL)
   {
      delete m_pDropDownFont;
      m_pDropDownFont = NULL;
   }
   if (m_pDimTextFont != NULL)
   {
      delete m_pDimTextFont;
      m_pDimTextFont = NULL;
   }
   
   if (m_pPictureCtrl != NULL)
   {
      ((CPictureEx *)m_pPictureCtrl)->DestroyWindow();
      delete ((CPictureEx *)m_pPictureCtrl);
      m_pPictureCtrl = NULL;
   }
   CEdit::OnDestroy();
}


void CSearchEdit::PreSubclassWindow()
{
   if (m_pDropDownWnd == NULL)
   {
      m_pDropDownWnd = new CSearchListWnd(this);
      m_pDropDownWnd->CreateEx(0, AfxRegisterWndClass(0), "", WS_POPUP|WS_BORDER,0, 0, 0, 0, m_hWnd, (HMENU)0);
      if (m_pDropDownFont != NULL) m_pDropDownWnd->m_pWndList->SetFont(m_pDropDownFont);
      if (g_hMouseHook == NULL) g_hMouseHook = SetWindowsHookEx(WH_MOUSE,MouseHookProc,NULL,::GetCurrentThreadId());
      AddMouseHookUser(this);
   }
   if (m_pPictureCtrl == NULL)
   {
       m_pPictureCtrl = new CPictureEx;
       CPictureEx *pPicture = (CPictureEx *)m_pPictureCtrl;
       pPicture->Create("", WS_CLIPCHILDREN | WS_CHILD, CRect(0, 0, 0, 0), this);
      //m_pPictureCtrl->LoadPicture(IDR_GIF_LOADING_SMALL,hApexTemplateDLL,"GIF");

      HINSTANCE hInsatance = AfxGetResourceHandle();
      AfxSetResourceHandle(g_hModuleInstance);
      if (pPicture->Load(MAKEINTRESOURCE(IDR_GIF_LOADING_SMALL), _T("GIF")))
          pPicture->Draw();
      AfxSetResourceHandle(hInsatance);
      CDC *pDc = GetDC();
      pPicture->SetBkColor(pDc->GetBkColor());
      ReleaseDC(pDc);
   }

   CEdit::PreSubclassWindow();
}

BEGIN_MESSAGE_MAP(CSearchEdit, CEdit)
	//{{AFX_MSG_MAP(CSearchEdit)
   ON_CONTROL_REFLECT_EX(EN_CHANGE,OnChange)
	ON_WM_DESTROY()
   ON_WM_SIZE()
   ON_WM_MOVE()
	ON_WM_TIMER()
   ON_WM_PAINT()
   ON_WM_SHOWWINDOW()
   ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
   ON_MESSAGE(WM_CC_SEARCHRESULT,OnSearchResult)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchEdit message handlers

BOOL CSearchEdit::PreTranslateMessage(MSG* pMsg) 
{
   BOOL bDeal = FALSE;
   if(pMsg->message==WM_KEYDOWN)
   {  
      CString strText;
      GetWindowText(strText);
      if (!strText.IsEmpty())
      {
         if (!IsDropDown())
         {
            if (pMsg->wParam==VK_DOWN) 
            {
               bDeal = TRUE;
               int nCount = (m_pDropDownWnd != NULL) ? m_pDropDownWnd->GetCount() : 0;
               if (nCount > 0) ShowDropDownWnd();
            }
         }
         else
         {
            int nCount = (m_pDropDownWnd != NULL) ? m_pDropDownWnd->GetCount() : 0;
            int nCurSel = (m_pDropDownWnd != NULL) ? m_pDropDownWnd->GetSel() : -1;
            if (pMsg->wParam==VK_ESCAPE)
            {
               ShowDropDownWnd(FALSE);
               bDeal = TRUE;
            }
            else if (pMsg->wParam==VK_DOWN) 
            {
               bDeal = TRUE;
               if (nCurSel < nCount-1) m_pDropDownWnd->SetSel(nCurSel+1);
            }
            else if (pMsg->wParam == VK_UP)
            {
               bDeal = TRUE;
               if (nCurSel > 0) m_pDropDownWnd->SetSel(nCurSel-1);
            }
            else if (pMsg->wParam == VK_END)
            {
               bDeal = TRUE;
               if (nCount > 0) m_pDropDownWnd->SetSel(nCount-1);
            }
            else if (pMsg->wParam == VK_HOME)
            {
               bDeal = TRUE;
               if (nCount > 0) m_pDropDownWnd->SetSel(0);
            }
            else if (pMsg->wParam == VK_PRIOR)
            {
               bDeal = TRUE;
               m_pDropDownWnd->SetSel(m_pDropDownWnd->GetIndexOffsetPage(nCurSel,-1));
            }
            else if (pMsg->wParam == VK_NEXT)
            {
               bDeal = TRUE;
               m_pDropDownWnd->SetSel(m_pDropDownWnd->GetIndexOffsetPage(nCurSel,1));
            }
            else if (pMsg->wParam == VK_RETURN) 
            {
               bDeal = TRUE;
               m_pDropDownWnd->OnClickItem();
            }
         }
      }
   }
   return bDeal ? bDeal : CEdit::PreTranslateMessage(pMsg);
}
LRESULT CSearchEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
   if(message == WM_KILLFOCUS)
   {
      HWND hSetWnd = (HWND)wParam;
      if (IsDropDown() && (hSetWnd != m_pDropDownWnd->m_hWnd) && !::IsChild(m_pDropDownWnd->m_hWnd,hSetWnd))
      {
         ShowDropDownWnd(FALSE);
      }
   }
   return CEdit::WindowProc(message, wParam, lParam);
}


void CSearchEdit::ShowDropDownWnd(BOOL bShow /*= TRUE*/)
{
   if (bShow)
   {
      if (!IsDropDown(TRUE))
      {
         UpdateDropDownPos();
         UpdateDropDownSize();
         //::AnimateWindow(m_pDropDownWnd->GetSafeHwnd(), 100, AW_VER_POSITIVE|AW_SLIDE);
         CRect rect;
         HRGN hRgn = CreateRectRgn(0, 0, 0, 0);
         m_pDropDownWnd->GetWindowRect(&rect);
         memset(&m_sAMInfo,0,sizeof(m_sAMInfo));
         m_sAMInfo.nType = 1;
         m_pDropDownWnd->GetWindowRect(&rect);
         m_sAMInfo.EndRect.right = rect.Width();
         m_sAMInfo.EndRect.bottom = rect.Height();
         m_sAMInfo.BeginRect.right = rect.Width();
         if (IsDropDown(FALSE))  //下拉未完成
         {
            m_pDropDownWnd->GetWindowRgn(hRgn);
            CRgn *pRgn = CRgn::FromHandle(hRgn);
            pRgn->GetRgnBox(&rect);
            m_sAMInfo.BeginRect = rect;
         }
         m_sAMInfo.CacheRect = m_sAMInfo.BeginRect;
         SetTimer(TIMER_DROPDOWN,9,NULL);
      }
      else
      {
         m_pDropDownWnd->ShowWindow(SW_SHOWNOACTIVATE);
      }
   }
   else
   {
      /*if (IsDropDown(FALSE))
      {
         CRect rect;
         HRGN hRgn = CreateRectRgn(0, 0, 0, 0);
         memset(&m_sAMInfo,0,sizeof(m_sAMInfo));
         m_sAMInfo.nType = 2;
         m_pDropDownWnd->GetWindowRgn(hRgn);
         CRgn *pRgn = CRgn::FromHandle(hRgn);
         pRgn->GetRgnBox(&rect);
         m_sAMInfo.BeginRect = rect;
         m_sAMInfo.CacheRect = m_sAMInfo.BeginRect;
         m_pDropDownWnd->GetWindowRect(&rect);
         m_sAMInfo.EndRect.right = rect.Width();
         SetTimer(TIMER_DROPDOWN,9,NULL);
      }
      else
      {
         m_pDropDownWnd->ShowWindow(SW_HIDE);
      }*/
      m_pDropDownWnd->ShowWindow(SW_HIDE);
   }
}


void CSearchEdit::UpdateDropDownPos()
{
   if (m_pDropDownWnd != NULL)
   {
      CRect rcItem,rect;
      GetWindowRect(&rect);
      m_pDropDownWnd->GetWindowRect(&rcItem);
      if (m_eWndAlign == align_left) OffsetRect(&rcItem,rect.left-rcItem.left,rect.bottom-1-rcItem.top);
      else OffsetRect(&rcItem,rect.right-rcItem.right,rect.bottom-1-rcItem.top);
      m_pDropDownWnd->MoveWindow(rcItem);
   }
}

void CSearchEdit::UpdateDropDownSize()
{
   CRect rect,rectItem;
   if (m_pDropDownWnd != NULL)
   {
      m_pDropDownWnd->GetWindowRect(rectItem);
      SIZE size;
      if (m_pDropDownSize != NULL)
      {
         size = *m_pDropDownSize;
      }
      else
      {
         GetWindowRect(&rect);
         size.cx = rect.Width();
         size.cy = DEFUAL_DROPDOWN_HEIGHT;
      }
      
      if (m_bWidenWithScrollBar)
      {  
         if (size.cy != rectItem.Height())
         {
            //高度改变,提前调整一次,为正确判断是否有scroll bar
            m_pDropDownWnd->m_pWndList->GetWindowRect(&rect);
            rect.bottom = rect.top +size.cy;
            rect.right = rect.left +size.cx;
            m_pDropDownWnd->m_pWndList->MoveWindow(rect);
         }
         m_pDropDownWnd->m_pWndList->GetWindowRect(&rect);
         int nWidth = rect.Width();
         m_pDropDownWnd->m_pWndList->GetClientRect(&rect);
         if (nWidth > rect.Width()) size.cx += GetSystemMetrics(SM_CXVSCROLL);
      }
      m_pDropDownWnd->MoveWindow(rectItem.left,rectItem.top,size.cx,size.cy);
   }
}

BOOL CSearchEdit::IsDropDown(BOOL bComplete)
{
   BOOL bIsDropDown = ((m_pDropDownWnd != NULL) && ::IsWindow(m_pDropDownWnd->m_hWnd) && m_pDropDownWnd->IsWindowVisible());
   if (bComplete)
   {
      CRect rect,VisRt;
      HRGN hRgn = CreateRectRgn(0, 0, 0, 0);
      m_pDropDownWnd->GetWindowRect(&rect);
      m_pDropDownWnd->GetWindowRgn(hRgn);
      CRgn *pRgn = CRgn::FromHandle(hRgn);
      pRgn->GetRgnBox(&VisRt);
      if ((VisRt.Width() != rect.Width()) || (VisRt.Height() != rect.Height())) bIsDropDown = FALSE;
   }
   return bIsDropDown;
}

BOOL CSearchEdit::OnChange() 
{
   if (IsWindowVisible() && IsWindowEnabled() && GetModify())
   {  
      SetTimer(TIMER_DELAY,m_nDelay,NULL);
   }
   return FALSE;
}
void CSearchEdit::OnTimer(UINT nIDEvent) 
{
   if (nIDEvent == TIMER_DELAY)
   {
      KillTimer(TIMER_DELAY);	
      if (m_bAsynSearch)
      {
         m_nUpdateFlags |= UPDATE_FLAG_NEWREQUEST;
         m_nVisableFlags |= VISABLE_FLAG_LOADING;
         UpdateMargins();
         if (m_hThread == NULL) m_hThread = AfxBeginThread(UpdateThread,(LPVOID)this)->m_hThread;
      }
      else
      {
         int nCount = CallBackDataFunc();
         PostMessage(WM_CC_SEARCHRESULT,(WPARAM)nCount,0);
      }
   }
   else if (nIDEvent == TIMER_DROPDOWN)
   { 
      int nEndFlag = 0,nBeginFlag = 0;
      int nOffsetHeight = (m_sAMInfo.EndRect.Height() - m_sAMInfo.BeginRect.Height())/7;
      if (m_sAMInfo.CacheRect == m_sAMInfo.BeginRect) nBeginFlag = 1;
      m_sAMInfo.CacheRect.bottom += nOffsetHeight;
      if (((m_sAMInfo.nType == 1) && (m_sAMInfo.CacheRect.Height() >= m_sAMInfo.EndRect.Height()))
         || ((m_sAMInfo.nType == 2) && (m_sAMInfo.CacheRect.Height() <= m_sAMInfo.EndRect.Height()))
         || (nOffsetHeight == 0))
      {
         m_sAMInfo.CacheRect = m_sAMInfo.EndRect;
         KillTimer(TIMER_DROPDOWN);
         nEndFlag = 1;
      }
      
      HRGN hRgn = CreateRectRgn(0,0,m_sAMInfo.CacheRect.Width(),m_sAMInfo.CacheRect.Height());
      m_pDropDownWnd->SetWindowRgn(hRgn,TRUE);
      if (nEndFlag && (2 == m_sAMInfo.nType)) 
      {
         m_pDropDownWnd->ShowWindow(SW_HIDE);
      }
      if (nBeginFlag && (1 == m_sAMInfo.nType))
      {
         m_pDropDownWnd->ShowWindow(SW_SHOWNOACTIVATE);
      }
   }  
   CEdit::OnTimer(nIDEvent);
}
LRESULT CSearchEdit::OnSearchResult(WPARAM wParam,LPARAM lParam)
{
   int nItemCount = max(0,(int)wParam);
   ExchangeCacheData();
   m_pDropDownWnd->ResetData(nItemCount);
   m_pDropDownWnd->SetSel(0,FALSE);

   BOOL bShow = (nItemCount > 0);
   ShowDropDownWnd(bShow);
   
   m_nVisableFlags &= ~VISABLE_FLAG_LOADING;
   UpdateMargins();
   return 1;
}


int CSearchEdit::CallBackDataFunc()
{
   int nItemCount = 0;
   if(m_pFunGetData != NULL)
   {
      CString strText;
      GetWindowText(strText);
      m_pVecCache->clear();
      m_pFunGetData(strText,m_lGetDataFuncParam,m_pVecCache);
      nItemCount = m_pVecCache->size();
   }
   return nItemCount;
}

void CSearchEdit::AsynSearch(BOOL bAsyn)
{
   m_bAsynSearch = bAsyn;
}


LRESULT CALLBACK CSearchEdit::MouseHookProc(int code, WPARAM wParam, LPARAM lParam)
{
   MOUSEHOOKSTRUCT *mhh = (MOUSEHOOKSTRUCT *)lParam;
   if (code >= 0)
   {
      if ((wParam == WM_NCLBUTTONDOWN) || (wParam == WM_LBUTTONDOWN) || (wParam == WM_NCMBUTTONDOWN) || (wParam == WM_MBUTTONDOWN) || (wParam == WM_NCRBUTTONDOWN) || (wParam == WM_RBUTTONDOWN))
      {
         for (int nIndex= 0; nIndex < g_arMouseHookUsers.GetSize(); nIndex++)
         {
            CSearchEdit *pSearcheEdit = (CSearchEdit *)g_arMouseHookUsers.GetAt(nIndex);
            if (pSearcheEdit->IsDropDown())
            {
               int nFlag = 1;
               if (pSearcheEdit->m_hWnd == mhh->hwnd) nFlag = 0;
               if (pSearcheEdit->m_pDropDownWnd->m_hWnd == mhh->hwnd) nFlag = 0;
               if (::IsChild(pSearcheEdit->m_pDropDownWnd->m_hWnd,mhh->hwnd)) nFlag = 0;
               if (nFlag == 1) pSearcheEdit->ShowDropDownWnd(FALSE);
            }
         }
      }
      else if (wParam == WM_MOUSEMOVE)
      {
         for (int nIndex= 0; nIndex < g_arMouseHookUsers.GetSize(); nIndex++)
         {
            CSearchEdit *pSearcheEdit = (CSearchEdit *)g_arMouseHookUsers.GetAt(nIndex);
            if ((pSearcheEdit->m_pDropDownWnd->m_pWndList->m_hWnd == mhh->hwnd) && pSearcheEdit->IsDropDown())
            {
               POINT pt = mhh->pt;
               pSearcheEdit->m_pDropDownWnd->m_pWndList->ScreenToClient(&pt);
               int nItem = pSearcheEdit->m_pDropDownWnd->m_pWndList->HitTest(pt);
               pSearcheEdit->m_pDropDownWnd->SetSel(nItem);
               break;
            }
         }
      }
   }
   return CallNextHookEx(g_hMouseHook,code,wParam,lParam);
}

void CSearchEdit::SetDropDownStyle(ALIMENT eAlign /*= align_left*/,PSIZE pSize /*= NULL*/,CFont *pFont /*= NULL*/,BOOL bWidenWithScrollBar /*= FALSE*/)
{
   if ((m_pDropDownSize == NULL) && (pSize != NULL))
   {
      m_pDropDownSize = new SIZE;
      *m_pDropDownSize = *pSize;
   }
   m_eWndAlign = eAlign;
   if (pFont != NULL) 
   {
      if (m_pDropDownFont != NULL)
      {
         delete m_pDropDownFont;
         m_pDropDownFont = NULL;
      }
      LOGFONT LogFont;
      pFont->GetLogFont(&LogFont);
      m_pDropDownFont = new CFont;
      m_pDropDownFont->CreateFontIndirect(&LogFont);
   }
   if ((m_pDropDownWnd != NULL) && (m_pDropDownFont != NULL)) m_pDropDownWnd->m_pWndList->SetFont(m_pDropDownFont);

   m_bWidenWithScrollBar = bWidenWithScrollBar;
}

void CSearchEdit::OnSize(UINT nType, int cx, int cy)
{
   CEdit::OnSize(nType,cx,cy);
   UpdateMargins();
   if ((m_pDropDownWnd != NULL) && (m_pDropDownSize == NULL))
   {
      UpdateDropDownSize();
   }
}

void CSearchEdit::OnMove(int x, int y)
{
    if (m_pDropDownWnd != NULL) UpdateDropDownPos();
}

void CSearchEdit::OnEndSelDropDownItem(RESULTINFO info)
{
 
}

void CSearchEdit::SetSearchDelay(UINT millisecondsDelay)
{
   m_nDelay = millisecondsDelay;
}
void CSearchEdit::SetDimText(CString strText)
{
   m_strDimText = strText;
}

void CSearchEdit::SetDimTextFont(LOGFONT *pLogFont)
{
   if (pLogFont != NULL) 
   {
      if (m_pDimTextFont != NULL) m_pDimTextFont->DeleteObject();
      else m_pDimTextFont = new CFont;
      m_pDimTextFont->CreateFontIndirect(pLogFont);
   }
}

void CSearchEdit::OnPaint()
{
   CEdit::OnPaint();
   CRect rect;
   if ((m_strDimText.GetLength() > 0) && (GetWindowTextLength() == 0) && (GetFocus() != this))
   {
      DWORD dwFormat,dwStyle;
      CDC * pDC = GetDC();
      if (pDC != NULL)
      {
         dwStyle = GetStyle();
         dwFormat = DT_SINGLELINE|DT_VCENTER;
         if ((dwStyle&ES_RIGHT) != 0) dwFormat |= DT_RIGHT;
         GetRect(rect);
         HGDIOBJ hOldFont = pDC->SelectObject((m_pDimTextFont != NULL) ? m_pDimTextFont->GetSafeHandle() : GetStockObject(DEFAULT_GUI_FONT));
         int nPreBkMode = pDC->SetBkMode(TRANSPARENT);
         COLORREF clrPreTextColor = pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
         pDC->DrawText(m_strDimText,rect,dwFormat);
         pDC->SetBkMode(nPreBkMode);
         pDC->SetTextColor(clrPreTextColor);
         pDC->SelectObject(hOldFont);
         ReleaseDC(pDC);
      }
   }
}

void CSearchEdit::OnShowWindow(BOOL bShow, UINT nStatus)
{
   CEdit::OnShowWindow(bShow,nStatus);
}

UINT CSearchEdit::UpdateThread(LPVOID pParam)
{
   CSearchEdit *pEdit = (CSearchEdit *)pParam;
   pEdit->m_nUpdateFlags &= ~UPDATE_FLAG_NEWREQUEST;
   int nCount = pEdit->CallBackDataFunc();
   //检查是否有追加请求
   if ((pEdit->m_nUpdateFlags&UPDATE_FLAG_NEWREQUEST) != 0)
   {
      UpdateThread(pParam);
   }
   else
   {
      ::PostMessage(pEdit->GetSafeHwnd(),WM_CC_SEARCHRESULT,nCount,0);
      pEdit->m_hThread = NULL; 
   }
   return 1;
}

void CSearchEdit::SetSearchFunc(pFunGetDataFunctor pFun,LPARAM lParam)
{
   m_pFunGetData = pFun;
   m_lGetDataFuncParam = lParam;
}

void CSearchEdit::ExchangeCacheData()
{
   vector<RESULTINFO> *pTemp;
   pTemp = m_pVecData;
   m_pVecData = m_pVecCache;
   m_pVecCache = pTemp;
}

void CSearchEdit::UpdateMargins()
{
   CRect rect,rcItem;
   DWORD dwMargins;
   int nLeftMargins;
   UINT nRighrMargins = 0;
   UpdateRightCtrl(nRighrMargins);
   
   GetWindowRect(rect);
   ScreenToClient(rect);
   rcItem = rect;
   rcItem.right -= nRighrMargins;

   CPictureEx *pPicture = (CPictureEx *)m_pPictureCtrl;
   if ((pPicture != NULL) && ((m_nVisableFlags&VISABLE_FLAG_LOADING) != 0))
   {
      rcItem.right -= 3;
      rcItem.top += 3;
      rcItem.bottom -= 3;
      rcItem.left = rcItem.right - rcItem.Height();
      pPicture->MoveWindow(rcItem);
      nRighrMargins += rcItem.Width();
   }
   if(pPicture != NULL) pPicture->ShowWindow(((m_nVisableFlags&VISABLE_FLAG_LOADING) != 0) ? SW_SHOW : SW_HIDE);
   
   
   dwMargins = GetMargins();
   nLeftMargins = LOWORD(dwMargins);
   SetMargins(nLeftMargins,(nRighrMargins == 0) ? nLeftMargins : nRighrMargins+nLeftMargins);
   ModifyStyle(WS_CLIPCHILDREN,(nRighrMargins == 0) ? 0 : WS_CLIPCHILDREN,0);
}


void CSearchEdit::UpdateRightCtrl(UINT &nRightMargin)
{

}

BOOL CSearchEdit::OnEraseBkgnd(CDC* pDC)
{
   if ((GetStyle()&WS_CLIPCHILDREN) != 0)
   {
      HWND hwndChild=::GetWindow(m_hWnd,GW_CHILD); //列出所有控件   
      while(hwndChild)   
      {  
         CWnd::FromHandle(hwndChild)->Invalidate(FALSE);
         hwndChild=::GetWindow(hwndChild, GW_HWNDNEXT);   
      }
   }
   return CEdit::OnEraseBkgnd(pDC);
}

//////////////////////////////////////////////////////////////////////////










/////////////////////////////////////////////////////////////////////////////
// CSearchListWnd

#define UpdateColumnWidthTimer  1  //更新列宽
CSearchListWnd::CSearchListWnd(CSearchEdit *pWndEdit)
:m_pWndEdit(pWndEdit)
,m_pWndList(NULL)
{
   
}

CSearchListWnd::~CSearchListWnd()
{
}

void CSearchListWnd::OnDestroy() 
{
   if (m_pWndList != NULL)
   {
      m_pWndList->DestroyWindow();
      delete m_pWndList;
      m_pWndList = NULL;
   }
   
   CWnd::OnDestroy();
}

BEGIN_MESSAGE_MAP(CSearchListWnd, CWnd)
	//{{AFX_MSG_MAP(CSearchListWnd)
   ON_NOTIFY(NM_CLICK, IDW_LISTVIEW, OnClickListView)
   ON_NOTIFY(NM_RCLICK, IDW_LISTVIEW, OnRClickListView)
   ON_NOTIFY(LVN_GETDISPINFO, IDW_LISTVIEW, OnGetdispinfoListView)
	ON_WM_CREATE()
   ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSearchListWnd message handlers

LRESULT CSearchListWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
  LRESULT lRelust;
  if (message == WM_MOUSEACTIVATE) lRelust =  MA_NOACTIVATE;
  else lRelust = CWnd::WindowProc(message, wParam, lParam);
  return lRelust;
}



int CSearchListWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
   CRect rect;
   GetClientRect(rect);
   rect.left-=LISTCTRL_EXTRA_WIDTH;
   m_pWndList = new CListCtrl;
   m_pWndList->Create(LVS_REPORT|LVS_OWNERDATA|LVS_NOCOLUMNHEADER|LVS_SINGLESEL|LVS_SHOWSELALWAYS,rect,this,IDW_LISTVIEW);
   m_pWndList->SetExtendedStyle(LVS_EX_FULLROWSELECT);
   m_pWndList->ModifyStyle(WS_HSCROLL | WS_VSCROLL,0,0); 
   m_pWndList->InsertColumn(0,"",LVCFMT_LEFT,rect.Width());
   m_pWndList->ShowWindow(SW_SHOW);
	return 0;
}

void CSearchListWnd::SetSel(int nIndex,BOOL bEnsureVisible /*= TRUE*/)
{
   if (m_pWndList != NULL)
   {
      int nCount = m_pWndList->GetItemCount();
      if ((nIndex >= 0) && (nIndex < nCount))
      {
         m_pWndList->SetItemState(nIndex,LVIS_SELECTED,LVIS_SELECTED);
         m_pWndList->SetSelectionMark(nIndex);
         if (bEnsureVisible) m_pWndList->EnsureVisible(nIndex,FALSE);
      }
   }
}

int CSearchListWnd::GetSel()
{
   int nSel = -1;
   if (m_pWndList != NULL) nSel = m_pWndList->GetSelectionMark();
   return nSel;
}

int CSearchListWnd::GetIndexOffsetPage(int nIndex,int nOffsetPage)
{
   int nIndexOffset;
   int nCount = m_pWndList->GetItemCount();
   if (nCount > 0)
   {
      nIndexOffset = (nIndex > 0) ? nIndex : 0;
      nIndexOffset += nOffsetPage*m_pWndList->GetCountPerPage();
      if (nIndexOffset < 0) nIndexOffset = 0;
      else if (nIndexOffset > nCount-1) nIndexOffset = nCount -1;
   }
   return nIndexOffset;
}


void CSearchListWnd::ResetData(int nCount)
{
   int nLastCount = m_pWndList->GetItemCount();
   m_pWndList->SetItemCountEx(nCount,0/*LVSICF_NOINVALIDATEALL*/);
   if (nLastCount != nCount) UpdateColumnWidth();
}

int CSearchListWnd::GetCount()
{  
   return m_pWndList->GetItemCount();
}

void CSearchListWnd::OnClickListView(NMHDR* pNMHDR, LRESULT* pResult)
{
   NMITEMACTIVATE *pNMITEM = (NMITEMACTIVATE *)pNMHDR;
   if ((pNMITEM->iItem >= 0) && (pNMITEM->iSubItem >= 0))
   {
      OnClickItem(pNMITEM->iItem);
   }
}
void CSearchListWnd::OnRClickListView(NMHDR* pNMHDR, LRESULT* pResult)
{
   NMITEMACTIVATE *pNMITEM = (NMITEMACTIVATE *)pNMHDR;
   if ((pNMITEM->iItem >= 0) && (pNMITEM->iSubItem >= 0))
   {
      OnClickItem(pNMITEM->iItem);
   }
}


void CSearchListWnd::OnClickItem(int nItem)
{
   if (nItem = -1) nItem = GetSel();
   if ((m_pWndEdit != NULL) && ((UINT)nItem < m_pWndEdit->m_pVecData->size()))
   {
      ShowWindow(SW_HIDE);
      m_pWndEdit->SetFocus();
      CSearchEdit::RESULTINFO info = m_pWndEdit->m_pVecData->at(nItem);
      m_pWndEdit->SetWindowText(info.strEditText);
      int nlength=m_pWndEdit->GetWindowTextLength();
      m_pWndEdit->SetSel(nlength,nlength,FALSE);
      m_pWndEdit->OnEndSelDropDownItem(info);
   }
}

void CSearchListWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
   CRect rect,rectItem;
   GetClientRect(&rect);
   if (m_pWndList != NULL)
   {
      rect.left-=LISTCTRL_EXTRA_WIDTH;
      m_pWndList->GetWindowRect(rectItem);
      m_pWndList->MoveWindow(rect);
      if (rectItem.Width() != rect.Width())  UpdateColumnWidth();
   }
}

BOOL CSearchListWnd::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
   LPNMHDR pNmhdr = (LPNMHDR)lParam;
   if ((pNmhdr->code == NM_CUSTOMDRAW) && (pNmhdr->hwndFrom == m_pWndList->m_hWnd))
   {
      NMLVCUSTOMDRAW * pCustomDraw = (NMLVCUSTOMDRAW *)lParam;
      int nItem = pCustomDraw->nmcd.dwItemSpec;
      int nState = m_pWndList->GetItemState(nItem,LVIS_SELECTED);
      switch (pCustomDraw->nmcd.dwDrawStage)
      {
      case CDDS_PREPAINT:
         *pResult = CDRF_NOTIFYITEMDRAW;
         return TRUE;
         break;
      case CDDS_ITEMPREPAINT:
         pCustomDraw->nmcd.uItemState &= ~CDIS_SELECTED;
         pCustomDraw->clrTextBk = ((nState&LVIS_SELECTED) != 0) ? GetSysColor(COLOR_HIGHLIGHT) : m_pWndList->GetBkColor();
         pCustomDraw->clrText = ((nState&LVIS_SELECTED) != 0) ? GetSysColor(COLOR_HIGHLIGHTTEXT) : m_pWndList->GetTextColor();
         *pResult = CDRF_SKIPDEFAULT;
         return FALSE;
      }
   }
	return CWnd::OnNotify(wParam, lParam, pResult);
}

void CSearchListWnd::OnGetdispinfoListView(NMHDR* pNMHDR, LRESULT* pResult)
{
   LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   if ((pDispInfo->item.mask&LVIF_TEXT) == LVIF_TEXT)
   {
      UINT nItem = pDispInfo->item.iItem;
      if ((m_pWndEdit->m_pVecData != NULL) && (nItem < m_pWndEdit->m_pVecData->size()))
      {
         m_strDispalyBuffer = m_pWndEdit->m_pVecData->at(nItem).strDispaly;
         pDispInfo->item.pszText = m_strDispalyBuffer.GetBuffer(m_strDispalyBuffer.GetLength());
      }
   }
   *pResult = 0;
}

void CSearchListWnd::UpdateColumnWidth()
{
   //m_pWndList->SetColumnWidth(0,LVSCW_AUTOSIZE);
   CRect rect;
   m_pWndList->GetClientRect(rect);
   if (m_pWndList->GetColumnWidth(0) != rect.Width()) m_pWndList->SetColumnWidth(0,rect.Width());
   //特殊处理: 部分情况GetClientRect结果未及时更新,width小于实际值,定时再检查一次，原因不详
   SetTimer(UpdateColumnWidthTimer,30,NULL);
}

void CSearchListWnd::OnTimer(UINT nIDEvent) 
{
  if (nIDEvent == UpdateColumnWidthTimer)
   {
      KillTimer(UpdateColumnWidthTimer);
      CRect rect;
      m_pWndList->GetClientRect(rect);
      if (m_pWndList->GetColumnWidth(0) < rect.Width())
      {
         m_pWndList->SetColumnWidth(0,rect.Width());
      }
   }
   CWnd::OnTimer(nIDEvent);
}