#ifndef WIN_IMPL_BASE_HPP
#define WIN_IMPL_BASE_HPP

#include "stdafx.h"
#include <ShellAPI.h>

namespace DuiLib
{

//////////////////////////////////////////////////////////////////////////

LPBYTE WindowImplBase::m_lpResourceZIPBuffer=NULL;

DUI_BEGIN_MESSAGE_MAP(WindowImplBase,CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
DUI_END_MESSAGE_MAP()

WindowImplBase::WindowImplBase():
m_bInit(false),
m_bKeepToSide(false),
m_bHided(false),
m_iDockState(DOCK_IDLE),
m_bOnToolBar(true),
m_pDropTarget(NULL),
m_bDrop(false)
{
}

void WindowImplBase::OnFinalMessage( HWND hWnd )
{
	if(m_pDropTarget)
		delete m_pDropTarget;
	m_PaintManager.RemovePreMessageFilter(this);
	m_PaintManager.RemoveNotifier(this);
	m_PaintManager.ReapObjects(m_PaintManager.GetRoot());

	delete this;
}

CMenuWnd* WindowImplBase::CreateMenuWnd(CDuiString sXML,CPoint pt, map<CDuiString,bool>* pMenuCheckInfo, bool bShowOnTop)
{
	CMenuWnd* pMenu = new CMenuWnd(m_hWnd);
	pMenu->Init(NULL,sXML.GetData(), pt, &m_PaintManager,pMenuCheckInfo, bShowOnTop);
	return pMenu;
}

void WindowImplBase::SetFileDrop(bool bDrop)
{
	m_bDrop = bDrop;
	if(m_bDrop)
	{
		m_pDropTarget = new CDropTargetEx(this);
		m_pDropTarget->DragDropRegister(m_hWnd);  
		m_pDropTarget->SetHDropCallBack(OnDropFiles);
	}
}

LRESULT WindowImplBase::ResponseDefaultKeyEvent(WPARAM wParam)
{
	return FALSE;
}

UINT WindowImplBase::GetClassStyle() const
{
	return CS_DBLCLKS;
}

UILIB_RESOURCETYPE WindowImplBase::GetResourceType() const
{
	return UILIB_FILE;
}

CDuiString WindowImplBase::GetZIPFileName() const
{
	return _T("");
}

LPCTSTR WindowImplBase::GetResourceID() const
{
	return _T("");
}

void WindowImplBase::ReloadZipResource(int ID)  
{  
	HRSRC hResource = ::FindResource(m_PaintManager.GetResourceDll(), MAKEINTRESOURCE(ID), _T("ZIPRES"));  
	if( hResource == NULL )  
		return ;  
	DWORD dwSize = 0;  
	HGLOBAL hGlobal = ::LoadResource(m_PaintManager.GetResourceDll(), hResource);  
	if( hGlobal == NULL )   
	{  
#if defined(WIN32) && !defined(UNDER_CE)  
		::FreeResource(hResource);  
#endif  
		return ;  
	}  
	dwSize = ::SizeofResource(m_PaintManager.GetResourceDll(), hResource);  
	if( dwSize == 0 )  
		return ;  
	m_lpResourceZIPBuffer = new BYTE[ dwSize ];  
	if (m_lpResourceZIPBuffer != NULL)  
	{  
		::CopyMemory(m_lpResourceZIPBuffer, (LPBYTE)::LockResource(hGlobal), dwSize);  
	}  
#if defined(WIN32) && !defined(UNDER_CE)  
	::FreeResource(hResource);  
#endif  
	CPaintManagerUI::SetResourceZip(m_lpResourceZIPBuffer, dwSize);  

	CPaintManagerUI::ReloadSkin();  
}  

CControlUI* WindowImplBase::CreateControl(LPCTSTR pstrClass)
{
	return NULL;
}

LRESULT WindowImplBase::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, bool& /*bHandled*/)
{
	if (uMsg == WM_KEYDOWN)
	{
		switch (wParam)
		{
		case VK_RETURN:
		case VK_ESCAPE:
		default:
			return ResponseDefaultKeyEvent(wParam);
			break;
		}
	}
	return FALSE;
}

LRESULT WindowImplBase::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT WindowImplBase::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

#if defined(WIN32) && !defined(UNDER_CE)

LRESULT WindowImplBase::OnNcButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	DWORD dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	if(wParam == HTCAPTION)
		dwStyle &= ~WS_THICKFRAME;
	else
		dwStyle |= WS_THICKFRAME;
	SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
	return 0;
}

