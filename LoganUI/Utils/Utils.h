#ifndef __UTILS_H__
#define __UTILS_H__

#pragma once
#include <time.h>
#include <string>
using namespace std;

namespace DuiLib
{
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class STRINGorID
	{
	public:
		STRINGorID(LPCTSTR lpString) : m_lpstr(lpString)
		{ }
		STRINGorID(UINT nID) : m_lpstr(MAKEINTRESOURCE(nID))
		{ }
		LPCTSTR m_lpstr;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CPoint : public tagPOINT
	{
	public:
		CPoint();
		CPoint(const POINT& src);
		CPoint(int x, int y);
		CPoint(LPARAM lParam);
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CSize : public tagSIZE
	{
	public:
		CSize();
		CSize(const SIZE& src);
		CSize(const RECT rc);
		CSize(int cx, int cy);
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiRect : public tagRECT
	{
	public:
		CDuiRect();
		CDuiRect(const RECT& src);
		CDuiRect(int iLeft, int iTop, int iRight, int iBottom);

		int GetWidth() const;
		int GetHeight() const;
		void Empty();
		bool IsNull() const;
		void Join(const RECT& rc);
		void ResetOffset();
		void Normalize();
		void Offset(int cx, int cy);
		void Inflate(int cx, int cy);
		void Deflate(int cx, int cy);
		void Union(CDuiRect& rc);
		void InflateRect(int nLeft,int nTop,int nRight,int nBottom);
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CStdPtrArray
	{
	public:
		CStdPtrArray(int iPreallocSize = 0);
		CStdPtrArray(const CStdPtrArray& src);
		~CStdPtrArray();

		void Empty();
		void Resize(int iSize);
		bool IsEmpty() const;
		int Find(LPVOID iIndex) const;
		bool Add(LPVOID pData);
		bool SetAt(int iIndex, LPVOID pData);
		bool InsertAt(int iIndex, LPVOID pData);
		bool Remove(int iIndex);
		int GetSize() const;
		LPVOID* GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;

	protected:
		LPVOID* m_ppVoid;
		int m_nCount;
		int m_nAllocated;
	};

	template<typename T = LPVOID>
	class UILIB_API TStdPtrArray : public CStdPtrArray
	{
	public:
		TStdPtrArray(int iPreallocSize = 0):CStdPtrArray(iPreallocSize){};
		TStdPtrArray(const TStdPtrArray& src):CStdPtrArray(src){};
		int Find(T iIndex) const {return CStdPtrArray::Find(iIndex);};
		bool Add(T pData){return CStdPtrArray::Add(pData);};
		bool SetAt(int iIndex, T pData){return CStdPtrArray::SetAt(iIndex,pData);};
		bool InsertAt(int iIndex, T pData){return CStdPtrArray::InsertAt(iIndex,pData);};
		bool Remove(int iIndex,bool bDeleteObj = false){
			if(bDeleteObj){
				T p = GetAt(iIndex);
				if(p)
					delete p;
			}
			return CStdPtrArray::Remove(iIndex);
		}
		T* GetData(){return static_cast<T>(CStdPtrArray::GetData());};
		T GetAt(int iIndex) const {return static_cast<T>(CStdPtrArray::GetAt(iIndex));};
		T operator[] (int nIndex) const{return static_cast<T>(CStdPtrArray::operator[](nIndex));};
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CStdValArray
	{
	public:
		CStdValArray(int iElementSize, int iPreallocSize = 0);
		~CStdValArray();

		void Empty();
		bool IsEmpty() const;
		bool Add(LPCVOID pData);
		bool Remove(int iIndex);
		int GetSize() const;
		LPVOID GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;

	protected:
		LPBYTE m_pVoid;
		int m_iElementSize;
		int m_nCount;
		int m_nAllocated;
	};

	template<typename T = LPVOID,typename T1 = T>
	class UILIB_API TStdValArray : public CStdValArray
	{
	public:
		TStdValArray(int iElementSize = sizeof(T), int iPreallocSize = 0):CStdValArray(iElementSize,iPreallocSize){};
		bool Add(const T1 pData){return CStdValArray::Add((LPCVOID)&pData);};
		bool InsertAt(int iIndex,const T pData){
			if( iIndex == m_nCount ) return Add(pData);
			if( iIndex < 0 || iIndex > m_nCount ) return false;
			if( ++m_nCount >= m_nAllocated) {
				int nAllocated = m_nAllocated * 2;
				if( nAllocated == 0 ) nAllocated = 11;
				LPBYTE pVoid = static_cast<LPBYTE>(realloc(m_pVoid, nAllocated * m_iElementSize));
				if( pVoid != NULL ) {
					m_nAllocated = nAllocated;
					m_pVoid = pVoid;
				}
				else {
					--m_nCount;
					return false;
				}
			}

			memmove(&m_pVoid + (iIndex+1) * m_iElementSize, &m_pVoid + iIndex * m_iElementSize,m_iElementSize);
			::CopyMemory(m_pVoid + (iIndex * m_iElementSize), &pData, m_iElementSize);
			return true;
		}
		T GetData(){return static_cast<T>(CStdValArray::GetData());};
		T GetAt(int iIndex) const {return *static_cast<T1*>(CStdValArray::GetAt(iIndex));};
		T operator[] (int nIndex) const{return (T)CStdValArray:::operator[](nIndex);};
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiString
	{
	public:
		enum { MAX_LOCAL_STRING_LEN = 63 };

		CDuiString();
		CDuiString(const TCHAR ch);
		CDuiString(const CDuiString& src);
		CDuiString(LPCTSTR lpsz, int nLen = -1);
		~CDuiString();

		void Empty();
		int GetLength() const;
		bool IsEmpty() const;
		TCHAR GetAt(int nIndex) const;
		void Append(LPCTSTR pstr);
		void Assign(LPCTSTR pstr, int nLength = -1);
		LPCTSTR GetData() const;

		void TrimLeft();		// 去除左边空格
		void TrimRight();	// 去除右边空格
		void TrimAll();

		void SetAt(int nIndex, TCHAR ch);
		operator LPCTSTR() const;

		TCHAR operator[] (int nIndex) const;
		const CDuiString& operator=(const CDuiString& src);
		const CDuiString& operator=(const TCHAR ch);
		const CDuiString& operator=(LPCTSTR pstr);
#ifdef _UNICODE
		const CDuiString& CDuiString::operator=(LPCSTR lpStr);
		const CDuiString& CDuiString::operator+=(LPCSTR lpStr);
#else
		const CDuiString& CDuiString::operator=(LPCWSTR lpwStr);
		const CDuiString& CDuiString::operator+=(LPCWSTR lpwStr);
#endif
		CDuiString operator+(const CDuiString& src) const;
		CDuiString operator+(LPCTSTR pstr) const;
		const CDuiString& operator+=(const CDuiString& src);
		const CDuiString& operator+=(LPCTSTR pstr);
		const CDuiString& operator+=(const TCHAR ch);

		bool operator == (LPCTSTR str) const;
		bool operator != (LPCTSTR str) const;
		bool operator <= (LPCTSTR str) const;
		bool operator <  (LPCTSTR str) const;
		bool operator >= (LPCTSTR str) const;
		bool operator >  (LPCTSTR str) const;

		int Compare(LPCTSTR pstr) const;
		int CompareNoCase(LPCTSTR pstr) const;

		void MakeUpper();
		void MakeLower();

		CDuiString Left(int nLength) const;
		CDuiString Mid(int iPos, int nLength = -1) const;
		CDuiString Right(int nLength) const;

		int Find(TCHAR ch, int iPos = 0) const;
		int Find(LPCTSTR pstr, int iPos = 0) const;
		int ReverseFind(TCHAR ch) const;
		int Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo);

		int __cdecl Format(LPCTSTR pstrFormat, ...);
		int __cdecl SmallFormat(LPCTSTR pstrFormat, ...);

	protected:
		LPTSTR m_pstr;
		TCHAR m_szBuffer[MAX_LOCAL_STRING_LEN + 1];
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//
	class UILIB_API CDuiCharConver
	{
	public:
		CDuiCharConver();
		static wstring ANSIToUnicode( const string& str );
		static string  UnicodeToANSI( const wstring& str );
		static wstring UTF8ToUnicode( const string& str );
		static string  UnicodeToUTF8( const wstring& str );
		static string  ANSIToUTF8( const string& str );
		static string  UTF8ToANSI( const string& str );
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//时间处理类
	//this is a class based on second, that can handle time operater
	class UILIB_API CDuiTime 
	{
	public:
		CDuiTime();                                                               //use current time
		CDuiTime(time_t t);                                                       //use time_t time
		CDuiTime(int year, int month, int day, int hour, int minute, int second); //use specific time
		virtual ~CDuiTime();

		static CDuiTime get_current_time();

	private:
		void time_to_tm();

	public:                         //for get value
		time_t get_time() const;    //get time_t value
		int get_year() const;       //get year
		int get_month() const;      //get month
		int get_day() const;        //get day
		int get_hour() const;       //get hour
		int get_minute() const;     //get minute
		int get_second() const;     //get second
		int get_week_day() const;   //get the current day of week(from 0 to 6)
		int get_month_day() const;  //get the last days of current month
		int get_year_day() const;   //get the last days of current year(from 1.1)

		//for operator
		CDuiTime& operator=(const CDuiTime& timesrc);
		CDuiTime& operator=(const time_t& timesrc);
		CDuiTime operator+(const CDuiTime& tk) const;
		CDuiTime operator+(const time_t t) const;
		CDuiTime operator-(const CDuiTime& tk) const;
		CDuiTime operator-(const time_t t) const;
		CDuiTime& operator+=(const CDuiTime& tk);
		CDuiTime& operator+=(const time_t t);
		CDuiTime& operator-=(const CDuiTime& tk);
		CDuiTime& operator-=(const time_t t);
		int operator==(const CDuiTime& tk) const;
		int operator!=(const CDuiTime& tk) const;
		int operator>(const CDuiTime& tk) const;
		int operator>=(const CDuiTime& tk) const;
		int operator<(const CDuiTime& tk) const;
		int operator<=(const CDuiTime& tk) const;

	protected:
		time_t t_;
		tm tm_;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	// 添加字体例如为XP系统添加微软雅黑

#ifndef tstring
	#ifdef UNICODE  
		typedef wstring tstring;  
	#else  
		typedef string  tstring;  
	#endif 
#endif

	class UILIB_API CDuiFont 
	{
	public:
		CDuiFont();
		~CDuiFont();

		/*@param filePath:字体文件相对本程序的路径，不需要加前缀\\，和后缀名 
		 *@param fontName:字体的名称，如宋体，微软雅黑  */
		bool AddFont(LPCTSTR szfilePath, LPCTSTR szFontName);  
		LPCTSTR GetFontName() const; 

	private:  
		tstring  m_strFilePath;  
		tstring  m_strFontName; 
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	struct TITEM
	{
		CDuiString Key;
		LPVOID Data;
		struct TITEM* pPrev;
		struct TITEM* pNext;
	};

	class UILIB_API CStdStringPtrMap
	{
	public:
		CStdStringPtrMap(int nSize = 83);
		~CStdStringPtrMap();

		void Resize(int nSize = 83);
		LPVOID Find(LPCTSTR key, bool optimize = true) const;
		bool Insert(LPCTSTR key, LPVOID pData);
		LPVOID Set(LPCTSTR key, LPVOID pData);
		bool Remove(LPCTSTR key);
		void RemoveAll();
		int GetSize() const;
		LPCTSTR GetAt(int iIndex) const;
		LPCTSTR operator[] (int nIndex) const;

	protected:
		TITEM** m_aT;
		int m_nBuckets;
		int m_nCount;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CWaitCursor
	{
	public:
		CWaitCursor();
		~CWaitCursor();

	protected:
		HCURSOR m_hOrigCursor;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class CVariant : public VARIANT
	{
	public:
		CVariant() 
		{ 
			VariantInit(this); 
		}
		CVariant(int i)
		{
			VariantInit(this);
			this->vt = VT_I4;
			this->intVal = i;
		}
		CVariant(float f)
		{
			VariantInit(this);
			this->vt = VT_R4;
			this->fltVal = f;
		}
		CVariant(LPOLESTR s)
		{
			VariantInit(this);
			this->vt = VT_BSTR;
			this->bstrVal = s;
		}
		CVariant(IDispatch *disp)
		{
			VariantInit(this);
			this->vt = VT_DISPATCH;
			this->pdispVal = disp;
		}

		~CVariant() 
		{ 
			VariantClear(this); 
		}
	};

}// namespace DuiLib

#endif // __UTILS_H__