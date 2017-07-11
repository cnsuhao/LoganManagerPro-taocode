#pragma once
#include "stdafx.h"

class CLoginWnd :public WindowImplBase
{
public:
	CLoginWnd();
	~CLoginWnd();
public:
	LPCTSTR GetWindowClassName()const;
	CDuiString GetSkinFile();
	CControlUI *CreateControl(LPCTSTR pstrClass);
	CDuiString GetSkinFolder();
	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	CAnimationTabLayoutUI *TabSelect;
	CComboEditUI *user; 
	CEditUI *pwd;
	string userName;
	string PassWord;
	bool bRem;
	bool bAuto;
	string Ip;
	string Port;
	map<string, vector<string>>userList;				//用户信息列表
	COptionUI*    currentOpt;
	bool         isCloud;
protected:
	void Notify(TNotifyUI& msg);
	void OnBtnClose(TNotifyUI& msg);						//关闭
	void OnBtnMin(TNotifyUI& msg);						//最小化
	void OnBtnSet(TNotifyUI& msg);						//设置
	void OnBtnLogin(TNotifyUI& msg);						//登录
	void OnBtnOkCancle(TNotifyUI& msg);				//取消&确定
	void OnBtnLogout(TNotifyUI& msg);					//
	void OnBtnErrorOk(TNotifyUI& msg);					//
	void OnReturn(TNotifyUI& msg);						//回车
	void OnItemSelect(TNotifyUI& msg);					//用户选择
	void OnOptionSelect(TNotifyUI& msg);				//登录状态选择
	void Init();

	/************************************/
	void ReadConfig();
	void WriteConfig();
	void LoginError(CDuiString err);

	void LoadUser();
	void UpLoadUser();

};


class CFG
{
protected:
	CFG(){};
	~CFG(){};
	static CFG* config;
public:
	string ip;
	string port;
	string token;
	static CFG* GetConfig()
	{
		if(!config)
		{
			config=new CFG();
		}
		return config;
	}
	static void Release()
	{
		delete config;
		config=NULL;
	}
};