LRESULT WindowImplBase::OnNcActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT WindowImplBase::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPRECT pRect=NULL;

	if ( wParam == TRUE)
	{
		LPNCCALCSIZE_PARAMS pParam = (LPNCCALCSIZE_PARAMS)lParam;
		pRect=&pParam->rgrc[0];
	}
	else
	{
		pRect=(LPRECT)lParam;
	}

	if ( ::IsZoomed(m_hWnd))
	{	// 最大化时，计算当前显示器最适合宽高度
		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTONEAREST), &oMonitor);
		CDuiRect rcWork = oMonitor.rcWork;
		CDuiRect rcMonitor = oMonitor.rcMonitor;
		rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

		pRect->right = pRect->left + rcWork.GetWidth();
		pRect->bottom = pRect->top + rcWork.GetHeight();
		return WVR_REDRAW;
	}

	return 0;
}

LRESULT WindowImplBase::OnNcPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

LRESULT WindowImplBase::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);
	
	if( !::IsZoomed(*this) )
	{
		RECT rcSizeBox = m_PaintManager.GetSizeBox();
		if( pt.y < rcClient.top + rcSizeBox.top )
		{
			if( pt.x < rcClient.left + rcSizeBox.left ) return HTTOPLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTTOPRIGHT;
			return HTTOP;
		}
		else if( pt.y > rcClient.bottom - rcSizeBox.bottom )
		{
			if( pt.x < rcClient.left + rcSizeBox.left ) return HTBOTTOMLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTBOTTOMRIGHT;
			return HTBOTTOM;
		}

		if( pt.x < rcClient.left + rcSizeBox.left ) return HTLEFT;
		if( pt.x > rcClient.right - rcSizeBox.right ) return HTRIGHT;
	}

	RECT rcCaption = m_PaintManager.GetCaptionRect();
	if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(pt));
			if( pControl && _tcsicmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
				_tcsicmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
				_tcsicmp(pControl->GetClass(), _T("FadeButtonUI")) != 0/* &&
				_tcsicmp(pControl->GetClass(), _T("TextUI")) != 0*/ )
				return HTCAPTION;
	}

	return HTCLIENT;
}

LRESULT WindowImplBase::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;

	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTONEAREST), &oMonitor);
	CDuiRect rcWork = oMonitor.rcWork;
	CDuiRect rcMonitor = oMonitor.rcMonitor;
	rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

	// 计算最大化时，正确的原点坐标
	lpMMI->ptMaxPosition.x	= rcWork.left;
	lpMMI->ptMaxPosition.y	= rcWork.top;

	lpMMI->ptMaxTrackSize.x =rcWork.GetWidth();
	lpMMI->ptMaxTrackSize.y =rcWork.GetHeight();

	lpMMI->ptMinTrackSize.x =m_PaintManager.GetMinInfo().cx;
	lpMMI->ptMinTrackSize.y =m_PaintManager.GetMinInfo().cy;

	bHandled = FALSE;
	return 0;
}

LRESULT WindowImplBase::OnMouseWheel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT WindowImplBase::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}
#endif

LRESULT WindowImplBase::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(wParam == nTimerClose)
	{
		if(OnAnimateClose())
		{
			KillTimer(m_hWnd,nTimerClose);
			Close(IDCLOSE);
		}
	}
	else
		bHandled = FALSE;
	return 0;
}

LRESULT WindowImplBase::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SIZE szRoundCorner = m_PaintManager.GetRoundCorner();
#if defined(WIN32) && !defined(UNDER_CE)
	if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
		CDuiRect rcWnd;
		::GetWindowRect(*this, &rcWnd);
		rcWnd.Offset(-rcWnd.left, -rcWnd.top);
		rcWnd.right++; rcWnd.bottom++;
		HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
		::SetWindowRgn(*this, hRgn, TRUE);
		::DeleteObject(hRgn);
	}
#endif
	bHandled = FALSE;
	return 0;
}

LRESULT WindowImplBase::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT WindowImplBase::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == SC_CLOSE)
	{
		bHandled = TRUE;
		SendMessage(WM_CLOSE);
		return 0;
	}
#if defined(WIN32) && !defined(UNDER_CE)
	BOOL bZoomed = ::IsZoomed(*this);
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	if( ::IsZoomed(*this) != bZoomed )
	{
	}
