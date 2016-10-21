#pragma once
#include "sqlite3.h"
#include "stdafx.h"

#define DB_FILE					"../bin/config/userinfo.db"

#pragma  comment(lib,"../lib/sqlite3.lib")

class DataBase
{
public:
	DataBase(void);
	~DataBase(void);
	static bool Save(string user,string pwd,string ip);
	static bool del(string user,string pwd,string ip);
	static bool Modify(string user, string pwd, string ip, bool rem, bool autolog);
	static vector<vector<string>> Load(string ip);
	static bool exist(string user,string ip);
protected:
	static vector<vector<string>>User;
	static bool isExist;
	static int Result(void *NotUsed, int argc, char **argv, char **azColName);
};
