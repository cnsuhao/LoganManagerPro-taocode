#include "StdAfx.h"
#include "UIAnimation.h"
#include <algorithm>

namespace DuiLib {

	CUIAnimation::CUIAnimation()
	{
	}

	CUIAnimation::~CUIAnimation()
	{
	}

	bool CUIAnimation::StartAnimation(int nElapse, int nTotalFrame, 
									  int nAnimationID /*= 0*/, bool bLoop/* = false*/)
	{
		int index = -1;
		CAnimationData* pData = GetAnimationDataByID(nAnimationID, index);
		if( NULL != pData 
			|| nElapse <= 0
			|| nTotalFrame <= 0
			|| NULL == OwnerControlUI())
		{
			ASSERT(false);
			return false;
		}

		CAnimationData* pAnimation = new CAnimationData(nElapse, nTotalFrame, nAnimationID, bLoop);
		if( NULL == pAnimation ) return false;
		
		if(OwnerControlUI()->GetManager()->SetTimer( OwnerControlUI(), 
			nAnimationID, nElapse ))
		{
			m_arAnimations.Add(pAnimation);
			return true;
		}
		else
		{
			delete pAnimation;
		}
		return false;
	}

	void CUIAnimation::StopAnimation(int nAnimationID /*= 0*/)
	{
		if(OwnerControlUI() == NULL) return;

		if(nAnimationID  != 0)
		{
			int index = -1;
			CAnimationData* pData = GetAnimationDataByID(nAnimationID, index);
			if( NULL != pData )
			{
				OwnerControlUI()->GetManager()->KillTimer(
					OwnerControlUI(), nAnimationID );
				m_arAnimations.Remove(index);
				delete pData;
				pData = NULL;
				return;
			}
		}
		else
		{
			int nCount = m_arAnimations.GetSize();
			for(int i=0; i<nCount; ++i)
			{
				CAnimationData* pData = (CAnimationData*)m_arAnimations[i];
				OwnerControlUI()->GetManager()->KillTimer(OwnerControlUI(), 
													pData->m_nAnimationID );
				delete pData;
			}
			m_arAnimations.Empty();
		}
	}

	bool CUIAnimation::IsAnimationRunning(int nAnimationID)
	{
		int index = -1;
		CAnimationData* pData = GetAnimationDataByID(nAnimationID, index);
		return NULL != pData;
	}

	int CUIAnimation::GetCurrentFrame(int nAnimationID/* = 0*/)
	{
		int index = -1;
		CAnimationData* pData = GetAnimationDataByID(nAnimationID, index);
		if( NULL == pData )
		{
			ASSERT(false);
			return -1;
		}
		return pData->m_nCurFrame;
	}

	bool CUIAnimation::SetCurrentFrame(int nFrame, int nAnimationID/* = 0*/)
	{
		int index = -1;
		CAnimationData* pData = GetAnimationDataByID(nAnimationID, index);
		if( NULL == pData)
		{
			ASSERT(false);
			return false;
		}

		if(nFrame >= 0 && nFrame <= pData->m_nTotalFrame)
		{
			pData->m_nCurFrame = nFrame;
			return true;
		}
		else
		{
			ASSERT(false);
		}
		return false;
	}

	void CUIAnimation::OnAnimationElapse(int nAnimationID)
	{
		if(OwnerControlUI() == NULL) return;

		int index = -1;
		CAnimationData* pData = GetAnimationDataByID(nAnimationID, index);
		if( NULL == pData ) return;

		int nCurFrame = pData->m_nCurFrame;
		if(nCurFrame == 0)
		{
			OnAnimationStart(nAnimationID, pData->m_bFirstLoop);
			pData->m_bFirstLoop = false;
		}

		OnAnimationStep(pData->m_nTotalFrame, nCurFrame, nAnimationID);

		if(nCurFrame >= pData->m_nTotalFrame)
		{
			OnAnimationStop(nAnimationID);
			if(pData->m_bLoop)
			{
				pData->m_nCurFrame = 0;
			}
			else
			{
				OwnerControlUI()->GetManager()->KillTimer(
					OwnerControlUI(), nAnimationID );
				m_arAnimations.Remove(index);
				delete pData;
				pData = NULL;
			}
		}

		if( NULL != pData )
		{
			++(pData->m_nCurFrame);
		}
	}

