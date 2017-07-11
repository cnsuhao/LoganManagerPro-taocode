#pragma once
#include "stdafx.h"
#include "../VideoPlayer/IVideoPlayer.h"
#include "IMediaPlayer.h"
#include "LoganPlayer.h"
#include "ILivePlayer.h"
#define _VLC_

class YoohooVideo;
class YoohooVideoWnd :public CWindowWnd
{
public:
	YoohooVideoWnd();
	LPCTSTR GetWindowClassName() const;
	void Init(YoohooVideo* pOwner);
	void PainBk();
	RECT CalPos();
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	YoohooVideo*	    m_pOwner;
};

class YoohooVideo :public CContainerUI
{
	friend class YoohooVideoWnd;
public:
	YoohooVideo(void);
	~YoohooVideo(void);
	void SetVisible(bool bVisible=true );
	void SetPos(RECT rc);
	HWND getHwnd();
	void Init();
#ifdef VLC_VIDEO
	IMediaPlayer     *videoPlayer;
#else
	ILivePlayer		 *videoPlayer;
#endif
private:
	YoohooVideoWnd *m_pwindows;
};
