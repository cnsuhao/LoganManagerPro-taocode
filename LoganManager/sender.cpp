#include "stdafx.h"
#include "sender.h"

DWORD WINAPI SendMsgProc(_In_  LPVOID Parameter)
{
	UdpBroadcastSender *pthis=(UdpBroadcastSender*)(Parameter);
	char msg[1024];
	strcpy(msg,pthis->msg);
	SOCKET sockClient	= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (SOCKET_ERROR == sockClient)
	{
		return	-2;
	}

	BOOL bBroadcast = TRUE;                             
	if (NO_ERROR != setsockopt ( sockClient,SOL_SOCKET,SO_BROADCAST, (CHAR *)&bBroadcast, sizeof(BOOL)))
	{
		return	-3;
	}


	SOCKADDR_IN addrServer = {0};                              
	addrServer.sin_family = AF_INET;
	addrServer.sin_addr.s_addr = htonl ( INADDR_BROADCAST );
	addrServer.sin_port = htons (pthis->port);          
	int		nConunter = 0;	     
	clock_t t_now,t_pre;
	t_now=t_pre=clock()/CLOCKS_PER_SEC;
	while(true) 
	{
		int n=sendto ( sockClient,msg, strlen(msg)+1, 0, (SOCKADDR *) &addrServer, sizeof ( SOCKADDR_IN ));
		OutputDebugStringA("send:");
		OutputDebugStringA(msg);
		Sleep(500);
		t_now=clock()/CLOCKS_PER_SEC;
		if(t_now-t_pre>5)
		{
			break;
		}
	}

	closesocket(sockClient);  

	return	0;
}

UdpBroadcastSender::UdpBroadcastSender()
{
	if(WSAStartup(MAKEWORD( 2, 2 ), &m_wsaData ) != NO_ERROR)     
	{
		OutputDebugStringA("WSAStartup error\n");
	}
}
UdpBroadcastSender::~UdpBroadcastSender()
{
	WSACleanup();
}
void UdpBroadcastSender::SendMsg(USHORT nport,string smsg)
{
	port=nport;
	if(!smsg.empty())
	{
		msg[0]='\0';
		strcpy(msg,smsg.c_str());
	}
	else
	{
		memset(msg,0,sizeof msg);
	}
	CreateThread(NULL, 0, SendMsgProc, this, 0, 0);
}