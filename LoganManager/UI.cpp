#include "StdAfx.h"
#include "UI.h"
#include "MainWnd.h"
namespace DuiLib
{


	void CtrlWnd::Init(CCamCtrolUI* p)
	{
		{
			power = p;
			if (m_hWnd == NULL)
			{
				RECT rcPos = CalPos();
				UINT uStyle = WS_CHILDWINDOW;
				POINT p;
				::ClientToScreen(power->GetManager()->GetPaintWindow(), &p);
				rcPos.left += p.x;
				rcPos.right += p.y;

				Create(power->m_pWindow->GetHWND(), NULL, uStyle, 0, rcPos);
				SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(2, 2));
			}
			::ShowWindow(m_hWnd, SW_SHOW);
			::SetFocus(m_hWnd);
		}
	}
	RECT CtrlWnd::CalPos()
	{
		RECT rc;
		rc = power->GetPos();
		::InflateRect(&rc, -1, -1);
		return rc;
	}
	void CtrlWnd::PainBkColor()
	{
		PAINTSTRUCT    ps;
		HDC dc = ::BeginPaint(m_hWnd, &ps);
		RECT rcClient;
		::GetClientRect(m_hWnd, &rcClient);
		CRenderEngine::DrawColor(dc, rcClient, 0x1c1c1c);
		::EndPaint(m_hWnd, &ps);
	}
	LRESULT CtrlWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_ERASEBKGND)
		{
			//PainBkColor();
			return 0;
		}
		if(uMsg==WM_PAINT)
		{
			/*if(!isfirst && !isplayed)
			{
				PAINTSTRUCT p;
				HDC hdc=BeginPaint(*this,&p);
				HPEN	pen=CreatePen(PS_SOLID,4,RGB(	30, 144, 255));
				HPEN old=(HPEN)SelectObject(hdc,pen);
				RECT rc;
				SetBkMode(hdc,TRANSPARENT);
				GetClientRect(*this,&rc);
				MoveToEx(hdc,rc.left+(rc.right-rc.left)/2-30,rc.top+(rc.bottom-rc.top)/2-30,0);
				LineTo(hdc,rc.left+(rc.right-rc.left)/2-30,rc.top+(rc.bottom-rc.top)/2);
				LineTo(hdc,rc.left+(rc.right-rc.left)/2-15,rc.top+(rc.bottom-rc.top)/2-15);
				LineTo(hdc,rc.left+(rc.right-rc.left)/2-30,rc.top+(rc.bottom-rc.top)/2-30);
				SelectObject(hdc,old);
				DeleteObject(pen);
				EndPaint(*this,&p);
			}*/
			
		}
		if (uMsg == WM_LBUTTONDOWN)
		{
//			if(power->player->isPlaying())
//			{
//				//power->player->stop();
//				PainBkColor();
//			}
//			else
//			{
//
//#ifndef NEW_VIDEO_PLAY
//				power->player->play(power->videosrc.URL["play"]);
//#else
//				power->player->Load(power->videosrc.URL["play"]);
//				power->player->Play();
//#endif
//				
//			}
			return 0;
		}
		if (uMsg == WM_RBUTTONDOWN)
		{
			FullScreen();
			return 0;
		}
		if(uMsg==WM_LBUTTONDBLCLK)
		{
			MessageBoxA(NULL, "you clicked me", "test", MB_OK);
		}
		return __super::HandleMessage(uMsg, wParam, lParam);
	}

	void CtrlWnd::FullScreen()
	{
		//// HACK : fix about class
		///***************must media first  after self *********************/
		::SendMessage(*this,WM_PAINT,0,0);

		if (!m_bFullScreenMode)
		{
			//meida full
			::GetWindowPlacement(power->m_pWindow->GetHWND(), &media_oldplace);
			media_parent = ::GetParent(power->m_pWindow->GetHWND());
			::SetParent(power->m_pWindow->GetHWND(), ::GetDesktopWindow());
			::SetWindowPos(power->m_pWindow->GetHWND(), HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0);
			//self
			::GetWindowPlacement(*this, &m_OldWndPlacement);
			hparent = ::GetParent(*this);
			::SetParent(*this, ::GetDesktopWindow());
			::SetWindowPos(*this, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0);

			m_bFullScreenMode = true;
		}
		else
		{
			//meida 
			::SetParent(power->m_pWindow->GetHWND(), media_parent);
			::SetWindowPlacement(power->m_pWindow->GetHWND(), &media_oldplace);
			::SetWindowPos(power->m_pWindow->GetHWND(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
			//self
			::SetParent(*this, hparent);
			::SetWindowPlacement(*this, &m_OldWndPlacement);
			::SetWindowPos(*this, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
			m_bFullScreenMode = false;
		}


	}


	CCamWnd::CCamWnd():m_pOwner(NULL),
		m_iDownCount(0),m_sDevID(_T(""))
	{
	}

	CDuiString CCamWnd::GetClass() const
	{
		return _T("CamWnd");
	}

	void CCamWnd::Init(CCamCtrolUI* pOwner)
	{
		m_pOwner = pOwner;

		if (m_hWnd == NULL)
		{
			RECT rcPos = CalPos();
			UINT uStyle = WS_CHILD;
			Create(m_pOwner->m_pWndParent->GetHWND(), NULL, uStyle, 0, rcPos);
			//::SetWindowPos(m_hWnd,NULL,0,0,200,200,SWP_NOZORDER|SWP_SHOWWINDOW);
			SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(2, 2));
		}
		::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
		::SetFocus(m_hWnd);

		std::string strID = m_pOwner->m_sID.GetData();
	}

	void CCamWnd::SetWndStytle()
	{
		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= ~WS_CAPTION;
		::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	}

	RECT CCamWnd::CalPos()
	{
		RECT rcClient ;
		::GetClientRect(m_pOwner->m_pWndParent->GetHWND(), &rcClient);
		return rcClient;
	}

	void CCamWnd::PainBkImage()
	{
		PAINTSTRUCT    ps;
		HDC dc = ::BeginPaint(m_hWnd, &ps);
		RECT rcClient ;
		::GetClientRect(m_hWnd, &rcClient);
		CRenderEngine::DrawImageString(dc, m_pOwner->m_pManager, rcClient, rcClient, _T("CamWndBg.png"), _T(""));
		::EndPaint(m_hWnd, &ps);
	}

	void CCamWnd::PainBkColor()
	{
		PAINTSTRUCT    ps;
		HDC dc = ::BeginPaint(m_hWnd, &ps);
		RECT rcClient;
		::GetClientRect(m_hWnd, &rcClient);
		CRenderEngine::DrawColor(dc,rcClient,0xff3c3c3c);
		::EndPaint(m_hWnd, &ps);
	}

	LPCTSTR CCamWnd::GetWindowClassName() const
	{
		return _T("CamWnd");
	}

	void CCamWnd::OnFinalMessage(HWND hWnd)
	{
		m_pOwner->m_pWindow = NULL;	
		m_pOwner->m_bConnect = false;
		delete this;
	}

	LRESULT CCamWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		//LRESULT lRes = 0;
	//	BOOL bHandled = TRUE;
		if(uMsg == WM_ERASEBKGND)
		{
			PainBkColor();
			
		}
		//else if (uMsg == WM_SIZE)
		//{
		//}
		//else if(uMsg == WM_KEYDOWN)
		//{
		//	MessageBoxA(NULL,"keydown",0,0);
		//}
		//else if (uMsg == WM_LBUTTONDOWN)
		//{
		//	::SetFocus(m_hWnd);
		//	m_pOwner->m_sLBtnDownClass = GetClass();
		//	/////////////////////
		//	m_iDownCount++;
		//	if(m_iDownCount == 1)
		//		SetTimer(m_hWnd,LBDOWNTIMER,200,NULL);
		//}
		//else if (uMsg == WM_LBUTTONUP)
		//{
		//	if (GetClass() != m_pOwner->m_sLBtnDownClass)
		//	{
		//		m_pOwner->OnChangeWnd();
		//	}
		//	m_pOwner->m_sLBtnDownClass = "";
		//}
		//else if (uMsg == WM_TIMER)
		//{
		//	if (wParam == LBDOWNTIMER)
		//	{
		//		if(m_iDownCount >= 2)
		//		{
		//			CCamLayoutUI* pLayout = (CCamLayoutUI*)m_pOwner->GetParent();
		//			if(pLayout->GetFullFlag() && m_pOwner->GetFullFlag() == false)
		//				m_pOwner->OnMainWndEscFull();
		//			else
		//				m_pOwner->FullScreen();
		//		}
		//		m_iDownCount = 0;
		//		KillTimer(m_hWnd,LBDOWNTIMER);
		//	}
		//}
		//else if (uMsg == WM_MOUSEMOVE)
		//{
		//	if (m_pOwner->GetFullFlag())
		//	{
		//		m_pOwner->OnUpdateToolBar();		
		//	}
		//}
		//else bHandled = FALSE;
		//if( !bHandled ) 
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		//return lRes;
	}



	//////////////////////////////////////////////////////////////////////////
	//
	CVNCWnd::CVNCWnd():m_pOwner(NULL),
		m_iDownCount(0),
		m_bConnect(false),
		m_sDevID(_T(""))
	{
		dScaleX = 1;
		dScaleX = 1;
		m_strIP = _T("");
		m_nPort = 0;
		m_bEnableControl = true;
	}

	CDuiString CVNCWnd::GetClass() const
	{
		return _T("VNCWnd");
	}

	void CVNCWnd::Init(CCamCtrolUI* pOwner)
	{
		m_pOwner = pOwner;

		if (m_hWnd == NULL)
		{
			RECT rcPos = CalPos();
			UINT uStyle = WS_CHILD;
			Create(m_pOwner->m_pWndParent->GetHWND()/*m_pOwner->GetManager()->GetPaintWindow()*/, NULL, uStyle, 0, rcPos);
			SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(2, 2));
		}
		Edit_SetReadOnly(m_hWnd,false);
		::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
		}

	void CVNCWnd::SetWndStytle()
	{
		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= ~WS_CAPTION;
		::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	}

	RECT CVNCWnd::CalPos()
	{
		RECT rcPos ;
		::GetClientRect(m_pOwner->m_pWndParent->GetHWND(), &rcPos);
		rcPos.bottom = rcPos.top + (rcPos.bottom-rcPos.top)/3;
		rcPos.right = rcPos.left + (rcPos.right-rcPos.left)/3;
		return rcPos;
	}

	void CVNCWnd::PainBkImage()
	{
		PAINTSTRUCT    ps;
		HDC dc = ::BeginPaint(m_hWnd, &ps);
		RECT rcClient ;
		::GetClientRect(m_hWnd, &rcClient);
		//HDC hdc = (m_pOwner->GetManager())->GetPaintDC();
		CRenderEngine::DrawImageString(dc, m_pOwner->m_pManager, rcClient, rcClient, _T("CamWndBg.png"), _T(""));
		::EndPaint(m_hWnd, &ps);
	}

	void CVNCWnd::PainBkColor()
	{
		PAINTSTRUCT    ps;
		HDC dc = ::BeginPaint(m_hWnd, &ps);
		RECT rcClient;
		::GetClientRect(m_hWnd, &rcClient);
		CRenderEngine::DrawColor(dc,rcClient,0xff2d2d2d);
		::EndPaint(m_hWnd, &ps);
	}

	LPCTSTR CVNCWnd::GetWindowClassName() const
	{
		return _T("CVNCWnd");
	}

	void  CVNCWnd::OnFinalMessage(HWND hWnd)
	{
		m_pOwner->m_pWndUp = NULL;
		m_pOwner->m_bConnect = false;
		delete this;
	}

	LRESULT CVNCWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		if(uMsg == WM_ERASEBKGND)
		{
			//			if (!m_pClient || !m_pClient->IsConnected())
			PainBkColor();
		}
		else if (uMsg == WM_PAINT)
		{
				PainBkColor();
		}
		else if (uMsg == WM_SIZE)
		{
			
		}
		else if (uMsg == WM_LBUTTONDBLCLK)
		{
			
		}
		else if (uMsg == WM_LBUTTONUP)
		{
			if (GetClass() != m_pOwner->m_sLBtnDownClass)
			{
				m_pOwner->OnChangeWnd();
			}
			m_pOwner->m_sLBtnDownClass = "";
			////////////////////////////////////
			
		}
		else if (uMsg == WM_LBUTTONDOWN)
		{
			if(m_pOwner->GetFullFlag() == false)
				::SetFocus(m_hWnd);
			m_pOwner->m_sLBtnDownClass = GetClass();
			/////////////////////
			if (!m_bConnect)
			{
				m_iDownCount++;
				if(m_iDownCount == 1)
					SetTimer(m_hWnd,LBDOWNTIMER,200,NULL);
			}
			/////////////////////
			
		}
		else if (uMsg == WM_MBUTTONDBLCLK)
		{
			
		}
		else if (uMsg == WM_TIMER)
		{
			if (wParam == LBDOWNTIMER)
			{
				if(m_iDownCount >= 2)
				{
					CCamLayoutUI* pLayout = (CCamLayoutUI*)m_pOwner->GetParent();
					if(pLayout->GetFullFlag() && m_pOwner->GetFullFlag() == false)
						m_pOwner->OnMainWndEscFull();
					else
						m_pOwner->FullScreen();
				}
				m_iDownCount = 0;
				KillTimer(m_hWnd,LBDOWNTIMER);
			}
			bHandled = FALSE;
		}
		else bHandled = FALSE;
		if( !bHandled ) return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		return lRes;
	}



	//////////////////////////////////////////////////////////////////////////
	//
	CCamWndParent::CCamWndParent():m_hParent(NULL),
		m_bFull(false)
	{

	}

	void CCamWndParent::Init(CCamCtrolUI* pOwner)
	{
		m_pOwner = pOwner;

		if (m_hWnd == NULL)
		{
			RECT rcPos = CalPos();
			UINT uStyle = WS_CHILD;
			m_hParent = m_pOwner->GetManager()->GetPaintWindow();
			Create(m_pOwner->GetManager()->GetPaintWindow(), NULL, uStyle, 0, rcPos);
			SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(2, 2));
		}
		::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
		::SetFocus(m_hWnd);
		SetTimer(m_hWnd,0,2*60*1000,NULL);//add wubc 刷新教室窗口标题定时器
	}

	RECT CCamWndParent::CalPos()
	{
		RECT rcPos = m_pOwner->GetPos();
		rcPos.top += 30;
		::InflateRect(&rcPos, -3, -3);
		return rcPos;
	}

	LPCTSTR CCamWndParent::GetWindowClassName() const
	{
		return _T("CamWndParent");
	}

	void    CCamWndParent::OnFinalMessage(HWND hWnd)
	{
		KillTimer(hWnd,0);
		m_pOwner->m_pWndParent = NULL;
		delete this;
	}

	LRESULT CCamWndParent::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		if(uMsg == WM_ERASEBKGND)
		{
			PainBkColor();
		}
		else
			if(uMsg == WM_TIMER)
			{
				m_pOwner->UpdateText();
			}
			else bHandled = FALSE;
			if( !bHandled ) return CCamWnd::HandleMessage(uMsg, wParam, lParam);
			return lRes;
	}

	void  CCamWndParent::FullScreen()
	{

		SetFocus(m_hWnd);
		{
			GetWindowRect(m_hWnd,&m_rcNomal);
			POINT pt0;
			pt0.x = m_rcNomal.left;
			pt0.y = m_rcNomal.top;

			POINT pt1;
			pt1.x = m_rcNomal.right;
			pt1.y = m_rcNomal.bottom;

			ScreenToClient(m_hParent,&pt0);

			m_rcNomal.right += pt0.x - m_rcNomal.left ;
			m_rcNomal.bottom += pt0.y - m_rcNomal.top ;
			m_rcNomal.left  = pt0.x;
			m_rcNomal.top   = pt0.y;

			::SetParent(m_hWnd,::GetDesktopWindow());

			int cx = ::GetSystemMetrics(SM_CXSCREEN); 
			int cy = ::GetSystemMetrics(SM_CYSCREEN);
			m_bFull = TRUE;
			MoveWindow(m_hWnd,0, 0, cx, cy,TRUE);
			::SetWindowPos(m_hWnd,HWND_TOPMOST,0,0,cx,cy,SWP_FRAMECHANGED|SWP_DEFERERASE);
			///
			if (m_pOwner->GetSelect() == NOSET || m_pOwner->GetSelect() == PIP)
			{
				if( m_pOwner->m_bVNCUp)
				{
					MoveWindow(m_pOwner->m_pWndUp->GetHWND(),0, 0, cx/3, cy/3,TRUE);
					::SetWindowPos(m_pOwner->m_pWndUp->GetHWND(),HWND_TOPMOST,0,0,cx/3,cy/3,SWP_FRAMECHANGED|SWP_DEFERERASE);
					MoveWindow(m_pOwner->m_pWindow->GetHWND(),0, 0, cx, cy,TRUE);
					::SetWindowPos(m_pOwner->m_pWindow->GetHWND(),HWND_TOPMOST,0,0,cx,cy,SWP_FRAMECHANGED|SWP_DEFERERASE);
				}
				else
				{
					MoveWindow(m_pOwner->m_pWindow->GetHWND(),0, 0, cx/3, cy/3,TRUE);
					::SetWindowPos(m_pOwner->m_pWindow->GetHWND(),HWND_TOPMOST,0,0,cx/3,cy/3,SWP_FRAMECHANGED|SWP_DEFERERASE);
					MoveWindow(m_pOwner->m_pWndUp->GetHWND(),0, 0, cx, cy,TRUE);
					::SetWindowPos(m_pOwner->m_pWndUp->GetHWND(),HWND_TOPMOST,0,0,cx,cy,SWP_FRAMECHANGED|SWP_DEFERERASE);
				}
			}
			else if (m_pOwner->GetSelect() == TILE)
			{
				MoveWindow(m_pOwner->m_pWndUp->GetHWND(),0, 0, cx/2, cy,TRUE);
				::SetWindowPos(m_pOwner->m_pWndUp->GetHWND(),HWND_TOPMOST,0,0,cx/2,cy,SWP_FRAMECHANGED|SWP_DEFERERASE);
				MoveWindow(m_pOwner->m_pWindow->GetHWND(),cx/2, 0, cx/2, cy,TRUE);
				::SetWindowPos(m_pOwner->m_pWindow->GetHWND(),HWND_TOPMOST,cx/2,0,cx/2+1,cy,SWP_FRAMECHANGED|SWP_DEFERERASE);
			}
			else if (m_pOwner->GetSelect() == SINGLE)
			{
				if(m_pOwner->GetSingleFlag())
				{
					MoveWindow(m_pOwner->m_pWndUp->GetHWND(),0, 0, cx, cy,TRUE);
					::SetWindowPos(m_pOwner->m_pWndUp->GetHWND(),HWND_TOPMOST,0,0,cx,cy,SWP_FRAMECHANGED|SWP_DEFERERASE|SWP_SHOWWINDOW);
					m_pOwner->m_pWindow->ShowWindow(false);
				}
				else
				{
					MoveWindow(m_pOwner->m_pWindow->GetHWND(),0, 0, cx, cy,TRUE);
					::SetWindowPos(m_pOwner->m_pWindow->GetHWND(),HWND_TOPMOST,0,0,cx,cy,SWP_FRAMECHANGED|SWP_DEFERERASE|SWP_SHOWWINDOW);
					m_pOwner->m_pWndUp->ShowWindow(false);
				}

			}
			m_pOwner->OnBtnFull();
		}
		{
			::SetParent(m_hWnd,m_hParent);
			m_bFull = FALSE;
			MoveWindow(m_hWnd,m_rcNomal.left,m_rcNomal.top,m_rcNomal.right-m_rcNomal.left,m_rcNomal.bottom-m_rcNomal.top,TRUE);
			///
			if (m_pOwner->GetSelect() == NOSET || m_pOwner->GetSelect() == PIP)
			{
				if (m_pOwner->m_bVNCUp)
				{
					MoveWindow(m_pOwner->m_pWndUp->GetHWND(),0,0,m_rcNomal.right-m_rcNomal.left/3,m_rcNomal.bottom-m_rcNomal.top/3,TRUE);
					MoveWindow(m_pOwner->m_pWindow->GetHWND(),0,0,m_rcNomal.right-m_rcNomal.left,m_rcNomal.bottom-m_rcNomal.top,TRUE);
				}
				else
				{
					MoveWindow(m_pOwner->m_pWindow->GetHWND(),0,0,m_rcNomal.right-m_rcNomal.left/3,m_rcNomal.bottom-m_rcNomal.top/3,TRUE);
					MoveWindow(m_pOwner->m_pWndUp->GetHWND(),0,0,m_rcNomal.right-m_rcNomal.left,m_rcNomal.bottom-m_rcNomal.top,TRUE);
				}
			}
			else if (m_pOwner->GetSelect() == TILE)
			{
				MoveWindow(m_pOwner->m_pWndUp->GetHWND(),0,0,m_rcNomal.right-m_rcNomal.left/2,m_rcNomal.bottom-m_rcNomal.top,TRUE);
				MoveWindow(m_pOwner->m_pWindow->GetHWND(),m_rcNomal.right-m_rcNomal.left/2,0,m_rcNomal.right-m_rcNomal.left/2+1,m_rcNomal.bottom-m_rcNomal.top,TRUE);
			}
			else if (m_pOwner->GetSelect() == SINGLE)
			{
				if(m_pOwner->GetSingleFlag())
				{
					MoveWindow(m_pOwner->m_pWndUp->GetHWND(),0,0,m_rcNomal.right-m_rcNomal.left,m_rcNomal.bottom-m_rcNomal.top,TRUE);
					m_pOwner->m_pWindow->ShowWindow(false);
				}
				else
				{
					MoveWindow(m_pOwner->m_pWindow->GetHWND(),0,0,m_rcNomal.right-m_rcNomal.left,m_rcNomal.bottom-m_rcNomal.top,TRUE);
					m_pOwner->m_pWndUp->ShowWindow(false);
				}
			}
			m_pOwner->OnBtnEscFull();
		}
	}

	bool  CCamWndParent::GetFullFlag()
	{
		return m_bFull;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	CCamCtrolUI::CCamCtrolUI():m_pWindow(NULL),
		m_pWndUp(NULL),
		m_pWndParent(NULL),
		ctrlwnd(NULL),
		m_iSelect(NOSET),
		m_bSingelUp(false),
		m_uResState(0),
		m_uVoiceState(0),
		m_bResSelect(FALSE),
		m_bVoiceSelect(FALSE),
		m_bHasWnd(FALSE),
		m_sID(""),
		m_sLBtnDownClass(""),
		m_bVNCUp(true),
		m_bConnect(false),
		m_bResIng(false),
		menu(NULL)
	{
		m_bCtrlOpen = false;
		m_bDoorOpen = false;
		m_bProjectorOpen = false;
		m_bSelect = false;
		player=new LoganPlayer();
		router=Router::getRouter();
	}

	CCamCtrolUI::~CCamCtrolUI()
	{
		if(player)
		delete player;
		player=NULL;

		DeleteSbuWnd();
		m_pManager->KillTimer(this);
	}

	LPCTSTR CCamCtrolUI::GetClass() const
	{
		return _T("CamCtrol");
	}

	LPVOID CCamCtrolUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("CamCtrol")) == 0 ) return static_cast<CCamCtrolUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	UINT   CCamCtrolUI::GetControlFlags() const
	{
		return UIFLAG_TABSTOP | (IsEnabled() ? UIFLAG_SETCURSOR : 0);
	}

	void CCamCtrolUI::Init()
	{		
		InitSubWnd();
		//m_pManager->SetTimer(this,123,10*60*1000);

		if(!menu)
		{
			menu=new CComboUI();
			menu->SetBkColor(0xff383838);
			menu->SetSelectedItemTextColor(Color::SkyBlue);
			menu->SetItemTextStyle(DT_VCENTER|DT_CENTER);
			menu->SetItemLineColor(0xff080808);
			menu->SetItemBkColor(0xff3c3c3c);
			menu->SetHotItemTextColor(Color::SkyBlue);
			menu->SetHotItemBkColor(0xff3c3c3c);
			menu->SetSelectedItemBkColor(0xff242424);
			menu->SetItemTextColor(Color::Cyan);
			menu->SetBkImage(_T("file='downsel.png' source='0,0,30,30 ' dest='40,3,60,23'"));
			CListLabelElementUI *lab1=new CListLabelElementUI();
			lab1->SetText(LURL);
			CListLabelElementUI *lab2=new CListLabelElementUI();
			lab2->SetText(SUBURL);
			CListLabelElementUI *lab3=new CListLabelElementUI();
			lab3->SetText(DURL);
			lab3->SetLeftBorderSize(0);
			menu->Add(lab1);
			menu->Add(lab2);
			menu->Add(lab3);
			__super::Add(menu);

			//menu->SelectItem(0);
		}
		player->setHwnd(GetHwnd());
		player->mute();//静音
	}

	void CCamCtrolUI::InitSubWnd()
	{
		

		if(!m_pWindow && !m_pWndUp)
		{
			m_pWndParent = new CCamWndParent();
			ASSERT(m_pWndParent);
			m_pWndParent->Init(this);
			m_pWndParent->SetWndStytle();
			m_pWndParent->ShowWindow();

			m_pWindow = new CCamWnd();
			ASSERT(m_pWindow);
			m_pWindow->Init(this);
			//m_pWindow->SetWndStytle();
			m_pWindow->ShowWindow();

			/*m_pWndUp = new CVNCWnd();
			ASSERT(m_pWndUp);
			m_pWndUp->Init(this);
			m_pWndUp->SetWndStytle();
			m_pWndUp->ShowWindow();

			m_bHasWnd = TRUE;*/
		}
		else
			m_bHasWnd = FALSE;
		if(!ctrlwnd)
		{
			ctrlwnd=new CtrlWnd();
			ctrlwnd->Init(this);
		}
	}

	void CCamCtrolUI::DeleteSbuWnd()
	{
		if( m_pWindow && m_pWndUp) 
		{
			::DestroyWindow(*m_pWindow);
			delete m_pWindow;
			m_pWindow = NULL;

			::DestroyWindow(*m_pWndUp);
			delete m_pWndUp;
			m_pWndUp = NULL;

			::DestroyWindow(*m_pWndParent);
			delete m_pWndParent;
			m_pWndParent = NULL;

			m_bHasWnd = FALSE;
		}
		else
			m_bHasWnd = TRUE;
	}

	void CCamCtrolUI::SetPos(RECT rc)
	{
		CControlUI::SetPos(rc);
		switch(m_iSelect)
		{
		case NOSET:
		case PIP:
			SetPosPIP();
			break;
		case TILE:
			SetPosTile();
			break;
		case  SINGLE:
			SetPosSingle(m_bSingelUp);
			break;
		}	
		RECT rcm=m_rcItem;
		rcm.left=m_rcItem.right-98-20;
		rcm.right=rcm.left+60;
		rcm.top+=5;
		rcm.bottom=rcm.top+20;
		if(menu)
		{
			//menu->SetFloat(true);
			menu->SetPos(rcm);		
		}
	}

	void CCamCtrolUI::SetPosPIP()
	{
		if( m_pWndParent != NULL ) 
		{
			if(m_pWndParent->GetFullFlag() == false)
			{
				RECT rcWnd = m_pWndParent->CalPos();
				::SetWindowPos(*m_pWndParent, HWND_TOP, rcWnd.left, rcWnd.top, rcWnd.right-rcWnd.left, rcWnd.bottom-rcWnd.top, SWP_NOACTIVATE);
				m_pWndParent->ShowWindow();
			}
		}
		if( m_pWindow != NULL  ) 
		{
			RECT rcWnd = m_pWindow->CalPos();
			::SetWindowPos(*m_pWindow, HWND_TOP, rcWnd.left, rcWnd.top,rcWnd.right-rcWnd.left, rcWnd.bottom-rcWnd.top, SWP_NOACTIVATE);
			m_pWindow->ShowWindow();
			
			::SetWindowPos(*ctrlwnd, HWND_TOP, rcWnd.left, rcWnd.top,rcWnd.right-rcWnd.left, rcWnd.bottom-rcWnd.top, SWP_NOACTIVATE);
			ctrlwnd->ShowWindow();

		}
		m_iSelect = PIP;
		m_bVNCUp = true;
	}

	void  CCamCtrolUI::SetPosTile()
	{
		if( m_pWndParent != NULL ) 
		{
			if(m_pWndParent->GetFullFlag() == false)
			{
				RECT rcWnd = m_pWndParent->CalPos();
				::SetWindowPos(*m_pWndParent, HWND_TOP, rcWnd.left, rcWnd.top, rcWnd.right-rcWnd.left, rcWnd.bottom-rcWnd.top, SWP_NOACTIVATE);
				m_pWndParent->ShowWindow();
			}
		}
		RECT rcPos ;
		::GetClientRect(m_pWndParent->GetHWND(), &rcPos);

		RECT rcWnd = rcPos;
		::SetWindowPos(*m_pWindow, HWND_TOP, rcWnd.left, rcWnd.top, rcWnd.right-rcWnd.left, rcWnd.bottom-rcWnd.top, SWP_NOACTIVATE);
		m_pWindow->ShowWindow();
		m_iSelect = TILE;
	}

	void CCamCtrolUI::SetPosSingle(bool bUp)
	{
		if( m_pWndParent != NULL ) 
		{
			if(m_pWndParent->GetFullFlag() == false)
			{
				RECT rcWnd = m_pWndParent->CalPos();
				::SetWindowPos(*m_pWndParent, HWND_TOP, rcWnd.left, rcWnd.top, rcWnd.right-rcWnd.left, rcWnd.bottom-rcWnd.top, SWP_NOACTIVATE);
				m_pWndParent->ShowWindow();
			}
		}
		RECT rcWnd ;
		::GetClientRect(m_pWndParent->GetHWND(), &rcWnd);
		{
			if (m_pWindow == NULL)
				return;
			::SetWindowPos(*m_pWindow, HWND_TOP, rcWnd.left, rcWnd.top, rcWnd.right-rcWnd.left, rcWnd.bottom-rcWnd.top, SWP_NOACTIVATE|SWP_SHOWWINDOW);
			m_pWndUp->ShowWindow(false);
		}

		m_iSelect = SINGLE;
		m_bSingelUp = bUp;
	}

	void CCamCtrolUI::DoEvent(TEventUI& event)
	{
		if (event.Type == UIEVENT_TIMER)
		{
			UpdateText();
			return;
		}
		if( event.Type == UIEVENT_SETCURSOR )
		{
			if(::PtInRect(&m_rcBtnRes,event.ptMouse)|| ::PtInRect(&m_rcBtnVoice,event.ptMouse))
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			else 
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
			return;
		}
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) 
		{
			if( m_pParent != NULL )
				m_pParent->DoEvent(event);
			else 
				__super::DoEvent(event);
			return;
		}
		if (event.Type == UIEVENT_MOUSEMOVE)
		{
			if( IsEnabled() ) 
			{
				if (::PtInRect(&m_rcBtnRes,event.ptMouse))
				{
					m_uResState |= UISTATE_HOT;
					m_uVoiceState &= ~UISTATE_HOT;
				}
				else if (::PtInRect(&m_rcBtnVoice,event.ptMouse))
				{
					m_uVoiceState |= UISTATE_HOT;
					m_uResState &= ~UISTATE_HOT;
				}
				else
				{
					m_uResState &= ~UISTATE_HOT;
					m_uVoiceState &= ~UISTATE_HOT;
				}
				Invalidate();
				return;
			}
		}
		if (event.Type == UIEVENT_BUTTONUP)
		{
			::SetFocus(m_pWindow->GetHWND());
			if (::PtInRect(&m_rcBtnRes,event.ptMouse))
			{
				CCamLayoutUI* pLayout = (CCamLayoutUI*)GetParent();
				if(m_bResSelect==false && pLayout->GetRecordCount()>3)
				{
					m_pManager->SendNotify(this,_T("CamRecord"));
					return;
				}
				m_bResSelect = !m_bResSelect;
			}
			else if (::PtInRect(&m_rcBtnVoice,event.ptMouse))
			{
				m_bVoiceSelect = !m_bVoiceSelect;
				// TODO : when cross the test modify volume 20
				if(m_bVoiceSelect)
				{	
					player->setVolume(20);
				}
					
				//m_pManager->SendNotify(this,_T("CamVoice"));
				
			}
			else
			{
				CCamLayoutUI* pLayout = (CCamLayoutUI*)GetParent();
				if (!pLayout->m_bSelMutil && !pLayout->m_bSelAll)
				{
					pLayout->SelectAll(false);
				}
				m_bSelect = !m_bSelect;
				router->mediaPlayer=player;
				router->camCtrl=this;
				Assign(router->vl,videosrc);
			}
		}
		if (event.Type == UIEVENT_DBLCLICK)
		{
				//FullScreen();
		}
		if(event.Type==UIEVENT_KEYDOWN)
		{
			if(event.wParam==VK_ESCAPE)
				SendMessageA(FindWindow("MainWnd","主窗口"),WM_KEYDOWN,VK_ESCAPE,0);
		}

		if(event.Type==UIEVENT_NOTIFY)
		{
			event;
			event.lParam;
			int a=0;
		}

		__super::DoEvent(event);
	}

	void CCamCtrolUI::PaintBorder(HDC hDC)
	{
		if(m_bSelect)
			CRenderEngine::DrawRect(hDC, m_rcItem,3 , 0xff009ad2);

		RECT rcBorder = m_rcItem;
		rcBorder.bottom = rcBorder.top + 2;
		CRenderEngine::DrawImageString(hDC,m_pManager,rcBorder,rcBorder,_T("cam_line_x.png"));
		rcBorder.bottom = m_rcItem.bottom;
		rcBorder.top = rcBorder.bottom - 2;
		CRenderEngine::DrawImageString(hDC,m_pManager,rcBorder,rcBorder,_T("cam_line_x.png"));
		rcBorder = m_rcItem;
		rcBorder.right = rcBorder.left + 2;
		CRenderEngine::DrawImageString(hDC,m_pManager,rcBorder,rcBorder,_T("cam_line_y.png"));
		rcBorder.right = m_rcItem.right;
		rcBorder.left = rcBorder.right - 2;
		CRenderEngine::DrawImageString(hDC,m_pManager,rcBorder,rcBorder,_T("cam_line_y.png"));
	}

	void CCamCtrolUI::PaintStatusImage(HDC hDC)
	{
		/////////////////////////////////////////左边蓝色
		RECT rcLab = m_rcItem;
		rcLab.left += 5;
		rcLab.right= rcLab.left + 3;
		rcLab.top += 10;
		rcLab.bottom = rcLab.top + 13;
		CRenderEngine::DrawImageString(hDC,m_pManager,rcLab,rcLab,_T("lab_blue.png"));
		/////////////////////////////////////////标题
		RECT rcText = rcLab;
		rcText.left = rcLab.right + 3;
		rcText.right = m_rcItem.right - 100;
		DWORD dwColor = 0xffc8c6c6;
		int iFont = 0;
		UINT dwSystle = DT_LEFT|DT_VCENTER;
		CRenderEngine::DrawText(hDC,m_pManager,rcText,m_sTitle,dwColor,iFont,dwSystle);
		/////////////////////////////////////////中控
		RECT rcCtrl={rcText.right+2,rcText.top-3,rcText.right+18,rcText.bottom+3};
		//CRenderEngine::DrawImageString(hDC,m_pManager,rcCtrl,rcCtrl,m_bCtrlOpen==true?"unlock.png":"lock.png");
		/////////////////////////////////////////使用门
		RECT rcDoor={rcCtrl.right+2,rcCtrl.top,rcCtrl.right+18,rcCtrl.bottom};
		//CRenderEngine::DrawImageString(hDC,m_pManager,rcDoor,rcDoor,m_bDoorOpen==false?"door-close.png":"door-open.png");
		/////////////////////////////////////////投影仪 下拉

		RECT rcProjector={rcDoor.left-42,rcDoor.top,rcDoor.right+18,rcDoor.top+30};
		rcDropBtn=rcProjector;
		//CRenderEngine::DrawImageString(hDC,m_pManager,rcProjector,rcProjector,m_bProjectorOpen==false?"file='CtrlWnd_btn_down.png' source='0,0,30,30' ":"file='CtrlWnd_btn_down.png' source='0,0,30,30'");
		
	/*	CButtonUI *btn=new CButtonUI;
		btn->SetPos(rcDropBtn);
		btn->SetBkColor(Color::Red);
		__super::Add(btn);*/

		
	//菜单
		

		/////////////////////////////////////////录制
		m_rcBtnRes = m_rcItem;
		m_rcBtnRes.left = m_rcItem.right - 18;//width
		m_rcBtnRes.right = m_rcItem.right -1;
		m_rcBtnRes.top += 6;
		m_rcBtnRes.bottom = m_rcBtnRes.top + 18;//height
		CDuiString image=_T("cam_record.png");
		CDuiString modify;
		if(m_uResState & UISTATE_HOT)
		{
			if (!m_bResSelect)
				modify = _T("source='72,0,90,18'");
			else
				modify = _T("source='18,0,36,18'");
			CRenderEngine::DrawImageString(hDC,m_pManager,m_rcBtnRes,m_rcBtnRes,image,modify);
		}
		else
		{
			if (!m_bResSelect)
				modify = _T("source='54,0,72,18'");
			else
				modify = _T("source='0,0,18,18'");
			CRenderEngine::DrawImageString(hDC,m_pManager,m_rcBtnRes,m_rcBtnRes,image,modify);
		}
		///////////////////////////////////////声音
		m_rcBtnVoice = m_rcBtnRes;
		m_rcBtnVoice.right = m_rcBtnVoice.left - 5;
		m_rcBtnVoice.left = m_rcBtnVoice.right - 20;
		m_rcBtnVoice.bottom = m_rcBtnVoice.top + 20;
		image = _T("cam_voice.png");
		if(m_uVoiceState & UISTATE_HOT)
		{
			if (m_bVoiceSelect)
				modify = _T("source='20,0,40,20'");
			else
				modify = _T("source='80,0,100,20'");
			CRenderEngine::DrawImageString(hDC,m_pManager,m_rcBtnVoice,m_rcBtnVoice,image,modify);
		}
		else
		{
			if (m_bVoiceSelect)
				modify = _T("source='0,0,20,20'");
			else
				modify = _T("source='60,0,80,20'");
			CRenderEngine::DrawImageString(hDC,m_pManager,m_rcBtnVoice,m_rcBtnVoice,image,modify);
		}
	}

	void CCamCtrolUI::SetVisible(bool bVisible)
	{
		if(m_pWndParent)
			m_pWndParent->ShowWindow(bVisible);
		if(m_pWindow)
			m_pWindow->ShowWindow(bVisible);
		/*if(m_pWndUp)
			m_pWndUp->ShowWindow(bVisible);*/

		if(menu)
			menu->SetVisible(bVisible);
		if( m_bVisible == bVisible ) 
			return;
		bool v = IsVisible();
		m_bVisible = bVisible;
		if( m_bFocused ) 
			m_bFocused = false;
	}

	HWND CCamCtrolUI::GetHwnd()
	{
		if(m_pWindow)
			return m_pWindow->GetHWND();
		else
			return NULL;
	}

	HWND CCamCtrolUI::GetHwndUp()
	{
		if(m_pWndUp)
			return m_pWndUp->GetHWND();
		else
			return NULL;
	}

	int  CCamCtrolUI::GetSelect()
	{
		return m_iSelect;
	}

	BOOL CCamCtrolUI::GetHasWndFlag()
	{
		return m_bHasWnd;
	}

	bool CCamCtrolUI::GetSingleFlag()
	{
		return m_bSingelUp;
	}

	bool CCamCtrolUI::GetFullFlag()
	{
		if(m_pWndParent)
			return m_pWndParent->GetFullFlag();
		else
			return false;
	}

	bool CCamCtrolUI::GetRecordFlag()
	{
		return m_bResSelect;
	}

	void CCamCtrolUI::SetRecordFlag(bool bRecord)
	{
		if (bRecord == m_bResSelect)
			return;
		m_bResSelect = bRecord;
		Invalidate();
	}

	int  CCamCtrolUI::GetPosType()
	{
		return m_iSelect;
	}

	bool CCamCtrolUI::FullScreen(bool bShowToolbar)
	{
		if(bShowToolbar)
			m_pManager->SendNotify(this,"showtoolbar",0,0,true);
		 m_pWndParent->FullScreen();
		 return true;
	}

	void CCamCtrolUI::SetText(LPCTSTR pstrText)
	{
		m_sTitle = pstrText;
		Invalidate();
	}

	void CCamCtrolUI::UpdateText()
	{
		
	}

	void CCamCtrolUI::OnBtnCtrl()
	{
		
	}

	void CCamCtrolUI::OnBtnEscFull()
	{
		m_pManager->SendNotify(this,_T("CamEscFull"));
	}

	void CCamCtrolUI::OnBtnFull()
	{
		m_pManager->SendNotify(this,_T("CamFullScreen"),0,m_iSelect);
	}

	void CCamCtrolUI::OnMainWndEscFull()
	{
		m_pManager->SendNotify(this,_T("MainWndEscFull"));
	}

	void CCamCtrolUI::OnUpdateToolBar()
	{
		m_pManager->SendNotify(this,_T("UpdateToolBar"));
	}

	void CCamCtrolUI::OnChangeWnd()
	{
		
	}

	

	////////////////////////////////////////////////////
	//

	CCamLayoutUI::CCamLayoutUI():m_hParent(NULL),
		m_iPosCount(0),
		m_iStartPos(0),
		m_bFull(false),
		m_pFileList(NULL)
	{
		m_bSelMutil = false;
		m_bSelAll = false;
	}

	LPCTSTR CCamLayoutUI::GetClass() const
	{
		return _T("CamLayout");
	}

	void CCamLayoutUI::Init()
	{
		m_bCheckCtrl = true;
	}

	void CCamLayoutUI::SetParentHwnd(HWND hwnd)
	{
		m_hParent = hwnd;
	}

	HWND CCamLayoutUI::GetParentHwnd()
	{
		return m_hParent;
	}

	void CCamLayoutUI::DoEvent(TEventUI& event)
	{
		if (event.Type == UIEVENT_DBLCLICK)
		{
			if (GetCount() && m_bFull)
			{
				m_pManager->SendNotify(this,_T("MainWndEscFull"));
			}
		}
		__super::DoEvent(event);
	}

	void CCamLayoutUI::SetPos(RECT rc)
	{
		CControlUI::SetPos(rc);
		if (m_iPosCount == 0)
		{
			int sub_count = GetCount();
			if (sub_count < 1)
				__super::SetPos(rc);
			else if (sub_count == 1)
			{
				SetPos1(rc);
			}
			else if (sub_count <= 4)
			{
				SetPos4(rc);
			}
			else if (sub_count <= 9)
			{
				SetPos9(rc);
			}
			else if (sub_count <= 16)
			{
				SetPos16(rc);
			}
			else if (sub_count <= 25)
			{
				SetPos25(rc);
			}
			else if (sub_count <= 36)
			{
				SetPos36(rc);
			}
			else if (sub_count <= 64)
			{
				SetPos64(rc);
			}
			else
			{
				SetPos64(rc);
			}
		}
		else		
		{
			if(m_iPosCount ==1)
				SetPos1(rc);
			else if(m_iPosCount == 4)
				SetPos4(rc);
			else if(m_iPosCount == 9)
				SetPos9(rc);
			else if(m_iPosCount == 16)
				SetPos16(rc);
			else if(m_iPosCount == 25)
				SetPos25(rc);
			else if(m_iPosCount == 36)
				SetPos36(rc);
			else if(m_iPosCount == 64)
				SetPos64(rc);
		}
	}

	void CCamLayoutUI::SetPos1(RECT rc,int iStartPos,bool bRe)
	{
		if(iStartPos >= GetCount())
			return;

		if (bRe)
		{
			if(iStartPos == 0)
				iStartPos = m_iStartPos;
		}
		m_iStartPos = iStartPos;

		////////////////////////////////////////	hide before
		int i = 0;
		while (i<iStartPos)
		{
			CCamCtrolUI* pCtrl = (CCamCtrolUI*)GetItemAt(i);
			if(pCtrl)
				pCtrl->SetVisible(false);
			++i;
		}
		////////////////////////////////////////	hide after
		i = iStartPos+1;
		int count = GetCount();
		while (i<count)
		{
			CCamCtrolUI* pCtrl = (CCamCtrolUI*)GetItemAt(i);
			if(pCtrl)
				pCtrl->SetVisible(false);
			++i;
		}
		///////////////////////////////////////////////////////

		CControlUI::SetPos(rc);
		CCamCtrolUI* pCtrl = (CCamCtrolUI*)GetItemAt(iStartPos);
		if (pCtrl == NULL)
			return;
		::InflateRect(&rc, -1, -1);
		pCtrl->SetVisible();
		pCtrl->SetPos(rc);

	}

	void CCamLayoutUI::SetPos4(RECT rc,int iStartPos,bool bRe)
	{
		SetPosSquare(2, rc,iStartPos,bRe);
	}

	void CCamLayoutUI::SetPos9(RECT rc,int iStartPos,bool bRe)
	{
		SetPosSquare(3, rc,iStartPos,bRe);
	}

	void CCamLayoutUI::SetPos16(RECT rc,int iStartPos,bool bRe)
	{
		SetPosSquare(4, rc,iStartPos,bRe);
	}

	void CCamLayoutUI::SetPos25(RECT rc,int iStartPos,bool bRe)
	{
		SetPosSquare(5, rc,iStartPos,bRe);
	}

	void CCamLayoutUI::SetPos36(RECT rc,int iStartPos,bool bRe)
	{
		SetPosSquare(6, rc,iStartPos,bRe);
	}

	void CCamLayoutUI::SetPos64(RECT rc,int iStartPos,bool bRe)
	{
		SetPosSquare(8, rc,iStartPos,bRe);
	}

	void CCamLayoutUI::SetPosSquare(int nSquareRoot,RECT rc,int iStartPos,bool bRe)
	{
		if(iStartPos >= GetCount())
			return;

		if (bRe)
		{
			if(iStartPos == 0)
				iStartPos = m_iStartPos;
		}
		m_iStartPos = iStartPos;
		////////////////////////////////////////	hide before
		int i = 0;
		while (i<iStartPos)
		{
			CCamCtrolUI* pCtrl = (CCamCtrolUI*)GetItemAt(i);
			if(pCtrl)
				pCtrl->SetVisible(false);
			++i;
		}
		////////////////////////////////////////	hide after
		int nCount = nSquareRoot*nSquareRoot;
		i = iStartPos + nCount;
		int  count = GetCount();
		while (i<count)
		{
			CCamCtrolUI* pCtrl = (CCamCtrolUI*)GetItemAt(i);
			if(pCtrl)
				pCtrl->SetVisible(false);
			++i;
		}
		///////////////////////////////////////////////////////

		CControlUI::SetPos(rc);
		::InflateRect(&rc, -1, -1);
		int sub_width = (rc.right-rc.left)/nSquareRoot;
		int sub_height = (rc.bottom-rc.top)/nSquareRoot;
		RECT rc_sub = rc;
		CCamCtrolUI* pCtrl = NULL;
		int index = iStartPos;
		i = 0;
		while (index < iStartPos+nCount)
		{
			pCtrl = (CCamCtrolUI*)GetItemAt(index);
			if (pCtrl == NULL)
				break;
			rc_sub.left = rc.left;
			rc_sub.left += (i%nSquareRoot)*sub_width;
			rc_sub.right = rc_sub.left + sub_width;
			rc_sub.top = rc.top;
			rc_sub.top  += (i/nSquareRoot)*sub_height;
			rc_sub.bottom= rc_sub.top + sub_height;
			pCtrl->SetVisible();
			pCtrl->SetPos(rc_sub);
			++index;
			++i;
		}
	}

	void CCamLayoutUI::SetPosCount(int count)
	{
		m_iPosCount = count;
	}

	int  CCamLayoutUI::GetPosCount()
	{
		return m_iPosCount;
	}

	void CCamLayoutUI::SetFullFlag(bool bFull)
	{
		m_bFull = bFull;
	}

	bool CCamLayoutUI::GetFullFlag()
	{
		return m_bFull;
	}

	bool CCamLayoutUI::Add(CDuiString sID)
	{
		if(GetItem(sID))
			return false;
		CCamCtrolUI* pCam = new CCamCtrolUI();
		pCam->SetID(sID);
		if(m_pFileList)
		{
			
		}
		__super::Add(pCam);
		return true;
	}

	bool CCamLayoutUI::AddAt(CDuiString sID,int iIndex)
	{
		if(GetItem(sID))
			return false;
		CCamCtrolUI* pCam = new CCamCtrolUI();
		pCam->SetID(sID);
		if(m_pFileList)
		{
			
		}
		if (!__super::AddAt(pCam,iIndex))
			return false;
		return true;
	}

	bool CCamLayoutUI::Remove(CDuiString sID)
	{
		return true;
	}

	void CCamLayoutUI::RemoveAll()
	{
		int iIndex = 0;
		while(iIndex < GetCount())
		{
			CCamCtrolUI* pItem = (CCamCtrolUI*)GetItemAt(iIndex);
			Remove(pItem->GetID());
			++iIndex;
		}
	}

	CCamCtrolUI* CCamLayoutUI::GetItem(CDuiString sID)
	{
		int nCount = GetCount();
		for (int i = 0;i < nCount; i++)
		{
			CCamCtrolUI* p = (CCamCtrolUI*)GetItemAt(i);
			if(p && p->GetID() == sID)
				return p;
		}
		return NULL;
	}

	CCamCtrolUI* CCamLayoutUI::GetFullScreenItem()
	{
		int nCount = GetCount();
		for (int i = 0;i < nCount; i++)
		{
			CCamCtrolUI* p = (CCamCtrolUI*)GetItemAt(i);
			if(p && p->GetFullFlag())
				return p;
		}
		return NULL;
	}

	void CCamLayoutUI::ReConnectClass()
	{
		int nCount = GetCount();
		for (int i = 0;i < nCount; i++)
		{
			CCamCtrolUI* p = (CCamCtrolUI*)GetItemAt(i);
			if(p && p->IsConnect() && p->IsVisible())
			{
				p->SetVisible(true);
			}
		}
	}

	void CCamLayoutUI::RecordVisualClass(bool bRecord)
	{
		int nCount = GetCount();
		for (int i = 0;i < nCount; i++)
		{
			CCamCtrolUI* p = (CCamCtrolUI*)GetItemAt(i);
			if(p && p->IsConnect() && p->IsVisible())
			{
				if( bRecord && GetRecordCount()>3)
				{
					m_pManager->SendNotify(this,_T("CamRecord"));
					return;
				}
				p->SetRecordFlag(bRecord);
			}
		}
	}

	int  CCamLayoutUI::GetRecordCount()
	{
		int nCount = GetCount();
		int iRecordCount = 0;
		for (int i = 0;i < nCount; i++)
		{
			CCamCtrolUI* p = (CCamCtrolUI*)GetItemAt(i);
			if(p && p->GetRecordFlag())
			{
				++iRecordCount;
			}
		}
		return iRecordCount;
	}

	void CCamLayoutUI::SelectAll(bool bSel)
	{
		int nCount = GetCount();
		for (int i = 0;i < nCount; i++)
		{
			CCamCtrolUI* pCtrl = (CCamCtrolUI*)GetItemAt(i);
			if(pCtrl)
				pCtrl->m_bSelect = bSel;
		}
		Invalidate();
	}

	////////////////////////////////////////////////
	///
	void CBtnCalendarUI::PaintText(HDC hDC)
	{
		RECT rcText = m_rcItem;
		rcText.left += 20;
		CRenderEngine::DrawText(hDC, m_pManager, rcText, m_sText, m_dwTextColor,m_iFont, m_uTextStyle);
	}

	////////////////////////////////////////////////
	///
	void CComboListUI::PaintText(HDC hDC)
	{
		RECT rcImage = m_rcItem;
		rcImage.left += m_rcTextPadding.left;
		rcImage.right -= m_rcTextPadding.right;
		rcImage.top += m_rcTextPadding.top;
		rcImage.bottom -= m_rcTextPadding.bottom;
		CDuiString sImageModify = "dest='2,0,92,24' source='0,0,90,24'";
		CRenderEngine::DrawImageString(hDC,m_pManager,rcImage,rcImage,m_sImage,sImageModify);
	}

	////////////////////////////////////////////////
	///
	void CComboBtnUI::DoEvent(TEventUI& event)
	{
		__super::DoEvent(event);
	}

	////////////////////////////////////////////////////
	//
	COptionElement::COptionElement()
	{
		m_bShowHtml = true;
		SetNorImg(_T("noselectnor.png"));
		SetHotImg(_T("notselecthot.png"));
		SetSelectNorImg(_T("select_nor.png"));
		SetSelectHotImg(_T("select_hot.png"));
	}

	void COptionElement::SetSelectNorImg(CDuiString sImg)
	{
		m_sSelNorImg.Format(_T("<x 2><i %s ><x 6>"),sImg);
	}

	void COptionElement::SetSelectHotImg(CDuiString sImg)
	{
		m_sSelHotImg.Format(_T("<x 2><i %s ><x 6>"),sImg);
	}

	void COptionElement::SetNorImg(CDuiString sImg)
	{
		m_sNorImg.Format(_T("<x 2><i %s ><x 6>"),sImg);
	}

	void COptionElement::SetHotImg(CDuiString sImg)
	{
		m_sHotImg.Format(_T("<x 2><i %s ><x 6>"),sImg);
	}

	void COptionElement::PaintText(HDC hDC)
	{
		DWORD oldTextColor = m_dwTextColor;
		if( m_dwSelectedTextColor != 0 )
			m_dwTextColor = m_dwSelectedTextColor;
		if( m_dwTextColor == 0 ) 
			m_dwTextColor = m_pManager->GetDefaultFontColor();

		int nLinks = 0;
		RECT rc = m_rcItem;
		rc.left += m_rcTextPadding.left;
		rc.right -= m_rcTextPadding.right;
		rc.top += m_rcTextPadding.top;
		rc.bottom -= m_rcTextPadding.bottom;

		CDuiString sText = m_sNorImg;
		if(m_bSelected)
		{
			if( (m_uButtonState & UISTATE_HOT) != 0 && m_sSelHotImg.IsEmpty() == false)
				sText = m_sSelHotImg;
			else if(m_sSelNorImg.IsEmpty() == false)
				sText = m_sSelNorImg;
		}
		else if( (m_uButtonState & UISTATE_HOT) != 0 && m_sHotImg.IsEmpty() == false)
		{
			sText = m_sHotImg;
		}
		sText += m_sText;

		CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, sText,m_dwTextColor,NULL, NULL, nLinks, DT_LEFT);

		m_dwTextColor = oldTextColor;
	}

	void COptionElement::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{	
		if( _tcscmp(pstrName, _T("noselectnorimg")) == 0 )
			SetNorImg(pstrValue);
		else if ( _tcscmp(pstrName, _T("noselecthotimg")) == 0 )
			SetHotImg(pstrValue);
		else if( _tcscmp(pstrName, _T("selectnorimg")) == 0 )
			SetSelectNorImg(pstrValue);
		else if( _tcscmp(pstrName, _T("selecthotimg")) == 0 )
			SetSelectHotImg(pstrValue);
		else
			__super::SetAttribute(pstrName,pstrValue);
	}
}