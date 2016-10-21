#include "StdAfx.h"
#include "HttpRequest.h"

#include "CMyCharConver.h"
#pragma  once




string HttpRequest::embedModel="";
string HttpRequest::iMacModel="";
string HttpRequest::lgCould="";

/********************************************************/

netCode HttpRequest::startRequestSimple(string httpArg)
{
	Net request;
	string type,code,msg;
	TiXmlDocument xml;
	netCode nres;
	OutputDebugStringA("GET------->");
	OutputDebugStringA(httpArg.c_str());
	OutputDebugStringA("\n");
	string res=request.GetRespond(httpArg);
	if(res=="")
	{
		nres.status=false;
		nres.msg="net error";
		return nres;
	}
	else
	{
		OutputDebugStringA(res.c_str());
		xml.Parse(res.c_str());
		TiXmlNode *root=xml.RootElement();
		for(TiXmlNode *i=root->FirstChildElement();i;i=i->NextSiblingElement())
		{
			if(strcmp(i->Value(),"type")==0)
			{
				type=string(i->FirstChild()->Value());
			}
			else if(strcmp(i->Value(),"code")==0)
			{
				code=string(i->FirstChild()->Value());
			}
			else if(strcmp(i->Value(),"msg")==0)
			{
				msg=string(i->FirstChild()->Value());
			}
		}
		if(code=="1")
		{
			nres.status=true;
			nres.msg=msg;
			return nres;
		}
		else
		{
			nres.status=false;
			nres.msg=msg;
			return nres;
		}
	}
}


netCode HttpRequest::startRequestMainDataNode(string httpArg,dataCallback callback,string id)
{
	Net request;
	netCode nc;
	TiXmlDocument xml;
	string type,code,msg;
	OutputDebugStringA("GET------->");
	OutputDebugStringA(httpArg.c_str());
	OutputDebugStringA("\n");
	string res=request.GetRespond(httpArg);
	if(res=="")
	{
		nc.status=false;
		nc.msg="net error";
		return nc;
	}
	else
	{
		xml.Parse(res.c_str());
		TiXmlElement *root=xml.RootElement();
		for(TiXmlElement *i=root->FirstChildElement();i;i=i->NextSiblingElement())
		{
			if(strcmp(i->Value(),"type")==0)
			{
				type=string(i->FirstChild()->Value());
			}
			else if(strcmp(i->Value(),"code")==0)
			{
				code=string(i->FirstChild()->Value());
			}
			else if(strcmp(i->Value(),"msg")==0)
			{
				msg=string(i->FirstChild()->Value());
			}
			else if(strcmp(i->Value(),"data")==0)
			{
				callback(i,id);

			}
		}
		if(code=="1")
		{
			nc.status=true;
			nc.msg=msg;
			return nc;
		}
		else
		{
			nc.status=false;
			nc.msg=msg;
			return nc;
		}
	}
}

/************************************************************************/
netCode HttpRequest::LogIn(string ip,string po,ServerModel model,string userName,string password)
{
	string mainUrl;
	switch(model)
	{
	case embed:
		mainUrl="http://"+ip+":"+po+"/"+embedModel+"?";
		break;
	case iMac:
		mainUrl="http://"+ip+":"+po+"/"+iMacModel+"?";
		break;
	case cloud:
		mainUrl="http://"+ip+":"+po+"/"+lgCould+"?";
		break;
	}
	string httpArg=mainUrl+"type=login&userName="+userName+"&password="+password;
	return startRequestSimple(httpArg);
}


queue<string> TempBuffer;
queue<Data_LG> TempBuffer_MAP;

map<string,queue<Data_LG>>NetData;//网络数据包含详细的教室信息

map<string,queue<string>>NetEasyData;//简单的网络数据

bool callLBClassData(TiXmlElement *node,string id)
{

	while(!NetData[id].empty())
	{
		NetData[id].pop();
	}
	if(node)
	{

		for(TiXmlElement *j=node->FirstChildElement();j;j=j->NextSiblingElement())
		{
			for(TiXmlElement *i=j->FirstChildElement();i;i=i->NextSiblingElement())
			{
				if(strcmp(i->Value(),"item")==0)
				{
					Data_LG dd;
					for(TiXmlAttribute *k=i->FirstAttribute();k;k=k->Next())
					{
						string c=CDuiCharConver::UTF8ToANSI(k->Name());
						string a=CDuiCharConver::UTF8ToANSI(k->Value());
						dd.Head[c]=a;

					}
					for(TiXmlElement *m=i->FirstChildElement();m;m=m->NextSiblingElement())
					{
						map<string,string>xx;
						for(TiXmlAttribute *h=m->FirstAttribute();h;h=h->Next())
						{
							string d=CDuiCharConver::UTF8ToANSI(h->Name());
							string b=CDuiCharConver::UTF8ToANSI(h->Value());
							xx[d]=b;	
						}
						dd.Body.push_back(xx);
					}
					NetData[id].push(dd);
				}


			}
		}

		return true;
	}
	return false;
}

