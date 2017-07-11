#include "StdAfx.h"
#include "LoginWnd.h"
#include "LoginWndCfg.h"
#include "DataBase.h"
#include "ReadConfig.h"
#include "HttpRequest.h"
#include "ConfigFile.h"
CLoginWnd::CLoginWnd()
{
	bRem = false;
	bAuto = false;
	user=NULL;
	pwd=NULL;
	TabSelect = NULL;
	currentOpt = NULL;
	isCloud =false;
}


CLoginWnd::~CLoginWnd()
{
}

LPCTSTR CLoginWnd::GetWindowClassName()const
{
	return _T("LoginWnd");
}

CDuiString CLoginWnd::GetSkinFile()
{
	return _T("LoginWnd.xml");
}
CControlUI* CLoginWnd::CreateControl(LPCTSTR pstrClass)
{
	if(0 == _tcscmp(pstrClass,_T("AnimationTabLayout")))
	{
		return new CAnimationTabLayoutUI;
	}
	return NULL;
}
CDuiString CLoginWnd::GetSkinFolder()
{

	return _T("Skin");

}


LRESULT CLoginWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return __super::OnSysCommand(uMsg, wParam, lParam, bHandled);
}
void CLoginWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender->GetName() == BTN_CLOSE)
		{
			OnBtnClose(msg);
		}
		else if (msg.pSender->GetName() == BTN_MIN)
		{
			OnBtnMin(msg);
		}
		else if (msg.pSender->GetName()==BTN_SETTING)
		{
			OnBtnSet(msg);
		}
		else if (msg.pSender->GetName() == BTN_OK || msg.pSender->GetName() == BTN_CANCLE)
		{
			OnBtnOkCancle(msg);
		}
		else if (msg.pSender->GetName()==BTN_LOGIN)
		{
			OnBtnLogin(msg);
		}
		else if (msg.pSender->GetName()==BTN_LOGOUT)
		{
			OnBtnLogout(msg);
		}
		else if (msg.pSender->GetName()==BTN_ERROR_OK)
		{
			OnBtnErrorOk(msg);
		}
	}
	if (msg.sType == DUI_MSGTYPE_SELECTCHANGED)
	{
		OnOptionSelect(msg);
	}
	if(msg.sType==DUI_MSGTYPE_ITEMSELECT)
	{
		OnItemSelect(msg);
	}
	if(msg.sType==DUI_MSGTYPE_RETURN)
	{
		OnReturn(msg);
	}
	//OutputDebugStringA(msg.sType);
}
void CLoginWnd::OnBtnClose(TNotifyUI& msg)
{
	Close(IDCANCEL);
}
void CLoginWnd::OnBtnMin(TNotifyUI& msg)
{
	::ShowWindow(m_hWnd, SW_MINIMIZE);
}
void CLoginWnd::OnBtnSet(TNotifyUI& msg)
{
	TabSelect->SelectItem(1);
}
void CLoginWnd::OnBtnLogin(TNotifyUI& msg)
{
	if(!user->GetText().IsEmpty()&&!pwd->GetText().IsEmpty())
	{
		TabSelect->SelectItem(2);
		SetTimer(m_hWnd,TIME_LOGIN_ID,2000,NULL);
	}
	else
	{
		LoginError(_T("用户名或密码不能为空"));
	}
	
}
void CLoginWnd::OnBtnOkCancle(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == BTN_OK)
	{
		WriteConfig();
		user->RemoveAll();
		LoadUser();
		user->SelectItem(0);
	}
	if (msg.pSender->GetName() == BTN_CANCLE)
	{
		ReadConfig();
	}
	ReadConfig();
	WriteConfig();
	TabSelect->SelectItem(0);
}
void CLoginWnd::OnBtnLogout(TNotifyUI& msg)
{
	KillTimer(m_hWnd,TIME_LOGIN_ID);
	TabSelect->SelectItem(0);
}
void CLoginWnd::OnBtnErrorOk(TNotifyUI& msg)
{
	TabSelect->SelectItem(0);
}
void CLoginWnd::OnReturn(TNotifyUI& msg)
{
	OnBtnLogin(msg);
}
void CLoginWnd::OnItemSelect(TNotifyUI& msg)
{
	if(msg.pSender->GetName()==user->GetName())
	{
		pwd->SetText(userList[user->GetText().GetData()][0].c_str());
		bRem=(userList[user->GetText().GetData()][1]=="1");
		bAuto=(userList[user->GetText().GetData()][2]=="1");
		userName=user->GetText().GetData();
		PassWord=pwd->GetText().GetData();
		
		COptionUI* opt_rem=static_cast<COptionUI*>(m_PaintManager.FindControl(OPT_REM));
		opt_rem->Selected(bRem);
		COptionUI* opt_auto=static_cast<COptionUI*>(m_PaintManager.FindControl(OPT_AUTO));
		opt_auto->Selected(bAuto);

	}
	if(bAuto)
		OnBtnLogin(msg);
}

