#include "StdAfx.h"
#include "ReadConfig.h"


bool ReadConfigure::IsSpace(char c)
{
	if(' '==c||'\t'==c)
		return true;
	else
		return false;
}

bool ReadConfigure::IsCommentChar(char c)
{
	switch(c)
	{
	case COMMENT_CHAR:
		return true;
	default:
		return false;
	}
}

bool ReadConfigure::ParseLine(const string &line,string &key,string &values)
{
	if(line.empty())
		return false;
	int start_pos=0,end_pos=line.size()-1,pos; 

	if((pos=line.find(COMMENT_CHAR))!=-1)
	{
		if(pos==0)
		{
			return false;
		}
		end_pos=pos-1;
	}

	string sub_line=line.substr(start_pos,end_pos-start_pos+1);

	if((pos=sub_line.find('='))==-1)
	{
		return false;
	}
	key=sub_line.substr(0,pos);
	values=sub_line.substr(pos+1,end_pos-pos);
	Trim(key);
	if(key.empty())
	{
		return false;
	}
	Trim(values);
	return true;
}

void ReadConfigure::Trim(string &str)
{
	if(str.empty())
		return;
	int i,start_pos,end_pos;
	for(i=0;i<str.size();++i)//去掉前面多余的空格
	{
		if(!IsSpace(str[i]))
		{
			break;
		}
	}

	if(i==str.size())//一片空白
	{
		str="";
		return;
	}

	start_pos=i;

	for(i=str.size()-1;i>=0;--i)
	{
		if(!IsSpace(str[i]))
		{
			break;
		}
	}
	end_pos=i;
	str=str.substr(start_pos,end_pos-start_pos+1);
}
map<string,string> ReadConfigure::Read(const string &FileName)
{
	map<string,string>m;
	m.clear();
	ifstream infile(FileName.c_str());
	if(!infile)
	{
		return m;
	}
	string line,key,values;
	while(getline(infile,line))
	{
		if(ParseLine(line,key,values))
		{
			m[key]=values;
		}
	}
	infile.close();
	return m;

}