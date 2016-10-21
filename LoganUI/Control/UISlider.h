#ifndef __UISLIDER_H__
#define __UISLIDER_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CSliderUI : public CProgressUI
	{
	public:
		CSliderUI();

		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetEnabled(bool bEnable = true);

		int GetChangeStep();
		void SetChangeStep(int step);
		void SetThumbSize(SIZE szXY);
		RECT GetThumbRect() const;
		LPCTSTR GetThumbImage() const;
		void SetThumbImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbHotImage() const;
		void SetThumbHotImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbPushedImage() const;
		void SetThumbPushedImage(LPCTSTR pStrImage);
		
		void    SetFrontImage(LPCTSTR pStrImage);
		LPCTSTR GetFrontImage();
		void    SetSpectrumImage(LPCTSTR pStrImage);
		LPCTSTR GetSpectrumImage();
		void	SetSpectrumValue(int nValue);

		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void PaintStatusImage(HDC hDC);

	protected:
		SIZE m_szThumb;
		UINT m_uButtonState;
		int m_nStep;

		CDuiString m_sFrontImage;//Ç°ÖÃÍ¼Æ¬
		CDuiString m_sSpectrumImage;//ÆµÆ×Í¼Æ¬
		int		   m_nPeak;//ÆµÆ×Öµ

		CDuiString m_sThumbImage;
		CDuiString m_sThumbHotImage;
		CDuiString m_sThumbPushedImage;
		CDuiString m_sImageModify;
	};
}

#endif // __UISLIDER_H__