queue<Data_LG> HttpRequest::LookUpClassRoom(string ip,string po,ServerModel model,string token,string params,string id)
{
	string mainUrl;
	switch(model)
	{
	case embed:
		mainUrl="http://"+ip+":"+po+"/"+embedModel+"?";
		break;
	case iMac:
		mainUrl="http://"+ip+":"+po+"/"+iMacModel+"?";
		break;
	case cloud:
		mainUrl="http://"+ip+":"+po+"/"+lgCould+"?";
		break;
	}
	string httpArg=mainUrl+"type=luboclasslist&token="+token+params;
	queue<Data_LG>qlist;
	if(startRequestMainDataNode(httpArg,callLBClassData,id).status)
	{
		while(!NetData[id].empty())
		{
			qlist.push(NetData[id].front());
			NetData[id].pop();
		}
		return qlist;
	}
	else
	{
		return qlist;
	}
}

bool callSubData(TiXmlElement *node,string id)
{
	while(!NetEasyData[id].empty())
		{
			NetEasyData[id].pop();
		}
		if(node)
		{
			for(TiXmlElement *i=node->FirstChildElement()->FirstChildElement();i!=NULL;i=i->NextSiblingElement())
			{
				NetEasyData[id].push(CMyCharConver::UTF8ToANSI(string(i->FirstChild()->Value())));
			}
			//return true;
		}
		return false;
}
string HttpRequest::LookUpUrl(string ip,string po,ServerModel model,string token,string name,string id)
{
	string mainUrl;
	switch(model)
	{
	case embed:
		mainUrl="http://"+ip+":"+po+"/"+embedModel+"?";
		break;
	case iMac:
		mainUrl="http://"+ip+":"+po+"/"+iMacModel+"?";
		break;
	case cloud:
		mainUrl="http://"+ip+":"+po+"/"+lgCould+"?";
		break;
	}
	string httpArg=mainUrl+"type=queryurl&token="+token+"&name="+name;
	string res;
	if(startRequestMainDataNode(httpArg,callSubData,id).status)
	{
		while(!NetEasyData[id].empty())
			{
				res=NetEasyData[id].front();
				NetEasyData[id].pop();
			}
		return res;
	}
	else
	{
		return res;
	}
}

netCode HttpRequest::DirControl(string ip,string po,ServerModel model, string token,DIR dir,string args)
{
	string mainUrl;
	switch(model)
	{
	case embed:
		mainUrl="http://"+ip+":"+po+"/"+embedModel+"?";
		break;
	case iMac:
		mainUrl="http://"+ip+":"+po+"/"+iMacModel+"?";
		break;
	case cloud:
		mainUrl="http://"+ip+":"+po+"/"+lgCould+"?";
		break;
	}	 
	string param;

	switch(dir)
	{
	case DOWN:param="&turn=2";break;
	case UP:param="&turn=1";break;
	case LEFT:param="&turn=3";break;
	case RIGHT:param="&turn=4";break;
	case STOP:param="&turn=0";break;
	case RESET:param="&turn=5";break;
	}
	
	string httpArg=mainUrl+"type=dirptz&token="+token+param+args;
	
	return startRequestSimple(httpArg);
}

