#include <WinSock2.h>
#include <stdio.h>
#include <iostream>
using namespace std;

#pragma comment(lib, "ws2_32.lib")

const int MAX_BUF_LEN = 255;

int main()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	// ����socket api
	wVersionRequested = MAKEWORD( 2, 2 );
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 )
	{
		return -1;
	}

	if ( LOBYTE( wsaData.wVersion ) != 2 ||
		HIBYTE( wsaData.wVersion ) != 2 )
	{
		WSACleanup( );
		return -1;
	}

	// ����socket
	SOCKET connect_socket;
	connect_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(INVALID_SOCKET == connect_socket)
	{
		err = WSAGetLastError();
		return -1;
	}

	SOCKADDR_IN sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(2800);
	sin.sin_addr.s_addr = INADDR_BROADCAST;

	bool bOpt = true;
	//���ø��׽���Ϊ�㲥����
	setsockopt(connect_socket, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt));

	int nAddrLen = sizeof(SOCKADDR);

	char buff[MAX_BUF_LEN] = "";
	int nLoop = 0;
	while(1)
	{
		nLoop++;
		sprintf(buff, "%d", nLoop);

		// ��������
		int nSendSize = sendto(connect_socket, buff, strlen(buff), 0, (SOCKADDR*)&sin, nAddrLen);
		if(SOCKET_ERROR == nSendSize)
		{
			err = WSAGetLastError();
			return -1;
		}
		printf("Send: %s/n", buff);
	}

	return 0;
}