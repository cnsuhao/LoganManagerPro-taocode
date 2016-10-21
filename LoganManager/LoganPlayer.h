#pragma once
#include "IMediaPlayer.h"
#include "stdafx.h"

#include "../include/vlc/vlc.h"
#pragma	comment(lib,"..\\lib\\libvlc.lib")
#pragma	comment(lib,"..\\lib\\libvlccore.lib")

typedef void (*callback)(void *data);
class LoganPlayer : public IMediaPlayer
{
	friend void onPlayer_event(const libvlc_event_t *event ,void *data);

public:
	LoganPlayer(void);
	~LoganPlayer(void);
	bool play(const std::string &path);
	void play();
	void pause();
	void stop();
	void setVolume(int vol);//设置音量
	void mute();
	void volumeUp();
	void volumeDown();
	int getVolume();
	void To(int pos);//跳到指定位置
	void forward();
	void backward();
	void setHwnd(HWND hwnd);
	HWND getHwnd();
	void flush(){}
	bool isOpened();
	bool isPlaying();
	int getPos();
	libvlc_time_t getTotaltime();
	libvlc_time_t getTime();

	void setCallbackPlaying(callback   pf);
	void setCallbackPosChanged(callback pf);
	void setCallbackEndPlayed(callback pf);
	void release();
protected:
	HWND playHwnd;
private:
	libvlc_instance_t *playerInstance;
	libvlc_media_player_t  *player;
	HWND  hwnd;
	callback   callbackPlaying;
	callback   callbackPosChanged;
	callback   callbackEndPlayed;
	void init();

};