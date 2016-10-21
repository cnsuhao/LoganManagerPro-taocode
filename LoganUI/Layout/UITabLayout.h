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
		void SetVerticalDirection(bool bVer=true);	//���ô�ֱ����
		void SetRightToLeft(bool right=true);		//���ô������󻬶�
		void SetBottomToTop(bool bottom=true);		//���ô������ϻ���
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
		bool m_bRight;	//�������󻬶�
		bool m_bBottom;	//�������ϻ���
		bool m_bIsVerticalDirection; //ˮƽ��ֱ
		int m_nPositiveDirection;	 //�����ٶ�
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
