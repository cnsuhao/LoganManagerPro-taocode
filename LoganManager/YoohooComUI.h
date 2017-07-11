#pragma once
#include "stdafx.h"
#include "YoohooVideo.h"
class YoohooComUI :public CVerticalLayoutUI
{
public:
	YoohooComUI(void);
	~YoohooComUI(void);
	int getVideoCount()const;
	YoohooVideo* getLocalVideo();
	YoohooVideo* operator[] (int index);
	YoohooVideo* getVideo(int index);
	bool addVideo();
	bool removeVideo();
	
private:
	int count;
	vector<YoohooVideo*> videoList;
	stack<YoohooVideo*> currentVideoStackList;
	stack<YoohooVideo*> waitVideoStackList;
	void VideoVisibleChange();
    void SetPos(RECT rc);
	void Init();
	void video_1(RECT rc);
	void video_2(RECT rc);
	void video_3(RECT rc);
	void video_4(RECT rc);
	void video_5(RECT rc);
	void video_6(RECT rc);
	void video_7(RECT rc);
	void video_8(RECT rc);
	void video_9(RECT rc);
	void video_10(RECT rc);
};
