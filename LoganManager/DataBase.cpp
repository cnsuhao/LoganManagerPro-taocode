#include "StdAfx.h"
#include "DataBase.h"

DataBase::DataBase(void)
{
}

DataBase::~DataBase(void)
{
}

bool DataBase::isExist=false;
vector<vector<string>> DataBase::User;

bool DataBase::Save(string user,string pwd,string ip)
{
	sqlite3 *pdb=NULL;
	int res=sqlite3_open(DB_FILE,&pdb);
	if(res!=SQLITE_OK)
	{
		sqlite3_close(pdb);
		return false;
	}
	char *err;
	string sql="insert into userTable values ('";
	sql+=user;
	sql+="','";
	sql+=pwd;
	sql+="','";
	sql+=ip;
	sql+="');";
	res=sqlite3_exec(pdb,sql.c_str(),0,0,&err);
	if(res!=SQLITE_OK)
	{
		sqlite3_close(pdb);
		return false;
	}
	sqlite3_close(pdb);
	return true;
}
int DataBase::Result(void *NotUsed, int argc, char **argv, char **azColName)  
{  
	vector<string>tempUnit(5);
	string s;
	for(int i=0;i<argc;i++)
	{
		s=string(argv[i]);
		tempUnit[i]=s;
	}
	User.push_back(tempUnit);
	return 0;  


}
vector<vector<string>> DataBase::Load(std::string ip)
{
	vector<vector<string>>dvres;
	sqlite3 *pdb=NULL;
	int res=sqlite3_open(DB_FILE,&pdb);
	if(res!=SQLITE_OK)
	{
		sqlite3_close(pdb);
		return dvres;
	}
	char *err;
	string sql="select * from userTable where ip='";
	sql+=ip;
	sql+="';";
	res=sqlite3_exec(pdb,sql.c_str(),Result,0,&err);
	dvres=User;
	User.clear();
	if(res!=SQLITE_OK)
	{
		sqlite3_close(pdb);
		return dvres;
	}
	sqlite3_close(pdb);
	return dvres;
}

bool isExist=false;
static int ex(void*x,int argc,char** argv,char** szName)
{
	if(argc==0)
	{
		isExist==false;
		return 1;
	}
	else
	{
		isExist=true;
		return 0;
	}

}


bool DataBase::exist(string user,string ip)
{
	sqlite3 *pdb=NULL;
	int res=sqlite3_open(DB_FILE,&pdb);
	if(res!=SQLITE_OK)
	{
		sqlite3_close(pdb);
		return false;
	}
	char *err;
	string sql="select * from userTable where username='";
	sql+=user;
	sql+="' and ip='";
	sql+=ip;
	sql+="';";
	res=sqlite3_exec(pdb,sql.c_str(),ex,0,&err);
	if(res!=SQLITE_OK)
	{
		sqlite3_close(pdb);
		return false;
	}
	sqlite3_close(pdb);
	return isExist;
}


bool DataBase::del(string user,string pwd,string ip)
{
	sqlite3 *pdb=NULL;
	int res=sqlite3_open(DB_FILE,&pdb);
	if(res!=SQLITE_OK)
	{
		sqlite3_close(pdb);
		return false;
	}
	char *err;
	char sqlUpdate[100];
	char sqlInsert[100];
	//如果存在就删除
	sprintf(sqlUpdate,"delete from userTable where username='%s' and ip='%s';",user.c_str(),ip.c_str());
	res=sqlite3_exec(pdb,sqlUpdate,0,0,&err);
	if(res!=SQLITE_OK)
	{
		sqlite3_close(pdb);
		return false;
	}
	sqlite3_close(pdb);
	return true;
}

bool DataBase::Modify(string user, string pwd, string ip,bool rem,bool autolog)
{
	sqlite3 *pdb = NULL;
	int res = sqlite3_open(DB_FILE, &pdb);
	if (res != SQLITE_OK)
	{
		sqlite3_close(pdb);
		return false;
	}
	char *err;
	char sqlUpdate[100];
	char sqlInsert[100];
	//如果存在就删除
	sprintf(sqlUpdate, "delete from userTable where username='%s' and ip='%s';", user.c_str(), ip.c_str());
	res = sqlite3_exec(pdb, sqlUpdate, 0, 0, &err);
	//然后插入
	sprintf(sqlInsert, "insert into userTable values ('%s','%s','%s','%d','%d');", user.c_str(), pwd.c_str(), ip.c_str(),rem?1:0,autolog?1:0);
	res = sqlite3_exec(pdb, sqlInsert, 0, 0, &err);
	if (res != SQLITE_OK)
	{
		sqlite3_close(pdb);
		return false;
	}
	sqlite3_close(pdb);
	return true;
}