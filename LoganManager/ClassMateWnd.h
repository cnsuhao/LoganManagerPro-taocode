#pragma once
#include "stdafx.h"
#include "HttpRequest.h"
#include <winsock2.h>
#include "MainWnd.h"
#include "UdpBroadcast.h"
#include "YoohooComUI.h"
#include "tcp_server.h"

enum TimerEvent
{
	TurnLeftEvent = 900,
	TurnUpEvent,
	TurnRightEvent,
	TurnDownEvent,
	TurnResetEvent,
	TurnStopEvent,
	TurnZoomUPEvent,
	TurnZoomDownEvent,
	TurnPreCallEvent,
	TurnPreSetEvent,
	UpdateLocalLoginEvent=1000,
	FirstLocalLoginEvent,
	UpdateCloudLoginEvent,
	FirstCloudLoginEvent,
	UpdateClassMateUIEvent,
	FirstLoadClassMateUIEvent,
	InitPlayerEvent,
	CallVideo,
	ListenData,
	LoadUrl,
	loadMSG,
	loadSend,
};

struct SS
{
	string targetIP;
	bool isRecv;
};

struct NetMsg
{
	string recvIP;
	string sendIP;
	string msg;
	bool used;
	bool operator <(const NetMsg& a)const
	{
		return a.msg<msg;
	}
};

class ClassMateWnd : public WindowImplBase
{
public:
	ClassMateWnd(void);
	~ClassMateWnd(void);
	LPCTSTR GetWindowClassName()const;
	CDuiString GetSkinFile();
	CControlUI *CreateControl(LPCTSTR pstrClass);
	CDuiString GetSkinFolder();
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam);
	string otheruser;
	string otherpwd;
	map<string,vector<string>>otherIP_PORT_DEV;
	map<string,string>otherStreamUrl;
	USHORT  recvport;
	string wantuser;
	string wantip;
	CUdpBroadcast server;
	bool checkMsg(map<string,string>a);
	set<string>fromIP;
	set<string>sendIP;
	vector<string>playneedip;
	vector<string>playsendip;
	set<string>playip;
	map<string,string>remotePLayerList;
	int fromMsg(string title,string msg);
	YoohooComUI* vvv;
	void addsubVideo(string address);
	string m_s;
	tcp_server *tcpServer;
	bool isTip;
	vector<string>TranVideoList;//正在进行视频的列表
	map<string,set<NetMsg>>MsgMap;//按发送者接收消息
private:
	int countTip;
	int countsend;
	int currentsend;
	int currentTip;
	USHORT  m_tcpPort;
	void Init();
	void Notify(TNotifyUI& msg);
	void OnBtnClose(TNotifyUI& msg);						
	void OnBtnMin(TNotifyUI& msg);						
	void OnBtnRestroe(TNotifyUI& msg);					
	void OnBtnMax(TNotifyUI& msg);
	void Load();
	bool oncheck(CTreeNodeUI *node);
	void removeNode(CTreeNodeUI *node);
	ServerModel dev;
	set<string>loadingIP;
	netCode code;
	string cloudIP,cloudPort,cloudUser,cloudPwd;
	queue<Data_LG> roomData;
	USHORT  sendport;
	vector<CTreeNodeUI*>currentVideo;//--------------
	//friend void Listen(ClassMateWnd* p); 
private:
	void OnCtrlNotify(TNotifyUI& msg);//云台控制事件
public:
	std::string m_streamType;//流类别
	int      m_speed;//速度
	int m_view;
	int      m_preCall;//预置位
	std::string m_loginIp,m_loginPort,m_user,m_pwd;
	netCode  m_localCode;
	map<std::string,std::string>m_urlmap;
	void OnLeft();
	void OnUp();
	void OnRight();
	void OnDown();
	void OnStop();
	void OnReset();
	void OnZoomUp();
	void OnZoomDown();
	void OnPreCall(int index);
	void OnPreSet(int index);

};