	CAnimationData* CUIAnimation::GetAnimationDataByID(int nAnimationID, int& index)
	{
		CAnimationData* pRet = NULL;
		index = -1;
		int nCount = m_arAnimations.GetSize();
		for(int i=0; i<nCount; ++i)
		{
			if(((CAnimationData*)m_arAnimations[i])->m_nAnimationID == nAnimationID)
			{
				pRet = (CAnimationData*)m_arAnimations[i];
				index = i;
				break;
			}
		}

		return pRet;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	CAnimationCtrlUI::CAnimationCtrlUI() : 
		CUIAnimation(*this), 
		m_nSpeed(1),
		m_pCurrentControl(this),
		m_bControlVisibleFlag(false),
		m_bVertical(false), 
		m_bTopLeft(true),
		m_iFont(-1),
		m_uTextStyle(DT_LEFT),
		m_dwTextColor(0xFFFFFFFF)
	{
	}

	CControlUI* CAnimationCtrlUI::OwnerControlUI()
	{
		return this;
	}

	void CAnimationCtrlUI::SetDirection(bool bVer,bool bTopLeft)
	{
		m_bVertical = bVer;
		m_bTopLeft = bTopLeft;
	}

	void CAnimationCtrlUI::Start(bool bTopLeft)
	{
		m_bTopLeft = bTopLeft;
		AnimationSwitch();
	}

	void CAnimationCtrlUI::AnimationSwitch()
	{
		m_rcItemOld = m_rcItem;
		if( !m_bVertical )
		{
			m_rcCurPos.top = m_rcItem.top;
			m_rcCurPos.bottom = m_rcItem.bottom;
			if (!m_bTopLeft)//从右往左
			{
				m_rcCurPos.left = m_rcItem.left + ( m_rcItem.right - m_rcItem.left ) * m_nSpeed;
				m_rcCurPos.right = m_rcItem.right + ( m_rcItem.right - m_rcItem.left ) * m_nSpeed;
			}
			else
			{
				m_rcCurPos.left = m_rcItem.left - ( m_rcItem.right - m_rcItem.left ) * m_nSpeed;
				m_rcCurPos.right = m_rcItem.right - ( m_rcItem.right - m_rcItem.left ) * m_nSpeed;
			}		
		}
		else
		{
			m_rcCurPos.left = m_rcItem.left;
			m_rcCurPos.right = m_rcItem.right;
			if (!m_bTopLeft)//从下往上
			{
				m_rcCurPos.top = m_rcItem.top - ( m_rcItem.bottom - m_rcItem.top ) * m_nSpeed;
				m_rcCurPos.bottom = m_rcItem.bottom - ( m_rcItem.bottom - m_rcItem.top ) * m_nSpeed;
			}
			else
			{
				m_rcCurPos.top = m_rcItem.top + ( m_rcItem.bottom - m_rcItem.top ) * m_nSpeed;
				m_rcCurPos.bottom = m_rcItem.bottom + ( m_rcItem.bottom - m_rcItem.top ) * m_nSpeed;
			}
		}

		StopAnimation( TAB_ANIMATION_ID );
		StartAnimation( TAB_ANIMATION_ELLAPSE, TAB_ANIMATION_FRAME_COUNT, TAB_ANIMATION_ID);
	}

	void CAnimationCtrlUI::DoEvent(TEventUI& event)
	{
		if( event.Type == UIEVENT_TIMER ) 
		{
			OnTimer(  event.wParam );
		}
		__super::DoEvent( event );
	}

	void CAnimationCtrlUI::OnTimer( int nTimerID )
	{
		OnAnimationElapse( nTimerID );
	}

	void CAnimationCtrlUI::OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID)
	{
		if( !m_bControlVisibleFlag )
		{
			m_bControlVisibleFlag = true;
			m_pCurrentControl->SetVisible( true );
		}

		int iStepLen = 0;
		if( !m_bVertical )
		{
			iStepLen = ( m_rcItemOld.right - m_rcItemOld.left ) * m_nSpeed / nTotalFrame;
			if( nCurFrame != nTotalFrame )
			{
				if (!m_bTopLeft)//从右往左
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
			iStepLen = ( m_rcItemOld.bottom - m_rcItemOld.top ) * m_nSpeed / nTotalFrame;
			if( nCurFrame != nTotalFrame )
			{
				if (!m_bTopLeft)//从下往上
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

	void CAnimationCtrlUI::OnAnimationStop(INT nAnimationID) 
	{
		NeedParentUpdate();
	}

	void CAnimationCtrlUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("direction")) == 0 && _tcscmp( pstrValue, _T("vertical")) == 0 )
		{
			m_bVertical = true; // pstrValue = "vertical" or "horizontal"
		}
		else if( _tcscmp(pstrName, _T("textcolor")) == 0 ) 
		{
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			m_dwTextColor = _tcstoul(pstrValue, &pstr, 16);
		}
		else if( _tcscmp(pstrName, _T("align")) == 0 ) 
		{
			if( _tcsstr(pstrValue, _T("left")) != NULL ) 
			{
				m_uTextStyle &= ~(DT_CENTER | DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
				m_uTextStyle |= DT_LEFT;
			}
			if( _tcsstr(pstrValue, _T("center")) != NULL ) 
			{
				m_uTextStyle &= ~(DT_LEFT | DT_RIGHT );
				m_uTextStyle |= DT_CENTER;
			}
			if( _tcsstr(pstrValue, _T("right")) != NULL ) 
			{
				m_uTextStyle &= ~(DT_LEFT | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				m_uTextStyle |= DT_RIGHT;
			}
			if( _tcsstr(pstrValue, _T("top")) != NULL ) 
			{
				m_uTextStyle &= ~(DT_BOTTOM | DT_VCENTER | DT_VCENTER);
				m_uTextStyle |= (DT_TOP | DT_SINGLELINE);
			}
			if( _tcsstr(pstrValue, _T("vcenter")) != NULL ) 
			{
				m_uTextStyle &= ~(DT_TOP | DT_BOTTOM );			
				m_uTextStyle |= (DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
			if( _tcsstr(pstrValue, _T("bottom")) != NULL ) 
			{
				m_uTextStyle &= ~(DT_TOP | DT_VCENTER | DT_VCENTER);
				m_uTextStyle |= (DT_BOTTOM | DT_SINGLELINE);
			}
		}
		else if( _tcscmp(pstrName, _T("font")) == 0 ) 
		{
			m_iFont = _ttoi(pstrValue);
		}

		return __super::SetAttribute(pstrName, pstrValue);
	}

	void CAnimationCtrlUI::PaintText(HDC hDC)
	{
		CRenderEngine::DrawText(hDC,m_pManager,m_rcItem,m_sText,m_dwTextColor,m_iFont,m_uTextStyle);
	}

} // namespace DuiLib