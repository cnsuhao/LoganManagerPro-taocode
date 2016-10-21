#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <map>
using namespace std;

#define  COMMENT_CHAR '#'

class ReadConfigure
{
private:
	static void Trim(string &str);
	static bool IsSpace(char c);
	static bool IsCommentChar(char c);
	static bool ParseLine(const string &line,string &key,string &values);
public:
	static map<string,string> Read(const string &FileName);
};