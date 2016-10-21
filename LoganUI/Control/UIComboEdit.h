#ifndef __UICOMBOEDIT_H__
#define __UICOMBOEDIT_H__

#pragma once

namespace DuiLib 
{
	/////////////////////////////////////////////////////////////////////////////////////
	//
	class CComboEditUI;
	class CComboWndEx : public CWindowWnd
	{
	public:
		void Init(CComboEditUI* pOwner);
		LPCTSTR GetWindowClassName() const;
		void OnFinalMessage(HWND hWnd);

		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

		void EnsureVisible(int iIndex);
		void Scroll(int dx, int dy);

	public:
		CPaintManagerUI m_pm;
		CComboEditUI* m_pOwner;
		CVerticalLayoutUI* m_pLayout;
		int m_iOldSel;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//
	class CComboEditWnd: public CWindowWnd
	{
	public:
		CComboEditWnd();

		void Init(CComboEditUI* pOwner);
		RECT CalPos();

		LPCTSTR GetWindowClassName() const;
		LPCTSTR GetSuperClassName() const;
		void OnFinalMessage(HWND hWnd);

		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	protected:
		CComboEditUI* m_pOwner;
		HBRUSH m_hBkBrush;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CComboEditUI : public CContainerUI, public IListOwnerUI //Combo控件
	{
		friend CComboWndEx;
		friend CComboEditWnd;
	public:
		CComboEditUI();//下拉框的大小及颜色

		LPCTSTR GetClass() const;
		LPVOID  GetInterface(LPCTSTR pstrName);

		void DoInit();//选择项<0，默认选择第0项
		UINT GetControlFlags() const;

		CDuiString GetText() const;
		void SetEnabled(bool bEnable = true);

		CDuiString GetDropBoxAttributeList();
		void SetDropBoxAttributeList(LPCTSTR pstrList);
		void SetDropBoxSizeBute(LPCTSTR pstrList);
		SIZE GetDropBoxSize() const;
		void SetDropBoxSize(SIZE szDropBox);//设置下拉框大小

		int  GetCurSel() const;  //获取当前选中项
		bool SelectItem(int iIndex, bool bTakeFocus = false);	//设置当前选中项
	    
		bool SetItemIndex(CControlUI* pControl, int iIndex);//按项的控件指针获取项序号
		bool Add(CControlUI* pControl);
		bool AddAt(CControlUI* pControl, int iIndex);
		bool Remove(CControlUI* pControl);
		bool RemoveAt(int iIndex);
		void RemoveAll();

		bool Activate();

		RECT GetTextPadding() const;
		void SetTextPadding(RECT rc);
		LPCTSTR GetArrowImage()const;
		void    SetArrowImage(LPCTSTR pStrImage);//设置下拉按钮图片
		LPCTSTR GetNormalImage() const;
		void	SetNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetHotImage() const;
		void	SetHotImage(LPCTSTR pStrImage);
		LPCTSTR GetPushedImage() const;
		void	SetPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetFocusedImage() const;
		void	SetFocusedImage(LPCTSTR pStrImage);
		LPCTSTR GetDisabledImage() const;
		void	SetDisabledImage(LPCTSTR pStrImage);

		TListInfoUI* GetListInfo();
		void SetItemFont(int index);
		void SetItemTextStyle(UINT uStyle);
		RECT GetItemTextPadding() const;
		void SetItemTextPadding(RECT rc);
		DWORD GetItemTextColor() const;
		void SetItemTextColor(DWORD dwTextColor);
		DWORD GetItemBkColor() const;
		void SetItemBkColor(DWORD dwBkColor);
		LPCTSTR GetItemBkImage() const;
		void SetItemBkImage(LPCTSTR pStrImage);
		bool IsAlternateBk() const;
		void SetAlternateBk(bool bAlternateBk);
		DWORD GetSelectedItemTextColor() const;
		void SetSelectedItemTextColor(DWORD dwTextColor);
		DWORD GetSelectedItemBkColor() const;
		void SetSelectedItemBkColor(DWORD dwBkColor);
		LPCTSTR GetSelectedItemImage() const;
		void SetSelectedItemImage(LPCTSTR pStrImage);
		DWORD GetHotItemTextColor() const;
		void SetHotItemTextColor(DWORD dwTextColor);
		DWORD GetHotItemBkColor() const;
		void SetHotItemBkColor(DWORD dwBkColor);
		LPCTSTR GetHotItemImage() const;
		void SetHotItemImage(LPCTSTR pStrImage);
		DWORD GetDisabledItemTextColor() const;
		void SetDisabledItemTextColor(DWORD dwTextColor);
		DWORD GetDisabledItemBkColor() const;
		void SetDisabledItemBkColor(DWORD dwBkColor);
		LPCTSTR GetDisabledItemImage() const;
		void SetDisabledItemImage(LPCTSTR pStrImage);
		DWORD GetItemLineColor() const;
		void SetItemLineColor(DWORD dwLineColor);
		bool IsItemShowHtml();
		void SetItemShowHtml(bool bShowHtml = true);

		SIZE EstimateSize(SIZE szAvailable);
		void SetPos(RECT rc);
		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	    
		void DoPaint(HDC hDC, const RECT& rcPaint);
		void PaintText(HDC hDC);
		void PaintStatusImage(HDC hDC);

		void SetReadOnly(bool bReadOnly);
		bool IsReadOnly() const;
		void SetMaxChar(UINT uMax);
		UINT GetMaxChar();
		void SetTextColor(DWORD dwColor);
		DWORD GetTextColor();
		void SetTestFont(int index);
		int  GetTextFont();
		void SetBtnWidth(int width);
		void SetFocus();

	protected:
		CComboWndEx* m_pComWnd;
		CComboEditWnd* m_pEditWnd;

		int m_iCurSel;
		bool m_bReadOnly;
		UINT m_uMaxChar;
		RECT m_rcTextPadding;
		CDuiString m_sDropBoxAttributes;
		SIZE m_szDropBox;
		UINT m_uButtonState;
		DWORD m_dwDropBoxColor;
		DWORD m_dwDropBoxBorderColor;

		CDuiString m_sArrowImage;
		CDuiString m_sNormalImage;
		CDuiString m_sHotImage;
		CDuiString m_sPushedImage;
		CDuiString m_sFocusedImage;
		CDuiString m_sDisabledImage;

		TListInfoUI m_ListInfo;

		int m_iBtnWidth;
		CDuiRect m_rcBtn;
		DWORD m_dwTextColor;
		int   m_iTextFont;
	};


} // namespace DuiLib

#endif // __UICOMBO_H__
