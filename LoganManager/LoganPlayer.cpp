#include "StdAfx.h"
#include "LoganPlayer.h"

LoganPlayer::LoganPlayer(void) :
playerInstance(NULL),
player(NULL),
hwnd(NULL),
callbackPlaying(NULL),
callbackPosChanged(NULL),
callbackEndPlayed(NULL)
{
	init();
}

LoganPlayer::~LoganPlayer(void)
{
	release();
}

void LoganPlayer::init()
{
	if (!playerInstance)
	{
		playerInstance = libvlc_new(0, NULL);

	}
}

void LoganPlayer::release()
{
	stop();
	if (playerInstance)
	{
		libvlc_release(playerInstance);
		playerInstance = NULL;
	}
}

bool LoganPlayer::play(const std::string &path)
{
	if (!playerInstance)
	{
		init();
	}
	if (path.empty() || !playerInstance)
	{
		return false;
	}
	bool isURL = false;
	std::vector<std::string>URL;
	URL.push_back("http");
	URL.push_back("https");
	URL.push_back("ftp");
	URL.push_back("rstp");
	URL.push_back("rtmp");
	for (int i = 0; i < URL.size(); i++)
	{
		if (!path.compare(0, URL[i].size(), URL[i]))
		{
			isURL = true;
			break;
		}
	}

	stop();
	bool isLocal = false;
	libvlc_media_t *m;
	if (isURL)
	{
		m = libvlc_media_new_location(playerInstance, path.c_str());
	}
	else
	{
		m = libvlc_media_new_path(playerInstance, path.c_str());
	}

	if (m)
	{
		if (player = libvlc_media_player_new_from_media(m))
		{
			libvlc_media_add_option(m,"network-caching=0");
			
			libvlc_video_set_mouse_input(player, 0);
			libvlc_media_player_set_hwnd(player, hwnd);
			libvlc_media_player_play(player);
			//libvlc_audio_set_delay(player,0);
			libvlc_audio_set_delay(player,1000);
			libvlc_event_manager_t *manager = libvlc_media_player_event_manager(player);
			libvlc_event_attach(manager, libvlc_MediaPlayerPlaying, ::onPlayer_event, this);
			libvlc_event_attach(manager, libvlc_MediaPlayerPositionChanged, ::onPlayer_event, this);
			libvlc_event_attach(manager, libvlc_MediaPlayerEndReached, ::onPlayer_event, this);
			isLocal = true;
		}
		libvlc_media_release(m);
	}
	return isLocal;

}

void LoganPlayer::stop()
{
	if (player)
	{
		libvlc_media_player_stop(player);
		libvlc_media_player_release(player);
		player = NULL;
	}
}

void LoganPlayer::play()
{
	if (player)
	{
		libvlc_media_player_play(player);
	}
}

void LoganPlayer::pause()
{
	if (player)
	{
		libvlc_media_player_pause(player);
	}
}

void LoganPlayer::setVolume(int vol)
{
	if (vol < 0)
		return;
	if (player)
	{
		libvlc_audio_set_volume(player, vol);
	}
}

void LoganPlayer::mute()
{
	setVolume(0);
}
void LoganPlayer::volumeUp()
{
	if (player)
	{
		int vol = libvlc_audio_get_volume(player);
		setVolume((int)(vol*1.2));
	}
}

void LoganPlayer::volumeDown()
{
	int vol = libvlc_audio_get_volume(player);
	setVolume((int)(vol*0.8));
}

int LoganPlayer::getPos()
{
	if (player)
	{
		return (int)(1000 * libvlc_media_player_get_position(player));
	}
	return 0;
}

void LoganPlayer::To(int pos)
{
	if (pos < 0 || pos>1000)
	{
		return;
	}
	if (player)
	{
		libvlc_media_player_set_position(player, pos / (float)1000.0);
	}
}

void LoganPlayer::backward()
{
	if (player)
	{
		libvlc_time_t _time = libvlc_media_player_get_time(player) - 5000;
		if (_time < 0)
		{
			_time = 0;
		}
		libvlc_media_player_set_time(player, _time);
	}
}

void LoganPlayer::forward()
{
	if (player)
	{
		libvlc_time_t _time = libvlc_media_player_get_time(player) + 5000;
		if (_time > getTotaltime())
		{
			_time = getTotaltime();
		}
		libvlc_media_player_set_time(player, _time);
	}
}

void LoganPlayer::setHwnd(HWND hwnd)
{
	if (::IsWindow(hwnd))
	{
		this->hwnd = hwnd;
	}
}

HWND LoganPlayer::getHwnd()
{
	return this->hwnd;
}

bool LoganPlayer::isOpened()
{
	return NULL != player;
}

bool LoganPlayer::isPlaying()
{
	if (player)
	{
		return (1 == libvlc_media_player_is_playing(player));
	}
	return false;
}

libvlc_time_t LoganPlayer::getTotaltime()
{
	if (player)
	{
		return libvlc_media_player_get_length(player);
	}
}

libvlc_time_t LoganPlayer::getTime()
{
	if (player)
	{
		return libvlc_media_player_get_time(player);
	}
	return 0;
}

int LoganPlayer::getVolume()
{
	if (player)
	{
		return libvlc_audio_get_volume(player);
	}
	return 0;
}

void LoganPlayer::setCallbackPlaying(callback pf)
{
	callbackPlaying = pf;
}

void LoganPlayer::setCallbackPosChanged(callback pf)
{
	callbackPosChanged = pf;
}
void LoganPlayer::setCallbackEndPlayed(callback pf)
{
	callbackEndPlayed = pf;
}

void onPlayer_event(const libvlc_event_t *event, void *data)
{
	LoganPlayer *loganp = (LoganPlayer*)data;
	callback pf = NULL;
	if (!loganp)
	{
		return;
	}
	switch (event->type)
	{
	case libvlc_MediaPlayerPlaying:
		pf = loganp->callbackPlaying;
		break;
	case libvlc_MediaPlayerPositionChanged:
		pf = loganp->callbackPosChanged;
		break;
	case libvlc_MediaPlayerEndReached:
		pf = loganp->callbackEndPlayed;
		break;
	case libvlc_MediaPlayerBuffering:
		break;
	default:
		
		break;
	}
	if (pf)
		pf(data);
}

