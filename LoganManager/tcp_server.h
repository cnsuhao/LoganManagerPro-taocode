#pragma once

#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")  


class tcp_server
{

public:
	tcp_server(int);
	virtual ~tcp_server();
	int start_listening();
	friend DWORD WINAPI AcceptProc(_In_ LPVOID paramer);
	string getCmd(string localIP);
	string recvip;
	map<string,string>cmdSet;//localIP-----command
protected:
	int port;
	SOCKET sSock; // Server Socket
	SOCKET cSock; // Client Socket
	HANDLE handle;
	map<string,int>videoCount;// luboIP-----count
	map<string,string>realVideo;//localIP-----luboIP
private:
	int acceptConns();

};