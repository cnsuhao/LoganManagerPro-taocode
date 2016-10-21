#ifndef __UITABLE_H__
#define __UITABLE_H__

#pragma once
#include <vector>
using namespace std;

namespace DuiLib 
{
	/////////////////////////////////////////////////////////////////////////////////////
	//
	class UILIB_API CTableHeaderUI : public CListHeaderItemUI
	{
	public:
		CTableHeaderUI();
		~CTableHeaderUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void DoEvent(TEventUI& event);
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//
	class UILIB_API CTableElementUI : public CListHeaderItemUI
	{
	public:
		CTableElementUI();
		~CTableElementUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//
	class UILIB_API CTableUI : public CListUI
	{
	public:
		CTableUI();
		~CTableUI();

		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetCol(int iColCount,int iColWeidth);
		void SetRow(int iRowCount,int iRowHeight);
		void SetColWeidth(int iColIndex,int iColWeidth);
		void SetRowHeight(int iRowIndex,int iRowHeight);

	private:
		int	m_iCol;//列数
		int m_iRow;//行数(不包括表头)
		int m_iDefColWeidth;
		int m_iDefRowHeight;
		vector<int> m_vColWeidth;
		vector<int> m_vRowHeight;
	};
}

#endif // __UITABLE_H__