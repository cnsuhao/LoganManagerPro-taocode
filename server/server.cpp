#pragma once
#include <Ws2tcpip.h>
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")

#include <string>
#include <set>
#include <vector>
#include <map>
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




int main()
{
	UdpBroadcastServer s;
	if(s.Open(2800)==0)
	{
		s.Close();
	}
}

CUdpBroadcastServer::CUdpBroadcastServer():
m_nThreadState(0),
m_hSendThread(NULL),
m_hRecvThread(NULL),
m_nSendSocket(-1),
m_nRecvSocket(-1)
{

}

CUdpBroadcastServer::~CUdpBroadcastServer()
{
	Close();
}

int CUdpBroadcastServer::Open(int nRecvPort)
{
	int nRet = 0;

	WSADATA wsaData;
	if(0 == WSAStartup(MAKEWORD(1,1), &wsaData))
	{
		//GetLocalIp();

		m_nSendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if(m_nSendSocket != INVALID_SOCKET)
		{
			int nBroadcast = 1;
//			if(setsockopt(m_nSendSocket,SOL_SOCKET,SO_BROADCAST,(const char *)&nBroadcast, sizeof(int)) == 0)
			{
				if(OpenRecv(nRecvPort))
				{
					//m_nSendPort = nPort;
					//m_strPassword = strPassword;
	
					m_nThreadState = 1;
					m_hRecvThread = CreateThread(NULL,0,CUdpBroadcastServer::RecvProc,(PVOID)this,0,NULL);
					//m_hSendThread = CreateThread(NULL,0,CUdpBroadcast::SendProc,(PVOID)this,0,NULL);
				}
				else
				{
					printf("Open Recv Socket failed \n");
					nRet = 4;
				}
			}
		}
		else
		{
			printf("Create Socket failed \n");
			nRet = 2;
		}
	}
	else
	{
		printf("Init Win32 Socket lib failed \n");
		nRet = 1;
	}

	return nRet;
}

void CUdpBroadcastServer::Close()
{
	m_nThreadState = 0;
	if(m_hSendThread)
	{
		WaitForSingleObject(m_hSendThread,INFINITE);
		CloseHandle(m_hSendThread);
		m_hSendThread = NULL;
	}
	if(m_hRecvThread)
	{
		WaitForSingleObject(m_hRecvThread,INFINITE);
		CloseHandle(m_hRecvThread);
		m_hRecvThread = NULL;
	}

	if(m_nSendSocket != -1)
	{
		closesocket(m_nSendSocket);
		m_nSendSocket = -1;
	}
	if(m_nRecvSocket != -1)
	{
		closesocket(m_nRecvSocket);
		m_nRecvSocket = -1;
	}
}
map<string,string> CUdpBroadcastServer::getList()
{
	return reslist;
}
bool CUdpBroadcastServer::OpenRecv(int nPort)
{
	bool bRet = false;

	m_nRecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(m_nRecvSocket != INVALID_SOCKET)
	{
		struct sockaddr_in addr;  
		addr.sin_family 		= AF_INET;  
		addr.sin_port 			= htons(nPort);  
		addr.sin_addr.s_addr 	= htonl(INADDR_ANY);
		if(bind(m_nRecvSocket, (struct sockaddr *)&addr, sizeof(addr)) != SOCKET_ERROR)
		{
			bRet = true;
		}
	}

	return bRet;
}

void CUdpBroadcastServer::GetLocalIp()
{
	/*char szHostName[256]; 
	if(gethostname(szHostName, sizeof(szHostName)) != SOCKET_ERROR) 
	{ 
		struct hostent *pHost = gethostbyname(szHostName);
		if(pHost)
		{
			m_strIpList = "";
			char szIP[32];
			char **ppList=pHost->h_addr_list;
			for(;*ppList!=NULL;ppList++)
			{
				inet_ntop(pHost->h_addrtype, *ppList, szIP, sizeof(szIP)); //inet_ntop属于ip 6，XP系统没有这函数
				m_strIpList += "&";
				m_strIpList += szIP;
			}
		}
	}  */
}

DWORD WINAPI CUdpBroadcastServer::SendProc(LPVOID lpParam)
{
	CUdpBroadcastServer *pThis = (CUdpBroadcastServer *)lpParam;
	if(pThis)
	{
		string strMsg = pThis->m_strPassword;

		struct sockaddr_in addrto;  
		addrto.sin_family = AF_INET;  
		addrto.sin_addr.s_addr = htonl(INADDR_BROADCAST); 
		addrto.sin_port = htons(pThis->m_nSendPort);  
		int nSocketLen = sizeof(addrto); 
		while(pThis->m_nThreadState)
		{
			int nSend = sendto(pThis->m_nSendSocket, strMsg.c_str(), strMsg.size(), 0, (sockaddr*)&addrto, nSocketLen);
			if(nSend <= 0)
			{
				printf("sendto(%s) failed \n", pThis->m_strPassword.c_str());
			}

			Sleep(2000);
		}
	}

	return 0;
}

DWORD WINAPI CUdpBroadcastServer::RecvProc(LPVOID lpParam)
{
	CUdpBroadcastServer *pThis = (CUdpBroadcastServer *)lpParam;
	if(pThis)
	{
		struct sockaddr_in addr = {0};  
		int addr_len = sizeof(struct sockaddr_in);
		char szRecvBuf[1024];
		int  nRecvLen = 0;

		while(pThis->m_nThreadState)
		{
			nRecvLen = 0;
			memset(szRecvBuf, 0, sizeof(szRecvBuf));

			fd_set read_fds;
			FD_ZERO(&read_fds);
			FD_SET(pThis->m_nRecvSocket, &read_fds);
			struct timeval TimeoutVal;
			TimeoutVal.tv_sec  = 2;
			TimeoutVal.tv_usec = 0;

			int nRet = select(pThis->m_nRecvSocket+1, &read_fds, NULL, NULL, &TimeoutVal);
			if(nRet <= 0)
			{
				continue;
			}
			else if(FD_ISSET(pThis->m_nRecvSocket, &read_fds))
			{
				nRecvLen = recvfrom(pThis->m_nRecvSocket,szRecvBuf,sizeof(szRecvBuf),0,(struct sockaddr *)&addr ,&addr_len);
				if(nRecvLen)
				{
					string strFromIP = inet_ntoa(addr.sin_addr);
					//IpTable[strFromIP]=string(szRecvBuf);
					OutputDebugStringA(strFromIP.c_str());
					printf("%s--->%s\n",strFromIP.c_str(),szRecvBuf);
					if(strcmp(szRecvBuf, "no") != 0)
					{
						//reslist.insert(make_pair(strFromIP,string(szRecvBuf)));
						pThis->reslist[strFromIP]=string(szRecvBuf);
						if(pThis->m_setClient.find(strFromIP) == pThis->m_setClient.end())
						{
							pThis->m_setClient.insert(strFromIP);
							printf("************************************\n\tNew Client --- %s \n************************************\n", strFromIP.c_str());
						}
					}
				}
			}

			Sleep(100);
		}
	}

	return 0;
}