void CLoginWnd::OnOptionSelect(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == OPT_REM)
	{
		COptionUI *op=static_cast<COptionUI*>(m_PaintManager.FindControl(OPT_REM));
		bRem = op->IsSelected();
	}
	if (msg.pSender->GetName() == OPT_AUTO)
	{
		bAuto = !bAuto;
	}

	COptionUI *opt_ctrl=static_cast<COptionUI*>(m_PaintManager.FindControl("opt_ctrl"));
	COptionUI *opt_local=static_cast<COptionUI*>(m_PaintManager.FindControl("opt_local"));
	if(msg.pSender->GetName()==_T("opt_ctrl"))
	{
		if(opt_ctrl->IsSelected())
		{
			currentOpt = opt_ctrl;
			isCloud = true;
		}		
	}

	if(msg.pSender->GetName()==_T("opt_local"))
	{
		if(opt_local->IsSelected())
		{
			currentOpt = opt_local;
			isCloud = false;
		}
		
	}
	if(currentOpt)
	{
	  OutputDebugStringA(currentOpt->GetName());
	}
	
}



void CLoginWnd::Init()
{
	COptionUI *opt_ctrl=static_cast<COptionUI*>(m_PaintManager.FindControl("opt_ctrl"));
	COptionUI *opt_local=static_cast<COptionUI*>(m_PaintManager.FindControl("opt_local"));
	opt_ctrl->SetGroup("fuck");
	opt_local->SetGroup("fuck");
	if (!TabSelect)
	{
		TabSelect = static_cast<CAnimationTabLayoutUI*>(m_PaintManager.FindControl(TAB_NAME));
	}
	
	if(!user)
	{
		user=static_cast<CComboEditUI*>(m_PaintManager.FindControl(USERNAME));
		userName=user->GetText().GetData();
	}

	if(!pwd)
	{
		pwd=static_cast<CEditUI*>(m_PaintManager.FindControl(PASSWORD));
		PassWord=pwd->GetText().GetData();
	}

	ifstream inToFile;
	inToFile.open(IPFILE, std::ios::in);
	//读取配置
	ReadConfig();
	//加载用户信息
	LoadUser();
	//加载网络配置
	map<string,string>m=ReadConfigure::Read(Net_Config);
	HttpRequest::embedModel=m["embed"];
	HttpRequest::iMacModel=m["iMac"];
	HttpRequest::lgCould=m["cloud"];

}
LRESULT CLoginWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_TIMER)
		return OnTimer(uMsg, wParam, lParam);
	return __super::HandleMessage(uMsg, wParam, lParam);
}
LRESULT CLoginWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(wParam==TIME_LOGIN_ID)
	{

		userName=user->GetText().GetData();
		PassWord=pwd->GetText().GetData();
		netCode code;
		if(isCloud)
		{
			code=HttpRequest::LogIn(Ip,Port,cloud,userName,PassWord);
			CFG *cfg=CFG::GetConfig();
			if(code.status)
			{
				cfg->ip=Ip;
				cfg->port=Port;
				cfg->token=code.msg;
				UpLoadUser();
				KillTimer(m_hWnd,TIME_LOGIN_ID);
				Close(IDOK);
			}
		}
		else
		{

			code=HttpRequest::LogIn(Ip,Port,embed,userName,PassWord);
			CFG *cfg=CFG::GetConfig();
			if(code.status)
			{
				ConfigFile newcfg("../bin/config/banban.conf");
				newcfg.addValue("ip",Ip,"localLogin");
				newcfg.addValue("port",Port,"localLogin");
				newcfg.addValue("user",userName,"localLogin");
				newcfg.addValue("pwd",PassWord,"localLogin");
				newcfg.addValue("env","0","dev");
				newcfg.save();
				cfg->ip=Ip;
				cfg->port=Port;
				cfg->token=code.msg;
				UpLoadUser();
				KillTimer(m_hWnd,TIME_LOGIN_ID);
				Close(10086);
			}
			else
			{
				code=HttpRequest::LogIn(Ip,Port,iMac,userName,PassWord);
				if(code.status)
				{
					ConfigFile newcfg("../bin/config/banban.conf");
					newcfg.addValue("ip",Ip,"localLogin");
					newcfg.addValue("port",Port,"localLogin");
					newcfg.addValue("user",userName,"localLogin");
					newcfg.addValue("pwd",PassWord,"localLogin");
					newcfg.addValue("env","1","dev");
					newcfg.save();
					cfg->ip=Ip;
					cfg->port=Port;
					cfg->token=code.msg;
					UpLoadUser();
					KillTimer(m_hWnd,TIME_LOGIN_ID);
					Close(10086);
				}
			}
		}
		CFG *cfg=CFG::GetConfig();
		if(code.status)
		{
			cfg->ip=Ip;
			cfg->port=Port;
			cfg->token=code.msg;
			UpLoadUser();
			KillTimer(m_hWnd,TIME_LOGIN_ID);
			Close(IDOK);
		}
		else
		{
			KillTimer(m_hWnd,TIME_LOGIN_ID);
			CDuiString err;
			err.Format(_T("%s"),code.msg.c_str());
			LoginError(err);
		}
		
	}

	return 0;
}


