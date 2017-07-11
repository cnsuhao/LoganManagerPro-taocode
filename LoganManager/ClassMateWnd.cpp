#include "StdAfx.h"
#include "ClassMateWnd.h"
#include "MainWndCfg.h"
#include "YoohooVideo.h"
//#include "../VideoPlayer/IVideoPlayer.h"
#include "LoganPlayer.h"
#include "YoohooComUI.h"
#include "stdafx.h"
#include "ConfigFile.h"
#include "ClientSocket.h"

#include "UdpBroadcast.h"
#include "tcp_client.h"
//DWORD WINAPI MsgProc(_In_ LPVOID paramer);
//
//class tcp_client
//{
//public:
//	tcp_client(string ip,USHORT port,char* sendmsg)
//	{
//		m_serverIP=ip;
//		m_serverPort=port;
//		WSADATA m_wsaData;
//		if(sendmsg!=NULL)
//		{
//			strcpy_s(msg,sendmsg);
//		}
//		if(WSAStartup(MAKEWORD(2,2),&m_wsaData)==NO_ERROR)
//		{
//			m_serverAddr.sin_family=AF_INET;
//			m_serverAddr.sin_addr.S_un.S_addr=inet_addr(m_serverIP.c_str());
//			m_serverAddr.sin_port=htons(m_serverPort);
//
//			m_clientSock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
//
//			connect(m_clientSock,(sockaddr*)&m_serverAddr,sizeof m_serverAddr);
//
//			CreateThread(NULL,0,MsgProc,this,0,0);
//
//		}
//	}
//	~tcp_client()
//	{
//
//	}
//
//public:
//	string           m_serverIP;
//	USHORT		   m_serverPort;
//	SOCKADDR_IN    m_serverAddr;
//	SOCKET         m_clientSock;
//	char data[1024];
//	char msg[1024];
//
//};
//
//DWORD WINAPI MsgProc(_In_ LPVOID paramer)
//{
//	tcp_client *p = (tcp_client*)paramer;
//	send(p->m_clientSock,p->msg,sizeof(p->msg),0);
//	recv(p->m_clientSock,p->data,sizeof(p->data),0);
//
//	return 0;
//}

string sendmsg="";
string inviteip="";
map<string,bool>record_log;
ClassMateWnd::ClassMateWnd(void)
{
	m_speed = 50;
	m_view =1;
	m_streamType="sublurl";
	m_preCall=1;
	isTip=false;
	countTip=0;
	currentTip=0;
	countsend=0;
	currentsend=0;
	tcpServer=NULL;
}

ClassMateWnd::~ClassMateWnd(void)
{
	server.Close();
	/*if(tcpServer)
	{
		delete tcpServer;
		tcpServer=NULL;
	}*/
}

DWORD WINAPI LoadOtherUrl(_In_  LPVOID Parameter)
{
	ClassMateWnd* pthis=(ClassMateWnd*)Parameter;
	for(map<string,vector<string>>::iterator iter=pthis->otherIP_PORT_DEV.begin();iter!=pthis->otherIP_PORT_DEV.end();iter++)
	{
		int cnntype;
		if(iter->second[1]=="1")
		{
			cnntype=embed;
		}
		else
		{
			cnntype=iMac;
		}
		netCode code=HttpRequest::LogIn(iter->first,iter->second[0],(ServerModel)cnntype,pthis->otheruser,pthis->otherpwd);
		if(code.status)
		{
			pthis->otherStreamUrl[iter->first]=CTL(HttpRequest::LookUpUrl(iter->first,iter->second[0],(ServerModel)cnntype,code.msg,"sublurl","look"),iter->first,iter->second[0]);
		}
		
	}
	return 0;
}


