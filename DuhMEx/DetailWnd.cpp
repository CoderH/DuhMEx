// DetailWnd.cpp: implementation of the CDetailWnd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DetailWnd.h"
using namespace Gdiplus;
using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define CS_CLOSEBTN                    0x00000001    //关闭按钮
#define CS_POSITIIONTAG                0x00000002    //位置标记
#define CS_ANTIA_ALIAS_BORDER          0x00000004    //边框抗锯齿
#define CS_AUTO_CLEAN_RESOURCE         0x00000008    //隐藏时清理资源

#define HEIGHT_TAG              10
#define WIDTH_TAG               22
#define WIDTH_CLOSEBTN          22
#define HEIGHT_CLOSEBTN         15
#define WIDTH_BORDER            1

#define MARGIN_CLOSEBTN_TOP     2 
#define MARGIN_CLOSEBTN_RIGHT   2 
#define MARGIN_TEXT             6

#define CIDC_BUTTON_CLOSE       1

static HHOOK      m_hKeyboardHook = NULL;
static HHOOK      m_hMouseHook = NULL;
static HHOOK      m_hCallWndRetHook = NULL;

CPtrArray m_arKeyboardHookUsers;
CPtrArray m_arMouseHookUsers;
CPtrArray m_arCallWndRetHookUsers;

IMPLEMENT_DYNAMIC(CDetailWnd, CWnd)
static CPtrArray * GetHookUsers(HHOOK hHook)
{
   if (hHook == m_hKeyboardHook) return &m_arKeyboardHookUsers;
   else if (hHook == m_hMouseHook) return &m_arMouseHookUsers;
   else if (hHook == m_hCallWndRetHook) return &m_arCallWndRetHookUsers;
   else return NULL;
}
static int FindHookUser(void *pUser,HHOOK hHook)
{
   int nIndex = -1;
   CPtrArray *pArUsers = GetHookUsers(hHook);
   if (pArUsers != NULL)
   {
      int nCount = pArUsers->GetSize();
      for (int i = 0; i < nCount; i++)
      {
         if (pArUsers->GetAt(i) == pUser)
         {
            nIndex = i;
            break;
         }
      }
   }
   return nIndex;
}