#else
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
#endif
	return lRes;
}

LRESULT WindowImplBase::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	if (!m_bOnToolBar)
	{
		styleValue = ::GetWindowLong(*this, GWL_EXSTYLE);
		styleValue |= WS_EX_TOOLWINDOW;//不在任务栏显示
		::SetWindowLong(*this, GWL_EXSTYLE, styleValue);
	}

	RECT rcClient;
	::GetClientRect(*this, &rcClient);
	::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
		rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

	m_PaintManager.Init(m_hWnd);
	m_PaintManager.AddPreMessageFilter(this);

	CDialogBuilder builder;
	CDuiString strResourcePath = m_PaintManager.GetInstancePath();
	strResourcePath += GetSkinFolder().GetData();
	m_PaintManager.SetResourcePath(strResourcePath.GetData());

	switch(GetResourceType())
	{
	case UILIB_ZIP:
		m_PaintManager.SetResourceZip(GetZIPFileName().GetData(), true);
		break;
	case UILIB_ZIPRESOURCE:
		{
			HRSRC hResource = ::FindResource(m_PaintManager.GetResourceDll(), GetResourceID(), _T("ZIPRES"));
			if( hResource == NULL )
				return 0L;
			DWORD dwSize = 0;
			HGLOBAL hGlobal = ::LoadResource(m_PaintManager.GetResourceDll(), hResource);
			if( hGlobal == NULL ) 
			{
#if defined(WIN32) && !defined(UNDER_CE)
				::FreeResource(hResource);
#endif
				return 0L;
			}
			dwSize = ::SizeofResource(m_PaintManager.GetResourceDll(), hResource);
			if( dwSize == 0 )
				return 0L;
			m_lpResourceZIPBuffer = new BYTE[ dwSize ];
			if (m_lpResourceZIPBuffer != NULL)
			{
				::CopyMemory(m_lpResourceZIPBuffer, (LPBYTE)::LockResource(hGlobal), dwSize);
			}
#if defined(WIN32) && !defined(UNDER_CE)
			::FreeResource(hResource);
#endif
			m_PaintManager.SetResourceZip(m_lpResourceZIPBuffer, dwSize);
		}
		break;
	}

	CControlUI* pRoot=NULL;
	if (GetResourceType()==UILIB_RESOURCE)
	{
		STRINGorID xml(_ttoi(GetSkinFile().GetData()));
		pRoot = builder.Create(xml, _T("xml"), this, &m_PaintManager);
	}
	else
		pRoot = builder.Create(GetSkinFile().GetData(), (UINT)0, this, &m_PaintManager);
	ASSERT(pRoot);
	if (pRoot==NULL)
	{
		MessageBox(NULL,_T("加载资源文件失败"),_T("Duilib"),MB_OK|MB_ICONERROR);
		ExitProcess(1);
		return 0;
	}
	m_PaintManager.AttachDialog(pRoot);
	m_PaintManager.AddNotifier(this);

	Init();
	return 0;
}

LRESULT WindowImplBase::OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT WindowImplBase::OnKeyUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT WindowImplBase::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT WindowImplBase::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT WindowImplBase::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT WindowImplBase::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT WindowImplBase::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;

	if(m_iDockState != DOCK_IDLE)
	{
		CPoint pos;
		GetCursorPos(&pos);
		if(PtInRect(&m_rcCurRect,pos))
			KeepTOSide_Show();
		else
			KeepTOSide_Hide();
	}

	return 0;
}

LRESULT WindowImplBase::OnMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;

	if (m_bKeepToSide)
	{
		GetWindowRect(m_hWnd,&m_rcCurRect);
		if (m_rcCurRect.left <= 0)
		{
			if(m_iDockState == DOCK_LEFT)
				return 0;
			m_iDockState = DOCK_LEFT;
			KeepTOSide_Hide();
		}
		else if (m_rcCurRect.top <= 0)
		{
			if(m_iDockState == DOCK_TOP)
				return 0;
			m_iDockState = DOCK_TOP;
			KeepTOSide_Hide();
		}
		else if (m_rcCurRect.right >= ::GetSystemMetrics(SM_CXSCREEN))
		{
			if(m_iDockState == DOCK_RIGHT)
				return 0;
			m_iDockState = DOCK_RIGHT;
			KeepTOSide_Hide();
		}
		else if(m_rcCurRect.left !=0 && m_rcCurRect.top != 0)
			m_iDockState = DOCK_IDLE;
	}
	
	return 0;
}

