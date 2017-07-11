#include "StdAfx.h"
#include "YoohooVideo.h"
#include "ILivePlayer.h"
YoohooVideoWnd::YoohooVideoWnd():m_pOwner(NULL)
{

}


YoohooVideo::YoohooVideo(void):m_pwindows(NULL)
{
#ifdef VLC_VIDEO
	videoPlayer = new LoganPlayer();
#else
	videoPlayer = ILivePlayer::GetInstance();
#endif
}

YoohooVideo::~YoohooVideo(void)
{
#ifdef VLC_VIDEO
	videoPlayer->stop();
	delete videoPlayer;
#else
	videoPlayer->Stop();
	ILivePlayer::ReleaseInstance(videoPlayer);
#endif
}

LPCTSTR YoohooVideoWnd::GetWindowClassName() const
{
	return _T("YoohooVideoWnd");
}
void YoohooVideoWnd::Init(YoohooVideo* pOwner)
{
	m_pOwner = pOwner;
	RECT rcpos=CalPos();
	RECT rcwnd;
	::GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(),&rcwnd);
	rcpos.left+=rcwnd.left;
	rcpos.right+=rcwnd.left;
	rcpos.top+=rcwnd.top;
	rcpos.bottom+=rcwnd.top;
	Create(m_pOwner->GetManager()->GetPaintWindow(),NULL,WS_CHILD|WS_EX_TOPMOST | ES_AUTOHSCROLL | WS_VISIBLE,0,rcpos);
	::ShowWindow(m_hWnd,SW_SHOW);
}
LRESULT YoohooVideoWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_ERASEBKGND)
	{
		PainBk();
	}
	return CWindowWnd::HandleMessage(uMsg,wParam,lParam);
}

RECT YoohooVideoWnd::CalPos()
{
	return m_pOwner->GetPos();
}
void YoohooVideoWnd::PainBk()
{
	CDuiRect rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	PAINTSTRUCT    ps;
	HDC hDC = ::BeginPaint(m_hWnd, &ps);
	HDC hMemDC = ::CreateCompatibleDC(hDC);
	HBITMAP hBitmap = ::CreateCompatibleBitmap(hDC, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
	::SelectObject(hMemDC, hBitmap);
	CRenderEngine::DrawColor(hMemDC, rcClient, 0x0);
	//CRenderEngine::DrawColor(hMemDC, rcClient, 0xffff0000);

	BitBlt(hDC, 0, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, hMemDC, 0, 0, SRCCOPY);
	DeleteDC(hMemDC);
	DeleteObject(hBitmap);
	::EndPaint(m_hWnd, &ps);
}

/**************************************************/
void YoohooVideo::SetVisible(bool bVisible )
{
	__super::SetVisible(bVisible);
	ShowWindow(getHwnd(),bVisible);
}
void YoohooVideo::SetPos(RECT rc)
{
	__super::SetPos(rc);
	if(m_pwindows)
	{
		RECT rcpos=m_pwindows->CalPos();
		::SetWindowPos(m_pwindows->GetHWND(),NULL,rcpos.left,rcpos.top,rcpos.right-rcpos.left,rcpos.bottom-rcpos.top,SWP_NOZORDER | SWP_NOACTIVATE);
	}
}
HWND YoohooVideo::getHwnd()
{
	if(!m_pwindows)
	{
		m_pwindows = new YoohooVideoWnd();
		ASSERT(m_pwindows);
		m_pwindows->Init(this);
	}
	return m_pwindows->GetHWND();
}
void YoohooVideo::Init()
{
	if(!m_pwindows)
	{
		m_pwindows = new YoohooVideoWnd();
		ASSERT(m_pwindows);
		m_pwindows->Init(this);
	}
#ifdef VLC_VIDEO
	videoPlayer->setHwnd(getHwnd());
#else
	videoPlayer->SetHWND(getHwnd());
#endif
	
}