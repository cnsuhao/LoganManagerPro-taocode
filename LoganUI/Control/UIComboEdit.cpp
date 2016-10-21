#include "StdAfx.h"

namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//
//

void CComboWndEx::Init(CComboEditUI* pOwner)
{
    m_pOwner = pOwner;
    m_pLayout = NULL;
    m_iOldSel = m_pOwner->GetCurSel();

    // Position the popup window in absolute space
    SIZE szDrop = m_pOwner->GetDropBoxSize();
    RECT rcOwner = pOwner->GetPos();
    RECT rc = rcOwner;
    rc.top = rc.bottom;
    rc.bottom = rc.top + szDrop.cy;
    if( szDrop.cx > 0 ) rc.right = rc.left + szDrop.cx;

    SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
    int cyFixed = 0;
    for( int it = 0; it < pOwner->GetCount(); it++ ) {
        CControlUI* pControl = static_cast<CControlUI*>(pOwner->GetItemAt(it));
        if( !pControl->IsVisible() ) continue;
        SIZE sz = pControl->EstimateSize(szAvailable);
        cyFixed += sz.cy;
    }
    cyFixed += 4; // CVerticalLayoutUI 默认的Inset 调整
    rc.bottom = rc.top + MIN(cyFixed, szDrop.cy);

    ::MapWindowRect(pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);

    MONITORINFO oMonitor = {};
    oMonitor.cbSize = sizeof(oMonitor);
    ::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
    CDuiRect rcWork = oMonitor.rcWork;
    if( rc.bottom > rcWork.bottom ) {
        rc.left = rcOwner.left;
        rc.right = rcOwner.right;
        if( szDrop.cx > 0 ) rc.right = rc.left + szDrop.cx;
        rc.top = rcOwner.top - MIN(cyFixed, szDrop.cy);
        rc.bottom = rcOwner.top;
        ::MapWindowRect(pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);
    }
    
    Create(pOwner->GetManager()->GetPaintWindow(), NULL, WS_POPUP, WS_EX_TOOLWINDOW, rc);
    // HACK: Don't deselect the parent's caption
    HWND hWndParent = m_hWnd;
    while( ::GetParent(hWndParent) != NULL ) hWndParent = ::GetParent(hWndParent);
    ::ShowWindow(m_hWnd, SW_SHOW);
    ::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
}

LPCTSTR CComboWndEx::GetWindowClassName() const
{
    return _T("ComboWnd");
}

void CComboWndEx::OnFinalMessage(HWND hWnd)
{
    m_pOwner->m_pComWnd = NULL;
    m_pOwner->m_uButtonState &= ~ UISTATE_PUSHED;
    m_pOwner->Invalidate();
    delete this;
}

LRESULT CComboWndEx::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if( uMsg == WM_CREATE )
	{
        m_pm.Init(m_hWnd);
        // The trick is to add the items to the new container. Their owner gets
        // reassigned by this operation - which is why it is important to reassign
        // the items back to the righfull owner/manager when the window closes.
        m_pLayout = new CVerticalLayoutUI;
        m_pm.UseParentResource(m_pOwner->GetManager());
        m_pLayout->SetManager(&m_pm, NULL, true);
        LPCTSTR pDefaultAttributes = m_pOwner->GetManager()->GetDefaultAttributeList(_T("VerticalLayout"));
        if( pDefaultAttributes ) {
            m_pLayout->ApplyAttributeList(pDefaultAttributes);
        }
        m_pLayout->SetInset(CDuiRect(2, 2, 2, 2));
		if(m_pOwner->m_dwDropBoxColor == 0)
			m_pLayout->SetBkColor(0xFFFFFFFF);
		else
			m_pLayout->SetBkColor(m_pOwner->m_dwDropBoxColor);
		if(m_pOwner->m_dwDropBoxBorderColor == 0)
			m_pLayout->SetBorderColor(0xFF85E4FF);
		else
			m_pLayout->SetBorderColor(m_pOwner->m_dwDropBoxBorderColor);
        m_pLayout->SetBorderSize(2);
        m_pLayout->SetAutoDestroy(false);
        m_pLayout->EnableScrollBar();
        m_pLayout->ApplyAttributeList(m_pOwner->GetDropBoxAttributeList());
        for( int i = 0; i < m_pOwner->GetCount(); i++ ) {
            m_pLayout->Add(static_cast<CControlUI*>(m_pOwner->GetItemAt(i)));
        }
        m_pm.AttachDialog(m_pLayout);
        
        return 0;
    }
    else if( uMsg == WM_CLOSE ) 
	{
        m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent(), false);
        m_pOwner->SetPos(m_pOwner->GetPos());
        m_pOwner->SetFocus();
    }
    else if( uMsg == WM_LBUTTONUP ) {
        POINT pt = { 0 };
        ::GetCursorPos(&pt);
        ::ScreenToClient(m_pm.GetPaintWindow(), &pt);
        CControlUI* pControl = m_pm.FindControl(pt);
        if( pControl && _tcscmp(pControl->GetClass(), _T("ScrollBarUI")) != 0 ) 
		{
			m_pOwner->m_sText = pControl->GetText();
			m_pOwner->Invalidate();
			PostMessage(WM_KILLFOCUS);
		}
    }
    else if( uMsg == WM_KEYDOWN ) {
        switch( wParam ) {
        case VK_ESCAPE:
            m_pOwner->SelectItem(m_iOldSel);
            EnsureVisible(m_iOldSel);
            // FALL THROUGH...
        case VK_RETURN:
            PostMessage(WM_KILLFOCUS);
            break;
        default:
            TEventUI event;
            event.Type = UIEVENT_KEYDOWN;
            event.chKey = (TCHAR)wParam;
            m_pOwner->DoEvent(event);
            EnsureVisible(m_pOwner->GetCurSel());
            return 0;
        }
    }
    else if( uMsg == WM_MOUSEWHEEL ) {
        int zDelta = (int) (short) HIWORD(wParam);
        TEventUI event = { 0 };
        event.Type = UIEVENT_SCROLLWHEEL;
        event.wParam = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0);
        event.lParam = lParam;
        event.dwTimestamp = ::GetTickCount();
        m_pOwner->DoEvent(event);
        EnsureVisible(m_pOwner->GetCurSel());
        return 0;
    }
    else if( uMsg == WM_KILLFOCUS ) {
        if( m_hWnd != (HWND) wParam ) PostMessage(WM_CLOSE);
    }

    LRESULT lRes = 0;
    if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
    return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

