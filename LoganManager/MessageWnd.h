#pragma once
#include "stdafx.h"

enum MessageTimer
{
	AUTOCLOSE = 100
};

class CMessageWnd :public WindowImplBase
{
public:
	CMessageWnd(CDuiString sContext,bool bAutoClose,int iTime,bool bNeedBtnCancle);
	~CMessageWnd();
	LPCTSTR GetWindowClassName() const;
	CDuiString GetSkinFile();
	CDuiString GetSkinFolder();
	CControlUI* CreateControl(LPCTSTR pstrClass);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void OnFinalMessage(HWND hWnd);
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void Init();

protected:
	void Notify(TNotifyUI& msg);
	void OnBtnClose(TNotifyUI& msg);
	void OnBtnOk(TNotifyUI& msg);
	void OnBtnCancle(TNotifyUI& msg);
	void OnWndInit(TNotifyUI& msg);

private:
	HWND m_hParent;
	CDuiString m_sContext;
	bool m_bAutoClose;
	int  m_iTime;
	bool m_bNeedBtnCancle;
	int  m_iOk;
};