LRESULT WindowImplBase::OnDropFile(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	if(wParam == 8)
	{
		vector<tstring>* vctString = (vector<tstring>* )lParam;
		if(vctString)
			delete vctString;
		bHandled = TRUE;
	}
	return 0;
}

LRESULT WindowImplBase::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch (uMsg)
	{
	case WM_CREATE:			lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
	case WM_CLOSE:			lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
	case WM_DESTROY:		lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
#if defined(WIN32) && !defined(UNDER_CE)
	case WM_NCACTIVATE:		lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
	case WM_NCCALCSIZE:		lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
	case WM_NCPAINT:		lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
	case WM_NCHITTEST:		lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
	case WM_GETMINMAXINFO:	lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
	case WM_MOUSEWHEEL:		lRes = OnMouseWheel(uMsg, wParam, lParam, bHandled); break;
#endif
	case WM_SIZE:			lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
	case WM_CHAR:		    lRes = OnChar(uMsg, wParam, lParam, bHandled); break;
	case WM_SYSCOMMAND:		lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
	case WM_KEYDOWN:		lRes = OnKeyDown(uMsg, wParam, lParam, bHandled); break;
	case WM_KEYUP:			lRes = OnKeyUp(uMsg, wParam, lParam, bHandled); break;
	case WM_KILLFOCUS:		lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;
	case WM_SETFOCUS:		lRes = OnSetFocus(uMsg, wParam, lParam, bHandled); break;
	case WM_LBUTTONUP:		lRes = OnLButtonUp(uMsg, wParam, lParam, bHandled); break;
	case WM_LBUTTONDOWN:	lRes = OnLButtonDown(uMsg, wParam, lParam, bHandled); break;
	case WM_MOUSEMOVE:		lRes = OnMouseMove(uMsg, wParam, lParam, bHandled); break;
	case WM_MOUSEHOVER:	    lRes = OnMouseHover(uMsg, wParam, lParam, bHandled); break;
	case WM_MENU:			lRes = OnMenu(uMsg,wParam,lParam);
	case WM_TIMER:			lRes = OnTimer(uMsg,wParam,lParam,bHandled);
	case WM_MOVE:			lRes = OnMove(uMsg,wParam,lParam,bHandled);
	case WM_NCLBUTTONDOWN:  lRes = OnNcButtonDown(uMsg,wParam,lParam,bHandled);
	case WM_DROPFILES:	lRes = OnDropFile(uMsg,wParam,lParam,bHandled);
	default:				bHandled = FALSE; break;
	}
	if (bHandled) return lRes;

	lRes = HandleCustomMessage(uMsg, wParam, lParam, bHandled);
	if (bHandled) return lRes;

	if (m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes))
		return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

LRESULT WindowImplBase::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LONG WindowImplBase::GetStyle()
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;

	return styleValue;
}

LRESULT WindowImplBase::OnMenu(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
#if 0
	CControlUI * pControl = (CControlUI *)lParam;
	if (pControl != NULL)
		m_PaintManager.SendNotify(pControl,DUI_MSGTYPE_MENU, wParam, lParam);
#else
	CDuiString *pName = (CDuiString *)wParam;
	if(pName)
	{
		CDuiString sName = *pName;
		bool bCheck = (bool)lParam;
		TNotifyUI msg;
		m_PaintManager.SendNotify(NULL,DUI_MSGTYPE_MENU, (WPARAM)(sName.GetData()), bCheck);
		delete pName;
	}
#endif
	return 0;
}

void WindowImplBase::OnClick(TNotifyUI& msg)
{
	CDuiString sCtrlName = msg.pSender->GetName();
	if( sCtrlName == _T("closebtn") )
	{
		Close();
		return; 
	}
	else if( sCtrlName == _T("minbtn"))
	{ 
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); 
		return; 
	}
	else if( sCtrlName == _T("maxbtn"))
	{ 
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); 
		return; 
	}
	else if( sCtrlName == _T("restorebtn"))
	{ 
		SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); 
		return; 
	}
	return;
}

void WindowImplBase::Notify(TNotifyUI& msg)
{
	return CNotifyPump::NotifyPump(msg);
}