void CComboWndEx::EnsureVisible(int iIndex)
{
    if( m_pOwner->GetCurSel() < 0 ) return;
    m_pLayout->FindSelectable(m_pOwner->GetCurSel(), false);
    RECT rcItem = m_pLayout->GetItemAt(iIndex)->GetPos();
    RECT rcList = m_pLayout->GetPos();
    CScrollBarUI* pHorizontalScrollBar = m_pLayout->GetHorizontalScrollBar();
    if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) rcList.bottom -= pHorizontalScrollBar->GetFixedHeight();
    int iPos = m_pLayout->GetScrollPos().cy;
    if( rcItem.top >= rcList.top && rcItem.bottom < rcList.bottom ) return;
    int dx = 0;
    if( rcItem.top < rcList.top ) dx = rcItem.top - rcList.top;
    if( rcItem.bottom > rcList.bottom ) dx = rcItem.bottom - rcList.bottom;
	//滚动到选择项的时候 选中位置在最顶端位置
	SIZE size = m_pOwner->GetDropBoxSize();
	dx += size.cy;
	dx = dx - (rcItem.bottom - rcItem.top);
	//
    Scroll(0, dx);
}

void CComboWndEx::Scroll(int dx, int dy)
{
    if( dx == 0 && dy == 0 ) return;
    SIZE sz = m_pLayout->GetScrollPos();
    m_pLayout->SetScrollPos(CSize(sz.cx + dx, sz.cy + dy));
}

////////////////////////////////////////////////////////
///
CComboEditWnd::CComboEditWnd() : m_pOwner(NULL), m_hBkBrush(NULL)
{
}

void CComboEditWnd::Init(CComboEditUI* pOwner)
{
	m_pOwner = pOwner;
	RECT rcPos = CalPos();
	UINT uStyle = WS_CHILD | ES_AUTOHSCROLL;

	Create(m_pOwner->GetManager()->GetPaintWindow(), NULL, uStyle, 0, rcPos);
	SetWindowFont(m_hWnd, m_pOwner->GetManager()->GetDefaultFontInfo()->hFont, TRUE);
	Edit_LimitText(m_hWnd, m_pOwner->GetMaxChar());
	
	Edit_SetText(m_hWnd, m_pOwner->GetText());
	Edit_SetModify(m_hWnd, FALSE);
	SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(0, 0));
	Edit_Enable(m_hWnd, m_pOwner->IsEnabled() == true);
	Edit_SetReadOnly(m_hWnd, m_pOwner->IsReadOnly() == true);
	::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
	::SetFocus(m_hWnd);
}

RECT CComboEditWnd::CalPos()
{
	CDuiRect rcPos = m_pOwner->GetPos();
	CDuiRect rcInset = m_pOwner->GetTextPadding();
	rcPos.left += rcInset.left;
	rcPos.right -= rcInset.right;
	rcPos.right -= m_pOwner->m_iBtnWidth;
	LONG lEditHeight = m_pOwner->GetManager()->GetDefaultFontInfo()->tm.tmHeight;
	if( lEditHeight < rcPos.GetHeight() ) 
	{
		rcPos.top += (rcPos.GetHeight() - lEditHeight) / 2;
		rcPos.bottom = rcPos.top + lEditHeight;
	}
	return rcPos;
}

LPCTSTR CComboEditWnd::GetWindowClassName() const
{
	return _T("ComboEditWnd");
}

LPCTSTR CComboEditWnd::GetSuperClassName() const
{
	return WC_EDIT;
}

void CComboEditWnd::OnFinalMessage(HWND /*hWnd*/)
{
	if( m_hBkBrush != NULL ) ::DeleteObject(m_hBkBrush);
	m_pOwner->m_pEditWnd = NULL;
	delete this;
}