DWORD WINAPI LIstenNetData(_In_  LPVOID Parameter)
{
	ClassMateWnd *pthis=(ClassMateWnd*)Parameter;
	pthis->remotePLayerList=pthis->server.getList();

	if(!pthis->remotePLayerList.empty())
	{
		for(map<string,string>::iterator i=pthis->remotePLayerList.begin();i!=pthis->remotePLayerList.end();i++)
		{
			OutputDebugStringA(i->second.c_str());
			string sendip=i->second.substr(i->second.find_last_of('#')+1);
			string recvip=i->second.substr(0,i->second.find_first_of('#'));
			string info=i->second.substr(i->second.find_first_of('#')+1);
			info=info.substr(0,info.find_first_of('#'));
			if(info=="yes")
			{
				int i=0;
				for(i=0;i<pthis->TranVideoList.size();i++)
				{
					if(pthis->TranVideoList[i]==sendip)
						break;
				}
				if(i==pthis->TranVideoList.size())
				{
					pthis->TranVideoList.push_back(sendip);
				}
			}
		}
	}
	if(pthis->checkMsg(pthis->remotePLayerList))
	{
		for(set<string>::iterator iter=pthis->fromIP.begin();iter!=pthis->fromIP.end();iter++)
		{
			pthis->playip.insert(*iter);
			string info=*iter;
			for(int i=0;i<pthis->playneedip.size();i++)
			{
				if(*iter==pthis->playneedip[i])
				{
					continue;
				}
				else
				{
					pthis->playneedip.push_back(*iter);
					pthis->isTip=true;
				}
				
			}
			if(pthis->playneedip.empty())
			{
				pthis->playneedip.push_back(*iter);
			}
		}

		for(set<string>::iterator iter=pthis->sendIP.begin();iter!=pthis->sendIP.end();iter++)
		{
			string info=*iter;
			pthis->playip.insert(*iter);
			for(int i=0;i<pthis->playsendip.size();i++)
			{
				if(*iter==pthis->playsendip[i])
				{
					continue;
				}
				else
				{
					pthis->playsendip.push_back(*iter);
					pthis->isTip=true;
				}

			}
			if(pthis->playsendip.empty())
			{
				pthis->playsendip.push_back(*iter);
			}
		}
	}
	return 0;
}
DWORD WINAPI CallVideoSender(_In_  LPVOID Parameter)
{
	ClassMateWnd *pthis=(ClassMateWnd*)(Parameter);
	//pthis->tcpServer->getCmd(pthis->m_s)
	return 0;
}

LPCTSTR ClassMateWnd::GetWindowClassName() const
{
	return _T("ClassMate");
}

CDuiString ClassMateWnd::GetSkinFile()
{
	return _T("ClassMate.xml");
}

void ClassMateWnd::addsubVideo(string address)
{
	vvv->addVideo();
#ifdef VLC_VIDEO
	vvv->getVideo(0)->videoPlayer->play(address);
#else
	vvv->getVideo(0)->videoPlayer->Load(address);
#endif
	SendMessage(WM_SIZE,0,0);
}
CControlUI* ClassMateWnd::CreateControl(LPCTSTR pstrClass)
{
	if(_tcscmp(pstrClass,_T("Video"))==0)
		return new YoohooComUI();
	return __super::CreateControl(pstrClass);
}

CDuiString ClassMateWnd::GetSkinFolder()
{
	return _T("skin");
}
int ClassMateWnd::fromMsg(string title,string msg)
{
	return (m_hWnd,msg.c_str(),title.c_str(),MB_YESNO);
}
void ClassMateWnd::Init()
{
	/**
	* load configuration file
	*/
	ConfigFile cfg("../bin/config/banban.conf");
	cloudIP=cfg.getValue("ip","cloudLogin");
	cloudPort=cfg.getValue("port","cloudLogin");
	cloudUser=cfg.getValue("user","cloudLogin");
	cloudPwd=cfg.getValue("pwd","cloudLogin");
	m_loginIp=cfg.getValue("ip","localLogin");
	m_loginPort=cfg.getValue("port","localLogin");
	m_user=cfg.getValue("user","localLogin");
	m_pwd=cfg.getValue("pwd","localLogin");
	sendport=atoi(cfg.getValue("sendport","UDP").c_str());
	recvport=atoi(cfg.getValue("recvport","UDP").c_str());
	otheruser=cfg.getValue("user","other");
	otherpwd=cfg.getValue("pwd","other");
	m_tcpPort=atoi(cfg.getValue("port","tcp").c_str());
	if(strcmp(cfg.getValue("env","dev").c_str(),"0")==0)
	{
		dev = embed;
	}
	else
	{
		dev=iMac;
	}
		vvv= static_cast<YoohooComUI*>(m_PaintManager.FindControl("videoTest"));
	Load();
	CButtonUI *btn_die=static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_die")));

	if(vvv->getVideoCount()==1)
	{
		btn_die->SetVisible(false);
	}
	//监听数据以及发送数据---探听客户端是否在线
	server.Open(recvport,recvport,m_loginIp);


	tcpServer = new tcp_server(m_tcpPort);
	tcpServer->start_listening();

	CTabLayoutUI *tab = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl("btn_ctrl"));
	tab->SelectItem(1);


}

