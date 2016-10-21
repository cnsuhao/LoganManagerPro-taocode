#include "Net.h"


Net::Net()
{
}

Net::~Net()
{
}

string  Net::GetRespond(string requestHttpArg)
{
	CURL *curl;

	// struct DATA chunk;

	//chunk.mem=NULL; 
	//chunk.size = 0;
	CRITICAL_SECTION sec;
	string buffer;
	InitializeCriticalSection(&sec);
	EnterCriticalSection(&sec);
	curl_global_init(CURL_GLOBAL_ALL);

	curl = curl_easy_init();

	curl_easy_setopt(curl, CURLOPT_URL, requestHttpArg.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buffer);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 5);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);


	/*curl_easy_setopt(curl, CURLOPT_URL, requestHttpArg.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);

	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buffer);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");*/

	curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	curl_global_cleanup();
	LeaveCriticalSection(&sec);
	DeleteCriticalSection(&sec);
	return buffer;

}