LRESULT CComboEditWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	if( uMsg == WM_KILLFOCUS ) 
		lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
	else if( uMsg == OCM_COMMAND ) 
	{
		if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE ) 
			lRes = OnEditChanged(uMsg, wParam, lParam, bHandled);
		else if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_UPDATE ) 
		{
			RECT rcClient;
			::GetClientRect(m_hWnd, &rcClient);
			::InvalidateRect(m_hWnd, &rcClient, FALSE);
		}
	}
	else if( uMsg == WM_KEYDOWN && TCHAR(wParam) == VK_RETURN ) 
	{
		m_pOwner->GetManager()->SendNotify(m_pOwner, _T("return"));
	}
	else if( uMsg == OCM__BASE + WM_CTLCOLOREDIT  || uMsg == OCM__BASE + WM_CTLCOLORSTATIC ) 
	{
		if( m_pOwner->GetBkColor() == 0xFFFFFFFF ) return NULL;
		::SetBkMode((HDC)wParam, TRANSPARENT);
		DWORD dwTextColor = 0x0;//m_pOwner->GetItemTextColor();
		::SetTextColor((HDC)wParam, RGB(GetBValue(dwTextColor),GetGValue(dwTextColor),GetRValue(dwTextColor)));
		if( m_hBkBrush == NULL ) 
		{
			DWORD clrColor = 0xFFFFFFFF;//m_pOwner->GetBkColor();
			m_hBkBrush = ::CreateSolidBrush(RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
		}
		return (LRESULT)m_hBkBrush;
	}
	else bHandled = FALSE;
	if( !bHandled ) return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	return lRes;
}

LRESULT CComboEditWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
	LPTSTR pstr = static_cast<LPTSTR>(_alloca(cchLen * sizeof(TCHAR)));
	ASSERT(pstr);
	if( pstr == NULL ) return 0;
	::GetWindowText(m_hWnd, pstr, cchLen);
	m_pOwner->m_sText = pstr;
	LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
	PostMessage(WM_CLOSE);
	return lRes;
}

LRESULT CComboEditWnd::OnEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if( m_pOwner == NULL ) return 0;
	// Copy text back
	int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
	LPTSTR pstr = static_cast<LPTSTR>(_alloca(cchLen * sizeof(TCHAR)));
	ASSERT(pstr);
	if( pstr == NULL ) return 0;
	::GetWindowText(m_hWnd, pstr, cchLen);
	m_pOwner->m_sText = pstr;
	m_pOwner->GetManager()->SendNotify(m_pOwner, _T("textchanged"));
	return 0;
}


////////////////////////////////////////////////////////
///

CComboEditUI::CComboEditUI() : m_pComWnd(NULL),m_pEditWnd(NULL),m_uMaxChar(255), m_bReadOnly(false), m_iCurSel(-1), m_uButtonState(0)
{
	m_iBtnWidth = 25;
	m_iTextFont = 0;
	m_dwTextColor = 0x0;

    m_szDropBox = CSize(0, 150);
	m_dwDropBoxColor = 0;
	m_dwDropBoxBorderColor = 0;
    ::ZeroMemory(&m_rcTextPadding, sizeof(m_rcTextPadding));
	SetTextPadding(CDuiRect(4, 3, 4, 3));

    m_ListInfo.nColumns = 0;
    m_ListInfo.nFont = -1;
    m_ListInfo.uTextStyle = DT_VCENTER;
    m_ListInfo.dwTextColor = 0xFF000000;
    m_ListInfo.dwBkColor = 0;
    m_ListInfo.bAlternateBk = false;
    m_ListInfo.dwSelectedTextColor = 0xFF000000;
    m_ListInfo.dwSelectedBkColor = 0xFFC1E3FF;
    m_ListInfo.dwHotTextColor = 0xFF000000;
    m_ListInfo.dwHotBkColor = 0xFFE9F5FF;
    m_ListInfo.dwDisabledTextColor = 0xFFCCCCCC;
    m_ListInfo.dwDisabledBkColor = 0xFFFFFFFF;
    m_ListInfo.dwLineColor = 0;
    m_ListInfo.bShowHtml = false;
    m_ListInfo.bMultiExpandable = false;
    ::ZeroMemory(&m_ListInfo.rcTextPadding, sizeof(m_ListInfo.rcTextPadding));
    ::ZeroMemory(&m_ListInfo.rcColumn, sizeof(m_ListInfo.rcColumn));
}

LPCTSTR CComboEditUI::GetClass() const
{
    return _T("ComboUI");
}

LPVOID CComboEditUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("Combo")) == 0 ) return static_cast<CComboEditUI*>(this);
    if( _tcscmp(pstrName, _T("IListOwner")) == 0 ) return static_cast<IListOwnerUI*>(this);
    return CContainerUI::GetInterface(pstrName);
}

UINT CComboEditUI::GetControlFlags() const
{
    return UIFLAG_TABSTOP;
}

void CComboEditUI::DoInit()
{
    if( m_iCurSel < 0 ) 
		SelectItem(0);
	
	if( m_pEditWnd)
		return;
	m_pEditWnd = new CComboEditWnd();
	ASSERT(m_pEditWnd);
	m_pEditWnd->Init(this);
	
	Invalidate();
}

int CComboEditUI::GetCurSel() const
{
    return m_iCurSel;
}

