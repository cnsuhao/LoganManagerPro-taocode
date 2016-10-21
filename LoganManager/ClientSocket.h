#pragma once

#include <iostream>
#include <windows.h>
using namespace std;

#include <winsock2.h>
#pragma comment(lib,"WS2_32.lib")

/*this is base class for client socket can use TCP or UDP protocol*/
class ClientSocket abstract
{
public:
	ClientSocket()
	{
		m_code = 0;
	}
	virtual ~ClientSocket(){}
	virtual int  SendMsg(const char *msg, int len) = 0;
	virtual int RecvMsg(char *msg, int len) = 0;
	virtual int GetCode(){ return m_code; }
protected:
	int m_code; 
};


/* use UDP method */
class ClientSocketUDP : public ClientSocket
{
public:
	ClientSocketUDP(const char* ip, u_short port);
	~ClientSocketUDP();
	int  SendMsg(const char *msg, int len);
	int RecvMsg(char *msg,int len);
private:
	WSADATA				m_wsd;
	SOCKET				m_socket;
	SOCKADDR_IN			m_serverAddress;

};

/*use TCP method */
class ClientSocketTCP : public ClientSocket
{
public:
	ClientSocketTCP(const char* ip,u_short port);
	~ClientSocketTCP();
	int SendMsg(const char* msg, int len);
	int RecvMsg(char* msg, int len);
private:
	WSADATA				m_wsd;
	SOCKET				m_socket;
	SOCKADDR_IN			m_serverAddress;
};



