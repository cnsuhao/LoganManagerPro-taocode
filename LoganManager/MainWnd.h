#pragma once
#include "stdafx.h"
#include "UI.h"
#include "MessageWnd.h"
#include "HttpRequest.h"
#include "LoganPlayer.h"
#include "Command.h"



//把服务器请求得到的url转换成本地的
static std::string CTL(std::string sourceUrl,std::string localMainIp,std::string port)
{
	std::string buffer=sourceUrl;
	if(sourceUrl.empty())
	{
		return buffer;
	}
	else
	{
		int st=buffer.find_first_of('/');
		int ed=buffer.find_last_of(':');
		buffer.replace(st+2,ed-st-2,localMainIp.c_str());
		return buffer;
	}
}




typedef struct
{
	char*									ip;
	char*									port;
	char*									token;
	ServerModel						model;
	int										index;								
}videoInfo;



static void Assign(_videoURL& a,_videoURL& b)
{
	a.ip=b.ip;
	a.model=b.model;
	a.port=b.port;
	a.token=b.token;
	a.URL=b.URL;
}


class CToolBarWnd;
class CMainWnd : public WindowImplBase
{
public:
	CMainWnd(void);
	~CMainWnd(void);
	static CMainWnd* pwnd;
	LPCTSTR GetWindowClassName()const;
	CDuiString GetSkinFile();
	CControlUI *CreateControl(LPCTSTR pstrClass);
	CDuiString GetSkinFolder();
	void ShowMessage(CDuiString sContext,bool bModel=false,bool bAutoClose=false,int iTime=0,bool bNeedBtnCancle = true);
protected:
	void Init();
	void Notify(TNotifyUI& msg);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void OnBtnClose(TNotifyUI& msg);						//关闭
	void OnBtnMin(TNotifyUI& msg);						//最小化
	void OnBtnRestroe(TNotifyUI& msg);					//恢复
	void OnBtnMax(TNotifyUI& msg);						//最大化
	void OnBtnLBCtrl(TNotifyUI& msg);					//录播控制
	void OnBtnNO_1(TNotifyUI& msg);						
	void OnBtnNO_4(TNotifyUI& msg);
	void OnBtnNO_9(TNotifyUI& msg);
	void OnBtnNO_16(TNotifyUI& msg);
	void OnBtnHideList(TNotifyUI& msg);					//隐藏
	void OnBtnShowList(TNotifyUI& msg);				//显示
	void OnBtnFull(TNotifyUI& msg);						//全屏
	
	void OnEsc();
	void OnBtnPageUp(TNotifyUI& msg);					//上一页
	void OnBtnPageDown(TNotifyUI& msg);				//下一页
	void ShowHideLayout(bool);
	void OnCtrlPanelChange(TNotifyUI& msg);			//控制面板事件
	void UpdateTextPage(int Page,bool bUpdatePageCount=true);
	void UpdateBtnImage(int iNew,int iOld);
	void OnBtnLeft(TNotifyUI& msg);
	void OnBtnRight(TNotifyUI& msg);
	void OnBtnUp(TNotifyUI& msg);
	void OnBtnDown(TNotifyUI& msg);
	void OnBtnReSet(TNotifyUI& msg);
	void OnBtnZoomUp(TNotifyUI& msg);
	void OnBtnZoomDown(TNotifyUI& msg);
	void OnSpeedChange(TNotifyUI& msg);
	void OnSelectRoomItem(TNotifyUI& msg);
	void OnSelectPlayStream(TNotifyUI& msg);
	void OnSelectView(TNotifyUI& msg);
	void OnBtnSwitch(TNotifyUI& msg);
	void OnPrePlace(TNotifyUI& msg);//预置位调用和设置
	void OnEnvironmentCtrl(TNotifyUI& msg);//环境控制
	void OnProjectorCtrl(TNotifyUI& msg);//投影控制
	void OnDoorCtrl(TNotifyUI& msg);// 门禁控制
	void OnSoundCtrl(TNotifyUI& msg);//音量控制
	friend void CALLBACK LoadSource(PTP_CALLBACK_INSTANCE Instance,PVOID Context);
	friend void CALLBACK CtrlDirLeft(PTP_CALLBACK_INSTANCE Instance,PVOID Context);
	friend void CALLBACK CtrlDirRight(PTP_CALLBACK_INSTANCE Instance,PVOID Context);
	friend void CALLBACK CtrlDirUp(PTP_CALLBACK_INSTANCE Instance,PVOID Context);
	friend void CALLBACK CtrlDirDown(PTP_CALLBACK_INSTANCE Instance,PVOID Context);
	friend void CALLBACK CtrlDirStop(PTP_CALLBACK_INSTANCE Instance,PVOID Context);
	friend void CALLBACK CtrlDirReset(PTP_CALLBACK_INSTANCE Instance,PVOID Context);
	friend void CALLBACK CtrlZoomUp(PTP_CALLBACK_INSTANCE Instance,PVOID Context);
	friend void CALLBACK CtrlZoomDown(PTP_CALLBACK_INSTANCE Instance,PVOID Context);
	friend void CALLBACK CtrlPlace(PTP_CALLBACK_INSTANCE Instance,PVOID Context);
	friend void CALLBACK CtrlSwitch(PTP_CALLBACK_INSTANCE Instance,PVOID Context);
public:
	CToolBarWnd* toolBarWnd;
	void UpdateToolBar();
private:
	int preplc;
	RECT windowsRect;
	vector<_videoURL>					videoURL;		//视频流信息
	PTP_POOL								tPool;             //线程池
	TP_CALLBACK_ENVIRON			pcbe;				//线程池环境变量
	int											videoNum;						//视频宫格数
	int											camLayoutCount;
	int											currentPage;
	int											pageCount;	
	CTextUI									*page;
	CCamLayoutUI						*camLayout;					//视频布局
	void ChangeTabSelect(int iParent,int iSub);			//Tab 切换
	void InitThreadPool();
	void AddWorkToThreadPool(PTP_SIMPLE_CALLBACK,PVOID);
	void ReleaseThreadPool();
	void UpdateClassRoom();
	void UpdataStatus();
	void XCmd(const char* ip,u_short port,Command cmd);
};


class Router //路由器
{
protected:
	Router(){mediaPlayer=NULL;camCtrl=NULL;}
	~Router(){}
	static Router* router;
public:
	_videoURL vl;
	IMediaPlayer		*mediaPlayer;//video
	CCamCtrolUI *camCtrl;
	int view;
	int speed;
	static Router* getRouter()
	{
		if(!router)
		{
			router=new Router();
		}
		return router;
	}
};