bool CComboEditUI::SelectItem(int iIndex, bool bTakeFocus)
{
    if( m_pComWnd != NULL ) 
		m_pComWnd->Close();
    if( iIndex == m_iCurSel ) 
		return true;
    int iOldSel = m_iCurSel;
    if( m_iCurSel >= 0 )
	{
        CControlUI* pControl = static_cast<CControlUI*>(m_items[m_iCurSel]);
        if( !pControl ) 
			return false;
        IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
        if( pListItem != NULL ) 
			pListItem->Select(false);
        m_iCurSel = -1;
    }
    if( iIndex < 0 ) 
		return false;
    if( m_items.GetSize() == 0 ) 
		return false;
    if( iIndex >= m_items.GetSize() ) 
		iIndex = m_items.GetSize() - 1;
    CControlUI* pControl = static_cast<CControlUI*>(m_items[iIndex]);
    if( !pControl || !pControl->IsVisible() || !pControl->IsEnabled() ) 
		return false;
    IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
    if( pListItem == NULL ) 
		return false;
    m_iCurSel = iIndex;
    if( m_pComWnd != NULL || bTakeFocus ) 
		pControl->SetFocus();
    pListItem->Select(true);
    if( m_pManager != NULL ) 
		m_pManager->SendNotify(this, _T("itemselect"), m_iCurSel, iOldSel);
    Invalidate();

    return true;
}

bool CComboEditUI::SetItemIndex(CControlUI* pControl, int iIndex)
{
    int iOrginIndex = GetItemIndex(pControl);
    if( iOrginIndex == -1 ) 
		return false;
    if( iOrginIndex == iIndex ) 
		return true;

    IListItemUI* pSelectedListItem = NULL;
    if( m_iCurSel >= 0 ) 
		pSelectedListItem = static_cast<IListItemUI*>(GetItemAt(m_iCurSel)->GetInterface(_T("ListItem")));
    if( !CContainerUI::SetItemIndex(pControl, iIndex) ) return false;
    int iMinIndex = min(iOrginIndex, iIndex);
    int iMaxIndex = max(iOrginIndex, iIndex);
    for(int i = iMinIndex; i < iMaxIndex + 1; ++i) {
        CControlUI* p = GetItemAt(i);
        IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
        if( pListItem != NULL ) {
            pListItem->SetIndex(i);
        }
    }
    if( m_iCurSel >= 0 && pSelectedListItem != NULL ) m_iCurSel = pSelectedListItem->GetIndex();
    return true;
}

bool CComboEditUI::Add(CControlUI* pControl)
{
    IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
    if( pListItem != NULL ) 
    {
        pListItem->SetOwner(this);
        pListItem->SetIndex(m_items.GetSize());
    }
    return CContainerUI::Add(pControl);
}

bool CComboEditUI::AddAt(CControlUI* pControl, int iIndex)
{
    if (!CContainerUI::AddAt(pControl, iIndex)) return false;

    // The list items should know about us
    IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
    if( pListItem != NULL ) {
        pListItem->SetOwner(this);
        pListItem->SetIndex(iIndex);
    }

    for(int i = iIndex + 1; i < GetCount(); ++i) {
        CControlUI* p = GetItemAt(i);
        pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
        if( pListItem != NULL ) {
            pListItem->SetIndex(i);
        }
    }
    if( m_iCurSel >= iIndex ) m_iCurSel += 1;
    return true;
}

bool CComboEditUI::Remove(CControlUI* pControl)
{
    int iIndex = GetItemIndex(pControl);
    if (iIndex == -1) return false;

    if (!CContainerUI::RemoveAt(iIndex)) return false;

    for(int i = iIndex; i < GetCount(); ++i) {
        CControlUI* p = GetItemAt(i);
        IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
        if( pListItem != NULL ) {
            pListItem->SetIndex(i);
        }
    }

    if( iIndex == m_iCurSel && m_iCurSel >= 0 ) {
        int iSel = m_iCurSel;
        m_iCurSel = -1;
        SelectItem(FindSelectable(iSel, false));
    }
    else if( iIndex < m_iCurSel ) m_iCurSel -= 1;
    return true;
}

bool CComboEditUI::RemoveAt(int iIndex)
{
    if (!CContainerUI::RemoveAt(iIndex)) return false;

    for(int i = iIndex; i < GetCount(); ++i) {
        CControlUI* p = GetItemAt(i);
        IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
        if( pListItem != NULL ) pListItem->SetIndex(i);
    }

    if( iIndex == m_iCurSel && m_iCurSel >= 0 ) {
        int iSel = m_iCurSel;
        m_iCurSel = -1;
        SelectItem(FindSelectable(iSel, false));
    }
    else if( iIndex < m_iCurSel ) m_iCurSel -= 1;
    return true;
}

void CComboEditUI::RemoveAll()
{
    m_iCurSel = -1;
    CContainerUI::RemoveAll();
}

