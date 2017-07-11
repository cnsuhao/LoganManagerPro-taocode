#pragma  once
#include "stdafx.h"
#include "UdpBroadcast.h"


CUdpBroadcast::CUdpBroadcast():
m_nThreadState(0),
m_hSendThread(NULL),
m_hRecvThread(NULL),
m_nSendSocket(-1),
m_nRecvSocket(-1)
{

}

CUdpBroadcast::~CUdpBroadcast()
{
	Close();
}

int CUdpBroadcast::Open(int nPort, int nRecvPort, string strPassword)
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
			if(setsockopt(m_nSendSocket,SOL_SOCKET,SO_BROADCAST,(const char *)&nBroadcast, sizeof(int)) == 0)
			{
				if(OpenRecv(nRecvPort))
				{
					m_nSendPort = nPort;
					m_strPassword = strPassword;
	
					m_nThreadState = 1;
					m_hRecvThread = CreateThread(NULL,0,CUdpBroadcast::RecvProc,(PVOID)this,0,NULL);
					m_hSendThread = CreateThread(NULL,0,CUdpBroadcast::SendProc,(PVOID)this,0,NULL);
				}
				else
				{
					printf("Open Recv Socket failed \n");
					nRet = 4;
				}
			}
			else
			{
				printf("Set Socket Option failed \n");
				nRet = 3;
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

void CUdpBroadcast::Close()
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
map<string,string> CUdpBroadcast::getList()const
{
	return maplist;
}
bool CUdpBroadcast::OpenRecv(int nPort)
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

void CUdpBroadcast::GetLocalIp()
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

void GetIpList(string strIpList, vector<string>& vecIP)
{
	std::size_t nStart=0, nIndex=strIpList.find_first_of("&", 0);
	while(nIndex != strIpList.npos)
	{
		if(nStart != nIndex)
		{
			if(nStart)
				vecIP.push_back(strIpList.substr(nStart,nIndex-nStart));
		}
		nStart = nIndex+1;
		nIndex = strIpList.find_first_of("&", nStart);
	}
	if(nStart != strIpList.npos)
	{
		vecIP.push_back(strIpList.substr(nStart));
	}
}

DWORD WINAPI CUdpBroadcast::SendProc(LPVOID lpParam)
{
	CUdpBroadcast *pThis = (CUdpBroadcast *)lpParam;
	if(pThis)
	{
		string strMsg = pThis->m_strPassword;

		struct sockaddr_in addrto;  
		addrto.sin_family = AF_INET;  
		addrto.sin_addr.s_addr = htonl(INADDR_BROADCAST); 
		//addrto.sin_addr.s_addr =inet_addr("192.168.255.255");
		addrto.sin_port = htons(pThis->m_nSendPort);  
		int nSocketLen = sizeof(addrto); 
		while(pThis->m_nThreadState)
		{
			int nSend = sendto(pThis->m_nSendSocket, strMsg.c_str(), strMsg.size(), 0, (sockaddr*)&addrto, nSocketLen);
			if(nSend <= 0)
			{
				printf("sendto(%s) failed \n", pThis->m_strPassword.c_str());
			}

			Sleep(1000);
		}
	}

	return 0;
}

DWORD WINAPI CUdpBroadcast::RecvProc(LPVOID lpParam)
{
	CUdpBroadcast *pThis = (CUdpBroadcast *)lpParam;
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
					OutputDebugStringA(szRecvBuf);
					string strFromIP = inet_ntoa(addr.sin_addr);
					if(strcmp(szRecvBuf, "no") != 0&&strcmp(szRecvBuf, "nono") != 0)
					{
						pThis->maplist[strFromIP]=string(szRecvBuf);
						if(pThis->m_setClient.find(strFromIP) == pThis->m_setClient.end())
						{
							pThis->m_setClient.insert(strFromIP);
							printf("************************************\n\tNew Client --- %s \n************************************\n", strFromIP.c_str());
						}
					}
				}
			}

			Sleep(1000);
		}
	}

	return 0;
}