netCode HttpRequest::ZoomControl(string ip,string po,ServerModel model, string token,ZOOM zoom,string args)
{
	string mainUrl;
	switch(model)
	{
	case embed:
		mainUrl="http://"+ip+":"+po+"/"+embedModel+"?";
		break;
	case iMac:
		mainUrl="http://"+ip+":"+po+"/"+iMacModel+"?";
		break;
	case cloud:
		mainUrl="http://"+ip+":"+po+"/"+lgCould+"?";
		break;
	}	 
	string param;

	switch(zoom)
	{
	case ZOOMUP:param="&zoom=100";break;
	case ZOOMDOWN:param="&zoom=-100";break;
	}

	string httpArg=mainUrl+"type=dirptz&token="+token+param+args;
	return startRequestSimple(httpArg);
}
netCode HttpRequest::PlaceControl(string ip,string port,ServerModel model,string token,string param)
{
	string mainUrl;
	switch(model)
	{
	case embed:
		mainUrl="http://"+ip+":"+port+"/"+embedModel+"?";
		break;
	case iMac:
		mainUrl="http://"+ip+":"+port+"/"+iMacModel+"?";
		break;
	case cloud:
		mainUrl="http://"+ip+":"+port+"/"+lgCould+"?";
		break;
	}	 
	string httpArg=mainUrl+"type=dirptz&token="+token+param;
	return startRequestSimple(httpArg);
}
netCode HttpRequest::SystemSetting(string ip, string po, ServerModel model, string token, UINT option)
{
	//option值—> 0:重启服务，1:重启服务器，2关闭服务器
	if(option<0 || option>2)
	{
		netCode res;
		res.status=false;
		res.msg="param error";
	}
	string mainUrl;
	switch (model)
	{
	case embed:
		mainUrl = "http://" + ip + ":" + po + "/" + embedModel + "?";
		break;
	case iMac:
		mainUrl = "http://" + ip + ":" + po + "/" + iMacModel + "?";
		break;
	case cloud:
		mainUrl = "http://" + ip + ":" + po + "/" + lgCould + "?";
		break;
	}
	string param;
	switch(option)
	{
	case 0:param="&option=0";break;
	case 1:param="&option=1";break;
	case 2:param="&option=2";break;
	}
	string httpArg = mainUrl + "type=sysconfig&token=" + token +param;
	
	return startRequestSimple(httpArg);

}

netCode HttpRequest::LogOut(string ip, string po, ServerModel model, string token,string userName)
{
	string mainUrl;
	switch(model)
	{
	case embed:
		mainUrl="http://"+ip+":"+po+"/"+embedModel+"?";
		break;
	case iMac:
		mainUrl="http://"+ip+":"+po+"/"+iMacModel+"?";
		break;
	case cloud:
		mainUrl="http://"+ip+":"+po+"/"+lgCould+"?";
		break;
	}
	string httpArg=mainUrl+"type=logout&userName="+userName+"&token="+token;
	return startRequestSimple(httpArg);
}

queue<string> HttpRequest::LookUpRecord(string ip, string po, ServerModel model, string token,string id)
{
	string mainUrl;
	switch (model)
	{
	case embed:
		mainUrl = "http://" + ip + ":" + po + "/" + embedModel + "?";
		break;
	case iMac:
		mainUrl = "http://" + ip + ":" + po + "/" + iMacModel + "?";
		break;
	case cloud:
		mainUrl = "http://" + ip + ":" + po + "/" + lgCould + "?";
		break;
	}
	string httpArg = mainUrl + "type=queryrecord&token=" + token ;
	string type, code, msg;
	queue<string>qlist;
	if(startRequestMainDataNode(httpArg,callSubData,id).status)
	{
		while(!NetEasyData[id].empty())
		{
			qlist.push(NetEasyData[id].front());
			NetEasyData[id].pop();
		}
		return qlist;
	}
	else
	{
		return qlist;
	}
}

netCode HttpRequest::RecordSetting(string ip, string po, ServerModel model, string token,unsigned int state)
{
	if(state<0 || state>2)
	{
		netCode res;
		res.msg="param error";
		res.status=false;
		return res;
	}
	string mainUrl;
	switch (model)
	{
	case embed:
		mainUrl = "http://" + ip + ":" + po + "/" + embedModel + "?";
		break;
	case iMac:
		mainUrl = "http://" + ip + ":" + po + "/" + iMacModel + "?";
		break;
	case cloud:
		mainUrl = "http://" + ip + ":" + po + "/" + lgCould + "?";
		break;
	}
	string param;
	switch(state)
	{
	case 0:param="&state=0";break;
	case 1:param="&state=1";break;
	case 2:param="&state=2";break;
	}
	string httpArg = mainUrl + "type=record&token=" + token +param;

	return startRequestSimple(httpArg);
}

netCode HttpRequest::SetView(string ip, string po, ServerModel model, string token, unsigned int view)
{
	string mainUrl;
	switch (model)
	{
	case embed:
		mainUrl = "http://" + ip + ":" + po + "/" + embedModel + "?";
		break;
	case iMac:
		mainUrl = "http://" + ip + ":" + po + "/" + iMacModel + "?";
		break;
	case cloud:
		mainUrl = "http://" + ip + ":" + po + "/" + lgCould + "?";
		break;
	}
	string param;
	stringstream s;
	s<<"&view="<<view;
	s>>param;
	s.clear();
	string httpArg = mainUrl + "type=setdirview&token=" + token +param;

	return startRequestSimple(httpArg);
}