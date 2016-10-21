#ifndef __UITABLAYOUT_H__
#define __UITABLAYOUT_H__

#pragma once
#include "Control/UIAnimation.h"
namespace DuiLib
{
	class UILIB_API CTabLayoutUI : public CContainerUI
	{
	public:
		CTabLayoutUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		bool Add(CControlUI* pControl);
		bool AddAt(CControlUI* pControl, int iIndex);
		bool Remove(CControlUI* pControl);
		void RemoveAll();
		int GetCurSel() const;
		bool SelectItem(int iIndex);
		bool SelectItem(CControlUI* pControl);

		void SetPos(RECT rc);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	protected:
		int m_iCurSel;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//
	class UILIB_API CAnimationTabLayoutUI : public CTabLayoutUI, public CUIAnimation
	{
	public:
		CAnimationTabLayoutUI();

		LPCTSTR GetClass() const;
		LPVOID  GetInterface(LPCTSTR pstrName);
		CControlUI* OwnerControlUI();
		void SetVerticalDirection(bool bVer=true);	//设置垂直滑动
		void SetRightToLeft(bool right=true);		//设置从右往左滑动
		void SetBottomToTop(bool bottom=true);		//设置从下往上滑动
		bool SelectItem( int iIndex );
		
	private:
		void DoEvent(TEventUI& event);
		void OnTimer( int nTimerID );
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void AnimationSwitch();
		void OnAnimationStart(INT nAnimationID, BOOL bFirstLoop) {}
		void OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID);
		void OnAnimationStop(INT nAnimationID);

	protected:
		bool m_bRight;	//从右往左滑动
		bool m_bBottom;	//从下往上滑动
		bool m_bIsVerticalDirection; //水平或垂直
		int m_nPositiveDirection;	 //控制速度
		RECT m_rcCurPos;
		RECT m_rcItemOld;
		CControlUI* m_pCurrentControl;
		bool m_bControlVisibleFlag;
		enum
		{
			TAB_ANIMATION_ID = 1,
			TAB_ANIMATION_ELLAPSE = 10,
			TAB_ANIMATION_FRAME_COUNT = 15,
		};
	};


}
#endif // __UITABLAYOUT_H__
