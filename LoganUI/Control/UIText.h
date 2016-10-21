#ifndef __UITEXT_H__
#define __UITEXT_H__

#pragma once

namespace DuiLib
{
	enum emRollDirection
	{
		RollDirection_Left = 0,	// 从左往右
		RollDirection_top,
		RollDirection_right,
		RollDirection_bottom
	};
	const UINT nRollTimer = 22;

	class UILIB_API CTextUI : public CLabelUI
	{
	public:
		CTextUI();
		~CTextUI();

		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		CDuiString* GetLinkContent(int iIndex);

		void DoEvent(TEventUI& event);
		SIZE EstimateSize(SIZE szAvailable);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void PaintText(HDC hDC);

		void StartRoll(UINT nStep=5,emRollDirection emDirection=RollDirection_right);
		void StopRoll();

	protected:
		enum { MAX_LINK = 8 };
		int m_nLinks;
		RECT m_rcLinks[MAX_LINK];
		CDuiString m_sLinks[MAX_LINK];
		int m_nHoverLink;
		int m_nLinePad;
		
		//文字滚动
		UINT m_nStep;		// 更新步长
		int m_nDirection;	// 滚动方向
		int m_nRollLength;	// 滚动绘画区域
	};

} // namespace DuiLib

#endif //__UITEXT_H__