void ClassMateWnd::Load()
{
	SetTimer(m_hWnd,FirstLocalLoginEvent,0,NULL);
	SetTimer(m_hWnd,InitPlayerEvent,300,NULL);
	SetTimer(m_hWnd,UpdateLocalLoginEvent,1000*1800,NULL);
	//SetTimer(m_hWnd,FirstCloudLoginEvent,0,NULL);
	//SetTimer(m_hWnd,UpdateCloudLoginEvent,1000*1800,NULL);//update token; 30min
	SetTimer(m_hWnd,UpdateClassMateUIEvent,1000,NULL);//
	SetTimer(m_hWnd,FirstLoadClassMateUIEvent,100,NULL);
	SetTimer(m_hWnd,ListenData,300,NULL);
	SetTimer(m_hWnd,LoadUrl,2000,NULL);
	SetTimer(m_hWnd,loadMSG,300,NULL);
	SetTimer(m_hWnd,loadSend,300,NULL);
}
bool ClassMateWnd::checkMsg(map<string,string>a)
{
	for(map<string,string>::iterator i=a.begin();i!=a.end();i++)
	{
		string s=i->second.substr(0,i->second.find_first_of('#'));//接收方
		string p =i->second.substr(i->second.find_first_of('#')+1);
		wantuser=p.substr(0,p.find_first_of('#'));//消息
		wantip=p.substr(p.find_first_of('#')+1);//发送方

		if(s==m_loginIp&&wantuser!="yes")
		{
			fromIP.insert(i->second);
		}
		else if(s==m_loginIp&&wantuser=="yes")
		{
			sendIP.insert(i->second);
		}
	}
	return true;
}
bool ClassMateWnd::oncheck(CTreeNodeUI *node)
{
	int i=0;
	for(i=0;i<currentVideo.size();i++)
	{
		if(node==currentVideo[i])
		{
			return false;
		}
			
	}
	if(i==currentVideo.size())
		currentVideo.push_back(node);
	return true;
}

