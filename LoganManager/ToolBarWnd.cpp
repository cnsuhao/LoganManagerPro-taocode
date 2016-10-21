#include "StdAfx.h"
#include "ToolBarWnd.h"
#include "UI.h"
#include "MainWnd.h"
CToolBarWnd::CToolBarWnd(CMainWnd* own)
{
	m_pOwn = own;
	m_pCam = NULL;
}

CToolBarWnd::~CToolBarWnd()
{

}

LPCTSTR CToolBarWnd::GetWindowClassName() const
{
	return "ToolBarFrameWnd";
}

CDuiString CToolBarWnd::GetSkinFile()
{
	return "ToolBarWnd.xml";
}

CDuiString CToolBarWnd::GetSkinFolder()
{
	return "Skin";
}
CControlUI* CToolBarWnd::CreateControl(LPCTSTR pstrClass)
{
	CControlUI* pReturn = NULL;
	return pReturn;
}

void CToolBarWnd::OnFinalMessage( HWND hWnd )
{
	delete this;
}

LRESULT CToolBarWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void CToolBarWnd::SetOwnCam(CCamCtrolUI* p)
{
	m_pCam = p;
	if(m_pCam == NULL)
		return;

	CButtonUI* pTemp = (CButtonUI*)m_PaintManager.FindControl(_T("btn_single"));
	int iPosSelect = m_pCam->GetPosType();
	if (iPosSelect == 2)
	{
		pTemp->SetNormalImage("btn_tool_single4.png");
		pTemp->SetHotImage("btn_tool_single3.png");
	}
	else
	{
		pTemp->SetNormalImage("btn_tool_single1.png");
		pTemp->SetHotImage("btn_tool_single2.png");
	}
}

CCamCtrolUI* CToolBarWnd::GetOwnCam()
{
	return m_pCam;
}

void CToolBarWnd::SetBtnFullVisual(BOOL bFull,BOOL bEsc)
{
	CControlUI* p = m_PaintManager.FindControl(_T("btn_full"));
	if(p)
		p->SetVisible(bFull);
	p = m_PaintManager.FindControl(_T("btn_esc_full"));
	if(p)
		p->SetVisible(bEsc);
}

void CToolBarWnd::SetBtnPipVisual(BOOL bPip,BOOL bTile)
{
	CControlUI* p = m_PaintManager.FindControl(_T("btn_pip"));
	if(p)
		p->SetVisible(bPip);
	p = m_PaintManager.FindControl(_T("btn_tile"));
	if(p)
		p->SetVisible(bTile);
}

void CToolBarWnd::Init()
{

}

void CToolBarWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == _T("click"))
	{
		if (msg.pSender->GetName() == _T("btn_ctrl"))
			OnBtnCtrl(msg);
		else if (msg.pSender->GetName() == _T("btn_full"))
			OnBtnFull(msg);
		else if (msg.pSender->GetName() == _T("btn_esc_full"))
			OnBtnEscFull(msg);
		else if (msg.pSender->GetName() == _T("btn_pip"))
			OnBtnPIP(msg);
		else if (msg.pSender->GetName() == _T("btn_tile"))
			OnBtnTile(msg);
		else if (msg.pSender->GetName() == _T("btn_single"))
			OnBtnSingle(msg);
	}
}

void CToolBarWnd::OnBtnCtrl(TNotifyUI& msg)
{
	if(m_pCam)
		m_pCam->OnBtnCtrl();
}

void CToolBarWnd::OnBtnFull(TNotifyUI& msg)
{
	if(m_pCam)
	{
		SetParent(m_pOwn->toolBarWnd->GetHWND(),m_pCam->m_pWndParent->GetHWND());
		m_pCam->FullScreen();
	}
	CControlUI* pTemp = m_PaintManager.FindControl(_T("btn_esc_full"));
	pTemp->SetVisible();
	msg.pSender->SetVisible(false);
}

void CToolBarWnd::OnBtnEscFull(TNotifyUI& msg)
{
	if(m_pCam)
	{
		m_pCam->FullScreen();
	}
	CControlUI* pTemp = m_PaintManager.FindControl(_T("btn_full"));
	pTemp->SetVisible();
	msg.pSender->SetVisible(false);
}

void CToolBarWnd::OnBtnPIP(TNotifyUI& msg)
{
	if(m_pCam)
	{
		m_pCam->SetPosPIP();
		SetBtnPipVisual(false,true);
		CDuiString name = m_pOwn->GetWindowClassName();
		if (name == _T("MainFrameWnd"))
			((CMainWnd*)m_pOwn)->UpdateToolBar();
	}
}

void CToolBarWnd::OnBtnTile(TNotifyUI& msg)
{
	if(m_pCam)
	{
		m_pCam->SetPosTile();
		SetBtnPipVisual(true,false);
		CDuiString name = m_pOwn->GetWindowClassName();
		if (name == _T("MainFrameWnd"))
			((CMainWnd*)m_pOwn)->UpdateToolBar();
	}
}

void CToolBarWnd::OnBtnSingle(TNotifyUI& msg)
{
	if(m_pCam)
	{
		m_pCam->SetPosSingle(!m_pCam->GetSingleFlag());

		CDuiString name = m_pOwn->GetWindowClassName();
		if (name == _T("MainWnd"))
			((CMainWnd*)m_pOwn)->UpdateToolBar();
	}
}