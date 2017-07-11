#pragma  once
#include "stdafx.h"
#pragma comment(lib,"WS2_32.lib")

class UdpBroadcastSender
{
public:
	UdpBroadcastSender();
	~UdpBroadcastSender();
	void SendMsg(USHORT port,string msg);
	USHORT port;
	char msg[1024];
private:
	WSADATA     m_wsaData;
	

};