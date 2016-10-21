#include "StdAfx.h"
#include "MainWnd.h"
#include "UI.h"
#include "MainWndCfg.h"
#include "HttpRequest.h"
#include "LoginWnd.h"
#include "ToolBarWnd.h"
#include "ReadConfig.h"
#include <sstream>

#include "Command.h"
#include "ClientSocket.h"

string										loginUserName;
string										loginPwd;


Router* Router::router=NULL;

CMainWnd* CMainWnd::pwnd=NULL;

void CMainWnd::XCmd(const char* ip,u_short port,Command cmd)//
{
	//SetTimer(h,time_id,1000,NULL);//等待指令1秒
	ClientSocket *s=new ClientSocketUDP(ip,port);
	Cmd::Init();
	char *p=(char*)cmd.getCommand();
	s->SendMsg(p,cmd.getCmdLength()+1);
	char rec[100];
	s->RecvMsg(rec,100);
	delete s;
	s=NULL;
}


CMainWnd::CMainWnd(void)
{
	videoNum=2;
	camLayoutCount=0;
	currentPage=1;
	pageCount=1;
	camLayout=NULL;
	page=NULL;
	videoURL.resize(MAX_VIDEO);
	InitThreadPool();

	map<string,string>config=ReadConfigure::Read(Login_Config);
	loginUserName=config["user"];
	loginPwd=config["password"];
	
}

CMainWnd::~CMainWnd(void)
{
	ReleaseThreadPool();
	KillTimer(*this,TIME_ID_PER);
	
}

LPCTSTR CMainWnd::GetWindowClassName()const
{
	return _T("MainWnd");
	
}

CDuiString CMainWnd::GetSkinFile()
{
	return _T("MainWnd.xml");
}
CControlUI* CMainWnd::CreateControl(LPCTSTR pstrClass)
{
	CControlUI* pReturn = NULL;
	if( _tcscmp(pstrClass, _T("CamCtrol")) == 0 )			pReturn = new CCamCtrolUI;
	else if( _tcscmp(pstrClass, _T("CamLayout")) == 0 )     pReturn = new CCamLayoutUI;
	else if( _tcscmp(pstrClass,_T("ComboList")) == 0)		pReturn = new CComboListUI;
	else if( _tcscmp(pstrClass,_T("ComboBtn")) == 0)		pReturn = new CComboBtnUI;
	else if( _tcscmp(pstrClass, _T("BtnCalendar")) == 0 )   pReturn = new CBtnCalendarUI;
	else if( _tcscmp(pstrClass, _T("OptionElement")) == 0 )   pReturn = new COptionElement;
	return pReturn;
}
void CMainWnd::InitThreadPool()
{
	tPool = CreateThreadpool(NULL);
	SetThreadpoolThreadMinimum(tPool,MIN_THREAD_POOL);
	SetThreadpoolThreadMaximum(tPool,MAX_THREAD_POOL);
	InitializeThreadpoolEnvironment(&pcbe);
	SetThreadpoolCallbackPool(&pcbe,tPool);

}
void CMainWnd::AddWorkToThreadPool(PTP_SIMPLE_CALLBACK pv,PVOID content)
{
	TrySubmitThreadpoolCallback(pv,content,&pcbe);
}
void CMainWnd::ReleaseThreadPool()
{
	DestroyThreadpoolEnvironment(&pcbe);
	CloseThreadpool(tPool);
}
CDuiString CMainWnd::GetSkinFolder()
{

	return _T("Skin");

}
LRESULT CMainWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(wParam==TIME_ID_PER)
	{
		UpdataStatus();
	}
	else if(wParam==TIME_ID_ABORT)
	{
		//KillTimer(*this,TIME_ID_PER);
		//KillTimer(*this,TIME_ID_ABORT);
	}
	else if(wParam>8000 &&wParam<9000)//用于指令
	{
		KillTimer(*this,wParam);
	}
	return 0;
}

LRESULT CMainWnd::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == VK_ESCAPE)
	{
		if(camLayout->GetFullFlag())
		OnEsc();
		return 0;
	}
	return __super::OnKeyDown(uMsg, wParam, lParam, bHandled);
	
	;
}

