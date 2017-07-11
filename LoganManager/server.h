#pragma once

#include <Ws2tcpip.h>
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")

#include <string>
#include <set>
#include <vector>
using namespace std;
class CUdpBroadcastServer
{
public:
	CUdpBroadcastServer();
	~CUdpBroadcastServer();

	int Open(int nRecvPort);
	void Close();
	map<string,string> getList();
private:
	bool OpenRecv(int nPort);
	void GetLocalIp();
	static DWORD WINAPI SendProc(LPVOID lpParam);
	static DWORD WINAPI RecvProc(LPVOID lpParam);

private:
	int m_nSendPort;
	int	m_nSendSocket;
	int m_nRecvSocket;
	string m_strPassword;
	string m_strIpList;

	int m_nThreadState;
	HANDLE	m_hSendThread;
	HANDLE	m_hRecvThread;

	set<string> m_setClient;
	map<string,string>reslist;
};