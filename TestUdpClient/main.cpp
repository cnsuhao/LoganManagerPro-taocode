#include <stdio.h>
#include <winsock.h>
#include <time.h>
#pragma comment(lib,"ws2_32.lib")

struct _Table
{
	USHORT port;
	char*   msg;
};

DWORD WINAPI SendProc(_In_  LPVOID Parameter)
{
	_Table *table=(_Table*)(Parameter);
	USHORT port=0;
	char msg[1024]={0};
	port=table->port;
	strcpy(msg,table->msg);
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
	addrServer.sin_port = htons (port);          
	int		nConunter = 0;	     
	clock_t t_now,t_pre;
	t_now=t_pre=clock()/CLOCKS_PER_SEC;
	while(true) 
	{
		sendto ( sockClient, msg, strlen(msg)+1, 0, (SOCKADDR *) &addrServer, sizeof ( SOCKADDR_IN ));
		Sleep(300);
		t_now=clock()/CLOCKS_PER_SEC;
		/*if(t_now-t_pre>3)
		{
			break;
		}*/
	}

	closesocket(sockClient);  

	return	0;
}

class UdpBroadcastSender
{
public:
	UdpBroadcastSender()
	{
		if(WSAStartup(MAKEWORD( 2, 2 ), &m_wsaData ) != NO_ERROR)     
		{
			OutputDebugStringA("WSAStartup error\n");
		}
	}
	~UdpBroadcastSender()
	{
		WSACleanup();
	}
	void SendMsg(USHORT port,char* msg)
	{
		_Table table;
		table.port=port;
		table.msg=msg;
		CreateThread(NULL, 0, SendProc, &table, 0, 0);
	}
private:
	WSADATA     m_wsaData;

};



DWORD WINAPI RecvProc(_In_  LPVOID Parameter)
{
	_Table *table=(_Table*)(Parameter);
	char	szIp[32] = {0};
	USHORT port=0;
	char msg[1024]={0};
	port=table->port;
	SOCKET sockServer	= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (SOCKET_ERROR == sockServer)
	{
		printf ("socket create failed. ip=[%s] errno=[%d] ", szIp, WSAGetLastError());
		return	-2;
	}


	SOCKADDR_IN	localAddress		= {0};
	localAddress.sin_family		= AF_INET;
	localAddress.sin_addr.s_addr	=INADDR_ANY;
	localAddress.sin_port			= htons(port);				
	if (0 != bind (sockServer, (sockaddr*)&localAddress, sizeof(localAddress)))
	{
		printf ("bind failed.ip=[%s] errno=[%d]\n", szIp, WSAGetLastError());
	}

	SOCKADDR_IN remoteAddress={0};
	char data[1024]={};
	while(true)
	{
		int lenfrom=sizeof(localAddress);
		if(recvfrom(sockServer,data,strlen(data)+1,0,(sockaddr*)&remoteAddress,&lenfrom)>0)
		{
			printf("%s-->%s\n",inet_ntoa(remoteAddress.sin_addr),data);
		}
	}
	

	closesocket(sockServer);  

	return	0;
}





class UdpBroadcastServer
{
public:
	UdpBroadcastServer()
	{
		if(WSAStartup(MAKEWORD( 2, 2 ), &m_wsaData ) != NO_ERROR)     
		{
			OutputDebugStringA("WSAStartup error\n");
		}
	}
	~UdpBroadcastServer()
	{
		WSACleanup();
	}
  void RecvMsg(USHORT port,char* msg)
  {
	 
		  _Table table;
		  table.port=port;
		  table.msg=msg;
		  CreateThread(NULL, 0, RecvProc, &table, 0, 0);
  }
private:
	WSADATA     m_wsaData;
};





int main()
{
	//WSADATA wsaData = {0};                                   
	//if(0 != WSAStartup(MAKEWORD( 2, 2 ), &wsaData ))     
	//{
	//	printf("WSAStartup failed.errno=[%d]", WSAGetLastError());//初始化失败返回-1
	//	return -1;
	//}

	//char hostname[1024] = {0};
	//gethostname(hostname, sizeof(hostname));    //获得本地主机名
	//PHOSTENT hostinfo = gethostbyname(hostname);//信息结构体
	//while(*(hostinfo->h_addr_list) != NULL)        //输出IP地址
	//{
	//	char* ip = inet_ntoa(*(struct in_addr *) *hostinfo->h_addr_list);

	//	CreateThread(NULL, 0, ThreadProc, ip, 0, 0);
	//	Sleep(100);
	//	hostinfo->h_addr_list++;
	//}
	UdpBroadcastSender c;
	c.SendMsg(2800,"fuck-U");

	//UdpBroadcastServer s;
	//s.RecvMsg(2800,"");
	Sleep(INFINITE);

	WSACleanup();
	return 0;
}