void WindowImplBase::KeepTOSide_Show()
{
	if(m_bHided)
	{
		m_bHided = FALSE;
		int iWidth,iHeight;
		switch(m_iDockState)
		{
		case DOCK_LEFT:
			iWidth = m_rcCurRect.GetWidth();
			m_rcCurRect.Offset(iWidth-2,0);
			break;
		case DOCK_TOP:
			iHeight = m_rcCurRect.GetHeight();
			m_rcCurRect.Offset(0,iHeight-2);
			break;
		case DOCK_RIGHT:
			iWidth = m_rcCurRect.GetWidth();
			m_rcCurRect.Offset(-iWidth+2,0);
			break;
		}
		SetWindowPos(m_hWnd,NULL,m_rcCurRect.left,m_rcCurRect.top,m_rcCurRect.GetWidth(),m_rcCurRect.GetHeight(),SWP_NOZORDER);
	}

}

void WindowImplBase::KeepTOSide_Hide()
{
	if(!IsZoomed(m_hWnd))
	{
		if(!m_bHided)
		{
			m_bHided = TRUE;
			int iWidth,iHeight;
			switch(m_iDockState)
			{
			case DOCK_LEFT:
				iWidth = m_rcCurRect.GetWidth();
				m_rcCurRect.left = 0;
				m_rcCurRect.right = iWidth;
				m_rcCurRect.Offset(-iWidth+2,0);
				break;
			case DOCK_TOP:
				iHeight = m_rcCurRect.GetHeight();
				m_rcCurRect.top = 0;
				m_rcCurRect.bottom = iHeight;
				m_rcCurRect.Offset(0,-iHeight+2);
				break;
			case DOCK_RIGHT:
				iWidth = ::GetSystemMetrics(SM_CXSCREEN)-m_rcCurRect.left;
				m_rcCurRect.Offset(iWidth-2,0);
				break;
			}
			SetWindowPos(m_hWnd,HWND_TOPMOST,m_rcCurRect.left,m_rcCurRect.top,m_rcCurRect.GetWidth(),m_rcCurRect.GetHeight(),SWP_SHOWWINDOW);
		}
	}
}

bool WindowImplBase::OnAnimateClose()
{
	CDuiRect rcClient;
	GetWindowRect(m_hWnd,&rcClient);
	rcClient.Deflate(20,20);
	int nWidth = rcClient.GetWidth();
	int nHeight = rcClient.GetHeight();
	if(nWidth <= 20 || nHeight <= 20 )
		return true;
	MoveWindow(m_hWnd,rcClient.left,rcClient.top,nWidth,nHeight,true);
	return false;
}

void EnumerateFiles(std::vector<tstring> &vctString)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(_T("*.*"), &fd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do 
		{
			// 如果为目录
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (_tcscmp(fd.cFileName, _T(".")) && _tcscmp(fd.cFileName, _T("..")))
				{
					::SetCurrentDirectory(fd.cFileName);    
					EnumerateFiles(vctString);              
					::SetCurrentDirectory(_T(".."));        
				}
			}
			// 如果为文件
			else
			{
				CDuiString strDir;
				TCHAR      lpDir[MAX_PATH];

				::GetCurrentDirectory(MAX_PATH, lpDir);
				strDir = lpDir;
				if ( strDir.Right(1) != _T("\\") )
				{
					strDir += _T("\\");
				}
				strDir += fd.cFileName;

				vctString.push_back(strDir.GetData());
			}
		} while (::FindNextFile(hFind, &fd));

		::FindClose(hFind);
	}
}

void WindowImplBase::OnDropFiles(WindowImplBase* pObj, HDROP hDrop)
{
	TCHAR szFilePathName[_MAX_PATH] = {0};  
	UINT  nNumOfFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0); //得到文件个数  
	vector<tstring>* vctString = new vector<tstring>;

	for (UINT nIndex=0 ; nIndex< nNumOfFiles; ++nIndex)  
	{  
		DragQueryFile(hDrop, nIndex, szFilePathName, _MAX_PATH);  //得到文件名  
		DWORD dwAttribute = ::GetFileAttributes(szFilePathName);
		
		if ( dwAttribute & FILE_ATTRIBUTE_DIRECTORY )//文件夹
		{          
			::SetCurrentDirectory(szFilePathName);       
			EnumerateFiles(*vctString);   
		}
		else
		{
			vctString->push_back(szFilePathName);
		}
	}  
	
	DragFinish(hDrop);
	::PostMessage(pObj->GetHWND(),WM_DROPFILES,8,(LPARAM)vctString);
}

}