void ClassMateWnd::removeNode(CTreeNodeUI *node)
{
	for(vector<CTreeNodeUI*>::iterator i=currentVideo.begin();i!=currentVideo.end();i++)
	{
		if(*i==node)
		{
			currentVideo.erase(i);
			break;
		}
	}
}
LRESULT ClassMateWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_TIMER)
		return OnTimer(uMsg, wParam, lParam);
	/*if(uMsg ==WM_MOUSEMOVE)
	{
		POINT pt;
		::GetCursorPos(&pt);
		::ScreenToClient(*this,&pt);
		CTabLayoutUI *lay =static_cast<CTabLayoutUI*>(m_PaintManager.FindControl("toollay"));
		if(lay->GetCurSel()!=0)
		{
			if(pt.y<lay->GetPos().top)
			{
				lay->SelectItem(0);
			}
		}
		return __super::HandleMessage(uMsg, wParam, lParam);
	}*/
	return __super::HandleMessage(uMsg, wParam, lParam);
}
LRESULT ClassMateWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(wParam==FirstCloudLoginEvent)
	{
		code=HttpRequest::LogIn(cloudIP,cloudPort,cloud,cloudUser,cloudPwd);
		if(code.status)
		{
			KillTimer(m_hWnd,FirstCloudLoginEvent);
		}
		else
		{
			KillTimer(m_hWnd,FirstCloudLoginEvent);
		}
	}
	else if(wParam==loadMSG)
	{
		CButtonUI *btn_die=static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_die")));
		if(tcpServer!=NULL)
		{
			string cmd=tcpServer->getCmd(m_s);
			if(!tcpServer->recvip.empty()&&loadingIP.find(tcpServer->recvip)==loadingIP.end())
			{
				addsubVideo(otherStreamUrl[tcpServer->recvip]);
				loadingIP.insert(tcpServer->recvip);
				SendMessage(WM_SIZE,0,0);

				if(!btn_die->IsVisible())
				{
					btn_die->SetVisible(true);
				}
			}
			if(!cmd.empty())
			{
				int x=vvv->getVideoCount();
				map<string,string>aa=server.getList();
				if(cmd=="recv" && loadingIP.size()<vvv->getVideoCount()&&loadingIP.find(aa[m_s])==loadingIP.end())
				{
					addsubVideo(otherStreamUrl[aa[m_s]]);
					loadingIP.insert(aa[m_s]);
					if(!btn_die->IsVisible())
					{
						btn_die->SetVisible(true);
					}
					SendMessage(WM_SIZE,0,0);
				}

				/*else if(cmd=="die")
				{

				}
				else if(cmd=="refu")
				{

				}
				else
				{
					if(loadingIP.find(cmd)!=loadingIP.end())
					{
						;
					}
					else
					{
						loadingIP.insert(cmd);
						addsubVideo(otherStreamUrl[cmd]);
						SendMessage(WM_SIZE,0,0);
					}
				}*/
				
			}
		}
		
	}
	else if(wParam==loadSend)
	{
		countsend=TranVideoList.size();
		if(currentsend<countsend)
		{
			++currentsend;
				addsubVideo(otherStreamUrl[TranVideoList[countsend-1]]);
			SendMessage(WM_SIZE,0,0);
		}
		
	}
	else if(wParam==InitPlayerEvent)
	{
		YoohooComUI *vv= static_cast<YoohooComUI*>(m_PaintManager.FindControl("videoTest"));
		if(!m_urlmap["cur"].empty())
		{
#ifdef VLC_VIDEO
			vv->getLocalVideo()->videoPlayer->play(m_urlmap["cur"]);
#else
			vv->getLocalVideo()->videoPlayer->Load(m_urlmap["cur"]);
#endif
			//if(vv->getLocalVideo()->videoPlayer->isPlaying())
			KillTimer(m_hWnd,InitPlayerEvent);
		}
	}
	else if(wParam==UpdateCloudLoginEvent)
	{
		//CreateThread(NULL,0,UpdateLocalLogin,this,0,0);
	}
	else if(wParam==FirstLocalLoginEvent)
	{
		//CreateThread(NULL,0,FirstLoad,this,0,0);
		m_localCode=HttpRequest::LogIn(m_loginIp,m_loginPort,dev,m_user,m_pwd);

		if(m_localCode.status)
		{
			m_urlmap["lurl"]= CTL(HttpRequest::LookUpUrl(m_loginIp,m_loginPort,dev,m_localCode.msg,"lurl","looklurl"),m_loginIp,m_loginPort);
			m_urlmap["sublurl"]= CTL(HttpRequest::LookUpUrl(m_loginIp,m_loginPort,dev,m_localCode.msg,"sublurl","looksublurl"),m_loginIp,m_loginPort);
			m_urlmap["durl"]= CTL(HttpRequest::LookUpUrl(m_loginIp,m_loginPort,dev,m_localCode.msg,"durl","lookdurl"),m_loginIp,m_loginPort);
			m_urlmap["cur"]=m_urlmap["sublurl"];
			KillTimer(m_hWnd,FirstLocalLoginEvent);
		}
	}
	else if(wParam==UpdateLocalLoginEvent)
	{
		m_localCode=HttpRequest::LogIn(m_loginIp,m_loginPort,dev,m_user,m_pwd);

		if(m_localCode.status)
		{
			m_urlmap["lurl"]= CTL(HttpRequest::LookUpUrl(m_loginIp,m_loginPort,dev,m_localCode.msg,"lurl","looklurl"),m_loginIp,m_loginPort);
			m_urlmap["sublurl"]= CTL(HttpRequest::LookUpUrl(m_loginIp,m_loginPort,dev,m_localCode.msg,"sublurl","looksublurl"),m_loginIp,m_loginPort);
			m_urlmap["durl"]= CTL(HttpRequest::LookUpUrl(m_loginIp,m_loginPort,dev,m_localCode.msg,"durl","lookdurl"),m_loginIp,m_loginPort);
			m_urlmap["cur"]=m_urlmap["sublurl"];
		}
	}
	else if(wParam==TurnLeftEvent)
	{
		OnLeft();
		KillTimer(m_hWnd,TurnLeftEvent);
	}
	else if(wParam==TurnUpEvent)
	{
		OnLeft();
		KillTimer(m_hWnd,TurnUpEvent);
	}
	else if(wParam==TurnRightEvent)
	{
		OnRight();
		KillTimer(m_hWnd,TurnRightEvent);
	}
	else if(wParam==TurnDownEvent)
	{
		OnDown();
		KillTimer(m_hWnd,TurnDownEvent);
	}
	else if(wParam==TurnResetEvent)
	{
		OnReset();
		KillTimer(m_hWnd,TurnResetEvent);
	}
	else if(wParam==TurnStopEvent)
	{
		OnStop();
		KillTimer(m_hWnd,TurnStopEvent);
	}
	else if(wParam==TurnZoomUPEvent)
	{
		OnZoomUp();
		KillTimer(m_hWnd,TurnZoomUPEvent);
	}
	else if(wParam==TurnZoomDownEvent)
	{
		OnZoomDown();
		KillTimer(m_hWnd,TurnZoomDownEvent);
	}
	else if(wParam==TurnPreCallEvent)
	{
		OnPreCall(m_preCall);
		KillTimer(m_hWnd,TurnPreCallEvent);
	}
	else if(wParam==TurnPreSetEvent)
	{
		OnPreSet(m_preCall);
		KillTimer(m_hWnd,TurnPreSetEvent);
	}
	else if(wParam==UpdateClassMateUIEvent)
	{
		currentTip=server.getList().size();
		if(currentTip!=countTip)
		{
			countTip=server.getList().size();
			roomData=HttpRequest::LookUpClassRoom(cloudIP,cloudPort,cloud,code.msg,"&page=0&size=100","load-room");
			if(roomData.size()>0)
			{
				CTreeViewUI *tree = static_cast<CTreeViewUI*>(m_PaintManager.FindControl("classroom"));
				tree->RemoveAll();
				queue<Data_LG>temp = roomData;
				while(!temp.empty())
				{
					Data_LG LG=temp.front();
					for(int i=0;i<LG.Body.size();i++)
					{
						if(LG.Body[i]["devtype"]=="1" &&LG.Body[i]["cnttype"]=="1")
						{
							CTreeNodeUI* subNode= new CTreeNodeUI();
							//subNode->SetSelItemTextColor(Color::Cyan);
							subNode->SetItemTextColor(Color::White);
							subNode->SetSelItemTextColor(Color::Cyan);
							subNode->SetItemHotTextColor(Color::Cyan);
							//subNode->SetItemHotTextColor(Color::Green);
							subNode->SetItemText((LG.Head["name"].c_str()+LG.Body[i]["ip"]).c_str());
							subNode->SetUserData(LG.Body[i]["ip"].c_str());
							subNode->GetFolderButton()->SetBkImage(_T(" file='notexpand.png' dest='5,3,15,13' "));
							subNode->GetFolderButton()->SetSelectedImage(_T(" file='expand.png' dest='5,3,15,13' "));
							map<string,string>clientInfo=server.getList();
							for(map<string,string>::iterator iter=clientInfo.begin();iter!=clientInfo.end();iter++)
							{
								if(LG.Body[i]["ip"]==iter->second)
								{
									CTreeNodeUI *node = new CTreeNodeUI();
									node->SetItemText(iter->first.c_str());
									node->SetItemTextColor(Color::White);
									node->SetSelItemTextColor(Color::Cyan);
									node->SetItemHotTextColor(Color::Cyan);
									subNode->Add(node);
								}
							}
							vector<string>s;
							s.resize(2);
							s[0]=LG.Body[i]["port"];
							s[1]=LG.Body[i]["cnttype"];
							otherIP_PORT_DEV[LG.Body[i]["ip"]]=s;
							if(LG.Body[i]["ip"]==m_loginIp)
							{
								subNode->SetItemTextColor(Color::Tan);
							}
							//rootNode->Add(subNode);
							tree->Add(subNode);
						}
					}

					temp.pop();
				}
				tree->SetVisibleCheckBtn(false);
			}
		}
		
		return 0;
	}
	else if(wParam==LoadUrl)
	{

		CreateThread(NULL,0,LoadOtherUrl,this,0,0);
		//KillTimer(m_hWnd,LoadUrl);
	}
	else if(wParam==CallVideo)
	{
		CreateThread(NULL,0,CallVideoSender,this,0,0);
		KillTimer(m_hWnd,CallVideo);
	}
	else if(wParam==ListenData)
	{
		CreateThread(NULL,0,LIstenNetData,this,0,0);
	}
	else if(wParam==FirstLoadClassMateUIEvent)
	{
		roomData=HttpRequest::LookUpClassRoom(cloudIP,cloudPort,cloud,code.msg,"&page=0&size=100","load-room");
		if(roomData.size()>0)
		{
			KillTimer(m_hWnd,FirstLoadClassMateUIEvent);
			CTreeViewUI *tree = static_cast<CTreeViewUI*>(m_PaintManager.FindControl("classroom"));
			tree->RemoveAll();
			queue<Data_LG>temp = roomData;
			while(!temp.empty())
			{
				Data_LG LG=temp.front();
				for(int i=0;i<LG.Body.size();i++)
				{
					if(LG.Body[i]["devtype"]=="1"&&LG.Body[i]["cnttype"]=="1")
					{
						CTreeNodeUI* subNode= new CTreeNodeUI();
						subNode->SetSelItemTextColor(Color::Cyan);
						subNode->SetItemTextColor(Color::White);
						subNode->SetItemText((LG.Head["name"].c_str()+LG.Body[i]["ip"]).c_str());
						subNode->SetUserData(LG.Body[i]["ip"].c_str());
						subNode->GetFolderButton()->SetBkImage(_T(" file='notexpand.png' dest='5,3,15,13' "));
						subNode->GetFolderButton()->SetSelectedImage(_T(" file='expand.png' dest='5,3,15,13' "));
						map<string,string>clientInfo=server.getList();
						for(map<string,string>::iterator iter=clientInfo.begin();iter!=clientInfo.end();iter++)
						{
							if(LG.Body[i]["ip"]==iter->second)
							{
								CTreeNodeUI *node = new CTreeNodeUI();
								node->SetItemTextColor(Color::White);
								node->SetItemText(iter->first.c_str());
								subNode->Add(node);
							}
						}
						vector<string>s;
						s.resize(2);
						s[0]=LG.Body[i]["port"];
						s[1]=LG.Body[i]["cnttype"];
						otherIP_PORT_DEV[LG.Body[i]["ip"]]=s;
						if(LG.Body[i]["ip"]==m_loginIp)
						{
							subNode->SetItemTextColor(Color::Tan);
						}
						//rootNode->Add(subNode);
						tree->Add(subNode);
					}
				}

				temp.pop();
			}
			tree->SetVisibleCheckBtn(false);
		}
		
	}
	return 0;
}
void ClassMateWnd::Notify(TNotifyUI& msg)
{
	CTabLayoutUI *tab = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl("btn_ctrl"));
	
	
	OnCtrlNotify(msg);

	if(msg.sType==DUI_MSGTYPE_CLICK)
	{
		if(msg.pSender->GetName()==BTN_CLOSE)
		{
			OnBtnClose(msg);
		}
		if(msg.pSender->GetName()==_T("btn_die"))
		{
			int x=vvv->getVideoCount();
			for(int i=1;i<x;i++)
			{
#ifdef VLC_VIDEO
				vvv->getVideo(0)->videoPlayer->stop();
#else
				vvv->getVideo(0)->videoPlayer->Stop();
#endif
				vvv->removeVideo();
			}
			loadingIP.clear();
			tcpServer->recvip.clear();
			tcpServer->cmdSet.clear();
			msg.pSender->SetVisible(false);
			SendMessage(WM_SIZE,0,0);
		}
		else if(msg.pSender->GetName()==BTN_MIN)
		{
			OnBtnMin(msg);
		}
		else if(msg.pSender->GetName()==BTN_MAX)
		{
			OnBtnMax(msg);
			msg.pSender->SetVisible(false);
			CButtonUI *btn_restore=static_cast<CButtonUI*>(m_PaintManager.FindControl(BTN_RESTORE));
			btn_restore->SetVisible(true);
		}
		else if(msg.pSender->GetName()==BTN_RESTORE)
		{
			OnBtnRestroe(msg);
			msg.pSender->SetVisible(false);
			CButtonUI *btn_max=static_cast<CButtonUI*>(m_PaintManager.FindControl(BTN_MAX));
			btn_max->SetVisible(true);
		}
		
	}
	else if(msg.sType==DUI_MSGTYPE_ITEMSELECT)
	{
		if(msg.pSender->GetName()==_T("classroom"))
		{
			OutputDebugString(msg.pSender->GetUserData());
			//CTreeViewUI *treeview=static_cast<CTreeViewUI*>(m_PaintManager.FindControl("classroom"));
			//treeview->SelectedNode
			//if(msg.pSender->GetName()==_T("classroom"))
			{
				//MessageBox(m_hWnd,"等待XXX加入视频","提示",MB_OKCANCEL);
			}

		}
	}
	else if(msg.sType==_T("itemrclick"))
	{
		CTreeNodeUI *node=static_cast<CTreeNodeUI*>(msg.pSender);
		//if(oncheck(node))
		{
			CDuiString s;
			s.Format("与%s进行视频",node->GetItemText());
			if(MessageBox(m_hWnd,s,"提示",MB_YESNO)==IDYES)
			{
				//发送多播包
				//string s=string(node->GetUserData())+"#"+string(node->GetItemText())+"#"+m_loginIp;
				sendmsg="send#"+m_loginIp;
				char s[1024];
				strcpy_s(s,sendmsg.c_str());
				tcp_client c(string(node->GetItemText()),m_tcpPort,s);
				m_s=string(node->GetItemText());
				SetTimer(m_hWnd,CallVideo,0,NULL);
			}
			/*else
			{
				removeNode(node);
				return;
			}*/
		}
	}
	else if(msg.sType==DUI_MSGTYPE_SELECTCHANGED)
	{
	/*	if(msg.pSender->GetTag()==12580)
		::MessageBox(m_hWnd,"hello","tip",MB_OKCANCEL);*/
	}
	else if(msg.sType==DUI_MSGTYPE_MOUSEENTER)
	{
		CTabLayoutUI *lay =static_cast<CTabLayoutUI*>(m_PaintManager.FindControl("toollay"));
		if(msg.pSender->GetName()==_T("btn_ctrl"))
		{
			lay->SelectItem(1);
		}
		else if(msg.pSender->GetName()==_T("btn_envo"))
		{
			lay->SelectItem(2);
		}
		else if(msg.pSender->GetName()==_T("btn_sound"))
		{
			lay->SelectItem(3);
		}
		else if(msg.pSender->GetName()==_T("btn_touying"))
		{
			lay->SelectItem(4);
		}
		else if(msg.pSender->GetName()==_T("btn_door"))
		{
			lay->SelectItem(5);
		}
		else if(msg.pSender->GetName()==_T("btn_ban"))
		{
			lay->SelectItem(6);
		}
	}
	OutputDebugStringA(msg.sType);
	
}