void CLoginWnd::ReadConfig()
{
	ifstream inToFile1, inToFile2;
	inToFile1.open(IPFILE, std::ios::in);
	CEditUI *edit_ip = static_cast<CEditUI*>(m_PaintManager.FindControl(IPNAME));
	char str[30];
	CDuiString cstr;
	inToFile1.getline(str, 29);
	cstr = string(str).c_str();
	edit_ip->SetText(cstr);
	Ip=string(str);
	inToFile1.close();
	/********************************************/
	inToFile2.open(PORTFILE, std::ios::in);
	CEditUI *edit_port = static_cast<CEditUI*>(m_PaintManager.FindControl(PORTNAME));
	inToFile2.getline(str, 29);
	cstr = string(str).c_str();
	edit_port->SetText(cstr);
	Port=string(str);
	inToFile2.close();
}

void CLoginWnd::WriteConfig()
{
	ofstream outToFile;
	outToFile.open(IPFILE, std::ios::out);
	CEditUI *edit_ip = static_cast<CEditUI*>(m_PaintManager.FindControl(IPNAME));
	outToFile << edit_ip->GetText();
	outToFile.close();
	/********************************************/
	outToFile.open(PORTFILE, std::ios::out);
	CEditUI *edit_port = static_cast<CEditUI*>(m_PaintManager.FindControl(PORTNAME));
	outToFile << edit_port->GetText();
	outToFile.close();
}

void CLoginWnd::LoginError(CDuiString err)//登录异常
{
		CTextUI *t=static_cast<CTextUI*>(m_PaintManager.FindControl(ERROR_TEXT));
		t->SetText(err);
		TabSelect->SelectItem(3);	
}

void CLoginWnd::LoadUser()
{
	vector<vector<string>> hh = DataBase::Load(Ip);
	userList.clear();
	for (int i = 0; i < hh.size(); i++)
	{
		vector<string>s;
		s.resize(3);
		s[0] = hh[i][1];		//密码
		s[1] = hh[i][3];		//记住
		s[2] = hh[i][4];		//登录
		userList[hh[i][0]] = s;
	}

	//显示
	user->RemoveAll();
	pwd->SetText("");
	for(map<string, vector<string>>::iterator i=userList.begin();i!=userList.end();i++)
	{
		CListLabelElementUI* L=new CListLabelElementUI();
		L->SetText((LPCTSTR)i->first.c_str());
		user->Add(L);
	}
	user->SelectItem(0);
	pwd->SetText(userList.begin()->second[0].c_str());
	if(userList.begin()->second[2]=="1")
	{
		CButtonUI *btn=static_cast<CButtonUI*>(m_PaintManager.FindControl(BTN_LOGIN));
		m_PaintManager.SendNotify(btn,DUI_MSGTYPE_CLICK,0,0);
	}

}
void CLoginWnd::UpLoadUser()
{
	if(bRem)
		DataBase::Modify(userName,PassWord,Ip,bRem,bAuto);
	/*else
	{
		DataBase::del(userName, PassWord, Ip);
	}*/
}



/**************************/


CFG* CFG::config=NULL;