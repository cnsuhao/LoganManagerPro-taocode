#include "StdAfx.h"
#include "tcp_server.h"
#include <stdio.h>
#include <winsock2.h>
#include <iostream>
#include "tcp_client.h"

DWORD WINAPI AcceptProc(_In_ LPVOID paramer)
{
	tcp_server *server=(tcp_server*)(paramer);
	sockaddr_in from;
	int fromlen=sizeof(from);
	while (true)
	{
		char temp[1024];
		server->cSock = accept(server->sSock,(SOCKADDR*)&from,&fromlen);

		if(server->cSock!=INVALID_SOCKET)
		{
			memset(temp,0,sizeof temp);
			recv(server->cSock,temp,sizeof temp,0);
			OutputDebugStringA(temp);
			char buffer[1024];
			string str=string(temp);
			if(str.substr(0,4)=="send")
			{
				sprintf(buffer,"%s发起了视频请求", inet_ntoa(from.sin_addr));
				if(MessageBoxA(NULL,buffer,"提示",MB_OKCANCEL)==IDOK)
				{
					server->recvip=str.substr(5);
					server->cmdSet[string(inet_ntoa(from.sin_addr))]=str.substr(5);
					server->realVideo[string(inet_ntoa(from.sin_addr))]=str.substr(5);
					char bb[]="recv";
					tcp_client c(string(inet_ntoa(from.sin_addr)),server->port,bb);
					//send(server->cSock,bb,sizeof bb,0);
				}
				else
				{
					char bb[]="refu";
					tcp_client c(string(inet_ntoa(from.sin_addr)),server->port,bb);
					//send(server->cSock,bb,sizeof bb,0);
				}

			}
			else if(strcmp(temp,"die")==0)
			{
				server->cmdSet[string(inet_ntoa(from.sin_addr))]="die";
				std::cout<<"挂断视频"<<std::endl;
				//char bb[]="die";
				//send(server->cSock,bb,sizeof bb,0);
			}
			else if(strcmp(temp,"refu")==0)
			{
				server->cmdSet[string(inet_ntoa(from.sin_addr))]="refu";
				MessageBoxA(NULL,"对方已拒绝视频","提示",MB_OK);
			}
			else if(strcmp(temp,"recv")==0)
			{
				server->cmdSet[string(inet_ntoa(from.sin_addr))]="recv";
				//server->realVideo[string(inet_ntoa(from.sin_addr))]=str.substr(5);
			}
			std::cout << "Connection from " << inet_ntoa(from.sin_addr) <<"\n";
            closesocket(server->cSock);
		}
	}
	return 0;
}

tcp_server::tcp_server( int port)
{
	tcp_server::port = port;
}

tcp_server::~tcp_server()
{
	closesocket(sSock);
	WSACleanup();
	CloseHandle(handle);
}
string tcp_server::getCmd(string localIP)
{
	return cmdSet[localIP];
}
int tcp_server::start_listening()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int wsaerr;
	wVersionRequested = MAKEWORD(2, 2);
	wsaerr = WSAStartup(wVersionRequested, &wsaData);
	if (wsaerr != 0)
	{
		printf("Server: The Winsock dll not found!n");
		WSACleanup();
		return 0;
	}
	sSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	/*unsigned long ul=1;
	ioctlsocket(sSock,FIONBIO,(unsigned long*)&ul);*/
	if(sSock == INVALID_SOCKET)
	{
		printf("Server: Error initializing socket!n");
		WSACleanup();
		return 0;
	}

	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_port = htons(port);
	service.sin_addr.s_addr =htonl(INADDR_ANY);
	if(bind(sSock,(SOCKADDR*)&service,sizeof(service))==SOCKET_ERROR)
	{
		printf("Server: Error binding socket to portn");
		WSACleanup();
		return 0;
	}

	if(listen(sSock,10)==SOCKET_ERROR)
		printf("listen(): Error listening on socket %d.n", WSAGetLastError());
	else
	{
		printf("Server: Waiting for connections...");
	}
	acceptConns();
	return 0;
}

// send 发起    recv 接受       refu  拒绝        die 挂断
int tcp_server::acceptConns()
{
	handle=CreateThread(NULL,0,AcceptProc,this,0,0);
	return 0;
}