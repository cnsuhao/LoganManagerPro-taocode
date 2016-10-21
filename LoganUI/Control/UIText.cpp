#include "StdAfx.h"
#include "UIText.h"

namespace DuiLib
{
	CTextUI::CTextUI() : m_nLinks(0), m_nHoverLink(-1),
						 m_nStep(0),m_nRollLength(0)
	{
		m_nLinePad = 2;
		m_uTextStyle = DT_WORDBREAK;
		m_rcTextPadding.left = 2;
		m_rcTextPadding.right = 2;
		::ZeroMemory(m_rcLinks, sizeof(m_rcLinks));
	}

	CTextUI::~CTextUI()
	{
		m_pManager->KillTimer(this);
	}

	LPCTSTR CTextUI::GetClass() const
	{
		return _T("TextUI");
	}

	LPVOID CTextUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_TEXT) == 0 ) return static_cast<CTextUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	UINT CTextUI::GetControlFlags() const
	{
		if( IsEnabled() && m_nLinks > 0 ) return UIFLAG_SETCURSOR;
		else return 0;
	}

	CDuiString* CTextUI::GetLinkContent(int iIndex)
	{
		if( iIndex >= 0 && iIndex < m_nLinks ) return &m_sLinks[iIndex];
		return NULL;
	}

	void CTextUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CLabelUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_SETCURSOR ) {
			for( int i = 0; i < m_nLinks; i++ ) {
				if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
					::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
					return;
				}
			}
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK && IsEnabled() ) {
			for( int i = 0; i < m_nLinks; i++ ) {
				if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
					Invalidate();
					return;
				}
			}
		}
		if( event.Type == UIEVENT_BUTTONUP && IsEnabled() ) {
			for( int i = 0; i < m_nLinks; i++ ) {
				if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
					m_pManager->SendNotify(this, DUI_MSGTYPE_LINK, i);
					return;
				}
			}
		}
		if( event.Type == UIEVENT_CONTEXTMENU )
		{
			return;
		}
		// When you move over a link
		if( m_nLinks > 0 && event.Type == UIEVENT_MOUSEMOVE && IsEnabled() ) {
			int nHoverLink = -1;
			for( int i = 0; i < m_nLinks; i++ ) {
				if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
					nHoverLink = i;
					break;
				}
			}

			if(m_nHoverLink != nHoverLink) {
				m_nHoverLink = nHoverLink;
				Invalidate();
				return;
			}      
		}
		if( event.Type == UIEVENT_MOUSELEAVE ) {
			if( m_nLinks > 0 && IsEnabled() ) {
				if(m_nHoverLink != -1) {
					m_nHoverLink = -1;
					Invalidate();
					return;
				}
			}
		}
		if ( event.Type == UIEVENT_TIMER)
		{
			if(event.wParam == nRollTimer)
			{
				m_nRollLength += m_nStep;
				Invalidate();
			}
		}

		CLabelUI::DoEvent(event);
	}

	SIZE CTextUI::EstimateSize(SIZE szAvailable)
	{
		RECT rcText = { 0, 0, MAX(szAvailable.cx, m_cxyFixed.cx), 9999 };
		rcText.left += m_rcTextPadding.left;
		rcText.right -= m_rcTextPadding.right;
		if( m_bShowHtml ) {   
			int nLinks = 0;
			CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, m_dwTextColor, NULL, NULL, nLinks, DT_CALCRECT | m_uTextStyle);
		}
		else {
			CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, m_dwTextColor, m_iFont, DT_CALCRECT | m_uTextStyle);
		}
		SIZE cXY = {rcText.right - rcText.left + m_rcTextPadding.left + m_rcTextPadding.right,
			rcText.bottom - rcText.top + m_rcTextPadding.top + m_rcTextPadding.bottom};

		if( m_cxyFixed.cy != 0 ) cXY.cy = m_cxyFixed.cy;
		return cXY;
	}

	void CTextUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("roll")) == 0 )
		{
			LPTSTR pstr = NULL;
			int nStep = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
			int nDiretion = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
			StartRoll(nStep,(emRollDirection)nDiretion);
		}
		else
			__super::SetAttribute(pstrName,pstrValue);
	}

	void CTextUI::PaintText(HDC hDC)
	{
		if( m_sText.IsEmpty() ) 
		{
			m_nLinks = 0;
			return;
		}

		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

		if( m_sText.IsEmpty() ) return;

		m_nLinks = lengthof(m_rcLinks);
		CDuiRect rc = m_rcItem;
		rc.left += m_rcTextPadding.left;
		rc.right -= m_rcTextPadding.right;
		rc.top += m_rcTextPadding.top;
		rc.bottom -= m_rcTextPadding.bottom;

		TFontInfo * pFont = m_pManager->GetFontInfo(m_iFont);
		int nHeight = rc.GetHeight();

		if( IsEnabled() ) 
		{
			if( m_bShowHtml )
				CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, m_dwTextColor,m_rcLinks, m_sLinks, m_nLinks, m_uTextStyle);
			else
			{
				if(m_nStep)
				{
					if(m_nDirection == RollDirection_Left)
					{
						if(m_nRollLength > rc.GetWidth())
							m_nRollLength = 0;
						rc.right = rc.left + m_nRollLength;

						if( pFont->tm.tmHeight < nHeight )
							rc.Deflate(0,(nHeight - pFont->tm.tmHeight)/2);
					}
					else if (m_nDirection == RollDirection_top)
					{
						if(m_nRollLength > rc.GetHeight())
							m_nRollLength = 0;
						rc.bottom = rc.top + m_nRollLength;
					}
					else if (m_nDirection == RollDirection_right)
					{
						if(m_nRollLength > rc.GetWidth())
							m_nRollLength = 0;
						rc.left = rc.right - m_nRollLength;

						if( pFont->tm.tmHeight < nHeight )
							rc.Deflate(0,(nHeight - pFont->tm.tmHeight)/2);
					}
					else if (m_nDirection == RollDirection_bottom)
					{
						if(m_nRollLength > rc.GetHeight())
							m_nRollLength = 0;
						rc.top = rc.bottom - m_nRollLength;
					}
				}

				CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, m_dwTextColor,m_iFont, m_uTextStyle);
			}
		}
		else 
		{
			if( m_bShowHtml )
				CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, m_dwDisabledTextColor,m_rcLinks, m_sLinks, m_nLinks, m_uTextStyle);
			else
				CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, m_dwDisabledTextColor,m_iFont, m_uTextStyle);
		}
	}

	void CTextUI::StartRoll(UINT nStep,emRollDirection emDirection)
	{
		m_nStep = nStep;
		m_nDirection = emDirection;
		m_pManager->SetTimer(this,nRollTimer,200);
	}

	void CTextUI::StopRoll()
	{
		m_pManager->KillTimer(this,nRollTimer);
	}
}
