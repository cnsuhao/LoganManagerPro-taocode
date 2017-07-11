#ifndef __VIDEOPLAYER_H__
#define __VIDEOPLAYER_H__

#define POINTER_64 __ptr64
#include "IVideoPlayer.h"

#define inline _inline
#define snprintf _snprintf
#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/fifo.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/avstring.h"
#include "libavutil/pixfmt.h"
#include "libavutil/time.h"
#pragma  comment(lib,"avcodec.lib")
#pragma  comment(lib,"avformat.lib")
#pragma  comment(lib,"avutil.lib")
#pragma  comment(lib,"swscale.lib")
#pragma  comment(lib,"swresample.lib")
}

#include "SDL.h"
#include "SDL_thread.h"
#pragma  comment(lib,"SDL.lib")
#pragma  comment(lib,"SDLmain.lib")

////////////////////////////////////////////////////
#define AVCODEC_MAX_AUDIO_FRAME_SIZE	192000 // 1 second of 48khz 32bit audio 
#define SDL_AUDIO_BUFFER_SIZE			1024
#define MAX_AUDIOQ_SIZE					(5 * 16 * 1024)
#define MAX_VIDEOQ_SIZE					(5 * 256 * 1024)
#define AV_SYNC_THRESHOLD				0.01
#define AV_NOSYNC_THRESHOLD				10.0
#define SAMPLE_CORRECTION_PERCENT_MAX	5
#define AUDIO_DIFF_AVG_NB				20
#define REFRESH_EVENT					(SDL_USEREVENT + 1)
#define VIDEO_PICTURE_QUEUE_SIZE		1

enum enAvSyncType
{
	AV_SYNC_AUDIO_MASTER,
	AV_SYNC_VIDEO_MASTER,
	AV_SYNC_EXTERNAL_MASTER,
};

typedef struct PacketQueue 
{
	AVPacketList *first_pkt;
	AVPacketList *last_pkt;
	int nb_packets;
	int size;
	SDL_mutex *mutex;
	SDL_cond *cond;
}PacketQueue;

typedef struct VideoPicture 
{
	SDL_Overlay *bmp;
	int width;
	int height;
	int allocated;
	double pts;
}VideoPicture;


////////////////////////////////////////////////////

class CVideoPlayer : public IVideoPlayer
{
public:
	CVideoPlayer();
	~CVideoPlayer();

	void SetListener(IVideoPlayerListener *pListener);
	void SetHWND(HWND hwnd);
	bool Load(string strPath);

	void Play();
	void Pause();
	void Stop();

	void SetVolume(int nVolume);
	void SetMute(bool bMute);

	void SetScale(enScale eScale);

	void Refresh();

	bool Seek(int64_t nPos);

private:
	bool InitFfmpeg();
	void UnInitFfmpeg();
	bool InitSdl();
	void UnInitSdl();

	void PktQueInit(PacketQueue *pQue);
	void PktQueUninit(PacketQueue *pQue);
	void PktQuePut(PacketQueue *pQue, AVPacket *pPkt);
	int  PktQueGet(PacketQueue *pQue, AVPacket *pPkt, int nBlock);
	void PktQueFlush(PacketQueue *pQue);

	double GetAudioClock();
	double GetVideoClock();
	double GetExternalClock();
	double GetMaterClock();

	int HandleAudio(double& dPts);
	int SyncAudio(int nNbSamples);

	void GetVideoPic();
	int	 ShowVideoPic();
	void RefrshVideo();
	int  HandleVideoPic(AVFrame *pFrame, double pts);
	double SyncVideo(AVFrame *pFrame, double pts);
	
	static void AudioPro(void *pUserdata, Uint8 *pStream, int nLen);
	static int VideoPro(void *pArg);
	static int DecodeThreadPro(void *pArg);
	static int EventPro(void *pArg);

private:
	IVideoPlayerListener *m_pListener;
	HWND	m_hWnd;
	string	m_strPath;
	int		m_nState;

	int     m_nVolume;
	bool	m_bMute;

	enScale m_eScale;

	bool	m_bResize;
	SIZE	m_szSize;
	bool	m_bRefresh;

	AVFormatContext *m_pFmtCtx;
	int64_t			m_nDuration;
	int				m_nFr;
	int				m_nVideoIndex;
	int				m_nAudioIndex;
	int				m_nAudioChn;
	int				m_nAudioChnLayout;
	int				m_nSr;
	SwsContext		*m_pSwsCtx;
	SwrContext		*m_pSwrCtx;

	SDL_mutex   *m_pMutex;
	SDL_cond    *m_pCond;
	SDL_Thread  *m_pDecThread;
	SDL_Thread  *m_pEventThread;

	int			m_nSyncType;
	double		m_dAudioClock;
	double		m_dVideoClock;
	double		m_dExternalClock;

	double		m_dCurrentPts;
	int64_t		m_nCurrentPtsTime;
	double      m_dVideoFrameTime;
	double      m_dVideoFrameLastPts;
	double      m_dVideoFrameLastDelay;

	double		m_dAudioDiffCnt;
	double		m_dAudioDiffThreshold;
	double		m_dAudioDiffAvgCoef;
	double      m_dAudioDiffAvgCnt;

	unsigned int    m_nAudioBufSize;
	unsigned int    m_nAudioBufIndex;
	uint8_t     *m_pAudioBuf;
	//uint8_t     m_szAudioBuf[AVCODEC_MAX_AUDIO_FRAME_SIZE];
	DECLARE_ALIGNED(16, uint8_t, m_szAudioBuf[AVCODEC_MAX_AUDIO_FRAME_SIZE*4]);

	AVFrame		*m_pAudioFrame;

	SDL_Surface	*m_pVideoSurface;
	VideoPicture m_VideoPic;
	int			 m_nVideoPicCnt;

	PacketQueue m_qVideoPkt;
	PacketQueue m_qAudioPkt;
};


#endif