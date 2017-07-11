#pragma  once
#include <Ws2tcpip.h>
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")

#include <string>
#include <set>
#include <map>
#include <vector>
using namespace std;

//流程：
//1、PC端发广播包请求验证
//2、目标机收到包后，验证正确后，发回复的广播包
//3、PC机收到回复的广播包，验证正确后，显示

class CUdpBroadcast
{
public:
	CUdpBroadcast();
	~CUdpBroadcast();

	int Open(int nPort, int nRecvPort, string strPassword);
	void Close();
	map<string,string>maplist;
	map<string,string>getList()const;
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
};