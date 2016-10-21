#pragma once
#include "stdafx.h"
#include "UI.h"
#include "MessageWnd.h"
#include "HttpRequest.h"
#include "LoganPlayer.h"
#include "Command.h"



//�ѷ���������õ���urlת���ɱ��ص�
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
	void OnBtnClose(TNotifyUI& msg);						//�ر�
	void OnBtnMin(TNotifyUI& msg);						//��С��
	void OnBtnRestroe(TNotifyUI& msg);					//�ָ�
	void OnBtnMax(TNotifyUI& msg);						//���
	void OnBtnLBCtrl(TNotifyUI& msg);					//¼������
	void OnBtnNO_1(TNotifyUI& msg);						
	void OnBtnNO_4(TNotifyUI& msg);
	void OnBtnNO_9(TNotifyUI& msg);
	void OnBtnNO_16(TNotifyUI& msg);
	void OnBtnHideList(TNotifyUI& msg);					//����
	void OnBtnShowList(TNotifyUI& msg);				//��ʾ
	void OnBtnFull(TNotifyUI& msg);						//ȫ��
	
	void OnEsc();
	void OnBtnPageUp(TNotifyUI& msg);					//��һҳ
	void OnBtnPageDown(TNotifyUI& msg);				//��һҳ
	void ShowHideLayout(bool);
	void OnCtrlPanelChange(TNotifyUI& msg);			//��������¼�
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
	void OnPrePlace(TNotifyUI& msg);//Ԥ��λ���ú�����
	void OnEnvironmentCtrl(TNotifyUI& msg);//��������
	void OnProjectorCtrl(TNotifyUI& msg);//ͶӰ����
	void OnDoorCtrl(TNotifyUI& msg);// �Ž�����
	void OnSoundCtrl(TNotifyUI& msg);//��������
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
	vector<_videoURL>					videoURL;		//��Ƶ����Ϣ
	PTP_POOL								tPool;             //�̳߳�
	TP_CALLBACK_ENVIRON			pcbe;				//�̳߳ػ�������
	int											videoNum;						//��Ƶ������
	int											camLayoutCount;
	int											currentPage;
	int											pageCount;	
	CTextUI									*page;
	CCamLayoutUI						*camLayout;					//��Ƶ����
	void ChangeTabSelect(int iParent,int iSub);			//Tab �л�
	void InitThreadPool();
	void AddWorkToThreadPool(PTP_SIMPLE_CALLBACK,PVOID);
	void ReleaseThreadPool();
	void UpdateClassRoom();
	void UpdataStatus();
	void XCmd(const char* ip,u_short port,Command cmd);
};


class Router //·����
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


