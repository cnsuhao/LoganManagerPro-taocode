#pragma once

#include "../Net/Net.h"
#include "tinystr.h"
#include "tinyxml.h"


#ifdef _DEBUG									
#pragma comment(lib,"..\\lib\\net_d.lib")
#else											
#pragma comment(lib,"..\\lib\\net.lib")
#endif

enum DIR{STOP=0,UP,DOWN,LEFT,RIGHT,RESET};
enum ZOOM{ZOOMUP,ZOOMDOWN};

typedef bool(*dataCallback)(TiXmlElement*,string id);//data的回调函数用来出来data节点数据,通过id实现并发

enum ServerModel
{
	embed = 1,			//嵌入式
	iMac,						//一体机
	cloud						//云资源
};
//typedef struct OPUrl
//{
//	string ip;
//	string port;
//	string token;
//	ServerModel model;
//	bool isOpen;
//}OPUrl;
//
//typedef struct VideoList
//{
//	int ID;//视频序列
//	OPUrl BaseOPUrl;
//	map<string, string>StreamCata;//播放流类别
//	string LocalName;
//}VideoList;


struct Data_LG
{
	map<string,string>Head;
	vector<map<string,string>> Body;

};

typedef struct 
{
	bool status;
	string msg;
}netCode;

/**********************************************
*							class HttpRequest
***********************************************
*
*ip:					用于http请求的IP地址
*po:					请求所需要的端口，一般为	80
*ServerModel:		服务器对接接口 嵌入式、一体机、云资源

**********************************************/

class HttpRequest
{
private:
	static netCode startRequestSimple(string httpArg);
	static netCode startRequestMainDataNode(string httpArg,dataCallback callback,string id);
public:
	static string embedModel;
	static string iMacModel;
	static string lgCould;
	static netCode LogIn(string ip,string po,ServerModel model,string userName,string password);//登录
	static queue<Data_LG> LookUpClassRoom(string ip,string po,ServerModel model,string token,string params,string id);//获取录播教室
	static string LookUpUrl(string ip,string port,ServerModel model,string token,string name,string id);//查询播放地址
	static netCode DirControl(string ip,string port,ServerModel model,string token,DIR dir,string param);//云台控制
	static netCode ZoomControl(string ip,string port,ServerModel model,string token,ZOOM zoom,string param);//云台zoom
	/* 预置位调用和设置时要指明 &precall=1                |          &preset=2  */
	static netCode PlaceControl(string ip,string port,ServerModel model,string token,string param);//预置位调用和设置
	static netCode SystemSetting(string ip, string port, ServerModel model, string token,UINT option);//服务器管理
	static netCode LogOut(string ip, string port, ServerModel model, string token,string userName);//退出
	static queue<string> LookUpRecord(string ip, string po, ServerModel model, string token,string id);//录制信息查询
	static netCode RecordSetting(string ip, string po, ServerModel model, string token, unsigned int state);//录制设置
	static netCode SetView(string ip, string po, ServerModel model, string token, unsigned int view);//直播画面设置


};