void CComboEditUI::DoEvent(TEventUI& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) 
	{
        if( m_pParent != NULL ) m_pParent->DoEvent(event);
        else CContainerUI::DoEvent(event);
        return;
    }

    if( event.Type == UIEVENT_SETFOCUS ) 
    {	
		if( m_pEditWnd)
			return;
		m_pEditWnd = new CComboEditWnd();
		ASSERT(m_pEditWnd);
		m_pEditWnd->Init(this);
		SelectItem(-1);
        Invalidate();
    }
    if( event.Type == UIEVENT_KILLFOCUS ) 
    {
        Invalidate();
    }
    if( event.Type == UIEVENT_BUTTONDOWN )
    {
        if( IsEnabled() ) 
		{
			if (::PtInRect(&m_rcBtn,event.ptMouse))
			{
				Activate();
				m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
			}
			else
			{
				if(/* IsFocused() && */m_pEditWnd == NULL )
				{
					m_pEditWnd = new CComboEditWnd();
					ASSERT(m_pEditWnd);
					m_pEditWnd->Init(this);
					SelectItem(-1);
					if( PtInRect(&m_rcItem, event.ptMouse) )
					{
						int nSize = GetWindowTextLength(*m_pEditWnd);
						if( nSize == 0 )
							nSize = 1;
						Edit_SetSel(*m_pEditWnd, 0, nSize);
					}
				}
				else if( m_pEditWnd != NULL )
				{
					int nSize = GetWindowTextLength(*m_pEditWnd);
					if( nSize == 0 )
						nSize = 1;
					Edit_SetSel(*m_pEditWnd, 0, nSize);
				}
				if( (m_uButtonState & UISTATE_HOT) != 0 ) 
				{
					m_uButtonState &= ~UISTATE_HOT;
					Invalidate();
				}
			}

        }
        return;
    }
    if( event.Type == UIEVENT_BUTTONUP )
    {
        if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) 
		{
            m_uButtonState &= ~ UISTATE_CAPTURED;
            Invalidate();
        }
        return;
    }
    if( event.Type == UIEVENT_MOUSEMOVE )
    {
        return;
    }
    if( event.Type == UIEVENT_KEYDOWN )
    {
        switch( event.chKey ) {
        case VK_F4:
            Activate();
            return;
        case VK_UP:
            SelectItem(FindSelectable(m_iCurSel - 1, false));
            return;
        case VK_DOWN:
            SelectItem(FindSelectable(m_iCurSel + 1, true));
            return;
        case VK_PRIOR:
            SelectItem(FindSelectable(m_iCurSel - 1, false));
            return;
        case VK_NEXT:
            SelectItem(FindSelectable(m_iCurSel + 1, true));
            return;
        case VK_HOME:
            SelectItem(FindSelectable(0, false));
            return;
        case VK_END:
            SelectItem(FindSelectable(GetCount() - 1, true));
            return;
        }
    }
    if( event.Type == UIEVENT_SCROLLWHEEL )
    {
        bool bDownward = LOWORD(event.wParam) == SB_LINEDOWN;
        SelectItem(FindSelectable(m_iCurSel + (bDownward ? 1 : -1), bDownward));
        return;
    }
    if( event.Type == UIEVENT_CONTEXTMENU )
    {
        return;
    }
    if( event.Type == UIEVENT_MOUSEENTER )
    {
        if( ::PtInRect(&m_rcItem, event.ptMouse ) ) 
		{
            if( (m_uButtonState & UISTATE_HOT) == 0  )
                m_uButtonState |= UISTATE_HOT;
            Invalidate();
        }
        return;
    }
    if( event.Type == UIEVENT_MOUSELEAVE )
    {
        if( (m_uButtonState & UISTATE_HOT) != 0 ) 
		{
            m_uButtonState &= ~UISTATE_HOT;
            Invalidate();
        }
        return;
    }
    CControlUI::DoEvent(event);
}

SIZE CComboEditUI::EstimateSize(SIZE szAvailable)
{
    if( m_cxyFixed.cy == 0 ) 
		return CSize(m_cxyFixed.cx, m_pManager->GetDefaultFontInfo()->tm.tmHeight + 12);
    return CControlUI::EstimateSize(szAvailable);
}

bool CComboEditUI::Activate()
{
    if( !CControlUI::Activate() ) 
		return false;
    if( m_pComWnd ) 
		return true;
    m_pComWnd = new CComboWndEx();
    ASSERT(m_pComWnd);
    m_pComWnd->Init(this);
    if( m_pManager != NULL ) 
		m_pManager->SendNotify(this, _T("dropdown"));
    Invalidate();
    return true;
}

CDuiString CComboEditUI::GetText() const
{
    if( m_iCurSel < 0 ) return m_sText;
    CControlUI* pControl = static_cast<CControlUI*>(m_items[m_iCurSel]);
    return pControl->GetText();
}

void CComboEditUI::SetEnabled(bool bEnable)
{
    CContainerUI::SetEnabled(bEnable);
    if( !IsEnabled() ) m_uButtonState = 0;
}

CDuiString CComboEditUI::GetDropBoxAttributeList()
{
    return m_sDropBoxAttributes;
}

void CComboEditUI::SetDropBoxAttributeList(LPCTSTR pstrList)
{
    m_sDropBoxAttributes = pstrList;
}

void CComboEditUI::SetDropBoxSizeBute(LPCTSTR pstrList)
{
	CDuiString temp(pstrList);
	int i = temp.Find(_T(","));
	if(i == -1)
		return;
	CDuiString x = temp.Left(i);
	CDuiString y = temp.Right(temp.GetLength()-i-1);
	m_szDropBox.cx = _ttoi(x.GetData());
	m_szDropBox.cy = _ttoi(y.GetData());
}

SIZE CComboEditUI::GetDropBoxSize() const
{
    return m_szDropBox;
}

void CComboEditUI::SetDropBoxSize(SIZE szDropBox)
{
    m_szDropBox = szDropBox;
}

RECT CComboEditUI::GetTextPadding() const
{
    return m_rcTextPadding;
}

void CComboEditUI::SetTextPadding(RECT rc)
{
    m_rcTextPadding = rc;
    Invalidate();
}

LPCTSTR CComboEditUI::GetArrowImage()const
{
	return m_sArrowImage;
}

