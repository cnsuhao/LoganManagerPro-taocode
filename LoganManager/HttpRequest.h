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

typedef bool(*dataCallback)(TiXmlElement*,string id);//data�Ļص�������������data�ڵ�����,ͨ��idʵ�ֲ���

enum ServerModel
{
	embed = 1,			//Ƕ��ʽ
	iMac,						//һ���
	cloud						//����Դ
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
//	int ID;//��Ƶ����
//	OPUrl BaseOPUrl;
//	map<string, string>StreamCata;//���������
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
*ip:					����http�����IP��ַ
*po:					��������Ҫ�Ķ˿ڣ�һ��Ϊ	80
*ServerModel:		�������Խӽӿ� Ƕ��ʽ��һ���������Դ

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
	static netCode LogIn(string ip,string po,ServerModel model,string userName,string password);//��¼
	static queue<Data_LG> LookUpClassRoom(string ip,string po,ServerModel model,string token,string params,string id);//��ȡ¼������
	static string LookUpUrl(string ip,string port,ServerModel model,string token,string name,string id);//��ѯ���ŵ�ַ
	static netCode DirControl(string ip,string port,ServerModel model,string token,DIR dir,string param);//��̨����
	static netCode ZoomControl(string ip,string port,ServerModel model,string token,ZOOM zoom,string param);//��̨zoom
	/* Ԥ��λ���ú�����ʱҪָ�� &precall=1                |          &preset=2  */
	static netCode PlaceControl(string ip,string port,ServerModel model,string token,string param);//Ԥ��λ���ú�����
	static netCode SystemSetting(string ip, string port, ServerModel model, string token,UINT option);//����������
	static netCode LogOut(string ip, string port, ServerModel model, string token,string userName);//�˳�
	static queue<string> LookUpRecord(string ip, string po, ServerModel model, string token,string id);//¼����Ϣ��ѯ
	static netCode RecordSetting(string ip, string po, ServerModel model, string token, unsigned int state);//¼������
	static netCode SetView(string ip, string po, ServerModel model, string token, unsigned int view);//ֱ����������


};


