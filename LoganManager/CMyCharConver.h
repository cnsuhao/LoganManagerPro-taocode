#pragma once
#include <Windows.h>
#include <string>
using namespace std;

class CMyCharConver
{
public:

	static wstring ANSIToUnicode( const string str )
	{
		int  len = 0;
		len = str.length();
		int  unicodeLen = ::MultiByteToWideChar( CP_ACP,
			0,
			str.c_str(),
			-1,
			NULL,
			0 );  
		wchar_t *  pUnicode;  
		pUnicode = new  wchar_t[unicodeLen+1];  
		memset(pUnicode,0,(unicodeLen+1)*sizeof(wchar_t));  
		::MultiByteToWideChar( CP_ACP,
			0,
			str.c_str(),
			-1,
			(LPWSTR)pUnicode,
			unicodeLen );  
		wstring  rt;  
		rt = ( wchar_t* )pUnicode;
		delete  pUnicode; 

		return  rt;  
	};

	static string UnicodeToANSI( const wstring str )
	{
		char*     pElementText;
		int    iTextLen;

		iTextLen = WideCharToMultiByte( CP_ACP,
			0,
			str.c_str(),
			-1,
			NULL,
			0,
			NULL,
			NULL );
		pElementText = new char[iTextLen + 1];
		memset( ( void* )pElementText, 0, sizeof( char ) * ( iTextLen + 1 ) );
		::WideCharToMultiByte( CP_ACP,
			0,
			str.c_str(),
			-1,
			pElementText,
			iTextLen,
			NULL,
			NULL );
		string strText;
		strText = pElementText;
		delete[] pElementText;
		return strText;
	};

	static wstring UTF8ToUnicode( const string str )
	{
		int  len = 0;
		len = str.length();
		int  unicodeLen = ::MultiByteToWideChar( CP_UTF8,
			0,
			str.c_str(),
			-1,
			NULL,
			0 );  
		wchar_t *  pUnicode;  
		pUnicode = new  wchar_t[unicodeLen+1];  
		memset(pUnicode,0,(unicodeLen+1)*sizeof(wchar_t));  
		::MultiByteToWideChar( CP_UTF8,
			0,
			str.c_str(),
			-1,
			(LPWSTR)pUnicode,
			unicodeLen );  
		wstring  rt;  
		rt = ( wchar_t* )pUnicode;
		delete  pUnicode; 

		return  rt;  
	};

	static string UnicodeToUTF8( const wstring str )
	{
		char*     pElementText;
		int    iTextLen;
		iTextLen = WideCharToMultiByte( CP_UTF8,
			0,
			str.c_str(),
			-1,
			NULL,
			0,
			NULL,
			NULL );
		pElementText = new char[iTextLen + 1];
		memset( ( void* )pElementText, 0, sizeof( char ) * ( iTextLen + 1 ) );
		::WideCharToMultiByte( CP_UTF8,
			0,
			str.c_str(),
			-1,
			pElementText,
			iTextLen,
			NULL,
			NULL );
		string strText;
		strText = pElementText;
		delete[] pElementText;
		return strText;
	};

	static string ANSIToUTF8( const string str )
	{ 
		wstring srtTemp = ANSIToUnicode(str);
		return UnicodeToUTF8(srtTemp);		
	};

	static string UTF8ToANSI( const string str )
	{
		wstring srtTemp = UTF8ToUnicode(str);
		return UnicodeToANSI(srtTemp);
	};

	static string UrlEncode(const string url) // url °Ù·ÖºÅ±àÂë
	{
		string src = ANSIToUTF8(url);
		static    char hex[] = "0123456789ABCDEF";
		string dst;
		for (size_t i = 0; i < src.size(); i++)
		{
			unsigned char ch = src[i];
			if(ch == 92)
				ch = '//';

			if ( isalnum(ch) || IsReserved_Character(ch) ) // ×ÖÄ¸»òÊý×Ö»ò±£Áô·ûºÅ
			{
				dst += ch;
			}
			else if(ch != '\n')
			{
				if (src[i] == ' ')
				{
					dst += '+';
				}
				else
				{
					unsigned char c = static_cast<unsigned char>(src[i]);
					dst += '%';
					dst += hex[c / 16];
					dst += hex[c % 16];
				}
			}
		}

		return dst;
	}

	static string UrlDecode(const char *src)  
	{  
		int len = strlen(src);  
		string dst;

		for(; *src != '\0'; src++)  
		{  
			if(*src == '%')  
			{  
				int code;
				if(sscanf(src+1, "%2x", &code) != 1)  
					code = '?';  
				dst += code;  
				src += 2;  
			}  
			else if(*src == '+')  
				dst += ' ';  
			else  
				dst += *src;  
		}  

		dst = UTF8ToANSI(dst);
		return dst;  
	} 

private:
	static bool IsReserved_Character(char c)  // url±£Áô·ûºÅ
	{
		bool bRt = false;
		string sStr(17,'//');
		sStr[1] = '!';
		sStr[2] = '*';
		sStr[3] = '.';
		sStr[4] = '(';
		sStr[5] = ')';
		sStr[6] = ';';
		sStr[7] = ':';
		sStr[8] = '@';
		sStr[9] = '&';
		sStr[10] = '=';
		sStr[11] = '+';
		sStr[12] = '$';
		sStr[13] = ',';
		sStr[14] = '?';
		sStr[15] = '#';
		sStr[16] = '[';
		sStr[17] = ']';

		if(sStr.find(c) != string::npos)
			return true;
		else
			return false;
	};

};