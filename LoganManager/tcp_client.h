#pragma once
#include <WinSock2.h>  
#include <stdio.h>  
#include <Windows.h>
#include <string>
using namespace std;
#pragma comment(lib, "ws2_32.lib")


class tcp_client
{
public:
	tcp_client(string ip,USHORT port,char* sendmsg);
	~tcp_client();

public:
	string           m_serverIP;
	USHORT		   m_serverPort;
	SOCKADDR_IN    m_serverAddr;
	SOCKET         m_clientSock;
	char data[1024];
	char msg[1024];
private:
	HANDLE handle;

};