void ClassMateWnd::OnLeft()
{
	stringstream sStream;
	string args;
	sStream<<"&speed="<<m_speed<<"&view="<<m_view-1;
	sStream>>args;
	sStream.clear();
	HttpRequest::DirControl(m_loginIp,m_loginPort,dev,m_localCode.msg,LEFT,args);
}
void ClassMateWnd::OnUp()
{
	stringstream sStream;
	string args;
	sStream<<"&speed="<<m_speed<<"&view="<<m_view-1;
	sStream>>args;
	sStream.clear();
	HttpRequest::DirControl(m_loginIp,m_loginPort,dev,m_localCode.msg,UP,args);
}
void ClassMateWnd::OnRight()
{
	stringstream sStream;
	string args;
	sStream<<"&speed="<<m_speed<<"&view="<<m_view-1;
	sStream>>args;
	sStream.clear();
	HttpRequest::DirControl(m_loginIp,m_loginPort,dev,m_localCode.msg,RIGHT,args);
}
void ClassMateWnd::OnDown()
{
	stringstream sStream;
	string args;
	sStream<<"&speed="<<m_speed<<"&view="<<m_view-1;
	sStream>>args;
	sStream.clear();
	HttpRequest::DirControl(m_loginIp,m_loginPort,dev,m_localCode.msg,DOWN,args);
}
void ClassMateWnd::OnStop()
{
	stringstream sStream;
	string args;
	sStream<<"&speed="<<m_speed<<"&view="<<m_view-1;
	sStream>>args;
	sStream.clear();
	HttpRequest::DirControl(m_loginIp,m_loginPort,dev,m_localCode.msg,STOP,args);
}
void ClassMateWnd::OnReset()
{
	stringstream sStream;
	string args;
	sStream<<"&speed="<<m_speed<<"&view="<<m_view-1;
	sStream>>args;
	sStream.clear();
	HttpRequest::DirControl(m_loginIp,m_loginPort,dev,m_localCode.msg,RESET,args);
}
void ClassMateWnd::OnZoomUp()
{
	stringstream sStream;
	string args;
	sStream<<"&view="<<m_view-1;
	sStream>>args;
	sStream.clear();
	HttpRequest::ZoomControl(m_loginIp,m_loginPort,dev,m_localCode.msg,ZOOMUP,args);
}
void ClassMateWnd::OnZoomDown()
{
	stringstream sStream;
	string args;
	sStream<<"&view="<<m_view-1;
	sStream>>args;
	sStream.clear();
	HttpRequest::ZoomControl(m_loginIp,m_loginPort,dev,m_localCode.msg,ZOOMDOWN,args);
}
void ClassMateWnd::OnPreCall(int index)
{
	stringstream sStream;
	string args;
	sStream<<"&view="<<m_view<<"&precall="<<index;
	sStream>>args;
	sStream.clear();
	HttpRequest::PlaceControl(m_loginIp,m_loginPort,dev,m_localCode.msg,args);
}
void ClassMateWnd::OnPreSet(int index)
{
	stringstream sStream;
	string args;
	sStream<<"&view="<<m_view<<"&preset="<<index;
	sStream>>args;
	sStream.clear();
	HttpRequest::PlaceControl(m_loginIp,m_loginPort,dev,m_localCode.msg,args);
}
void ClassMateWnd::OnBtnClose(TNotifyUI& msg)
{
	Close(IDOK);
}