void CComboEditUI::SetArrowImage(LPCTSTR pStrImage)
{
	m_sArrowImage = pStrImage;
	Invalidate();

	CDuiString sModify = m_sArrowImage;
	int nPos1 = sModify.Find(_T("source"));
	int nPos2 = sModify.Find(_T("'"), nPos1 + 7);
	if (nPos2 == -1) return; //first
	int nPos3 = sModify.Find(_T("'"), nPos2 + 1);
	if (nPos3 == -1) return; //second

	CDuiRect rcBmpPart;
	LPTSTR lpszValue = NULL;
	rcBmpPart.left = _tcstol(sModify.GetData() + nPos2 + 1, &lpszValue, 10);  ASSERT(lpszValue);    
	rcBmpPart.top = _tcstol(lpszValue + 1, &lpszValue, 10);    ASSERT(lpszValue);    
	rcBmpPart.right = _tcstol(lpszValue + 1, &lpszValue, 10);  ASSERT(lpszValue);    
	rcBmpPart.bottom = _tcstol(lpszValue + 1, &lpszValue, 10); ASSERT(lpszValue);  
	m_iBtnWidth = rcBmpPart.GetWidth();
	
}

LPCTSTR CComboEditUI::GetNormalImage() const
{
    return m_sNormalImage;
}

void CComboEditUI::SetNormalImage(LPCTSTR pStrImage)
{
    m_sNormalImage = pStrImage;
    Invalidate();
}

LPCTSTR CComboEditUI::GetHotImage() const
{
    return m_sHotImage;
}

void CComboEditUI::SetHotImage(LPCTSTR pStrImage)
{
    m_sHotImage = pStrImage;
    Invalidate();
}

LPCTSTR CComboEditUI::GetPushedImage() const
{
    return m_sPushedImage;
}

void CComboEditUI::SetPushedImage(LPCTSTR pStrImage)
{
    m_sPushedImage = pStrImage;
    Invalidate();
}

LPCTSTR CComboEditUI::GetFocusedImage() const
{
    return m_sFocusedImage;
}

void CComboEditUI::SetFocusedImage(LPCTSTR pStrImage)
{
    m_sFocusedImage = pStrImage;
    Invalidate();
}

LPCTSTR CComboEditUI::GetDisabledImage() const
{
    return m_sDisabledImage;
}

void CComboEditUI::SetDisabledImage(LPCTSTR pStrImage)
{
    m_sDisabledImage = pStrImage;
    Invalidate();
}

TListInfoUI* CComboEditUI::GetListInfo()
{
    return &m_ListInfo;
}

void CComboEditUI::SetItemFont(int index)
{
    m_ListInfo.nFont = index;
    Invalidate();
}

void CComboEditUI::SetItemTextStyle(UINT uStyle)
{
	m_ListInfo.uTextStyle = uStyle;
	Invalidate();
}

RECT CComboEditUI::GetItemTextPadding() const
{
	return m_ListInfo.rcTextPadding;
}

void CComboEditUI::SetItemTextPadding(RECT rc)
{
    m_ListInfo.rcTextPadding = rc;
    Invalidate();
}

void CComboEditUI::SetItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwTextColor = dwTextColor;
    Invalidate();
}

void CComboEditUI::SetItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwBkColor = dwBkColor;
}

void CComboEditUI::SetItemBkImage(LPCTSTR pStrImage)
{
    m_ListInfo.sBkImage = pStrImage;
}

DWORD CComboEditUI::GetItemTextColor() const
{
	return m_ListInfo.dwTextColor;
}

DWORD CComboEditUI::GetItemBkColor() const
{
	return m_ListInfo.dwBkColor;
}

LPCTSTR CComboEditUI::GetItemBkImage() const
{
	return m_ListInfo.sBkImage;
}

bool CComboEditUI::IsAlternateBk() const
{
    return m_ListInfo.bAlternateBk;
}

void CComboEditUI::SetAlternateBk(bool bAlternateBk)
{
    m_ListInfo.bAlternateBk = bAlternateBk;
}

void CComboEditUI::SetSelectedItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwSelectedTextColor = dwTextColor;
}

void CComboEditUI::SetSelectedItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwSelectedBkColor = dwBkColor;
}

void CComboEditUI::SetSelectedItemImage(LPCTSTR pStrImage)
{
	m_ListInfo.sSelectedImage = pStrImage;
}

DWORD CComboEditUI::GetSelectedItemTextColor() const
{
	return m_ListInfo.dwSelectedTextColor;
}

DWORD CComboEditUI::GetSelectedItemBkColor() const
{
	return m_ListInfo.dwSelectedBkColor;
}

LPCTSTR CComboEditUI::GetSelectedItemImage() const
{
	return m_ListInfo.sSelectedImage;
}

void CComboEditUI::SetHotItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwHotTextColor = dwTextColor;
}

void CComboEditUI::SetHotItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwHotBkColor = dwBkColor;
}

void CComboEditUI::SetHotItemImage(LPCTSTR pStrImage)
{
    m_ListInfo.sHotImage = pStrImage;
}

DWORD CComboEditUI::GetHotItemTextColor() const
{
	return m_ListInfo.dwHotTextColor;
}
DWORD CComboEditUI::GetHotItemBkColor() const
{
	return m_ListInfo.dwHotBkColor;
}

