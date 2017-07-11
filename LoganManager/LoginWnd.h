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
	map<string, vector<string>>userList;				//�û���Ϣ�б�
	COptionUI*    currentOpt;
	bool         isCloud;
protected:
	void Notify(TNotifyUI& msg);
	void OnBtnClose(TNotifyUI& msg);						//�ر�
	void OnBtnMin(TNotifyUI& msg);						//��С��
	void OnBtnSet(TNotifyUI& msg);						//����
	void OnBtnLogin(TNotifyUI& msg);						//��¼
	void OnBtnOkCancle(TNotifyUI& msg);				//ȡ��&ȷ��
	void OnBtnLogout(TNotifyUI& msg);					//
	void OnBtnErrorOk(TNotifyUI& msg);					//
	void OnReturn(TNotifyUI& msg);						//�س�
	void OnItemSelect(TNotifyUI& msg);					//�û�ѡ��
	void OnOptionSelect(TNotifyUI& msg);				//��¼״̬ѡ��
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