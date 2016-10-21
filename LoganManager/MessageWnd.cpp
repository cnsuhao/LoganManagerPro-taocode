#include "StdAfx.h"
#include "MessageWnd.h"

#include "MessageWnd.h"


CMessageWnd::CMessageWnd(CDuiString sContext,bool bAutoClose,int iTime,bool bNeedBtnCancle):m_hParent(NULL)
{
	m_sContext = sContext;
	m_bAutoClose = bAutoClose;
	m_iTime = iTime;
	m_bNeedBtnCancle = bNeedBtnCancle;
	m_iOk = 0;
}

CMessageWnd::~CMessageWnd()
{
	
}

LPCTSTR CMessageWnd::GetWindowClassName() const
{
	return _T("MessageFrameWnd");
}

CDuiString CMessageWnd::GetSkinFile()
{
	return _T("MessageWnd.xml");
}

CDuiString CMessageWnd::GetSkinFolder()
{
	return _T("Skin");
}
CControlUI* CMessageWnd::CreateControl(LPCTSTR pstrClass)
{
	CControlUI* pReturn = NULL;
	return pReturn;
}

LRESULT CMessageWnd::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_hParent = GetParent(m_hWnd);
	if(m_hParent)
	{
		char *sMessage = new char[m_sContext.GetLength()+2];
		strcpy(sMessage,m_sContext);
		::PostMessage(m_hParent,WM_MESSAGEBOX,m_iOk,(LPARAM)sMessage);
	}
	bHandled = false;
	return 0;
}

LRESULT CMessageWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_TIMER)
	{
		if(wParam == AUTOCLOSE)
		{
			KillTimer(m_hWnd,AUTOCLOSE);
			Close();
		}
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void CMessageWnd::Init()
{
	CTextUI* pText = (CTextUI*)m_PaintManager.FindControl(_T("text_context"));
	if(pText)
	{
		pText->SetTextStyle(DT_VCENTER|DT_CENTER);
		pText->SetText(m_sContext);
	}
	if(m_bAutoClose && m_iTime!=0)
		SetTimer(m_hWnd,AUTOCLOSE,m_iTime,NULL);
}

void CMessageWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == _T("click"))
	{
		if (msg.pSender->GetName() == _T("btn_close"))
			OnBtnClose(msg);
		if (msg.pSender->GetName() == _T("btn_ok"))
			OnBtnOk(msg);
		if (msg.pSender->GetName() == _T("btn_cancle"))
			OnBtnCancle(msg);
	}
	else if (msg.sType == _T("windowinit"))
		OnWndInit(msg);
}

void CMessageWnd::OnBtnClose(TNotifyUI& msg)
{
	m_iOk = 0;
	Close();
}

void CMessageWnd::OnBtnOk(TNotifyUI& msg)
{
	/*m_hParent = GetParent(m_hWnd);
	if(m_hParent)
	{
		char *sMessage = new char[m_sContext.GetLength()+2];
		strcpy(sMessage,m_sContext);
		::PostMessage(m_hParent,WM_MESSAGEBOX,1,(LPARAM)sMessage);
	}*/
	m_iOk = 1;
	Close();
}

void CMessageWnd::OnBtnCancle(TNotifyUI& msg)
{
	/*m_hParent = GetParent(m_hWnd);
	if(m_hParent)
	{
		char *sMessage = new char[m_sContext.GetLength()+2];
		strcpy(sMessage,m_sContext);
		::PostMessage(m_hParent,WM_MESSAGEBOX,0,(LPARAM)sMessage);
	}*/
	m_iOk = 0;
	Close();
}

void CMessageWnd::OnWndInit(TNotifyUI& msg)
{
	if (!m_bNeedBtnCancle)
	{
		CControlUI* pTemp = m_PaintManager.FindControl(_T("btn_cancle"));
		pTemp->SetVisible(m_bNeedBtnCancle);
		pTemp = m_PaintManager.FindControl(_T("btn_ok"));
		RECT rc;
		GetWindowRect(m_hWnd,&rc);
		int center = (rc.right-rc.left)/2;
		rc = pTemp->GetPos();
		int length = (rc.right-rc.left)/2;
		rc.left = center - length;
		rc.right= center + length;
		pTemp->SetPos(rc);
	}
}

void CMessageWnd::OnFinalMessage( HWND hWnd )
{
	delete this;
}
