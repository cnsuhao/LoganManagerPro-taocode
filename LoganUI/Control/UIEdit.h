#ifndef __UIEDIT_H__
#define __UIEDIT_H__

#pragma once

namespace DuiLib
{
	class CEditWnd;

	class UILIB_API CEditUI : public CLabelUI
	{
		friend class CEditWnd;
	public:
		CEditUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		void SetEnabled(bool bEnable = true);
		void SetText(LPCTSTR pstrText);
		void SetMaxChar(UINT uMax);
		UINT GetMaxChar();
		void SetReadOnly(bool bReadOnly);
		bool IsReadOnly() const;
		void SetPasswordMode(bool bPasswordMode);
		bool IsPasswordMode() const;
		void SetPasswordChar(TCHAR cPasswordChar);
		TCHAR GetPasswordChar() const;
		void SetNumberOnly(bool bNumberOnly);
		bool IsNumberOnly() const;
		int GetWindowStyls() const;

		LPCTSTR GetNormalImage();
		void SetNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetHotImage();
		void SetHotImage(LPCTSTR pStrImage);
		LPCTSTR GetFocusedImage();
		void SetFocusedImage(LPCTSTR pStrImage);
		LPCTSTR GetDisabledImage();
		void SetDisabledImage(LPCTSTR pStrImage);
		void SetNativeEditBkColor(DWORD dwBkColor); //编辑窗口的背景颜色
		DWORD GetNativeEditBkColor() const;
		void SetNativeEditTextColor( LPCTSTR pStrColor ); //编辑窗口的文本颜色(获取焦点时)
		DWORD GetNativeEditTextColor() const;

		void SetTipValue(LPCTSTR pStrTipValue);//文本框内提示文字，当文本框text为空时显示并失去焦点时显示  
		CDuiString GetTipValue();
		void SetTipValueColor(LPCTSTR pStrColor);  
		DWORD GetTipValueColor();  
		LPCTSTR GetSrcTipValue(); 

		void SetSel(long nStartChar, long nEndChar);
		void SetSelAll();
		void SetReplaceSel(LPCTSTR lpszReplace);

		void SetPos(RECT rc);
		void SetVisible(bool bVisible = true);
		void SetInternVisible(bool bVisible = true);
		SIZE EstimateSize(SIZE szAvailable);
		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintStatusImage(HDC hDC);
		void PaintText(HDC hDC);

		void SetFocus();

	protected:
		CEditWnd* m_pWindow;

		UINT m_uMaxChar;
		bool m_bReadOnly;
		bool m_bPasswordMode;
		TCHAR m_cPasswordChar;
		UINT m_uButtonState;
		CDuiString m_sNormalImage;
		CDuiString m_sHotImage;
		CDuiString m_sFocusedImage;
		CDuiString m_sDisabledImage;
		DWORD m_dwEditbkColor;
		DWORD m_dwEditTextColor;
		int   m_iWindowStyls;

		CDuiString m_sTipValue;  
		CDuiString m_sSrcTipValue;  
		DWORD      m_sTipValueColor;
	};
}
#endif // __UIEDIT_H__