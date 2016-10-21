#pragma once


#include "stdafx.h"
#include "WinBase.h"
#include <comdef.h>
#include "UI.h"

class CMainWnd;
class  CToolBarWnd :public WindowImplBase
{
public:
	CToolBarWnd(CMainWnd* own);
	~CToolBarWnd();
	LPCTSTR GetWindowClassName() const;
	CDuiString GetSkinFile();
	CDuiString GetSkinFolder();
	CControlUI* CreateControl(LPCTSTR pstrClass);
	void OnFinalMessage( HWND hWnd );
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void SetOwnCam(CCamCtrolUI* p);
	CCamCtrolUI* GetOwnCam();
	void SetBtnFullVisual(BOOL bFull,BOOL bEsc);
	void SetBtnPipVisual(BOOL bPip,BOOL bTile);

protected:
	void Init();
	void Notify(TNotifyUI& msg);
	void OnBtnCtrl(TNotifyUI& msg);
	void OnBtnFull(TNotifyUI& msg);
	void OnBtnEscFull(TNotifyUI& msg);
	void OnBtnPIP(TNotifyUI& msg);
	void OnBtnTile(TNotifyUI& msg);
	void OnBtnSingle(TNotifyUI& msg);

	CCamCtrolUI* m_pCam;
private:
	CMainWnd* m_pOwn;

};