void CALLBACK LoadSource(PTP_CALLBACK_INSTANCE Instance,PVOID Context)
{
	_videoURL* p=static_cast<_videoURL*>(Context);
	netCode code=HttpRequest::LogIn(p->ip,p->port,p->model,loginUserName,loginPwd);
	if(code.status)
	{
		char temp[100];
		strcpy(temp,code.msg.c_str());
		p->token=string(temp);
		CDuiString sid1,sid2,sid3;
		sid1.Format("%d",&(p->ip));
		sid2.Format("%d",&(p->port));
		sid3.Format("%d",&(p->token));
		p->URL["lurl"]=CTL(HttpRequest::LookUpUrl(p->ip,p->port,p->model,p->token,"lurl",sid1.GetData()),p->ip,p->port);
		p->URL["durl"]=CTL(HttpRequest::LookUpUrl(p->ip,p->port,p->model,p->token,"durl",sid2.GetData()),p->ip,p->port);
		p->URL["sublurl"]=CTL(HttpRequest::LookUpUrl(p->ip,p->port,p->model,p->token,"sublurl",sid3.GetData()),p->ip,p->port);
		p->URL["play"]=p->URL["lurl"];

	}
	OutputDebugStringA(p->URL["lurl"].c_str());

}
void CALLBACK CtrlDirLeft(PTP_CALLBACK_INSTANCE Instance,PVOID Context)
{
	CRITICAL_SECTION sec;
	InitializeCriticalSection(&sec);
	EnterCriticalSection(&sec);
	Router* rt=Router::getRouter();
	stringstream sStream;
	string args;
	sStream<<"&speed="<<rt->speed<<"&view="<<rt->view;
	sStream>>args;
	sStream.clear();
	HttpRequest::DirControl(rt->vl.ip,rt->vl.port,rt->vl.model,rt->vl.token,LEFT,args);
	LeaveCriticalSection(&sec);
	DeleteCriticalSection(&sec);

}
void CALLBACK CtrlDirRight(PTP_CALLBACK_INSTANCE Instance,PVOID Context)
{
	CRITICAL_SECTION sec;
	InitializeCriticalSection(&sec);
	EnterCriticalSection(&sec);
	Router* rt=Router::getRouter();
	stringstream sStream;
	string args;
	sStream<<"&speed="<<rt->speed<<"&view="<<rt->view;
	sStream>>args;
	sStream.clear();
	HttpRequest::DirControl(rt->vl.ip,rt->vl.port,rt->vl.model,rt->vl.token,RIGHT,args);
	LeaveCriticalSection(&sec);
	DeleteCriticalSection(&sec);

}
void CALLBACK CtrlDirUp(PTP_CALLBACK_INSTANCE Instance,PVOID Context)
{
	CRITICAL_SECTION sec;
	InitializeCriticalSection(&sec);
	EnterCriticalSection(&sec);
	Router* rt=Router::getRouter();
	stringstream sStream;
	string args;
	sStream<<"&speed="<<rt->speed<<"&view="<<rt->view;
	sStream>>args;
	sStream.clear();
	HttpRequest::DirControl(rt->vl.ip,rt->vl.port,rt->vl.model,rt->vl.token,UP,args);
	LeaveCriticalSection(&sec);
	DeleteCriticalSection(&sec);
}
void CALLBACK CtrlDirDown(PTP_CALLBACK_INSTANCE Instance,PVOID Context)
{
	CRITICAL_SECTION sec;
	InitializeCriticalSection(&sec);
	EnterCriticalSection(&sec);
	Router* rt=Router::getRouter();
	stringstream sStream;
	string args;
	sStream<<"&speed="<<rt->speed<<"&view="<<rt->view;
	sStream>>args;
	sStream.clear();
	HttpRequest::DirControl(rt->vl.ip,rt->vl.port,rt->vl.model,rt->vl.token,DOWN,args);
	LeaveCriticalSection(&sec);
	DeleteCriticalSection(&sec);

}
void CALLBACK CtrlDirStop(PTP_CALLBACK_INSTANCE Instance,PVOID Context)
{
	CRITICAL_SECTION sec;
	InitializeCriticalSection(&sec);
	EnterCriticalSection(&sec);
	Router* rt=Router::getRouter();
	stringstream sStream;
	string args;
	sStream<<"&speed="<<rt->speed<<"&view="<<rt->view;
	sStream>>args;
	sStream.clear();
	HttpRequest::DirControl(rt->vl.ip,rt->vl.port,rt->vl.model,rt->vl.token,STOP,args);
	LeaveCriticalSection(&sec);
	DeleteCriticalSection(&sec);

}
void CALLBACK CtrlDirReset(PTP_CALLBACK_INSTANCE Instance,PVOID Context)
{
	CRITICAL_SECTION sec;
	InitializeCriticalSection(&sec);
	EnterCriticalSection(&sec);
	Router* rt=Router::getRouter();
	stringstream sStream;
	string args;
	sStream<<"&speed="<<rt->speed<<"&view="<<rt->view;
	sStream>>args;
	sStream.clear();
	HttpRequest::DirControl(rt->vl.ip,rt->vl.port,rt->vl.model,rt->vl.token,RESET,args);
	LeaveCriticalSection(&sec);
	DeleteCriticalSection(&sec);

}
void CALLBACK CtrlZoomUp(PTP_CALLBACK_INSTANCE Instance,PVOID Context)
{
	Router* rt=Router::getRouter();
	stringstream sStream;
	string args;
	sStream<<"&view="<<rt->view;
	sStream>>args;
	sStream.clear();
	HttpRequest::ZoomControl(rt->vl.ip,rt->vl.port,rt->vl.model,rt->vl.token,ZOOMUP,args);

}
void CALLBACK CtrlZoomDown(PTP_CALLBACK_INSTANCE Instance,PVOID Context)
{
	Router* rt=Router::getRouter();
	stringstream sStream;
	string args;
	sStream<<"&view="<<rt->view;
	sStream>>args;
	sStream.clear();
	HttpRequest::ZoomControl(rt->vl.ip,rt->vl.port,rt->vl.model,rt->vl.token,ZOOMDOWN,args);

}
void CALLBACK CtrlPlace(PTP_CALLBACK_INSTANCE Instance,PVOID Context)
{
	Router* rt=Router::getRouter();
	stringstream sStream;
	string args;
	char *plc=static_cast<char*>(Context);
	sStream<<"&view="<<rt->view<<plc;
	sStream>>args;
	sStream.clear();
	HttpRequest::PlaceControl(rt->vl.ip,rt->vl.port,rt->vl.model,rt->vl.token,args);

}
void CALLBACK CtrlSwitch(PTP_CALLBACK_INSTANCE Instance,PVOID Context)
{
	Router* rt=Router::getRouter();
	HttpRequest::SetView(rt->vl.ip,rt->vl.port,rt->vl.model,rt->vl.token,rt->view);
}