static void RemoveHookUser(void *pUser,HHOOK hHook)
{
   CPtrArray *pArUsers = GetHookUsers(hHook);
   if (pArUsers != NULL)
   {
      int nIndex = FindHookUser(pUser,hHook);
      if (nIndex >= 0) pArUsers->RemoveAt(nIndex);
   }
}
static void AddHookUser(void *pUser,HHOOK hHook)
{
   CPtrArray *pArUsers = GetHookUsers(hHook);
   if (pArUsers != NULL)
   {
      int nIndex = FindHookUser(pUser,hHook);
      if (nIndex < 0) pArUsers->Add(pUser);
   }
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Duh::CDetailWnd::CDetailWnd(BOOL bAutoCleanResource /*= FALSE*/)
{
   m_pUserPtr = NULL;
   //m_pBtnClose = NULL;
   m_nMaxWidth = 1000;
   SetBkColor(RGB(255, 255, 255));
   SetTitleColor(RGB(0,0,0));
   SetContentColor(RGB(20, 20, 20));
   SetBorderColor(RGB(183,183,183));
   m_dwStyle = (CS_CLOSEBTN|CS_POSITIIONTAG|CS_ANTIA_ALIAS_BORDER);
   if (bAutoCleanResource) m_dwStyle|= CS_AUTO_CLEAN_RESOURCE;
   LoadDefaultFont();
}

Duh::CDetailWnd::~CDetailWnd()
{
   
}

BEGIN_MESSAGE_MAP(CDetailWnd,CWnd)
   ON_WM_ERASEBKGND()
   ON_WM_PAINT()
   ON_BN_CLICKED(CIDC_BUTTON_CLOSE,OnClickCloseBtn)
END_MESSAGE_MAP()

BOOL Duh::CDetailWnd::Create(CWnd *pParent)
{
    pParent = GetSafeOwner(pParent);
   BOOL bRes = CreateEx(0,AfxRegisterWndClass(0),"",WS_POPUP,CRect(0,0,0,0),pParent,0,NULL);
   /*if (bRes)
   {
      m_pBtnClose = new CMultiTypeOwnerDrawBtn;
      m_pBtnClose->Create("",0,CRect(0,0,0,0),this,CIDC_BUTTON_CLOSE);
      m_pBtnClose->SetTemplate(CMultiTypeOwnerDrawBtn::Template_Close);
      m_pBtnClose->SetNormalClr(RGB(0,0,0),m_clrBk);
      m_pBtnClose->SetHighlightClr(RGB(255,255,255),RGB(232, 17, 35));
      m_pBtnClose->SetNoActive(FALSE);
      m_pBtnClose->ShowWindow(SW_SHOW);
   }*/
   return bRes;
}

LRESULT CALLBACK Duh::CDetailWnd::KeyboardHookProc(int code, WPARAM wParam, LPARAM lParam)
{
   //(lParam & 0x80000000) 按键放开
   if ((code >= 0) && (lParam & 0x80000000))
   {
      int nIndex;
      CPtrArray *arUsers = GetHookUsers(m_hKeyboardHook);
      for (nIndex = 0; (arUsers != NULL && nIndex < arUsers->GetSize()); nIndex++)
      {
         CDetailWnd *pWnd = (CDetailWnd *)arUsers->GetAt(nIndex);
               if ((pWnd != NULL) && ::IsWindowVisible(pWnd->GetSafeHwnd())) pWnd->Hide();
      }
   }
   return ::CallNextHookEx(m_hKeyboardHook,code,wParam,lParam);
}

LRESULT CALLBACK Duh::CDetailWnd::MouseHookProc(int code, WPARAM wParam, LPARAM lParam)
{
   if (code >= 0)
   {
      MOUSEHOOKSTRUCT *mhh = (MOUSEHOOKSTRUCT *)lParam;
      int nIndex;
      CPtrArray *arUsers = GetHookUsers(m_hMouseHook);
      if (arUsers != NULL)
      {
         switch ((UINT)wParam)
         {
         case WM_NCLBUTTONDOWN:
         case WM_LBUTTONDOWN:
         case WM_NCMBUTTONDOWN:
         case WM_MBUTTONDOWN:
         case WM_NCRBUTTONDOWN:
         case WM_RBUTTONDOWN:
         case WM_MOUSEWHEEL:
            for (nIndex = 0; nIndex < arUsers->GetSize(); nIndex++)
            {
               CDetailWnd *pWnd = (CDetailWnd *)arUsers->GetAt(nIndex);
               if ((pWnd != NULL) && pWnd->IsWindowVisible())
               {  
                  if ((pWnd->m_hWnd != mhh->hwnd) && !::IsChild(pWnd->m_hWnd,mhh->hwnd)) 
                  {
                     pWnd->Hide();
                  }
               }
            }
            break;
         default:
            break;
         }
      }
   }
   return ::CallNextHookEx(m_hMouseHook,code,wParam,lParam);
}

LRESULT CALLBACK Duh::CDetailWnd::CallWndRetProc(int code, WPARAM wParam, LPARAM lParam)
{
   return ::CallNextHookEx(m_hCallWndRetHook,code,wParam,lParam);
}

void Duh::CDetailWnd::PostNcDestroy()
{
   CWnd::PostNcDestroy();
  /* if (m_pBtnClose != NULL)
   {
      m_pBtnClose->DestroyWindow();
      delete m_pBtnClose;
      m_pBtnClose = NULL;
   }*/
   RemoveHookUser(this,m_hMouseHook);
   RemoveHookUser(this,m_hKeyboardHook);
   RemoveHookUser(this,m_hCallWndRetHook);
   if ((GetHookUsers(m_hMouseHook)->GetSize() <= 0) && (m_hMouseHook != NULL))
   {
      ::UnhookWindowsHookEx(m_hMouseHook);
      m_hMouseHook = NULL;
   }
   if ((GetHookUsers(m_hKeyboardHook)->GetSize() <= 0) && (m_hKeyboardHook != NULL))
   {
      ::UnhookWindowsHookEx(m_hKeyboardHook);
      m_hKeyboardHook = NULL;
   }
   if ((GetHookUsers(m_hCallWndRetHook)->GetSize() <= 0) && (m_hCallWndRetHook != NULL)) 
   {
      ::UnhookWindowsHookEx(m_hCallWndRetHook);
      m_hCallWndRetHook = NULL;
   }
   if ((m_dwStyle&CS_AUTO_CLEAN_RESOURCE) != 0)
   {
      if (m_pUserPtr != NULL) *m_pUserPtr = NULL;
      delete this;
   }
}


void Duh::CDetailWnd::SetMaxWidth(int nMaxWidth)
{
   m_nMaxWidth = nMaxWidth;
}


void Duh::CDetailWnd::Popup(CString strTitle, CString strContent, CRect rcPosition)
{
   m_strTitle = strTitle;
   m_strContent = strContent;
   PositionWindow(rcPosition);
   ShowWindow(SW_SHOWNOACTIVATE);

   if (m_hMouseHook == NULL) m_hMouseHook = ::SetWindowsHookEx(WH_MOUSE,(HOOKPROC)MouseHookProc,NULL,::GetCurrentThreadId());
   AddHookUser(this,m_hMouseHook);
   if (m_hKeyboardHook == NULL) m_hKeyboardHook = ::SetWindowsHookEx(WH_KEYBOARD,(HOOKPROC)KeyboardHookProc,NULL,::GetCurrentThreadId());
   AddHookUser(this,m_hKeyboardHook);
}

void Duh::CDetailWnd::Popup(CString strTitle, CString strContent, CPoint ptPosition)
{
   CRect rect;
   rect.left = ptPosition.x;
   rect.right = ptPosition.x;
   rect.top = ptPosition.y;
   rect.bottom = ptPosition.y;
   Popup(strTitle,strContent,rect);
}

void Duh::CDetailWnd::Popup(CWnd *pParent, CString strTitle, CString strContent, CRect position, ExAttribute *pExAttribute /*= NULL*/)
{
   CDetailWnd *pWnd = new CDetailWnd(TRUE);
   pWnd->Create(pParent);
   if (pExAttribute != NULL)
   {
      if (pExAttribute->nMaxWidth > 0) pWnd->SetMaxWidth(pExAttribute->nMaxWidth);
      if (pExAttribute->clrBk != CLR_NONE) pWnd->SetBkColor(pExAttribute->clrBk);
      if (pExAttribute->clrBorder != CLR_NONE) pWnd->SetBorderColor(pExAttribute->clrBorder);
      if (pExAttribute->clrTitle != CLR_NONE) pWnd->SetTitleColor(pExAttribute->clrTitle);
      if (pExAttribute->clrContent != CLR_NONE) pWnd->SetContentColor(pExAttribute->clrContent);
      if (pExAttribute->pLfTitle != NULL) pWnd->SetTitleFont(pExAttribute->pLfTitle);
      if (pExAttribute->pLfContent != NULL) pWnd->setContentFont(pExAttribute->pLfContent);
      pWnd->HideCloseBtn(pExAttribute->bHideCloseBtn);
      pWnd->HidePositionTag(pExAttribute->bHidePositionTag);
   }
   pWnd->Popup(strTitle,strContent,position);
}

void Duh::CDetailWnd::Popup(CWnd *pParent, CString strTitle, CString strContent, CPoint position, ExAttribute *pExAttribute /*= NULL*/)
{
   CRect rect;
   rect.left = position.x;
   rect.right = position.x;
   rect.top = position.y;
   rect.bottom = position.y;
   Popup(pParent,strTitle,strContent,rect,pExAttribute);
}

BOOL Duh::CDetailWnd::OnEraseBkgnd(CDC* pDC)
{
   if ((m_dwStyle&CS_ANTIA_ALIAS_BORDER) == 0)
   {
      CRect rect;
      GetClientRect(&rect);
      CBrush brush;
      brush.CreateSolidBrush(m_clrBk);
      pDC->FillRect(rect,&brush);
   }
   return TRUE;
}

void Duh::CDetailWnd::OnPaint()
{
   CPaintDC dc(this);
   dc.SetBkMode(TRANSPARENT);
   if ((m_dwStyle&CS_ANTIA_ALIAS_BORDER) != 0)
   {
      Graphics graphics(dc.m_hDC);
      graphics.SetSmoothingMode(SmoothingModeAntiAlias);
      Color penColor;
      penColor.SetFromCOLORREF(m_clrBorder);
      Pen pen(penColor,WIDTH_BORDER);

      int nCount = m_rectInfo.vecRgnPoint.size();
      Point *point= new Point[nCount];
      int nIndex = 0;
      std::vector<Gdiplus::Point>::iterator iter = m_rectInfo.vecRgnPoint.begin();
      for(iter=m_rectInfo.vecRgnPoint.begin();(iter != (m_rectInfo.vecRgnPoint.end()) && (nIndex < nCount));iter++)
      {
         point[nIndex] = *iter;
         nIndex ++;
      }
      
      Color BrushColor;
      BrushColor.SetFromCOLORREF(m_clrBk);
      SolidBrush brush(BrushColor);
      graphics.FillPolygon(&brush,point,nIndex);
      graphics.DrawPolygon(&pen,point,nIndex);
      delete point;
      point = NULL;
   }
   else
   {
      CBrush br;
      br.CreateSolidBrush(m_clrBorder);
      dc.FrameRgn(&m_rectInfo.rgnWindow,&br,WIDTH_BORDER,WIDTH_BORDER);
   }

   if (!m_strTitle.IsEmpty())
   {
      CFont *pOldFont = dc.SelectObject(&m_ftTitle);
      dc.SetTextColor(m_clrTitle);
      dc.DrawText(m_strTitle,&m_rectInfo.drawTitle,DT_LEFT|DT_NOPREFIX|DT_EXPANDTABS|DT_SINGLELINE);
      dc.SelectObject(pOldFont);
   }
   if (!m_strContent.IsEmpty())
   {
      CFont *pOldFont = dc.SelectObject(&m_ftContent);
      dc.SetTextColor(m_clrContent);
      dc.DrawText(m_strContent,&m_rectInfo.drawContent,DT_LEFT|DT_NOPREFIX|DT_EXPANDTABS|DT_WORDBREAK|DT_EDITCONTROL);
      dc.SelectObject(pOldFont);
   }
}

void Duh::CDetailWnd::HideCloseBtn(BOOL bHide)
{
   if (bHide) m_dwStyle &= ~CS_CLOSEBTN;
   else m_dwStyle |= CS_CLOSEBTN;
}

void Duh::CDetailWnd::HidePositionTag(BOOL bHide)
{
   if (bHide) m_dwStyle &= ~CS_POSITIIONTAG;
   else m_dwStyle |= CS_POSITIIONTAG;
}


void Duh::CDetailWnd::InitAllRect(RECTINFO &rectInfo)
{
   int nMaxWidth = m_nMaxWidth;
   rectInfo.drawContent.SetRectEmpty();
   rectInfo.drawTitle.SetRectEmpty();
   rectInfo.closeBtn.SetRectEmpty();
   rectInfo.client.SetRectEmpty();
   rectInfo.window.SetRectEmpty();
   if (rectInfo.rgnWindow.GetSafeHandle() != NULL) rectInfo.rgnWindow.DeleteObject();
   rectInfo.vecRgnPoint.clear();

   CClientDC dc(this);

   if (!m_strTitle.IsEmpty())
   {
      rectInfo.drawTitle.left = MARGIN_TEXT;
      CFont *pOldFont = dc.SelectObject(&m_ftTitle);
      rectInfo.drawTitle.right = rectInfo.drawTitle.left + dc.GetTextExtent(m_strTitle).cx;
      rectInfo.drawTitle.top = MARGIN_TEXT;
      rectInfo.drawTitle.bottom = rectInfo.drawTitle.top + dc.GetTextExtent(m_strTitle).cy;
      dc.SelectObject(&pOldFont);
   }
   if ((m_dwStyle&CS_CLOSEBTN) != 0)
   {
      rectInfo.closeBtn.left = rectInfo.drawTitle.right + MARGIN_TEXT;
      rectInfo.closeBtn.right = rectInfo.closeBtn.left + WIDTH_CLOSEBTN;
      rectInfo.closeBtn.top = MARGIN_CLOSEBTN_TOP;
      rectInfo.closeBtn.bottom = rectInfo.closeBtn.top + HEIGHT_CLOSEBTN;
   }
   
   rectInfo.client.bottom = max(rectInfo.drawTitle.bottom,rectInfo.closeBtn.bottom);
   rectInfo.client.right = max(rectInfo.drawTitle.right,rectInfo.closeBtn.right+MARGIN_CLOSEBTN_RIGHT);
   if (rectInfo.client.right > m_nMaxWidth)
   {
      int nOffsetX = rectInfo.client.right - m_nMaxWidth;
      rectInfo.drawTitle.right -= nOffsetX;
      rectInfo.closeBtn.OffsetRect(-nOffsetX,0);
      rectInfo.client.right = max(rectInfo.drawTitle.right,rectInfo.closeBtn.right+MARGIN_CLOSEBTN_RIGHT);
   }
   
   if (!m_strContent.IsEmpty())
   {
      int nTextWidth = 0;
      int nContentMaxWidth = nMaxWidth - MARGIN_TEXT*2;
      CString strContent = m_strContent;
      while (!strContent.IsEmpty())
      {
         CString strText = strContent.SpanExcluding("\n");
         strText = strText.SpanExcluding("\r");
         CFont *pOldFont = dc.SelectObject(&m_ftContent);
         nTextWidth = max(dc.GetTextExtent(strText).cx,nTextWidth);
         dc.SelectObject(&pOldFont);
         if (strContent.GetLength() > strText.GetLength()+1) strContent = strContent.Mid(strText.GetLength()+1);
         else strContent.Empty();
      }
      nTextWidth = min(nTextWidth,nContentMaxWidth);
      if (nTextWidth < 10) nTextWidth = 10;
      CRect rect;
      rect.SetRectEmpty();
      rect.right = nTextWidth;
      dc.DrawText(m_strContent,&rect,DT_CALCRECT|DT_LEFT|DT_NOPREFIX|DT_EXPANDTABS|DT_WORDBREAK|DT_EDITCONTROL);

      rectInfo.drawContent.top = rectInfo.client.bottom + MARGIN_TEXT;
      rectInfo.drawContent.left = MARGIN_TEXT;
      rectInfo.drawContent.right =  rectInfo.drawContent.left + rect.Width();
      rectInfo.drawContent.bottom = rectInfo.drawContent.top + rect.Height();

      rectInfo.client.bottom += MARGIN_TEXT + rectInfo.drawContent.Height() + MARGIN_TEXT;
      rectInfo.client.right = max(rectInfo.client.right,rectInfo.drawContent.right+MARGIN_TEXT);
   }
  
   //windows宽度调整,重新右对齐closBtn
   if ((m_dwStyle&CS_CLOSEBTN) != 0)
   {
      int nOffsetX = rectInfo.client.right -MARGIN_CLOSEBTN_RIGHT - rectInfo.closeBtn.right;
      if (nOffsetX > 0) OffsetRect(rectInfo.closeBtn,nOffsetX,0);
   }
   rectInfo.window = rectInfo.client;

   if ((m_dwStyle&CS_POSITIIONTAG) != 0)
   {  
      //默认显示目标上方
       rectInfo.window.bottom += HEIGHT_TAG;
       CRgn rgnTag,rgnClient;
       CPoint arPoint[3];
       
       arPoint[0].x = rectInfo.window.Width()/2 - WIDTH_TAG/2;
       arPoint[0].y = rectInfo.client.Height();
       arPoint[1].x = rectInfo.window.Width()/2;
       arPoint[1].y = rectInfo.window.Height();
       arPoint[2].x = rectInfo.window.Width()/2 + WIDTH_TAG/2;
       arPoint[2].y = rectInfo.client.Height();
       rgnTag.CreatePolygonRgn(&arPoint[0], 3, ALTERNATE);
       rgnClient.CreateRectRgnIndirect(&rectInfo.client);
       rectInfo.rgnWindow.CreateRectRgn(0,0,1,1);
       rectInfo.rgnWindow.CombineRgn(&rgnTag, &rgnClient, RGN_OR);
   }
   else
   {
       rectInfo.window.bottom += HEIGHT_TAG;
       rectInfo.rgnWindow.CreateRectRgnIndirect(rectInfo.window);
   }

}

void Duh::CDetailWnd::OffSetAllRect(RECTINFO &rectInfo, CPoint wndPosition, CPoint TagPosition)
{
   rectInfo.window.OffsetRect(wndPosition);
   
   rectInfo.vecRgnPoint.clear();
   if ((m_dwStyle&CS_POSITIIONTAG) != 0) 
   {
      CRgn rgnTag,rgnClient;
      CPoint arPoint[3];
      TagPosition.x = max(TagPosition.x,rectInfo.window.left+WIDTH_TAG/2);
      TagPosition.x = min(TagPosition.x,rectInfo.window.right-WIDTH_TAG/2);
      TagPosition.x -= rectInfo.window.left;
      //显示在目标底部
      if ((wndPosition.y >= TagPosition.y))
      {
         rectInfo.client.OffsetRect(0,HEIGHT_TAG);
         rectInfo.drawTitle.OffsetRect(0,HEIGHT_TAG);
         rectInfo.closeBtn.OffsetRect(0,HEIGHT_TAG);
         rectInfo.drawContent.OffsetRect(0,HEIGHT_TAG);
         
         arPoint[0].x = TagPosition.x - WIDTH_TAG/2;
         arPoint[0].y = rectInfo.client.top;
         arPoint[1].x = TagPosition.x;
         arPoint[1].y = 0;
         arPoint[2].x = TagPosition.x + WIDTH_TAG/2;
         arPoint[2].y = rectInfo.client.top;
         
         if ((m_dwStyle&CS_ANTIA_ALIAS_BORDER) != 0)
         {
            int nRBMargin = max(WIDTH_BORDER/2,1);
            int nLTMargin = WIDTH_BORDER/2;
            rectInfo.vecRgnPoint.push_back(Point(m_rectInfo.client.right-nRBMargin,m_rectInfo.client.top+nLTMargin));
            rectInfo.vecRgnPoint.push_back(Point(m_rectInfo.client.right-nRBMargin,m_rectInfo.client.bottom-nRBMargin));
            rectInfo.vecRgnPoint.push_back(Point(m_rectInfo.client.left+nLTMargin,m_rectInfo.client.bottom-nRBMargin));
            rectInfo.vecRgnPoint.push_back(Point(m_rectInfo.client.left+nLTMargin,m_rectInfo.client.top+nLTMargin));
            rectInfo.vecRgnPoint.push_back(Point(arPoint[0].x+1+nRBMargin,arPoint[0].y+nLTMargin));
            rectInfo.vecRgnPoint.push_back(Point(arPoint[1].x,arPoint[1].y+1));
            rectInfo.vecRgnPoint.push_back(Point(arPoint[2].x-1-nRBMargin,arPoint[2].y+nLTMargin));
         } 
      }
      else
      {
         arPoint[0].x = TagPosition.x - WIDTH_TAG/2;
         arPoint[0].y = rectInfo.client.Height();
         arPoint[1].x = TagPosition.x;
         arPoint[1].y = rectInfo.window.Height();
         arPoint[2].x = TagPosition.x + WIDTH_TAG/2;
         arPoint[2].y = rectInfo.client.Height();
         
         
         if ((m_dwStyle&CS_ANTIA_ALIAS_BORDER) != 0)
         {
            int nRBMargin = max(WIDTH_BORDER/2,1);
            int nLTMargin = WIDTH_BORDER/2;
            rectInfo.vecRgnPoint.push_back(Point(m_rectInfo.client.left+nLTMargin,m_rectInfo.client.bottom-nRBMargin));
            rectInfo.vecRgnPoint.push_back(Point(m_rectInfo.client.left+nLTMargin,m_rectInfo.client.top+nLTMargin));
            rectInfo.vecRgnPoint.push_back(Point(m_rectInfo.client.right-nRBMargin,m_rectInfo.client.top+nLTMargin));
            rectInfo.vecRgnPoint.push_back(Point(m_rectInfo.client.right-nRBMargin,m_rectInfo.client.bottom-nRBMargin));
            rectInfo.vecRgnPoint.push_back(Point(arPoint[2].x-1-nRBMargin,arPoint[2].y-nRBMargin));
            rectInfo.vecRgnPoint.push_back(Point(arPoint[1].x,arPoint[1].y-1));
            rectInfo.vecRgnPoint.push_back(Point(arPoint[0].x+1+nRBMargin,arPoint[0].y-nRBMargin));
         } 
      }
      rgnTag.CreatePolygonRgn(&arPoint[0], 3, ALTERNATE);
      rgnClient.CreateRectRgnIndirect(&rectInfo.client);
      CRgn rgnWindow;
      rgnWindow.CreateRectRgn(0,0,1,1);
      rgnWindow.CombineRgn(&rgnTag, &rgnClient, RGN_OR);
      rectInfo.rgnWindow.CopyRgn(&rgnWindow); 
   }
   else
   {
      if ((m_dwStyle&CS_ANTIA_ALIAS_BORDER) != 0)
      {
         int nRBMargin = max(WIDTH_BORDER/2,1);
         int nLTMargin = WIDTH_BORDER/2;
         rectInfo.vecRgnPoint.push_back(Point(m_rectInfo.client.left+nLTMargin,m_rectInfo.client.bottom-nRBMargin));
         rectInfo.vecRgnPoint.push_back(Point(m_rectInfo.client.left+nLTMargin,m_rectInfo.client.top+nLTMargin));
         rectInfo.vecRgnPoint.push_back(Point(m_rectInfo.client.right-nRBMargin,m_rectInfo.client.top+nLTMargin));
         rectInfo.vecRgnPoint.push_back(Point(m_rectInfo.client.right-nRBMargin,m_rectInfo.client.bottom-nRBMargin));
      }
   }
}

void Duh::CDetailWnd::GetWindowOverflow(CRect windowRect, OVERFLOWLENGTH &overflowLength)
{
   memset(&overflowLength,0,sizeof(overflowLength));
   CRect rect;
	::SystemParametersInfo(SPI_GETWORKAREA,0,&rect,0);
   overflowLength.top = max(rect.top - windowRect.top,0);
   overflowLength.bottom = max(windowRect.top+windowRect.Height() - rect.bottom,0);
   overflowLength.left = max(rect.left - windowRect.left,0);
   overflowLength.right = max(windowRect.left + windowRect.Width() - rect.right,0);
}

void Duh::CDetailWnd::PositionWindow(CRect rcPosition)
{
   //初始化所有rect信息
   InitAllRect(m_rectInfo);  

   CRect rcWin;
   CPoint tagPosition;
   rcWin = m_rectInfo.window;

   //默认对齐方式: 显示在底部, 水平居中
   tagPosition.y = rcPosition.bottom;
   tagPosition.x = rcPosition.left+rcPosition.Width()/2;
   rcWin.OffsetRect(tagPosition.x - rcWin.Width()/2,tagPosition.y);

   OVERFLOWLENGTH overflowLength;
   GetWindowOverflow(rcWin,overflowLength);
   //底部溢出屏幕，更改为显示在顶部
   if (overflowLength.bottom > 0) rcWin.OffsetRect(0,rcPosition.top-rcWin.Height() - rcWin.top);
   if (overflowLength.right > 0)
   {
      //右侧溢出屏幕，左移
      int OffsetX = min(rcWin.right - rcPosition.left,overflowLength.right);
      rcWin.OffsetRect(-OffsetX,0);
      if (rcWin.right < tagPosition.x) tagPosition.x = rcWin.right;
   }
   else if (overflowLength.left > 0)
   {
      //左侧溢出屏幕，右移
      int nOffsetX = min(rcPosition.right - rcWin.left,overflowLength.left);
      rcWin.OffsetRect(nOffsetX,0);
      if (rcWin.left > tagPosition.x) tagPosition.x = rcWin.left;
   }

   //调整所有rect信息
   OffSetAllRect(m_rectInfo,rcWin.TopLeft(),tagPosition);
   
  /* if (((m_dwStyle&CS_CLOSEBTN)!= 0) && (m_pBtnClose != NULL))
   {
      m_pBtnClose->MoveWindow(m_rectInfo.closeBtn);
   }*/

   MoveWindow(m_rectInfo.window);

   CRgn rgnWindow;
   rgnWindow.CreateRectRgn(0,0,1,1);
   rgnWindow.CopyRgn(&m_rectInfo.rgnWindow);
   SetWindowRgn(rgnWindow,FALSE);
}

void Duh::CDetailWnd::SetBkColor(COLORREF clrBk)
{
   m_clrBk = clrBk;
   /*if ((m_dwStyle&CS_CLOSEBTN) && (m_pBtnClose != NULL))
   {
      m_pBtnClose->SetNormalClr(RGB(0,0,0),m_clrBk);
   }*/
}

void Duh::CDetailWnd::SetTitleColor(COLORREF clrTitle)
{
   m_clrTitle = clrTitle;
}

void Duh::CDetailWnd::SetContentColor(COLORREF clrContent)
{
   m_clrContent = clrContent;
}

void Duh::CDetailWnd::SetBorderColor(COLORREF clrBorder)
{
   m_clrBorder = clrBorder;
}

void Duh::CDetailWnd::LoadDefaultFont()
{
   LOGFONT lf;
   memset(&lf,0,sizeof(lf));
   lf.lfCharSet = GB2312_CHARSET;
   lf.lfHeight = -13;
   strcpy(lf.lfFaceName,"微软雅黑");
   lf.lfWeight = FW_BOLD;
   SetTitleFont(&lf);

   memset(&lf,0,sizeof(lf));
   lf.lfCharSet = GB2312_CHARSET;
   lf.lfHeight = -12;
   strcpy(lf.lfFaceName, "微软雅黑");
   setContentFont(&lf);
}

void Duh::CDetailWnd::SetTitleFont(PLOGFONT pLogFont)
{
   if (pLogFont != NULL)
   {
      if (m_ftTitle.GetSafeHandle() != NULL) m_ftTitle.DeleteObject();
      m_ftTitle.CreateFontIndirect(pLogFont);
   }
}

void Duh::CDetailWnd::setContentFont(PLOGFONT pLogFont)
{ 
   if (pLogFont != NULL)
   {
      if (m_ftContent.GetSafeHandle() != NULL) m_ftContent.DeleteObject();
      m_ftContent.CreateFontIndirect(pLogFont);
   }
}

void Duh::CDetailWnd::Hide()
{
   if ((m_dwStyle&CS_AUTO_CLEAN_RESOURCE) != 0) PostMessage(WM_CLOSE);
   else ShowWindow(SW_HIDE);
}

void Duh::CDetailWnd::SetUserPointer(CDetailWnd **pUserPtr)
{
   m_pUserPtr = pUserPtr;
}

void Duh::CDetailWnd::OnClickCloseBtn()
{
   Hide();
}

LRESULT Duh::CDetailWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
   LRESULT lRelust;
   if (message == WM_MOUSEACTIVATE) lRelust =  MA_NOACTIVATE;
   else lRelust = CWnd::WindowProc(message, wParam, lParam);
  return lRelust;
}
