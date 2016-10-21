#include "StdAfx.h"

namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	CTableHeaderUI::CTableHeaderUI()
	{
		
	}

	CTableHeaderUI::~CTableHeaderUI()
	{

	}

	LPCTSTR CTableHeaderUI::GetClass() const
	{
		return _T("TableHeaderUI");
	}

	LPVOID CTableHeaderUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_LISTHEADER) == 0 ) 
			return this;
		return CListHeaderItemUI::GetInterface(pstrName);
	}

	void CTableHeaderUI::DoEvent(TEventUI& event)
	{
		if( event.Type == UIEVENT_MOUSEMOVE )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 )
			{
				RECT rc = m_rcItem;
				if( m_iSepWidth >= 0 )
				{
					rc.right -= ptLastMouse.x - event.ptMouse.x;
				}
				else 
				{
					rc.left -= ptLastMouse.x - event.ptMouse.x;
				}
				if( rc.right - rc.left > GetMinWidth() ) 
				{
					m_cxyFixed.cx = rc.right - rc.left;
					ptLastMouse = event.ptMouse;
					if( GetParent() ) 
						GetParent()->NeedParentUpdate();
					///////////////////////////////
					//更改各列宽
					
					///////////////////////////////
				}
			}
			return;
		}
		__super::DoEvent(event);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	CTableElementUI::CTableElementUI()
	{
		SetDragable(false);
		//SetSepWidth(0);
	}

	CTableElementUI::~CTableElementUI()
	{
		
	}

	LPCTSTR CTableElementUI::GetClass() const
	{
		return _T("TableElementUI");
	}

	LPVOID CTableElementUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_TABLEELEMENT) == 0 ) 
			return this;
		return CListHeaderItemUI::GetInterface(pstrName);
	}

	UINT CTableElementUI::GetControlFlags() const
	{
		if( IsEnabled() && m_iSepWidth != 0 ) 
			return UIFLAG_SETCURSOR;
		else return 0;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	CTableUI::CTableUI():
		m_iCol(0),
		m_iRow(0),
		m_iDefColWeidth(50),
		m_iDefRowHeight(25)
	{

	}

	 CTableUI::~CTableUI()
	{

	}

	LPCTSTR CTableUI::GetClass() const
	{
		return _T("TableUI");
	}

	UINT CTableUI::GetControlFlags() const
	{
		return UIFLAG_TABSTOP;
	}

	LPVOID CTableUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_TABLE) == 0 ) return static_cast<CTableUI*>(this);
		return CListUI::GetInterface(pstrName);
	}

	void CTableUI::SetCol(int iColCount,int iColWeidth)
	{
		if(m_iCol)
			return;

		for (int i=0; i<iColCount; ++i)
		{
			CTableHeaderUI* pItem = new CTableHeaderUI;
			pItem->SetFixedWidth(iColWeidth);
			CDuiString sText;
			sText.Format("%s-%d","Header",i+1);
			pItem->SetText(sText);
			pItem->SetSepImage("list_header_sep.png");
			m_pHeader->Add(pItem);

			m_vColWeidth.push_back(iColWeidth);
		}

		m_iCol = iColCount;
	}

	void CTableUI::SetRow(int iRowCount,int iRowHeight)
	{
		if(m_iRow || m_iCol==0)
			return;
		
		m_pHeader->SetFixedHeight(iRowHeight);

		for (int i=0; i<iRowCount; ++i)
		{
			CListContainerElementUI* pRow = new CListContainerElementUI;
			pRow->SetFixedHeight(iRowHeight);
			Add(pRow);
			for (int j=0; j<m_iCol; ++j)
			{
				CTableElementUI* pItem = new CTableElementUI;
				pItem->SetFixedWidth(m_vColWeidth.at(j));
				CDuiString sText;
				sText.Format("%s:%d-%d","Element ",i+1,j+1);
				pItem->SetText(sText);
				pItem->SetSepImage("list_header_sep.png");
				pRow->Add(pItem);
			}

			m_vColWeidth.push_back(iRowHeight);
		}

		m_iRow = iRowCount;
	}

	void CTableUI::SetColWeidth(int iColIndex,int iColWeidth)
	{

	}

	void CTableUI::SetRowHeight(int iRowIndex,int iRowHeight)
	{

	}
	
	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
}