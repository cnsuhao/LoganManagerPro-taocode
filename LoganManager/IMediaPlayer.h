#pragma  once

__interface IMediaPlayer
{
	virtual void play() = 0;
	virtual bool play(const std::string &path) = 0;
	virtual void pause() = 0;
	virtual void stop() = 0;
	virtual void setVolume(int vol) = 0;
	virtual void volumeUp() = 0;
	virtual void mute()=0;
	virtual void volumeDown() = 0;
	virtual int	 getVolume() = 0;
	virtual void setHwnd(HWND hwnd) = 0;
	virtual HWND getHwnd() = 0;
	virtual bool isOpened() = 0;
	virtual bool isPlaying() = 0;
	virtual void flush() = 0;
	virtual void forward() = 0;
	virtual void backward() = 0;
};