void CMainWnd::Init()
{
	camLayout=static_cast<CCamLayoutUI*>(m_PaintManager.FindControl(_T("camlayout")));
	ASSERT(camLayout);

	page = (CTextUI*)m_PaintManager.FindControl(_T("text_page"));
	page->SetTextStyle(DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	
	map<string,string>m=ReadConfigure::Read("../bin/config/netconfig.cfg");
	HttpRequest::embedModel=m["embed"];
	HttpRequest::iMacModel=m["iMac"];
	HttpRequest::lgCould=m["cloud"];
	UpdateClassRoom();
	for(int i=0;i<NUM_OF_VIDEO;i++)
	{
		stringstream s;
		s<<i;
		string str;
		s>>str;
		s.clear();
		camLayout->Add(str.c_str());
	}

	camLayout->GetItem("0")->m_bSelect =true;
	Router *router=Router::getRouter();
	router->mediaPlayer=camLayout->GetItem("0")->player;
	router->camCtrl=camLayout->GetItem("0");
	Assign(router->vl,camLayout->GetItem("0")->videosrc);
	preplc=1;
	
}

void CMainWnd::ShowMessage(CDuiString sContext,bool bModel,bool bAutoClose,int iTime,bool bNeedBtnCancle)
{
	CMessageWnd* pWnd = new CMessageWnd(sContext,bAutoClose,iTime,bNeedBtnCancle);
	pWnd->Create(m_hWnd, _T("MmrpsScore"), UI_WNDSTYLE_DIALOG, 0L, 0, 0, 0, 0);
	pWnd->CenterWindow();
	if(bModel)
		pWnd->ShowModal();
}
void CMainWnd::OnBtnClose(TNotifyUI& msg)
{
	Close(IDOK);
}

void CMainWnd::OnBtnMin(TNotifyUI& msg)
{
	::ShowWindow(m_hWnd, SW_MINIMIZE);
}

void CMainWnd::OnBtnRestroe(TNotifyUI& msg)
{
	::ShowWindow(m_hWnd, SW_RESTORE);
}
void CMainWnd::OnBtnMax(TNotifyUI& msg)
{
	::ShowWindow(m_hWnd, SW_MAXIMIZE);
}

void CMainWnd::OnBtnNO_1(TNotifyUI& msg)
{
	if(videoNum == 1)
		return;
	if(camLayout && camLayout->GetCount())
	{
		camLayoutCount = 1;
		camLayout->SetPosCount(camLayoutCount);
		if (msg.wParam == 0 && msg.lParam == 0)
		{
			camLayout->SetPos1(camLayout->GetPos(),0,true);
			currentPage = 1;
			UpdateTextPage(1,true);
		}
		else
		{
			camLayout->SetPos1(camLayout->GetPos(),msg.wParam,false);
			currentPage = msg.lParam;
			UpdateTextPage(msg.lParam,true);
		}

		UpdateBtnImage(1,videoNum);
		videoNum = 1;
	}
}

void CMainWnd::OnBtnNO_4(TNotifyUI& msg)
{
	if(videoNum == 4)
		return;
	if(camLayout && camLayout->GetCount())
	{
		camLayoutCount = 4;
		currentPage = 1;
		camLayout->SetPosCount(camLayoutCount);
		camLayout->SetPos4(camLayout->GetPos(),0,false);

		UpdateTextPage(1,true);
		UpdateBtnImage(4,videoNum);
		videoNum = 4;
	}
}
void CMainWnd::OnBtnNO_9(TNotifyUI& msg)
{
	if(videoNum == 9)
		return;
	if(camLayout && camLayout->GetCount())
	{
		camLayoutCount = 9;
		currentPage = 1;
		camLayout->SetPosCount(camLayoutCount);
		camLayout->SetPos9(camLayout->GetPos(),0,false);

		UpdateTextPage(1,true);
		UpdateBtnImage(9,videoNum);
		videoNum = 9;
	}
}

void CMainWnd::OnBtnNO_16(TNotifyUI& msg)
{
	if(videoNum == 16)
		return;
	if(camLayout && camLayout->GetCount())
	{
		camLayoutCount = 16;
		currentPage = 1;
		camLayout->SetPosCount(camLayoutCount);
		camLayout->SetPos16(camLayout->GetPos(),0,false);

		UpdateTextPage(1,true);
		UpdateBtnImage(16,videoNum);
		videoNum = 16;
	}
}

void CMainWnd::OnBtnHideList(TNotifyUI& msg)
{
	CControlUI* plist = m_PaintManager.FindControl(VER_LIST);
	if(plist)
		plist->SetVisible(false);

	CButtonUI* pBtn = (CButtonUI*)m_PaintManager.FindControl(BTN_SHOW);
	if(pBtn)
		pBtn->SetVisible(true);
	msg.pSender->SetVisible(false);
}
void CMainWnd::OnBtnShowList(TNotifyUI& msg)
{
	CControlUI* plist = m_PaintManager.FindControl(VER_LIST);
	if(plist)
		plist->SetVisible(true);

	CButtonUI* pBtn = (CButtonUI*)m_PaintManager.FindControl(BTN_HIDE);
	if(pBtn)
		pBtn->SetVisible(true);
	msg.pSender->SetVisible(false);
}
void CMainWnd::OnBtnFull(TNotifyUI& msg)
{
	if(camLayout->GetCount() == 0)
	{
		ShowMessage("尚无视频播放",false,true,2000,false);
		return;
	}
	ShowHideLayout(false);
	RECT m_rcCaption = m_PaintManager.GetCaptionRect();
	m_PaintManager.SetCaptionRect(m_rcCaption);
	if(!::IsZoomed(m_hWnd))
		::GetWindowRect(m_hWnd,&windowsRect);
	else
	{
		CButtonUI* pBtnMax = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_max")));
		pBtnMax->SetVisible(true);
		CButtonUI* pBtnRestore = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_restore")));
		pBtnRestore->SetVisible(false);
	}
	::SetParent(m_hWnd,::GetDesktopWindow());
	int screenX = GetSystemMetrics(SM_CXSCREEN);
	int screenY = GetSystemMetrics(SM_CYSCREEN);
	MoveWindow(m_hWnd,0, 0, screenX, screenY,TRUE);
	SetWindowPos(m_hWnd,HWND_NOTOPMOST,0,0,screenX,screenY,SWP_FRAMECHANGED|SWP_DEFERERASE);
	//m_bTopMost = false;
	//if(m_pToolBar)
	//	m_pToolBar->ShowWindow(false);
	camLayout->SetFullFlag(true);
}

void CMainWnd::OnEsc()
{
	//m_PaintManager.SetCaptionRect(m_rcCaption);
	ShowHideLayout(true);
	SetWindowPos(m_hWnd,HWND_NOTOPMOST,windowsRect.left,windowsRect.top,windowsRect.right-windowsRect.left,windowsRect.bottom-windowsRect.top,SWP_SHOWWINDOW);
	camLayout->SetFullFlag(false);
}

void CMainWnd::OnBtnPageUp(TNotifyUI& msg)
{
	CDuiString sPage = page->GetText();
	if (sPage.IsEmpty())
		return;
	if(currentPage == 1)
	{
		return;
	}
	--currentPage;
	UpdateTextPage(currentPage,false);
	/////////////////////////////////////////////////////////
	switch (camLayoutCount)
	{
	case 1:
		camLayout->SetPos1(camLayout->GetPos(),(currentPage-1)*camLayoutCount,false);
		break;
	case 4:
		camLayout->SetPos4(camLayout->GetPos(),(currentPage-1)*camLayoutCount,false);
		break;
	case 9:
		camLayout->SetPos9(camLayout->GetPos(),(currentPage-1)*camLayoutCount,false);
		break;
	case  16:
		camLayout->SetPos16(camLayout->GetPos(),(currentPage-1)*camLayoutCount,false);
		break;
	default:
		camLayout->SetPos16(camLayout->GetPos(),(currentPage-1)*16,false);
	}
}
void CMainWnd::OnBtnPageDown(TNotifyUI& msg)
{
	CDuiString sPage = page->GetText();
	if (sPage.IsEmpty())
		return;
	switch (camLayoutCount)
	{
	case 1:
		camLayout->SetPos1(camLayout->GetPos(),currentPage*camLayoutCount,false);
		break;
	case 4:
		camLayout->SetPos4(camLayout->GetPos(),currentPage*camLayoutCount,false);
		break;
	case 9:
		camLayout->SetPos9(camLayout->GetPos(),currentPage*camLayoutCount,false);
		break;
	case  16:
		camLayout->SetPos16(camLayout->GetPos(),currentPage*camLayoutCount,false);
		break;
	default:
		camLayout->SetPos16(camLayout->GetPos(),currentPage*16,false);
	}
	/////////////////////////////////////////////////////////
	if(currentPage == pageCount)
	{
		return;
	}
	++currentPage;
	UpdateTextPage(currentPage,false);
}
void CMainWnd::ShowHideLayout(bool bShow)
{
	CContainerUI* pCon = (CContainerUI*)m_PaintManager.FindControl(_T("hor_caption"));
	if(pCon)
		pCon->SetVisible(bShow);

	pCon = (CContainerUI*)m_PaintManager.FindControl(_T("hor_search"));
	if(pCon)
		pCon->SetVisible(bShow);

	pCon = (CContainerUI*)m_PaintManager.FindControl(_T("hor_left"));
	if(pCon)
		pCon->SetVisible(bShow);

	pCon = (CContainerUI*)m_PaintManager.FindControl(_T("hor_right"));
	if(pCon)
		pCon->SetVisible(bShow);

	pCon = (CContainerUI*)m_PaintManager.FindControl(_T("hor_top"));
	if(pCon)
		pCon->SetVisible(bShow);

	pCon = (CContainerUI*)m_PaintManager.FindControl(_T("hor_bottom"));
	if(pCon)
		pCon->SetVisible(bShow);

	pCon = (CContainerUI*)m_PaintManager.FindControl(_T("hor_update"));
	if(pCon)
		pCon->SetVisible(bShow);

	/*pCon = (CContainerUI*)paint_manager_.FindControl(_T("ver_button"));
	if(pCon)
		pCon->SetVisible(bShow);*/

	pCon = (CContainerUI*)m_PaintManager.FindControl(_T("ver_list"));
	if(pCon)
	{
		pCon->SetVisible(bShow);
		CButtonUI* pBtn = (CButtonUI*)m_PaintManager.FindControl(_T("btn_hide_list"));
		if(pBtn)
			pBtn->SetVisible(bShow);
		pBtn = (CButtonUI*)m_PaintManager.FindControl(_T("btn_show_list"));
		if(pBtn)
			pBtn->SetVisible(!bShow);
	}
}


void CMainWnd::ChangeTabSelect(int iParent,int iSub)
{
	CTabLayoutUI* pTab = (CTabLayoutUI*)m_PaintManager.FindControl(_T("tab_list_content"));
	if(pTab)
		pTab->SelectItem(iParent);
	pTab = (CTabLayoutUI*)m_PaintManager.FindControl(_T("tab_list"));
	if(pTab)
		pTab->SelectItem(iSub);
	CComboListUI* pTemp = (CComboListUI*)m_PaintManager.FindControl(_T("combo_content"));
	if (pTemp)
	{
		CDuiString image;
		image.Format("%s%d%s","list_0",iSub+1,".png");
		pTemp->m_sImage = image;
	}

	/*if(iSub == 6)
	{
		time_t timer;
		struct tm *tblock;
		timer = time(NULL);
		tblock = localtime(&timer);
		CStdString sTime;
		sTime.Format("%d-%02d-%02d",tblock->tm_year+1900,tblock->tm_mon+1,tblock->tm_mday);
		CControlUI* pTemp = paint_manager_.FindControl(_T("btn_calendar"));
		pTemp->SetText(sTime);
		UpdateTimeTable(sTime,m_sContrlID);
	}*/
}


void CMainWnd::OnCtrlPanelChange(TNotifyUI& msg)
{
		if(msg.pSender->GetName()==BTN_CTRL_UP)
		{
			OnBtnUp(msg);
		}
		else if(msg.pSender->GetName()==BTN_CTRL_DOWN)
		{
			OnBtnDown(msg);
		}
		else if(msg.pSender->GetName()==BTN_CTRL_LEFT)
		{
			OnBtnLeft(msg);
		}
		else if(msg.pSender->GetName()==BTN_CTRL_RIGHT)
		{
			OnBtnRight(msg);
		}
		else if(msg.pSender->GetName()==BTN_CTRL_HOME)
		{
			OnBtnReSet(msg);
		}
		else if(msg.pSender->GetName()==BTN_CTRL_ZOOMUP)
		{
			OnBtnZoomUp(msg);
		}
		else if(msg.pSender->GetName()==BTN_CTRL_ZOOMDOWN)
		{
			OnBtnZoomDown(msg);
		}

	else if(msg.sType==DUI_MSGTYPE_VALUECHANGED)
	{
		if(msg.pSender->GetName()==SLD_CTRL_SPEED)
		{
			OnSpeedChange(msg);
		}
	}//end value change
}

void CMainWnd::OnBtnLeft(TNotifyUI& msg)
{
	if(msg.sType==DUI_MSGTYPE_CLICK)
	{
		AddWorkToThreadPool(CtrlDirLeft,NULL);
	}
	if(msg.sType==DUI_MSGTYPE_CLICKUP)
	{
		AddWorkToThreadPool(CtrlDirStop,NULL);
	}
}

void CMainWnd::OnBtnRight(TNotifyUI& msg)
{
	if(msg.sType==DUI_MSGTYPE_CLICK)
	{
		AddWorkToThreadPool(CtrlDirRight,NULL);
	}
	if(msg.sType==DUI_MSGTYPE_CLICKUP)
	{
		AddWorkToThreadPool(CtrlDirStop,NULL);
	}
}
void CMainWnd::OnBtnUp(TNotifyUI& msg)
{
	if(msg.sType==DUI_MSGTYPE_CLICK)
	{
		AddWorkToThreadPool(CtrlDirUp,NULL);
	}
	if(msg.sType==DUI_MSGTYPE_CLICKUP)
	{
		AddWorkToThreadPool(CtrlDirStop,NULL);
	}
}
void CMainWnd::OnBtnDown(TNotifyUI& msg)
{
	if(msg.sType==DUI_MSGTYPE_CLICK)
	{
		AddWorkToThreadPool(CtrlDirDown,NULL);
	}
	if(msg.sType==DUI_MSGTYPE_CLICKUP)
	{
		AddWorkToThreadPool(CtrlDirStop,NULL);
	}
}
void CMainWnd::OnBtnReSet(TNotifyUI& msg)
{
	if(msg.sType==DUI_MSGTYPE_CLICK)
	{
		AddWorkToThreadPool(CtrlDirReset,NULL);
	}
}

void CMainWnd::OnBtnZoomUp(TNotifyUI& msg)
{
	if(msg.sType==DUI_MSGTYPE_CLICK)
	{
		AddWorkToThreadPool(CtrlZoomUp,NULL);
	}
	if(msg.sType==DUI_MSGTYPE_CLICKUP)
	{
		AddWorkToThreadPool(CtrlDirStop,NULL);
	}
}

void CMainWnd::OnBtnZoomDown(TNotifyUI& msg)
{
	if(msg.sType==DUI_MSGTYPE_CLICK)
	{
		AddWorkToThreadPool(CtrlZoomDown,NULL);
	}
	if(msg.sType==DUI_MSGTYPE_CLICKUP)
	{
		AddWorkToThreadPool(CtrlDirStop,NULL);
	}
}

void CMainWnd::OnSpeedChange(TNotifyUI& msg)
{
		Router* rt=Router::getRouter();
		CSliderUI *sld=static_cast<CSliderUI*>(m_PaintManager.FindControl(SLD_CTRL_SPEED));
		rt->speed=sld->GetValue();
		char s[100];
		sprintf(s,"speed:%d\n",sld->GetValue());
		OutputDebugStringA(s);

}
void CMainWnd::OnSelectRoomItem(TNotifyUI& msg)
{
	if(msg.pSender->GetName()==FILE_LIST)
	{
		CFileListUI *flist=static_cast<CFileListUI*>(m_PaintManager.FindControl(FILE_LIST));
		
		
		int curPoint=0;
		int curRoot=0;//当前根
		Node *Root=flist->GetRoot();
		int subRootCount=Root->num_children();
		int curSelected=flist->GetCurSel();
		for(int i=0;i<subRootCount;i++)
		{
			Node *subNode=Root->child(i);//获取子根节点
				++curPoint;
				for(int j=0;j<subNode->num_children();j++)
				{
					if(curPoint==curSelected)
					{
						Node *curNode=subNode->child(j);
						int curIndex=curPoint-curRoot-1;
						Router *rt=Router::getRouter();
						CSliderUI *sld=static_cast<CSliderUI*>(m_PaintManager.FindControl(SLD_CTRL_SPEED));
						rt->speed=sld->GetValue();
						// HACK : when view has complete need update
						rt->view=0;
						if(rt->camCtrl)
						{
							CDuiString s=curNode->data()._text;
							s=s.Left(s.Find('('));
							rt->camCtrl->SetText(s);
						}
						Assign(rt->vl,videoURL[curIndex]);
						Assign(rt->camCtrl->videosrc,videoURL[curIndex]);
						if(rt->mediaPlayer)
						{
							rt->mediaPlayer->stop();
							rt->mediaPlayer->play(rt->vl.URL["play"]);

						}
						
					}
					++curPoint;
				}
				++curRoot;
		}
		
	}
}
void CMainWnd::OnSelectPlayStream(TNotifyUI& msg)
{
	// 播放流的切换
	int count_video=camLayout->GetCount();
	for(int i=0;i<count_video;i++)
	{
		CDuiString sId;
		sId.Format("%d",i);
		if(msg.pSender==camLayout->GetItem(sId)->menu)
		{
			if(camLayout->GetItem(sId)->menu->GetText()==DURL)
			{
				camLayout->GetItem(sId)->videosrc.URL["play"]=camLayout->GetItem(sId)->videosrc.URL["durl"];
			}
			else if(camLayout->GetItem(sId)->menu->GetText()==LURL)
			{
				camLayout->GetItem(sId)->videosrc.URL["play"]=camLayout->GetItem(sId)->videosrc.URL["lurl"];
			}
			else if(camLayout->GetItem(sId)->menu->GetText()==SUBURL)
			{
			camLayout->GetItem(sId)->videosrc.URL["play"]=camLayout->GetItem(sId)->videosrc.URL["sublurl"];
			}	
			camLayout->GetItem(sId)->player->play(camLayout->GetItem(sId)->videosrc.URL["play"]);
		}
	}
}

void CMainWnd::OnSelectView(TNotifyUI &msg)
{
	if(msg.pSender->GetName()=="btn_view_1" ||msg.pSender->GetName()=="btn_view_2"||
		msg.pSender->GetName()=="btn_view_3"||msg.pSender->GetName()=="btn_view_4"||
		msg.pSender->GetName()=="btn_view_5"||msg.pSender->GetName()=="btn_view_6")
	{
		CTextUI *txt=static_cast<CTextUI*>(m_PaintManager.FindControl("sel_view"));
		Router *rt=Router::getRouter();
		rt->view=atoi(msg.pSender->GetUserData().GetData());
		rt->view-=1;
		txt->SetText(msg.pSender->GetUserData());
	}
}

void CMainWnd::OnBtnSwitch(TNotifyUI &msg)
{
	if(msg.pSender->GetName()=="btn_switch")
	{
		AddWorkToThreadPool(CtrlSwitch,NULL);
	}
}

void CMainWnd::OnPrePlace(TNotifyUI &msg)
{
		
		if(msg.sType==DUI_MSGTYPE_SELECTCHANGED)
		{
			if(msg.pSender->GetName()=="opt_pre_1")
			{
				char p[100]="&precall=1";
				preplc=1;
				AddWorkToThreadPool(CtrlPlace,p);
			}
			else if(msg.pSender->GetName()=="opt_pre_2")
			{
				char p[100]="&precall=2";
				preplc=2;
				AddWorkToThreadPool(CtrlPlace,p);
			}
			else if(msg.pSender->GetName()=="opt_pre_3")
			{
				char p[100]="&precall=3";
				preplc=3;
				AddWorkToThreadPool(CtrlPlace,p);
			}
			else if(msg.pSender->GetName()=="opt_pre_4")
			{
				char p[100]="&precall=4";
				preplc=4;
				AddWorkToThreadPool(CtrlPlace,p);
			}
			else if(msg.pSender->GetName()=="opt_pre_5")
			{
				char p[100]="&precall=5";
				preplc=5;
				AddWorkToThreadPool(CtrlPlace,p);
			}
			else if(msg.pSender->GetName()=="opt_pre_6")
			{
				char p[100]="&precall=6";
				preplc=6;
				AddWorkToThreadPool(CtrlPlace,p);
			}
			else if(msg.pSender->GetName()=="opt_pre_7")
			{
				char p[100]="&precall=7";
				preplc=7;
				AddWorkToThreadPool(CtrlPlace,p);
			}
			else if(msg.pSender->GetName()=="opt_pre_8")
			{
				char p[100]="&precall=8";
				preplc=8;
				AddWorkToThreadPool(CtrlPlace,p);
			}
		}
		if(msg.sType==DUI_MSGTYPE_CLICK)
		{
			if(msg.pSender->GetName()=="btn_pre_set")
			{
				char p[100];
				sprintf(p,"&preset=%d",preplc);
				char q[100];
				strcpy(q,p);
				AddWorkToThreadPool(CtrlPlace,q);
			}
			
		}


}
void CMainWnd::OnEnvironmentCtrl(TNotifyUI& msg)
{
	if(msg.pSender->GetName()=="opt_total_light")//总灯光
	{

	}
	else if(msg.pSender->GetName()=="opt_white_light")//白板灯光
	{

	}
	else if(msg.pSender->GetName()=="	opt_tow_light")//二排灯光
	{

	}
	else if(msg.pSender->GetName()=="opt_last_light")// 后排灯光
	{

	}
	else if(msg.pSender->GetName()=="opt_gobal")//全局窗帘
	{

	}
}
void CMainWnd::OnProjectorCtrl(TNotifyUI& msg)
{
	if(msg.pSender->GetName()=="opt_system")//系统开关
	{
		
	}
	else if(msg.pSender->GetName()=="opt_touyin")//投影开关
	{
		ClientSocket *s=new ClientSocketUDP("127.0.0.1",3200);
		Cmd::Init();//once
		char *p=(char*)Cmd::ProjectorOpen.getCommand();
		s->SendMsg(p,Cmd::ProjectorOpen.getCmdLength()+1);
		char rec[100];
		s->RecvMsg(rec,100);
		//MessageBoxA(0,rec,0,0);
		delete s;
		s=NULL;
	}
	/*电动幕*/
	else if(msg.pSender->GetName()=="opt_up")//屏升
	{

	}
	else if(msg.pSender->GetName()=="opt_stop")//停止
	{

	}
	else if(msg.pSender->GetName()=="opt_down")//屏降
	{

	}
	/*信号切换*/
	else if(msg.pSender->GetName()=="opt_destop")//台式
	{

	}
	else if(msg.pSender->GetName()=="opt_notebook")//笔记本
	{

	}
	else if(msg.pSender->GetName()=="opt_show")//展示台
	{

	}
	else if(msg.pSender->GetName()=="opt_waijie")//外接视频
	{

	}
}
void CMainWnd::OnDoorCtrl(TNotifyUI& msg)
{
	 if(msg.pSender->GetName()=="opt_front_door")//前门
	 {

	 }
	 else if(msg.pSender->GetName()=="opt_middle_door")//中门
	 {

	 }
	 else if(msg.pSender->GetName()=="opt_last_door")//后门
	 {

	 }
}
void CMainWnd::OnSoundCtrl(TNotifyUI& msg)
{
	if(msg.pSender->GetName()=="slider_total_voice")//总音量
	{
	
	}
	else if(msg.pSender->GetName()=="slider_taishi_voice")//台式音量
	{

	}
	else if(msg.pSender->GetName()=="slider_shouti_voice")//手提音量
	{

	}
	else if(msg.pSender->GetName()=="slider_waijie_voice")//外接音量
	{

	}
	else if(msg.pSender->GetName()=="slider_mai_voice")//麦克风音量
	{

	}
}
void CMainWnd::Notify(TNotifyUI& msg)
{
	OutputDebugStringA(msg.sType);
	OnCtrlPanelChange(msg);//控制面板
	OnSelectRoomItem(msg);//教室的选择
	OnPrePlace(msg);//预置位
	static bool exe=false;
	if(!exe)
	{
		OnBtnNO_4(msg);
		exe=true;
	}
	if(msg.sType==DUI_MSGTYPE_CLICK)
	{
		OnSelectView(msg);
		OnBtnSwitch(msg);
		if(msg.pSender->GetName()==BTN_CLOSE)
		{
			OnBtnClose(msg);
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
		else if(msg.pSender->GetName()==BTN_LB_CTRL)
		{
			ChangeTabSelect(1,4);
		}
		else if(msg.pSender->GetName()==BTN_HOME)
		{
			ChangeTabSelect(0,-1);
		}
		else if(msg.pSender->GetName()==BTN_CLASSROOM)
		{
			ChangeTabSelect(1,0);
		}
		else if (msg.pSender->GetName() == _T("btn_touyin"))
			ChangeTabSelect(1,1);
		else if (msg.pSender->GetName() == _T("btn_environment"))
			ChangeTabSelect(1,2);
		else if (msg.pSender->GetName() == _T("btn_voice_ctrl"))
			ChangeTabSelect(1,3);
		else if (msg.pSender->GetName() == _T("btn_record_ctrl"))
			ChangeTabSelect(1,4);
		else if (msg.pSender->GetName() == _T("btn_door_ctrl"))
			ChangeTabSelect(1,5);
		else if (msg.pSender->GetName() == _T("btn_time_table"))
			ChangeTabSelect(1,6);
		else if(msg.pSender->GetName()==BTN_NO_1)
		{
			OnBtnNO_1(msg);
		}
		else if(msg.pSender->GetName()==BTN_NO_4)
		{
			OnBtnNO_4(msg);
		}
		else if(msg.pSender->GetName()==BTN_NO_9)
		{
			OnBtnNO_9(msg);
		}
		else if(msg.pSender->GetName()==BTN_NO_16)
		{
			OnBtnNO_16(msg);
		}
		else if(msg.pSender->GetName()==BTN_HIDE)
		{
			OnBtnHideList(msg);
		}
		else if(msg.pSender->GetName()==BTN_SHOW)
		{
			OnBtnShowList(msg);
		}
		else if(msg.pSender->GetName()==BTN_FULL)
		{
			OnBtnFull(msg);
		}
		else if(msg.pSender->GetName()==BTN_PAGEUP)
		{
			OnBtnPageUp(msg);
		}
		else if(msg.pSender->GetName()==BTN_PAGEDOWN)
		{
			OnBtnPageDown(msg);
		}
	}//end click

	if(msg.sType==DUI_MSGTYPE_ITEMSELECT)
	{
		OutputDebugStringA("-------\n");
	}

	if(msg.sType==DUI_MSGTYPE_ITEMSELECT)
	{
		OnSelectPlayStream(msg);
	}
	if(msg.sType==DUI_MSGTYPE_SELECTCHANGED)
	{
		OnEnvironmentCtrl(msg);
		OnProjectorCtrl(msg);
		OnDoorCtrl(msg);
	}

	if(msg.sType==DUI_MSGTYPE_VALUECHANGED)
	{
		OnSoundCtrl(msg);
	}

}


void CMainWnd::UpdateTextPage(int iPage,bool bUpdatePageCount)
{
	if (bUpdatePageCount)
	{
		if(camLayoutCount == 0)
		{
			if (16>= camLayout->GetCount())
				camLayoutCount = camLayout->GetPosCount();
			else
			{
				pageCount = (camLayout->GetCount()%16);
				if(pageCount)
					pageCount = camLayout->GetCount()/16 +1;
				else
					pageCount = camLayout->GetCount()/16;
			}
		}
		else if(camLayoutCount == 1)
			pageCount = camLayout->GetCount();
		else
		{
			pageCount = (camLayout->GetCount()%camLayoutCount);
			if(pageCount)
				pageCount = camLayout->GetCount()/camLayoutCount +1;
			else
				pageCount = camLayout->GetCount()/camLayoutCount;
		}
	}

	CDuiString text;
	if(pageCount <= 0)
		text = "";
	else
	{
		if(iPage > pageCount)
			iPage = pageCount;
		text.Format("%d/%d",iPage,pageCount);
	}
	page->SetText(text);
}


void CMainWnd::UpdateBtnImage(int iNew,int iOld)
{
	CButtonUI* pTemp = NULL;
	CDuiString image;
	if (iNew == 1 || iOld == 1)
	{
		pTemp = (CButtonUI*)m_PaintManager.FindControl(_T("btn_1"));
		image = pTemp->GetNormalImage();
		if (image == _T("btn_1_01.png"))
		{
			pTemp->SetNormalImage("btn_1_03.png");
			pTemp->SetHotImage("btn_1_04.png");
		}
		else
		{
			pTemp->SetNormalImage("btn_1_01.png");
			pTemp->SetHotImage("btn_1_02.png");
		}
	}
	//////////////////////////////////////////////////////////////////////
	if (iNew == 4 || iOld == 4)
	{
		pTemp = (CButtonUI*)m_PaintManager.FindControl(_T("btn_4"));
		image = pTemp->GetNormalImage();
		if (image == _T("btn_4_01.png"))
		{
			pTemp->SetNormalImage("btn_4_03.png");
			pTemp->SetHotImage("btn_4_04.png");
		}
		else
		{
			pTemp->SetNormalImage("btn_4_01.png");
			pTemp->SetHotImage("btn_4_02.png");
		}
	}
	//////////////////////////////////////////////////////////////////////
	if (iNew == 9 || iOld == 9)
	{
		pTemp = (CButtonUI*)m_PaintManager.FindControl(_T("btn_9"));
		image = pTemp->GetNormalImage();
		if (image == _T("btn_9_01.png"))
		{
			pTemp->SetNormalImage("btn_9_03.png");
			pTemp->SetHotImage("btn_9_04.png");
		}
		else
		{
			pTemp->SetNormalImage("btn_9_01.png");
			pTemp->SetHotImage("btn_9_02.png");
		}
	}
	//////////////////////////////////////////////////////////////////////
	if (iNew == 16 || iOld == 16)
	{
		pTemp = (CButtonUI*)m_PaintManager.FindControl(_T("btn_16"));
		image = pTemp->GetNormalImage();
		if (image == _T("btn_16_01.png"))
		{
			pTemp->SetNormalImage("btn_16_03.png");
			pTemp->SetHotImage("btn_16_04.png");
		}
		else
		{
			pTemp->SetNormalImage("btn_16_01.png");
			pTemp->SetHotImage("btn_16_02.png");
		}
	}
}

void CMainWnd::UpdateToolBar()
{
	if (camLayout)
	{
		if(camLayout->GetCount() == 0)
		{
			return;
		}
		CCamCtrolUI* p = camLayout->GetFullScreenItem();
		if(p)
		{
			toolBarWnd->ShowWindow(true);
			return;
		}

		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(m_hWnd,&pt);
		RECT rcLay = camLayout->GetPos();
		::InflateRect(&rcLay, -2, -2);
		rcLay.top += 30;
		if (::PtInRect(&rcLay,pt))
		{
			CCamCtrolUI* pCam = (CCamCtrolUI*)m_PaintManager.FindControl(pt);
			if (pCam->GetName() == _T("camlayout"))
			{
				if (toolBarWnd)
					toolBarWnd->ShowWindow(false);
				return;
			}
			if (pCam != NULL && pCam->GetHasWndFlag())
			{
				RECT rcPos = pCam->GetPos();
				rcPos.bottom = rcPos.top + 30;
				if (PtInRect(&rcPos,pt))
				{
					if(toolBarWnd)
						toolBarWnd->ShowWindow(false);
					return;
				}
				rcPos = pCam->GetPos();
				rcPos.left += 3;
				rcPos.bottom -= 2;
				if(!toolBarWnd)
				{
					toolBarWnd = new CToolBarWnd(this);
					toolBarWnd->Create(pCam->m_pWndParent->GetHWND(),"",UI_WNDSTYLE_CHILD, 0L, 0, 0, 0, 0);
				}
				toolBarWnd->SetOwnCam(pCam);
				RECT rcTool;
				GetWindowRect(toolBarWnd->GetHWND(),&rcTool);

				::SetParent(toolBarWnd->GetHWND(),m_hWnd);
				::SetWindowPos(toolBarWnd->GetHWND(),HWND_TOP,rcPos.left,rcPos.top,0,0,SWP_SHOWWINDOW|SWP_NOSIZE);	

				if(pCam->GetSelect() == 0)
					toolBarWnd->SetBtnPipVisual(false,true);
				else 
					toolBarWnd->SetBtnPipVisual(true,false);

				return;
			}
		}
		else if (toolBarWnd)
		{
			toolBarWnd->ShowWindow(false);
		}
	}
}
void CMainWnd::UpdateClassRoom()
{
	CFG *cfg=CFG::GetConfig();
	queue<Data_LG>roomInfo=HttpRequest::LookUpClassRoom(cfg->ip,cfg->port,cloud,cfg->token,"&page=0&size=100","init_room");

	CFileListUI* pFileList = (CFileListUI*)(m_PaintManager.FindControl(_T("filelist")));
	pFileList->RemoveAll();
	int j=-1;
	int k=0;
	while(!roomInfo.empty())
	{
		Data_LG D=roomInfo.front();
		roomInfo.pop();
		CDuiString s;
		CDuiString stitle;
		stitle.Format("%d",++j);
		s.Format("%s(%s)",D.Head["name"].c_str(),D.Head["location"].c_str());
			Node* rootNode= pFileList->AddNode(s,NULL,false,stitle);
			pFileList->SetItemShowHtml(true);
			pFileList->ExpandNode(rootNode);
			pFileList->ExpandNode(rootNode);
			pFileList->SetNoSelectImg(_T("notselect.png"));
			pFileList->SetSelectImg(_T("select.png"));
			for(int i=0;i<D.Body.size();i++)
			{
				CDuiString substr;
				stitle.Format("%d",++j);
				substr.Format("%s(%s:%s)",D.Body[i]["name"].c_str(),D.Body[i]["ip"].c_str(),D.Body[i]["port"].c_str());
				if(k>=videoURL.size())
				{
					videoURL.resize(videoURL.size()+BIT_SIZE);
				}
				videoURL[k].ip=D.Body[i]["ip"];
				videoURL[k].port=D.Body[i]["port"];
				if(D.Body[i]["cnttype"]=="1") 
				{
					videoURL[k].model=embed;
				}
				if(D.Body[i]["cnttype"]=="2")
				{
					videoURL[k].model=iMac;
				}
				++k;
				Node* childNode=pFileList->AddNode(substr,rootNode,true,stitle);
			}
	}
	for(int i=0;i<videoURL.size();i++)
	{
		if(!videoURL[i].ip.empty())
		{
			if(CMainWnd::pwnd)
			AddWorkToThreadPool(LoadSource,&(CMainWnd::pwnd->videoURL[i]));
		}
		else
		{
			continue;
		}
	}

	SetTimer(*this,TIME_ID_PER,400,NULL);
	SetTimer(*this,TIME_ID_ABORT,2000*pFileList->GetCount(),NULL);//每一个最多2秒的等待时间，其实对于多线程时间远小于此 
}

void CMainWnd::UpdataStatus()
{
	// 更新显示 网络是否加载成功的数据
	CFileListUI* pFileList = (CFileListUI*)(m_PaintManager.FindControl(_T("filelist")));
	Node* rootNode= pFileList->GetRoot();
	int ccout=0;
	for(int i=0;i<rootNode->num_children();i++)
	{
		Node *node=rootNode->child(i);
			if(node)
			{
				for(int j=0;j<node->num_children();j++)
				{
					Node *subnode=node->child(j);
					CDuiString str;
					if(!videoURL[ccout].token.empty())
					{
						str.Format("<x 30> <c #00ffff>%s</c>",subnode->data()._text.GetData());
						subnode->data()._pListElement->SetText(str);
					}
					++ccout;
				}
			}
	}


	//MessageBox(NULL,"1",0,0);

}