void ClassMateWnd::OnBtnMin(TNotifyUI& msg)
{
	::ShowWindow(m_hWnd, SW_MINIMIZE);
}

void ClassMateWnd::OnBtnRestroe(TNotifyUI& msg)
{
	::ShowWindow(m_hWnd, SW_RESTORE);
}
void ClassMateWnd::OnBtnMax(TNotifyUI& msg)
{
	::ShowWindow(m_hWnd, SW_MAXIMIZE);
}

/**
* 云台控制
*/
void ClassMateWnd::OnCtrlNotify(TNotifyUI& msg)
{
	YoohooComUI *vv= static_cast<YoohooComUI*>(m_PaintManager.FindControl("videoTest"));
	if(msg.sType==DUI_MSGTYPE_ITEMSELECT)
	{
		if(msg.pSender->GetName()==_T("stream_switch"))
		{
			if(_tcscmp(msg.pSender->GetText(),_T("标清"))==0)
			{
				m_streamType="sublurl";
				m_urlmap["cur"]=m_urlmap["sublurl"];
#ifdef VLC_VIDEO
				vv->getLocalVideo()->videoPlayer->play(m_urlmap["sublurl"]);
#else
				vv->getLocalVideo()->videoPlayer->Load(m_urlmap["sublurl"]);
#endif
				
			}
			else if(_tcscmp(msg.pSender->GetText(),_T("高清"))==0)
			{
				m_streamType="lurl";
				m_urlmap["cur"]=m_urlmap["lur"];
#ifdef VLC_VIDEO
				vv->getLocalVideo()->videoPlayer->play(m_urlmap["lurl"]);
#else
				vv->getLocalVideo()->videoPlayer->Load(m_urlmap["lurl"]);
#endif
			}
			else if(_tcscmp(msg.pSender->GetText(),_T("导播"))==0)
			{
				m_streamType = "durl";
				m_urlmap["cur"]=m_urlmap["durl"];
#ifdef VLC_VIDEO
				vv->getLocalVideo()->videoPlayer->play(m_urlmap["durl"]);
#else
				vv->getLocalVideo()->videoPlayer->Load(m_urlmap["durl"]);
#endif
			}
		}
	}
	else if(msg.sType==DUI_MSGTYPE_CLICK)
	{
		if(msg.pSender->GetName()==_T("btn_left"))
		{
			SetTimer(m_hWnd,TurnLeftEvent,0,NULL);
		}
		else if(msg.pSender->GetName()==_T("btn_right"))
		{
			SetTimer(m_hWnd,TurnRightEvent,0,NULL);
		}
		else if(msg.pSender->GetName()==_T("btn_up"))
		{
			SetTimer(m_hWnd,TurnUpEvent,0,NULL);
		}
		else if(msg.pSender->GetName()==_T("btn_down"))
		{
			SetTimer(m_hWnd,TurnDownEvent,0,NULL);
		}
		else if(msg.pSender->GetName()==_T("btn_home"))
		{
			SetTimer(m_hWnd,TurnResetEvent,0,NULL);
		}
		else if(msg.pSender->GetName()==_T("btn_add"))
		{
			SetTimer(m_hWnd,TurnZoomUPEvent,0,NULL);
		}
		else if(msg.pSender->GetName()==_T("btn_sub"))
		{
			SetTimer(m_hWnd,TurnZoomDownEvent,0,NULL);
		}
	}
	else if(msg.sType==DUI_MSGTYPE_CLICKUP)
	{
		SetTimer(m_hWnd,TurnStopEvent,0,NULL);
		if(msg.pSender->GetName()==_T("btn_pre_set"))
		{
			SetTimer(m_hWnd,TurnPreSetEvent,0,NULL);
		}
		else if(msg.pSender->GetName()==_T("btn_next"))
		{
			if(m_view==6)
			{
				m_view=1;
			}
			else
			{
				++m_view;
			}
			CDuiString s;
			s.Format("%d",m_view);
			CLabelUI *lab= static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lab_view")));
			lab->SetText(s);
		}
		else if(msg.pSender->GetName()==_T("btn_back"))
		{
			if(m_view==1)
			{
				m_view=6;
			}
			else
			{
				--m_view;
			}
			CDuiString s;
			s.Format("%d",m_view);
			CLabelUI *lab= static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lab_view")));
			lab->SetText(s);
		}
	}
	else if(msg.sType==DUI_MSGTYPE_SELECTCHANGED)
	{
		if(msg.pSender->GetName()==_T("opt_pre_1"))
		{
			m_preCall =1;
			SetTimer(m_hWnd,TurnPreCallEvent,0,NULL);
		}
		else if(msg.pSender->GetName()==_T("opt_pre_2"))
		{
			m_preCall = 2;
			SetTimer(m_hWnd,TurnPreCallEvent,0,NULL);
		}
		else if(msg.pSender->GetName()==_T("opt_pre_3"))
		{
			m_preCall = 3;
			SetTimer(m_hWnd,TurnPreCallEvent,0,NULL);
		}
		else if(msg.pSender->GetName()==_T("opt_pre_4"))
		{
			m_preCall = 4;
			SetTimer(m_hWnd,TurnPreCallEvent,0,NULL);
		}
		else if(msg.pSender->GetName()==_T("opt_pre_5"))
		{
			m_preCall = 5;
			SetTimer(m_hWnd,TurnPreCallEvent,0,NULL);
		}
		else if(msg.pSender->GetName()==_T("opt_pre_6"))
		{
			m_preCall = 6;
			SetTimer(m_hWnd,TurnPreCallEvent,0,NULL);
		}
		else if(msg.pSender->GetName()==_T("opt_pre_7"))
		{
			m_preCall = 7;
			SetTimer(m_hWnd,TurnPreCallEvent,0,NULL);
		}
		else if(msg.pSender->GetName()==_T("opt_pre_8"))
		{
			m_preCall =8;
			SetTimer(m_hWnd,TurnPreCallEvent,0,NULL);
		}
	}
	else if(msg.sType==DUI_MSGTYPE_VALUECHANGED)
	{
		if(msg.pSender->GetName()==_T("slider_rate"))
		{
			CSliderUI *speedUI=static_cast<CSliderUI*>(m_PaintManager.FindControl("slider_rate"));
			m_speed = speedUI->GetValue();
		}
	}
}

/*************/