LPCTSTR CComboEditUI::GetHotItemImage() const
{
	return m_ListInfo.sHotImage;
}

void CComboEditUI::SetDisabledItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwDisabledTextColor = dwTextColor;
}

void CComboEditUI::SetDisabledItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwDisabledBkColor = dwBkColor;
}

void CComboEditUI::SetDisabledItemImage(LPCTSTR pStrImage)
{
    m_ListInfo.sDisabledImage = pStrImage;
}

DWORD CComboEditUI::GetDisabledItemTextColor() const
{
	return m_ListInfo.dwDisabledTextColor;
}

DWORD CComboEditUI::GetDisabledItemBkColor() const
{
	return m_ListInfo.dwDisabledBkColor;
}

LPCTSTR CComboEditUI::GetDisabledItemImage() const
{
	return m_ListInfo.sDisabledImage;
}

DWORD CComboEditUI::GetItemLineColor() const
{
	return m_ListInfo.dwLineColor;
}

void CComboEditUI::SetItemLineColor(DWORD dwLineColor)
{
    m_ListInfo.dwLineColor = dwLineColor;
}

bool CComboEditUI::IsItemShowHtml()
{
    return m_ListInfo.bShowHtml;
}

void CComboEditUI::SetItemShowHtml(bool bShowHtml)
{
    if( m_ListInfo.bShowHtml == bShowHtml ) return;

    m_ListInfo.bShowHtml = bShowHtml;
    Invalidate();
}

void CComboEditUI::SetPos(RECT rc)
{
    // Put all elements out of sight
    RECT rcNull = { 0 };
    for( int i = 0; i < m_items.GetSize(); i++ ) 
		static_cast<CControlUI*>(m_items[i])->SetPos(rcNull);
    // Position this control
    CControlUI::SetPos(rc);
}

void CComboEditUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if( _tcscmp(pstrName, _T("textpadding")) == 0 ) 
	{
        RECT rcTextPadding = { 0 };
        LPTSTR pstr = NULL;
        rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
        rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
        rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
        SetTextPadding(rcTextPadding);
    }
	else if( _tcscmp(pstrName, _T("arrowimage")) == 0 ) SetArrowImage(pstrValue);
    else if( _tcscmp(pstrName, _T("normalimage")) == 0 ) SetNormalImage(pstrValue);
    else if( _tcscmp(pstrName, _T("hotimage")) == 0 ) SetHotImage(pstrValue);
    else if( _tcscmp(pstrName, _T("pushedimage")) == 0 ) SetPushedImage(pstrValue);
    else if( _tcscmp(pstrName, _T("focusedimage")) == 0 ) SetFocusedImage(pstrValue);
    else if( _tcscmp(pstrName, _T("disabledimage")) == 0 ) SetDisabledImage(pstrValue);
    else if( _tcscmp(pstrName, _T("dropbox")) == 0 ) SetDropBoxAttributeList(pstrValue);
	else if( _tcscmp(pstrName, _T("dropboxsize")) == 0 ) SetDropBoxSizeBute(pstrValue);
	else if( _tcscmp(pstrName, _T("dropboxcolor")) == 0 ) 
	{
		if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		m_dwDropBoxColor = clrColor;
	}
	else if( _tcscmp(pstrName, _T("dropboxbordercolor")) == 0 ) 
	{
		if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		m_dwDropBoxBorderColor = clrColor;
	}
    else if( _tcscmp(pstrName, _T("itemfont")) == 0 ) m_ListInfo.nFont = _ttoi(pstrValue);
    else if( _tcscmp(pstrName, _T("itemalign")) == 0 )
	{
        if( _tcsstr(pstrValue, _T("left")) != NULL ) {
            m_ListInfo.uTextStyle &= ~(DT_CENTER | DT_RIGHT);
            m_ListInfo.uTextStyle |= DT_LEFT;
        }
        if( _tcsstr(pstrValue, _T("center")) != NULL ) {
            m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_RIGHT);
            m_ListInfo.uTextStyle |= DT_CENTER;
        }
        if( _tcsstr(pstrValue, _T("right")) != NULL ) {
            m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_CENTER);
            m_ListInfo.uTextStyle |= DT_RIGHT;
        }
    }
    if( _tcscmp(pstrName, _T("itemtextpadding")) == 0 ) 
	{
        RECT rcTextPadding = { 0 };
        LPTSTR pstr = NULL;
        rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
        rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
        rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
        SetItemTextPadding(rcTextPadding);
    }
    else if( _tcscmp(pstrName, _T("itemtextcolor")) == 0 ) 
	{
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetItemTextColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itembkcolor")) == 0 ) 
	{
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetItemBkColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itembkimage")) == 0 ) SetItemBkImage(pstrValue);
    else if( _tcscmp(pstrName, _T("itemaltbk")) == 0 ) SetAlternateBk(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("itemselectedtextcolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetSelectedItemTextColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemselectedbkcolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetSelectedItemBkColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemselectedimage")) == 0 ) SetSelectedItemImage(pstrValue);
    else if( _tcscmp(pstrName, _T("itemhottextcolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetHotItemTextColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemhotbkcolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetHotItemBkColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemhotimage")) == 0 ) SetHotItemImage(pstrValue);
    else if( _tcscmp(pstrName, _T("itemdisabledtextcolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetDisabledItemTextColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemdisabledbkcolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetDisabledItemBkColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemdisabledimage")) == 0 ) SetDisabledItemImage(pstrValue);
    else if( _tcscmp(pstrName, _T("itemlinecolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetItemLineColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemshowhtml")) == 0 ) SetItemShowHtml(_tcscmp(pstrValue, _T("true")) == 0);
    else CContainerUI::SetAttribute(pstrName, pstrValue);
}

void CComboEditUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    CControlUI::DoPaint(hDC, rcPaint);
}

void CComboEditUI::PaintStatusImage(HDC hDC)
{
	m_rcBtn = m_rcItem;
	m_rcBtn.left = m_rcBtn.right - m_iBtnWidth;

    if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
    else m_uButtonState &= ~ UISTATE_FOCUSED;
    if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
    else m_uButtonState &= ~ UISTATE_DISABLED;

    if( (m_uButtonState & UISTATE_DISABLED) != 0 )
	{
        if( !m_sDisabledImage.IsEmpty() ) {
            if( !DrawImage(hDC, (LPCTSTR)m_sDisabledImage) ) m_sDisabledImage.Empty();
            else return;
        }
    }
    else if( (m_uButtonState & UISTATE_PUSHED) != 0 ) 
	{
        if( !m_sPushedImage.IsEmpty() ) 
		{
            if( !DrawImage(hDC, (LPCTSTR)m_sPushedImage) ) m_sPushedImage.Empty();
            else return;
        }
    }
    else if( (m_uButtonState & UISTATE_HOT) != 0 ) 
	{
        if( !m_sHotImage.IsEmpty() ) 
		{
            if( !DrawImage(hDC, (LPCTSTR)m_sHotImage) ) m_sHotImage.Empty();
            else return;
        }
    }
    else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) 
	{
        if( !m_sFocusedImage.IsEmpty() ) {
            if( !DrawImage(hDC, (LPCTSTR)m_sFocusedImage) ) m_sFocusedImage.Empty();
            else return;
        }
    }
    if( !m_sNormalImage.IsEmpty() ) 
	{
        if( !DrawImage(hDC, (LPCTSTR)m_sNormalImage) ) m_sNormalImage.Empty();
        else return;
    }
}

void CComboEditUI::PaintText(HDC hDC)
{
    RECT rcText = m_rcItem;
    rcText.left += m_rcTextPadding.left;
	rcText.left += 2;//自己添加的
    rcText.right -= m_rcTextPadding.right;
	rcText.right -= m_iBtnWidth;
    rcText.top += m_rcTextPadding.top;
    rcText.bottom -= m_rcTextPadding.bottom;

    if( m_iCurSel >= 0 ) 
	{
        CControlUI* pControl = static_cast<CControlUI*>(m_items[m_iCurSel]);
        IListItemUI* pElement = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
        if( pElement != NULL ) 
		{
            pElement->DrawItemText(hDC, rcText);
        }
        else 
		{
            RECT rcOldPos = pControl->GetPos();
            pControl->SetPos(rcText);
            pControl->DoPaint(hDC, rcText);
            pControl->SetPos(rcOldPos);
        }
    }
	else
	{
		if (m_sText != _T(""))
		{
			CDuiRect rc_edit = m_rcItem;
			rc_edit.left += m_rcTextPadding.left;
			rc_edit.right -= m_rcTextPadding.right;
			rc_edit.right -= m_iBtnWidth;
			LONG lEditHeight = m_pManager->GetDefaultFontInfo()->tm.tmHeight;
			if( lEditHeight < rc_edit.GetHeight() ) 
			{
				rc_edit.top += (rc_edit.GetHeight() - lEditHeight) / 2;
				rc_edit.bottom = rc_edit.top + lEditHeight;
			}
			CRenderEngine::DrawText(hDC,m_pManager,rc_edit,m_sText,m_dwTextColor,m_iTextFont,DT_VCENTER|DT_LEFT);
		}
	}
}

void CComboEditUI::SetReadOnly(bool bReadOnly)
{
	if( m_bReadOnly == bReadOnly ) return;

	m_bReadOnly = bReadOnly;
	if( m_pComWnd != NULL ) Edit_SetReadOnly(*m_pComWnd, m_bReadOnly);
	Invalidate();
}

bool CComboEditUI::IsReadOnly() const
{
	return m_bReadOnly;
}

void CComboEditUI::SetMaxChar(UINT uMax)
{
	m_uMaxChar = uMax;
	if( m_pEditWnd != NULL ) Edit_LimitText(*m_pEditWnd, m_uMaxChar);
}

UINT CComboEditUI::GetMaxChar()
{
	 return m_uMaxChar;
}

void CComboEditUI::SetTextColor(DWORD dwColor)
{
	m_dwTextColor = dwColor;
	Invalidate();
}

DWORD CComboEditUI::GetTextColor()
{
	return m_dwTextColor;
}

void CComboEditUI::SetTestFont(int index)
{
	m_iTextFont = index;
	Invalidate();
}

int  CComboEditUI::GetTextFont()
{
	return m_iTextFont;
}

void CComboEditUI::SetBtnWidth(int width)
{
	m_iBtnWidth = width;
	m_rcBtn.left = m_rcBtn.right - width;
	Invalidate();
}

void CComboEditUI::SetFocus()
{
	if( m_pManager != NULL ) 
		m_pManager->SetFocus(this);
}

} // namespace DuiLib
