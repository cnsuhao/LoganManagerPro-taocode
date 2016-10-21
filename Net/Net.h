
#pragma once

#ifndef __NET_API__
#define __NET_API__


#define WIN32_LEAN_AND_MEAN             
// Windows  
#include <windows.h>
#include <SDKDDKVer.h>
// C
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// other

#include <queue>
#include <stack>
#include <map>
#include <vector>
#include <string>

using namespace std;
/*------------------CURL------------------------*/
#include "../include/curl/curl.h"				//
#include "../include/curl/easy.h"				//
#include "../include/curl/types.h"				//
/*----------------------------------------------*/

#pragma comment(lib,"..//lib//libcurl.lib")

#ifdef BUILDING
# define NETAPI __declspec (dllexport)
#else
# define NETAPI __declspec (dllimport)
#endif


struct DATA
{
	char *mem;//保存的内容
	size_t size;//数据大小
};

//内存分配

inline void *memRealloc(void *ptr, size_t size)
{
	if (ptr)//如果存在但是内存不够
		return realloc(ptr, size);
	else
		return malloc(size);
}

// curl的回调函数实现数据的保存

static size_t writeToMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t realsize = size * nmemb;
	struct DATA *mem = (struct DATA *)data;

	mem->mem = (char *)memRealloc(mem->mem, mem->size + realsize + 1);
	if (mem->mem) {
		memcpy(&(mem->mem[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->mem[mem->size] = 0;
	}
	return realsize;
}


static size_t writeCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t sres = 0;
	std::string *buffer = (std::string*)data;
	if (ptr&&buffer)
	{
		buffer->append((char*)ptr, size*nmemb);
		sres = nmemb;
	}
	return sres;

}

class NETAPI Net
{
public:
	Net();
	~Net();
	string GetRespond(string requestHttpArg);
private:

};


#endif // !__NET_API__



