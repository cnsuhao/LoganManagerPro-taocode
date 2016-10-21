#include "StdAfx.h"
#include "ClientSocket.h"


/****************************UDP*******************************/
ClientSocketUDP::ClientSocketUDP(const char* ip, u_short port)
{
	m_code = 0;
	if (WSAStartup(MAKEWORD(2, 2), &m_wsd) != NOERROR)
	{
		m_code = WSAGetLastError();
		return;
	}
	m_serverAddress.sin_family = AF_INET;
	m_serverAddress.sin_addr.S_un.S_addr = inet_addr(ip);
	m_serverAddress.sin_port = htons(port);
	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_socket == INVALID_SOCKET)
	{
		m_code = WSAGetLastError();
	}
}

ClientSocketUDP::~ClientSocketUDP()
{
	closesocket(m_socket);
	WSACleanup();
}

int  ClientSocketUDP::SendMsg(const char *msg, int len)
{
	if (m_code==0)
		if (sendto(m_socket, msg, len, 0, (sockaddr*)&m_serverAddress, sizeof(m_serverAddress)) == SOCKET_ERROR)
		{
			m_code = WSAGetLastError();
		}
		return m_code;
}

int ClientSocketUDP::RecvMsg(char *msg,int len)
{
	int nserver = sizeof(m_serverAddress);
	if (recvfrom(m_socket, msg, len, 0, (sockaddr*)&m_serverAddress,&nserver ) == SOCKET_ERROR)
	{
		m_code = WSAGetLastError();
	}
	return m_code;
}
/************************END UDP********************************/




/*____________________TCP_________________________*/

ClientSocketTCP::ClientSocketTCP(const char* ip,u_short port)
{
	m_code = 0;

	if (WSAStartup(MAKEWORD(2, 2), &m_wsd) != NO_ERROR)
	{
		m_code = WSAGetLastError();
		return;
	}

	m_serverAddress.sin_family = AF_INET;
	m_serverAddress.sin_addr.S_un.S_addr = inet_addr(ip);
	m_serverAddress.sin_port = htons(port);

	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (m_socket == INVALID_SOCKET)
	{
		m_code = WSAGetLastError();
		return;
	}

	if (connect(m_socket, (sockaddr*)&m_serverAddress, sizeof(m_serverAddress)) == INVALID_SOCKET)
	{
		m_code = WSAGetLastError();
		return;
	}

}

ClientSocketTCP::~ClientSocketTCP()
{
	closesocket(m_socket);
	WSACleanup();
}

int ClientSocketTCP::SendMsg(const char* msg, int len)
{
	if (send(m_socket, msg, len, 0) == SOCKET_ERROR)
	{
		m_code = WSAGetLastError();
	}
	return m_code;
}
int ClientSocketTCP::RecvMsg(char* msg, int len)
{
	int bytercv=0;


	while (bytercv!=SOCKET_ERROR)
	{
		if ((bytercv = recv(m_socket, msg, len, 0)) == SOCKET_ERROR)
		{
			m_code = WSAGetLastError();
		}
		if (bytercv == 0)
		{
			break;
		}
		cout << "byte recv:" << bytercv << endl;
	}

	return m_code;
}

/*_________________END TCP________________*/