#include "stdafx.h"
#include "UITabLayout.h"

namespace DuiLib
{
	CTabLayoutUI::CTabLayoutUI() : m_iCurSel(-1)
	{
	}

	LPCTSTR CTabLayoutUI::GetClass() const
	{
		return _T("TabLayoutUI");
	}

	LPVOID CTabLayoutUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_TABLAYOUT) == 0 ) return static_cast<CTabLayoutUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	bool CTabLayoutUI::Add(CControlUI* pControl)
	{
		bool ret = CContainerUI::Add(pControl);
		if( !ret ) return ret;

		if(m_iCurSel == -1 && pControl->IsVisible())
		{
			m_iCurSel = GetItemIndex(pControl);
		}
		else
		{
			pControl->SetVisible(false);
		}

		return ret;
	}

	bool CTabLayoutUI::AddAt(CControlUI* pControl, int iIndex)
	{
		bool ret = CContainerUI::AddAt(pControl, iIndex);
		if( !ret ) return ret;

		if(m_iCurSel == -1 && pControl->IsVisible())
		{
			m_iCurSel = GetItemIndex(pControl);
		}
		else if( m_iCurSel != -1 && iIndex <= m_iCurSel )
		{
			m_iCurSel += 1;
		}
		else
		{
			pControl->SetVisible(false);
		}

		return ret;
	}

	bool CTabLayoutUI::Remove(CControlUI* pControl)
	{
		if( pControl == NULL) return false;

		int index = GetItemIndex(pControl);
		bool ret = CContainerUI::Remove(pControl);
		if( !ret ) return false;

		if( m_iCurSel == index)
		{
			if( GetCount() > 0 )
			{
				m_iCurSel=0;
				GetItemAt(m_iCurSel)->SetVisible(true);
			}
			else
				m_iCurSel=-1;
			NeedParentUpdate();
		}
		else if( m_iCurSel > index )
		{
			m_iCurSel -= 1;
		}

		return ret;
	}

	void CTabLayoutUI::RemoveAll()
	{
		m_iCurSel = -1;
		CContainerUI::RemoveAll();
		NeedParentUpdate();
	}

	int CTabLayoutUI::GetCurSel() const
	{
		return m_iCurSel;
	}

	bool CTabLayoutUI::SelectItem(int iIndex)
	{
		if( iIndex < 0 || iIndex >= m_items.GetSize() ) return false;
		if( iIndex == m_iCurSel ) return true;

		int iOldSel = m_iCurSel;
		m_iCurSel = iIndex;
		for( int it = 0; it < m_items.GetSize(); it++ )
		{
			if( it == iIndex ) {
				GetItemAt(it)->SetVisible(true);
				GetItemAt(it)->SetFocus();
				SetPos(m_rcItem);
			}
			else GetItemAt(it)->SetVisible(false);
		}
		NeedParentUpdate();

		if( m_pManager != NULL ) {
			m_pManager->SetNextTabControl();
			m_pManager->SendNotify(this, DUI_MSGTYPE_TABSELECT, m_iCurSel, iOldSel);
		}
		return true;
	}

	bool CTabLayoutUI::SelectItem( CControlUI* pControl )
	{
		int iIndex=GetItemIndex(pControl);
		if (iIndex==-1)
			return false;
		else
			return SelectItem(iIndex);
	}

	void CTabLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("selectedid")) == 0 ) SelectItem(_ttoi(pstrValue));
		return CContainerUI::SetAttribute(pstrName, pstrValue);
	}

	void CTabLayoutUI::SetPos(RECT rc)
	{
		CControlUI::SetPos(rc);
		rc = m_rcItem;

		// Adjust for inset
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;

		for( int it = 0; it < m_items.GetSize(); it++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it);
				continue;
			}

			if( it != m_iCurSel ) continue;

			RECT rcPadding = pControl->GetPadding();
			rc.left += rcPadding.left;
			rc.top += rcPadding.top;
			rc.right -= rcPadding.right;
			rc.bottom -= rcPadding.bottom;

			SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };

			SIZE sz = pControl->EstimateSize(szAvailable);
			if( sz.cx == 0 ) {
				sz.cx = MAX(0, szAvailable.cx);
			}
			if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
			if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();

			if(sz.cy == 0) {
				sz.cy = MAX(0, szAvailable.cy);
			}
			if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
			if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();

			RECT rcCtrl = { rc.left, rc.top, rc.left + sz.cx, rc.top + sz.cy};
			pControl->SetPos(rcCtrl);
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	CAnimationTabLayoutUI::CAnimationTabLayoutUI() : 
	CUIAnimation( *this ), 
		m_nPositiveDirection( 1 ),
		m_pCurrentControl( NULL ),
		m_bControlVisibleFlag( false ),
		m_bIsVerticalDirection( false ), 
		m_bRight(false),
		m_bBottom(false)
	{
	}

	LPCTSTR CAnimationTabLayoutUI::GetClass() const
	{
		return _T("AnimationTabLayoutUI");
	}

	LPVOID CAnimationTabLayoutUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("AnimationTabLayoutUI")) == 0 ) 
			return static_cast<CAnimationTabLayoutUI*>(this);
		return CTabLayoutUI::GetInterface(pstrName);
	}

	CControlUI* CAnimationTabLayoutUI::OwnerControlUI()
	{
		return this;
	}

	void CAnimationTabLayoutUI::SetVerticalDirection(bool bVer)
	{
		m_bIsVerticalDirection = bVer;
	}

	void CAnimationTabLayoutUI::SetRightToLeft(bool right)
	{
		if (m_bIsVerticalDirection)
			return;
		m_bRight = right;
	}

	void CAnimationTabLayoutUI::SetBottomToTop(bool bottom)
	{
		if (m_bIsVerticalDirection)
		{
			m_bBottom = bottom;
		}
	}

	bool CAnimationTabLayoutUI::SelectItem( int iIndex )
	{
		if( iIndex < 0 || iIndex >= m_items.GetSize() ) return false;
		if( iIndex == m_iCurSel ) return true;
		if( iIndex > m_iCurSel ) m_nPositiveDirection = -1;
		if( iIndex < m_iCurSel ) m_nPositiveDirection = 1;

		int iOldSel = m_iCurSel;
		m_iCurSel = iIndex;
		for( int it = 0; it < m_items.GetSize(); it++ )
		{
			if( it == iIndex ) {
				GetItemAt(it)->SetVisible(true);
				GetItemAt(it)->SetFocus();

				m_bControlVisibleFlag = false;
				m_pCurrentControl = static_cast<CControlUI*>(m_items[it]);
				if( NULL != m_pCurrentControl )
					m_pCurrentControl->SetVisible( true );
				AnimationSwitch();
			}
			else GetItemAt(it)->SetVisible(false);
		}
		NeedParentUpdate();

		if( m_pManager != NULL ) {
			m_pManager->SetNextTabControl();
			m_pManager->SendNotify(this, _T("tabselect"), m_iCurSel, iOldSel);
		}
		return true;
	}

	void CAnimationTabLayoutUI::AnimationSwitch()
	{
		m_rcItemOld = m_rcItem;
		if( !m_bIsVerticalDirection )
		{
			m_rcCurPos.top = m_rcItem.top;
			m_rcCurPos.bottom = m_rcItem.bottom;
			if (m_bRight)//从右往左
			{
				m_rcCurPos.left = m_rcItem.left + ( m_rcItem.right - m_rcItem.left ) * m_nPositiveDirection;
				m_rcCurPos.right = m_rcItem.right + ( m_rcItem.right - m_rcItem.left ) * m_nPositiveDirection;
			}
			else
			{
				m_rcCurPos.left = m_rcItem.left - ( m_rcItem.right - m_rcItem.left ) * m_nPositiveDirection;
				m_rcCurPos.right = m_rcItem.right - ( m_rcItem.right - m_rcItem.left ) * m_nPositiveDirection;
			}		
		}
		else
		{
			m_rcCurPos.left = m_rcItem.left;
			m_rcCurPos.right = m_rcItem.right;
			if (m_bBottom)//从下往上
			{
				m_rcCurPos.top = m_rcItem.top - ( m_rcItem.bottom - m_rcItem.top ) * m_nPositiveDirection;
				m_rcCurPos.bottom = m_rcItem.bottom - ( m_rcItem.bottom - m_rcItem.top ) * m_nPositiveDirection;
			}
			else
			{
				m_rcCurPos.top = m_rcItem.top + ( m_rcItem.bottom - m_rcItem.top ) * m_nPositiveDirection;
				m_rcCurPos.bottom = m_rcItem.bottom + ( m_rcItem.bottom - m_rcItem.top ) * m_nPositiveDirection;
			}
		}

		StopAnimation( TAB_ANIMATION_ID );
		StartAnimation( TAB_ANIMATION_ELLAPSE, TAB_ANIMATION_FRAME_COUNT, TAB_ANIMATION_ID );
	}

	void CAnimationTabLayoutUI::DoEvent(TEventUI& event)
	{
		if( event.Type == UIEVENT_TIMER ) 
		{
			OnTimer(  event.wParam );
		}
		__super::DoEvent( event );
	}

	void CAnimationTabLayoutUI::OnTimer( int nTimerID )
	{
		OnAnimationElapse( nTimerID );
	}

	void CAnimationTabLayoutUI::OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID)
	{
		if( !m_bControlVisibleFlag )
		{
			m_bControlVisibleFlag = true;
			m_pCurrentControl->SetVisible( true );
		}

		int iStepLen = 0;
		if( !m_bIsVerticalDirection )
		{
			iStepLen = ( m_rcItemOld.right - m_rcItemOld.left ) * m_nPositiveDirection / nTotalFrame;
			if( nCurFrame != nTotalFrame )
			{
				if (m_bRight)//从右往左
				{
					m_rcCurPos.left = m_rcCurPos.left - iStepLen;
					m_rcCurPos.right = m_rcCurPos.right - iStepLen;
				}
				else
				{
					m_rcCurPos.left = m_rcCurPos.left + iStepLen;
					m_rcCurPos.right = m_rcCurPos.right + iStepLen;
				}		
			}
			else
			{
				m_rcItem = m_rcCurPos = m_rcItemOld;
			}
		}
		else
		{
			iStepLen = ( m_rcItemOld.bottom - m_rcItemOld.top ) * m_nPositiveDirection / nTotalFrame;
			if( nCurFrame != nTotalFrame )
			{
				if (m_bBottom)//从下往上
				{
					m_rcCurPos.top = m_rcCurPos.top + iStepLen;
					m_rcCurPos.bottom = m_rcCurPos.bottom +iStepLen;
				}
				else
				{
					m_rcCurPos.top = m_rcCurPos.top - iStepLen;
					m_rcCurPos.bottom = m_rcCurPos.bottom - iStepLen;
				}
			}
			else
			{
				m_rcItem = m_rcCurPos = m_rcItemOld;	
			}	
		}
		SetPos(m_rcCurPos);
	}

	void CAnimationTabLayoutUI::OnAnimationStop(INT nAnimationID) 
	{
		NeedParentUpdate();
	}

	void CAnimationTabLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("animation_direction")) == 0 && _tcscmp( pstrValue, _T("vertical")) == 0 ) m_bIsVerticalDirection = true; // pstrValue = "vertical" or "horizontal"
		return CTabLayoutUI::SetAttribute(pstrName, pstrValue);
	}


}
