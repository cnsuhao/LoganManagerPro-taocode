#ifndef __UIANIMATION_H__
#define __UIANIMATION_H__

#pragma once

namespace DuiLib {

	class CControlUI;

	class UILIB_API IUIAnimation
	{
	public:
		virtual ~IUIAnimation() { NULL; }

		virtual CControlUI* OwnerControlUI() = 0;
		virtual bool StartAnimation(int nElapse, int nTotalFrame, int nAnimationID = 0, bool bLoop = false) = 0;
		virtual void StopAnimation(int nAnimationID = 0) = 0;
		virtual bool IsAnimationRunning(int nAnimationID) = 0;
		virtual int GetCurrentFrame(int nAnimationID = 0) = 0;
		virtual bool SetCurrentFrame(int nFrame, int nAnimationID = 0) = 0;

		virtual void OnAnimationStep(int nTotalFrame, int nCurFrame, int nAnimationID) = 0;
		virtual void OnAnimationStart(int nAnimationID, bool bFirstLoop) = 0;
		virtual void OnAnimationStop(int nAnimationID) = 0;

		virtual void OnAnimationElapse(int nAnimationID) = 0;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//
	class UILIB_API CAnimationData
	{
	public:
		CAnimationData(int nElipse, int nFrame, int nID, bool bLoop)
		{
			m_bFirstLoop = true;
			m_nCurFrame = 0;
			m_nElapse = nElipse;
			m_nTotalFrame = nFrame;
			m_bLoop = bLoop;
			m_nAnimationID = nID;
		}

	//protected:
	public:
		friend class CDUIAnimation;

		int m_nAnimationID;
		int m_nElapse;

		int m_nTotalFrame;
		int m_nCurFrame;

		bool m_bLoop;
		bool m_bFirstLoop;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//
	class UILIB_API CUIAnimation: public IUIAnimation
	{
	public:
		CUIAnimation();
		virtual ~CUIAnimation();
		virtual CControlUI* OwnerControlUI() = 0;
		virtual bool StartAnimation(int nElapse, int nTotalFrame, int nAnimationID = 0, bool bLoop = false);
		virtual void StopAnimation(int nAnimationID = 0);
		virtual bool IsAnimationRunning(int nAnimationID);
		virtual int GetCurrentFrame(int nAnimationID = 0);
		virtual bool SetCurrentFrame(int nFrame, int nAnimationID = 0);

		virtual void OnAnimationStart(int nAnimationID, bool bFirstLoop) {};
		virtual void OnAnimationStep(int nTotalFrame, int nCurFrame, int nAnimationID) {};
		virtual void OnAnimationStop(int nAnimationID) {};

		virtual void OnAnimationElapse(int nAnimationID);

	protected:
		CAnimationData* GetAnimationDataByID(int nAnimationID, int& index);

	protected:
		CStdPtrArray m_arAnimations;

	};

	/////////////////////////////////////////////////////////////////////////////////////
	//
	class UILIB_API CAnimationCtrlUI : public CControlUI, public CUIAnimation
	{
	public:
		CAnimationCtrlUI();
		CControlUI* OwnerControlUI();

		void SetDirection(bool bVer=true,bool bTopLeft=true); //设置滑动方向
		void Start(bool bTopLeft=true);

	private:
		void DoEvent(TEventUI& event);
		void OnTimer( int nTimerID );
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void PaintText(HDC hDC);
		void AnimationSwitch();
		void OnAnimationStart(INT nAnimationID, BOOL bFirstLoop) {}
		void OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID);
		void OnAnimationStop(INT nAnimationID);

	protected:
		DWORD m_dwTextColor;
		UINT  m_uTextStyle;
		int	  m_iFont;
		bool m_bVertical; //水平或垂直
		bool m_bTopLeft;  //从左边(水平)或上边(垂直)
		int  m_nSpeed;	  //控制速度
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

} // namespace DuiLib

#endif // __UIANIMATION_H__