#include "StdAfx.h"
#include "tcp_client.h"

DWORD WINAPI MsgProc(_In_ LPVOID paramer)
{
	tcp_client *p = (tcp_client*)paramer;
	send(p->m_clientSock,p->msg,sizeof(p->msg),0);
	recv(p->m_clientSock,p->data,sizeof(p->data),0);

	return 0;
}
tcp_client::tcp_client(string ip,USHORT port,char* sendmsg)
	{
		m_serverIP=ip;
		m_serverPort=port;
		WSADATA m_wsaData;
		if(sendmsg!=NULL)
		{
			strcpy_s(msg,sendmsg);
		}
		if(WSAStartup(MAKEWORD(2,2),&m_wsaData)==NO_ERROR)
		{
			m_serverAddr.sin_family=AF_INET;
			m_serverAddr.sin_addr.S_un.S_addr=inet_addr(m_serverIP.c_str());
			m_serverAddr.sin_port=htons(m_serverPort);

			m_clientSock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

			connect(m_clientSock,(sockaddr*)&m_serverAddr,sizeof m_serverAddr);

			handle=CreateThread(NULL,0,MsgProc,this,0,0);

		}
	}
tcp_client::~tcp_client()
	{
		//WSACleanup();
		//closesocket(m_clientSock);
		//CloseHandle(handle);
	}

