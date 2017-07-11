#define IVIDEOPLAYER_API __declspec(dllexport) 
#include "VideoPlayer.h"

////////////////////////
IVideoPlayer *IVideoPlayer::GetInstance()
{
	CVideoPlayer *pReturn = new CVideoPlayer();
	return (IVideoPlayer *)pReturn;
}

void IVideoPlayer::ReleaseInstance(IVideoPlayer *pInst)
{
	if(pInst)
	{
		delete (CVideoPlayer *)pInst;
	}
}

////////////////////////
AVPacket PktFlush;
static Uint32 SdlTimerCallback(Uint32 interval, void *opaque)
{
	SDL_Event event;
	event.type = REFRESH_EVENT;
	event.user.data1 = opaque;
	SDL_PushEvent(&event);
	return 0;
}

uint64_t global_video_pkt_pts = AV_NOPTS_VALUE;
int GetFrameBuf(struct AVCodecContext *c, AVFrame *pic)
{
	int nRet = avcodec_default_get_buffer(c, pic);
	uint64_t *pPts = (uint64_t *)av_malloc(sizeof(uint64_t));
	*pPts = global_video_pkt_pts;
	pic->opaque = pPts;
	return nRet;
}

void ReleaseFrameBuf(struct AVCodecContext *c, AVFrame *pic)
{
	if(pic) 
		av_freep(&pic->opaque);
	avcodec_default_release_buffer(c, pic);
}

////////////////////////
CVideoPlayer::CVideoPlayer():
m_pListener(NULL),
m_hWnd(NULL),
m_nState(0),
m_nVolume(128),
m_bMute(false),
m_pFmtCtx(NULL),
m_bResize(false),
m_bRefresh(false),
m_nDuration(0),
m_nVideoIndex(-1),
m_nAudioIndex(-1),
m_pSwsCtx(NULL),
m_pSwrCtx(NULL),
m_pVideoSurface(NULL),
m_pDecThread(NULL),
m_pEventThread(NULL),
m_nSyncType(AV_SYNC_VIDEO_MASTER),
m_dAudioClock(0.0),
m_dVideoClock(0.0),
m_dExternalClock(0.0),
m_dCurrentPts(0.0),
m_nCurrentPtsTime(0),
m_dAudioDiffCnt(0.0),
m_dAudioDiffThreshold(0.0),
m_dAudioDiffAvgCoef(0.0),
m_dAudioDiffAvgCnt(0.0),
m_pAudioBuf(NULL),
m_pAudioFrame(NULL),
m_pMutex(NULL),
m_pCond(NULL),
m_eScale(Scale_Src)
{
	memset(&m_qVideoPkt, 0, sizeof(PacketQueue));
	memset(&m_qAudioPkt, 0, sizeof(PacketQueue));
	memset(&m_VideoPic, 0 , sizeof(VideoPicture));
	av_register_all();
	avformat_network_init();
}

CVideoPlayer::~CVideoPlayer()
{
	Stop();
	avformat_network_deinit();
}

void CVideoPlayer::SetListener(IVideoPlayerListener *pListener)
{
	m_pListener = pListener;
}

void CVideoPlayer::SetHWND(HWND hwnd)
{
	char szEnv[64];     
	sprintf(szEnv, "SDL_WINDOWID=%d", hwnd);
	SDL_putenv(szEnv);
	m_hWnd = hwnd;
}

bool CVideoPlayer::Load(string strPath)
{
	bool bRet = false;

	Stop();
	m_strPath = strPath;
	m_nState = 1;
	if(InitFfmpeg() && InitSdl())
	{
		GetVideoPic();
		m_dExternalClock = GetExternalClock();
		m_pEventThread = SDL_CreateThread(EventPro, this);
		m_pDecThread = SDL_CreateThread(DecodeThreadPro, this);

		global_video_pkt_pts = AV_NOPTS_VALUE;
		m_nVideoPicCnt = 0;
		SDL_AddTimer(20, SdlTimerCallback, this);

		bRet = true;
	}

	return bRet;
}

void CVideoPlayer::Play()
{
	if(m_pFmtCtx)
		av_read_play(m_pFmtCtx);
	m_nState = 1;
}

void CVideoPlayer::Pause()
{
	if(m_pFmtCtx)
		av_read_pause(m_pFmtCtx);
	m_nState = 2;
}

void CVideoPlayer::Stop()
{
	m_nState = 0;
	if(m_pDecThread)
	{
		SDL_WaitThread(m_pDecThread, NULL);
		m_pDecThread = NULL;
	}
	if(m_pEventThread)
	{
		SDL_WaitThread(m_pEventThread, NULL);
		m_pEventThread = NULL;
	}
	UnInitFfmpeg();
	UnInitSdl();
	PktQueFlush(&m_qVideoPkt);
	PktQueFlush(&m_qAudioPkt);
}

void CVideoPlayer::SetVolume(int nVolume)
{
	m_nVolume = nVolume;
}

void CVideoPlayer::SetMute(bool bMute)
{
	m_bMute = bMute;
	SDL_PauseAudio(bMute);
}

void CVideoPlayer::SetScale(enScale eScale)
{
	m_eScale = eScale;
	m_bRefresh = true;
}

void CVideoPlayer::Refresh()
{
	if(m_nAudioIndex != -1) 
	{
		PktQueFlush(&m_qVideoPkt);
		PktQuePut(&m_qVideoPkt, &PktFlush);
	}
	if(m_nVideoIndex != -1) 
	{
		PktQueFlush(&m_qAudioPkt);
		PktQuePut(&m_qAudioPkt, &PktFlush);
	}
	if(m_pVideoSurface)
	{
		SDL_FillRect(m_pVideoSurface,NULL,SDL_MapRGB(m_pVideoSurface->format, 0, 0, 0));
		SDL_UpdateRect(m_pVideoSurface, 0, 0, 0, 0);
	}
}

bool CVideoPlayer::Seek(int64_t nPos)
{
	bool bRet = false;

	int nStreamIndex = -1;
	if(m_nVideoIndex != -1) 
		nStreamIndex = m_nVideoIndex;
	else if(m_nAudioIndex != -1) 
		nStreamIndex = m_nAudioIndex;

	if(nStreamIndex != -1) 
	{
		AVRational av_time_base_q = {0};
		av_time_base_q.num = 1;
		av_time_base_q.den = AV_TIME_BASE;
		int64_t nTargetPos = av_rescale_q(nPos, av_time_base_q, m_pFmtCtx->streams[nStreamIndex]->time_base);
		if(av_seek_frame(m_pFmtCtx, nStreamIndex, nTargetPos, AVSEEK_FLAG_FRAME) >= 0) 
		{
			if(m_nAudioIndex != -1) 
			{
				PktQueFlush(&m_qVideoPkt);
				PktQuePut(&m_qVideoPkt, &PktFlush);
			}
			if(m_nVideoIndex != -1) 
			{
				PktQueFlush(&m_qAudioPkt);
				PktQuePut(&m_qAudioPkt, &PktFlush);
			}
			
			m_dExternalClock = GetExternalClock();
			bRet = true;
		}
	}

	return bRet;
}

bool CVideoPlayer::InitFfmpeg()
{
	bool bRet = false;
	//av_register_all();
	//avcodec_register_all();
	//int dbug1=1;
	//char buf[1024];
	//int err_code=avformat_open_input(&m_pFmtCtx, m_strPath.c_str(), NULL, NULL);
	//av_strerror(err_code, buf, 1024);
	//char res[2000];
	//sprintf(res,"Couldn't open file %s: %d(%s)", m_strPath.c_str(), err_code, buf);
	//MessageBoxA(NULL,res,0,0);
	//return -1; 
	//Sleep(2000);
	//int dbug2=1;
	//dbug2=avformat_find_stream_info(m_pFmtCtx, NULL);
	if(avformat_open_input(&m_pFmtCtx, m_strPath.c_str(), NULL, NULL) == 0 &&
		avformat_find_stream_info(m_pFmtCtx, NULL) >= 0) 
	{
		av_dump_format(m_pFmtCtx, 0, m_strPath.c_str(), 0);
		m_nDuration = m_pFmtCtx->duration/1000000;
		if(m_nDuration < 0)
			m_nDuration = 0;

		int nRet = 0;
		m_nVideoIndex = -1;
		m_nAudioIndex = -1;
		for(int i=0; i<m_pFmtCtx->nb_streams; i++)
		{
			AVCodecContext *pCodecCtx = m_pFmtCtx->streams[i]->codec;
			if(pCodecCtx->codec_type == AVMEDIA_TYPE_VIDEO) 
			{
				AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
				if(!pCodec || (avcodec_open2(pCodecCtx, pCodec, NULL) < 0))
					break;
				
				m_nVideoIndex = i;
				pCodecCtx->get_buffer = GetFrameBuf;
				pCodecCtx->release_buffer = ReleaseFrameBuf;

				if(m_nDuration && m_pFmtCtx->streams[i]->nb_frames!=0)
					m_nFr = m_pFmtCtx->streams[i]->nb_frames / m_nDuration;
				else
					m_nFr = m_pFmtCtx->streams[i]->r_frame_rate.num / m_pFmtCtx->streams[i]->r_frame_rate.den;

				m_pSwsCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
				if(!m_pSwsCtx)
					nRet = 1;
			}
			else if(pCodecCtx->codec_type == AVMEDIA_TYPE_AUDIO)
			{
				AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
				if(!pCodec || (avcodec_open2(pCodecCtx, pCodec, NULL) < 0))
					break;

				int nChnLayout = (pCodecCtx->channel_layout && pCodecCtx->channels==av_get_channel_layout_nb_channels(pCodecCtx->channel_layout))
									  ? pCodecCtx->channel_layout : av_get_default_channel_layout(pCodecCtx->channels);
				m_nAudioChnLayout = nChnLayout;
				m_nAudioChnLayout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
				int nChnNb = av_get_channel_layout_nb_channels(m_nAudioChnLayout);
				while(nChnNb > 0 && (nChnNb == 3 || nChnNb == 5 || nChnNb > 6)) 
				{
					nChnNb--;
					m_nAudioChnLayout = av_get_default_channel_layout(nChnNb);
				}
				m_nAudioChn = av_get_channel_layout_nb_channels(m_nAudioChnLayout);
				m_nSr = pCodecCtx->sample_rate;
				m_nAudioIndex = i;
				
				m_pAudioFrame = av_frame_alloc();
				if(m_pAudioFrame)
				{
					if(pCodecCtx->sample_fmt != AV_SAMPLE_FMT_S16 || nChnLayout != m_nAudioChnLayout)
					{
						m_pSwrCtx = swr_alloc_set_opts(NULL,m_nAudioChnLayout, AV_SAMPLE_FMT_S16, pCodecCtx->sample_rate,
							nChnLayout, pCodecCtx->sample_fmt, pCodecCtx->sample_rate,0, NULL);
						if(m_pSwrCtx && swr_init(m_pSwrCtx) >= 0)
							;
						else
							nRet = 3;
					}
				}
				else
				{
					nRet = 2;
				}
			}
		}
		if((m_nVideoIndex != -1 || m_nAudioIndex != -1) && nRet==0)
		{
			av_init_packet(&PktFlush);
			PktFlush.data = (uint8_t *)"FLUSH";

			bRet = true;
		}
	}

	return bRet;
}

void CVideoPlayer::UnInitFfmpeg()
{
	if(m_pSwsCtx)
	{
		sws_freeContext(m_pSwsCtx);
		m_pSwsCtx = NULL;
	}

	if(m_pAudioFrame)
	{
		av_frame_free(&m_pAudioFrame);
		m_pAudioFrame = NULL;
	}

	if(m_pSwrCtx)
	{
		swr_free(&m_pSwrCtx);
		m_pSwrCtx = NULL;
	}

	if(m_pFmtCtx)
	{
		if(m_nVideoIndex != -1)
			avcodec_close(m_pFmtCtx->streams[m_nVideoIndex]->codec);
		if(m_nAudioIndex != -1)
			avcodec_close(m_pFmtCtx->streams[m_nAudioIndex]->codec);
		avformat_close_input(&m_pFmtCtx);
		m_nVideoIndex = -1;
		m_nAudioIndex = -1;
		m_pFmtCtx = NULL;
	}
}

bool CVideoPlayer::InitSdl()
{
	bool bRet = false;

	Uint32 flags = SDL_INIT_TIMER;
	if(m_nVideoIndex != -1)
		flags |= SDL_INIT_VIDEO;
	if(m_nAudioIndex != -1)
		flags |= SDL_INIT_AUDIO;
	if(SDL_Init(flags) == 0)
	{
		int nRet = 0;
		if(m_nAudioIndex != -1)
		{
			SDL_AudioSpec cSpecDesired, cSpecObtained;
			cSpecDesired.channels = m_nAudioChn;
			cSpecDesired.freq = m_nSr;
			cSpecDesired.format = AUDIO_S16SYS;
			cSpecDesired.silence = 0;
			cSpecDesired.samples = SDL_AUDIO_BUFFER_SIZE;
			cSpecDesired.callback = AudioPro;
			cSpecDesired.userdata = this;

			if(SDL_OpenAudio(&cSpecDesired, &cSpecObtained)==-1 || cSpecObtained.format!=AUDIO_S16SYS || cSpecObtained.channels!=cSpecDesired.channels) 
			{
				nRet = 1;
			}
			else
			{
				m_nAudioBufIndex = 0;
				m_nAudioBufSize = 0;
				m_dAudioDiffAvgCoef = exp(log(0.01 / AUDIO_DIFF_AVG_NB));
				m_dAudioDiffAvgCnt = 0;
				m_dAudioDiffThreshold = 2.0 * SDL_AUDIO_BUFFER_SIZE / m_nSr;

				PktQueInit(&m_qAudioPkt);
				SDL_PauseAudio(m_bMute);
			}
		}
		if(m_nVideoIndex != -1)
		{
			int nVideoWidth = 0, nVideoHeight = 0;
			if(m_hWnd)
			{
				RECT rcVideo;
				GetWindowRect(m_hWnd,&rcVideo);
				nVideoWidth = rcVideo.right - rcVideo.left;
				nVideoHeight = rcVideo.bottom - rcVideo.top;
			}
			m_pVideoSurface = SDL_SetVideoMode(nVideoWidth, nVideoHeight, 0, SDL_HWSURFACE|SDL_ASYNCBLIT|SDL_HWACCEL|SDL_RESIZABLE);
			if(m_pVideoSurface)
			{
				m_dVideoFrameTime = (double)av_gettime() / 1000000.0;
				m_dVideoFrameLastDelay = 40e-3;
				m_nCurrentPtsTime = av_gettime();

				PktQueInit(&m_qVideoPkt);
				SDL_CreateThread(CVideoPlayer::VideoPro, this);
			}
			else
			{
				nRet = 2;
			}
		}
		
		if(nRet == 0)
		{
			m_pMutex = SDL_CreateMutex();
			m_pCond = SDL_CreateCond();
			bRet = true;
		}
	}

	return bRet;
}

void CVideoPlayer::UnInitSdl()
{
	SDL_Quit();
}

void CVideoPlayer::PktQueInit(PacketQueue *pQue)
{
	memset(pQue, 0, sizeof(PacketQueue));
	pQue->mutex = SDL_CreateMutex();
	pQue->cond = SDL_CreateCond();
}

void CVideoPlayer::PktQueUninit(PacketQueue *pQue)
{
	if(pQue->mutex)
	{
		SDL_DestroyMutex(pQue->mutex);
		pQue->mutex = NULL;
	}
	if(pQue->cond)
	{
		SDL_DestroyCond(pQue->cond);
		pQue->cond = NULL;
	}
}

void CVideoPlayer::PktQuePut(PacketQueue *pQue, AVPacket *pPkt)
{
	if(pPkt == &PktFlush || av_dup_packet(pPkt) == 0) 
	{
		AVPacketList *pPktList = (AVPacketList *)av_malloc(sizeof(AVPacketList));
		if(pPktList)
		{
			pPktList->pkt = *pPkt;
			pPktList->next = NULL;

			SDL_LockMutex(pQue->mutex);

			if(!pQue->last_pkt) 
				pQue->first_pkt = pPktList;
			else 
				pQue->last_pkt->next = pPktList;
			pQue->last_pkt = pPktList;
			pQue->nb_packets++;
			pQue->size += pPktList->pkt.size;

			SDL_CondSignal(pQue->cond);
			SDL_UnlockMutex(pQue->mutex);
		}
	}
}

int CVideoPlayer::PktQueGet(PacketQueue *pQue, AVPacket *pPkt, int nBlock)
{
	SDL_LockMutex(pQue->mutex);
	int nRet = 0;

	while(m_nState) 
	{
		AVPacketList *pPktList = pQue->first_pkt;
		if(pPktList) 
		{
			pQue->first_pkt = pPktList->next;
			if(!pQue->first_pkt) 
				pQue->last_pkt = NULL;
		
			pQue->nb_packets--;
			pQue->size -= pPktList->pkt.size;
			*pPkt = pPktList->pkt;
			av_free(pPktList);

			nRet = 1;
			break;
		} 
		else if(nBlock) //阻塞获取
		{
			//SDL_CondWait(pQue->cond, pQue->mutex);
			SDL_CondWaitTimeout(pQue->cond, pQue->mutex, 500);
		}
		else 
		{
			break;
		}
	}

	SDL_UnlockMutex(pQue->mutex);
	return nRet;
}

void CVideoPlayer::PktQueFlush(PacketQueue *pQue)
{
	SDL_LockMutex(pQue->mutex);

	AVPacketList *pPktList, *pPktListNext;
	for(pPktList = pQue->first_pkt; pPktList != NULL; pPktList = pPktListNext) 
	{
		pPktListNext = pPktList->next;
		av_free_packet(&pPktList->pkt);
		av_freep(&pPktList);
	}
	pQue->last_pkt = NULL;
	pQue->first_pkt = NULL;
	pQue->nb_packets = 0;
	pQue->size = 0;

	SDL_UnlockMutex(pQue->mutex);
}

// 获取音频目前播放的时间
// audio_callback函数花费了时间来把数据从声音包中移到我们的输出缓冲区中。
// 这意味着我们声音时钟中记录的时间比实际的要早太多。
// 所以我们必须要检查一下我们还有多少没有写入
double CVideoPlayer::GetAudioClock()
{
	double dPts = m_dAudioClock;
	
	unsigned int nBufSize = m_nAudioBufSize - m_nAudioBufIndex;
	if(nBufSize && m_nAudioIndex != -1) 
	{
		unsigned int nSecByte = m_nSr * m_nAudioChn * 2;
		dPts -= (double)nBufSize / nSecByte;
	}

	return dPts;
}

double CVideoPlayer::GetVideoClock()
{
	if(m_nState == 1)
	{
		double dDelay = (av_gettime() - m_nCurrentPtsTime)/1000000.0;
		return m_dCurrentPts + dDelay;
	}
	else
	{
		return 0.0;
	}
}

double CVideoPlayer::GetExternalClock()
{
	return av_gettime()/1000000.0 - m_dExternalClock;
}

double CVideoPlayer::GetMaterClock()
{
	if(m_nSyncType == AV_SYNC_AUDIO_MASTER)
		return GetAudioClock();
	else if(m_nSyncType == AV_SYNC_VIDEO_MASTER)
		return GetVideoClock();
	else 
		return GetExternalClock();
}

int CVideoPlayer::HandleAudio(double& dPts)
{
	int nRet = -1;

	if(m_pAudioFrame)
	{
		AVPacket avPkt = {0};
		av_init_packet(&avPkt);

		AVStream *pStream = m_pFmtCtx->streams[m_nAudioIndex];
		int nPktSize = 0;
		bool bContinue = true;
		while(bContinue && m_nState==1)
		{
			while(nPktSize > 0)
			{
				avcodec_get_frame_defaults(m_pAudioFrame);
				int nGetFrame = 0;
				int nDecLen = avcodec_decode_audio4(pStream->codec, m_pAudioFrame, &nGetFrame, &avPkt);
				if(nDecLen >= 0)
				{
					nPktSize -= nDecLen;
					if(nGetFrame)
					{
						nDecLen = av_samples_get_buffer_size(NULL, pStream->codec->channels, m_pAudioFrame->nb_samples, pStream->codec->sample_fmt, 1);
						int nReSampleLen = nDecLen;
						if(m_pSwrCtx)
						{
							int nWantNbSamples = SyncAudio(m_pAudioFrame->nb_samples);
							if(nWantNbSamples != m_pAudioFrame->nb_samples)
							{
								if(swr_set_compensation(m_pSwrCtx, nWantNbSamples-m_pAudioFrame->nb_samples, nWantNbSamples) < 0)
									break;
							}

							const uint8_t **pIn = (const uint8_t **)m_pAudioFrame->extended_data;
							uint8_t *pOut[] = {m_szAudioBuf};
							int nOutCnt = sizeof(m_szAudioBuf) / m_nAudioChn / av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
							int nSwrLen = swr_convert(m_pSwrCtx, pOut, nOutCnt, pIn, m_pAudioFrame->nb_samples);
							if(nSwrLen >= 0) 
							{
								if(nSwrLen == nOutCnt)
								{
									swr_init(m_pSwrCtx);
								}
								else
								{
									m_pAudioBuf = m_szAudioBuf;
									nReSampleLen = nSwrLen * m_nAudioChn * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);

									dPts = m_dAudioClock;
									m_dAudioClock += (double)nDecLen / (pStream->codec->channels * pStream->codec->sample_rate * av_get_bytes_per_sample(pStream->codec->sample_fmt));

									nRet = nReSampleLen;
									bContinue = false;
								}
							}
							break;
						}
						else
						{
							m_pAudioBuf = m_pAudioFrame->data[0];
							nRet = nReSampleLen;
							bContinue = false;
						}
					}
					else
					{
						break;
					}
				}
				else
				{
					nPktSize = 0;
					break;
				}
			}
			av_free_packet(&avPkt);

			if(bContinue && PktQueGet(&m_qAudioPkt, &avPkt, 1) == 1) 
			{
				if(avPkt.data == PktFlush.data) 
				{
					avcodec_flush_buffers(pStream->codec);
				}
				else
				{
					nPktSize = avPkt.size;
					if(avPkt.pts != AV_NOPTS_VALUE)
						m_dAudioClock = av_q2d(pStream->time_base) * avPkt.pts;
				}
			}
			else
			{
				bContinue = false;
			}
		}
	}

	return nRet;
}

int CVideoPlayer::SyncAudio(int nNbSamples)
{
	int nWantNbSamples = nNbSamples;

	if(m_nSyncType != AV_SYNC_AUDIO_MASTER)
	{
		double dDiff = GetAudioClock() - GetMaterClock();
		if(fabs(dDiff) < AV_NOSYNC_THRESHOLD) 
		{
			m_dAudioDiffCnt = dDiff + m_dAudioDiffAvgCoef * m_dAudioDiffCnt;
			if (m_dAudioDiffAvgCnt < AUDIO_DIFF_AVG_NB) 
			{
				m_dAudioDiffAvgCnt++;
			} 
			else
			{
				double dAvgDiff = m_dAudioDiffCnt * (1.0 - m_dAudioDiffAvgCoef);
				if(fabs(dAvgDiff) >= m_dAudioDiffThreshold) 
				{
					nWantNbSamples = nNbSamples + (int)(dDiff * m_nSr);
					int nMinNbSamples = ((nNbSamples * (100 - SAMPLE_CORRECTION_PERCENT_MAX) / 100.0));
					int nMaxNbSamples = ((nNbSamples * (100 + SAMPLE_CORRECTION_PERCENT_MAX) / 100.0));
					nWantNbSamples = FFMIN(FFMAX(nWantNbSamples, nMinNbSamples), nMaxNbSamples);
				}
			}
		} 
		else 
		{
			m_dAudioDiffAvgCnt = 0;
			m_dAudioDiffCnt = 0;
		}
	}

	return nWantNbSamples;
}

void CVideoPlayer::GetVideoPic()
{
	if(m_VideoPic.allocated)
	{
		if(m_VideoPic.bmp)
		{
			SDL_FreeYUVOverlay(m_VideoPic.bmp);
			m_VideoPic.bmp = NULL;
		}
		m_VideoPic.allocated = 0;
	}

	m_VideoPic.width = m_pFmtCtx->streams[m_nVideoIndex]->codec->width;
	m_VideoPic.height = m_pFmtCtx->streams[m_nVideoIndex]->codec->height;
	m_VideoPic.bmp = SDL_CreateYUVOverlay(m_VideoPic.width, m_VideoPic.height, SDL_YV12_OVERLAY, m_pVideoSurface);
	if(m_VideoPic.bmp)
		m_VideoPic.allocated = 1;
}

int	CVideoPlayer::ShowVideoPic()
{
	int nRet = 0;

	if(m_VideoPic.bmp) 
	{
		SDL_Rect rcShow;
		if(m_eScale == Scale_Full)
		{
			rcShow.x = 0;
			rcShow.y = 0;
			rcShow.w = m_pVideoSurface->w;
			rcShow.h = m_pVideoSurface->h;
		}
		else 
		{
			float fRatio = 1.0;
			if(m_eScale == Scale_Src)
			{
				AVCodecContext *pAvCtx = m_pFmtCtx->streams[m_nVideoIndex]->codec;
				fRatio = (float)pAvCtx->width / (float)pAvCtx->height;
			}
			else if(m_eScale == Scale_1_1)
			{
				fRatio = 1.0;
			}
			else if(m_eScale == Scale_4_3)
			{
				fRatio = (float)4 / 3;
			}
			else if(m_eScale == Scale_16_9)
			{
				fRatio = (float)16 / 9;
			}

			int nH = m_pVideoSurface->h;
			int nW = ((int)(nH * fRatio)) & -3;
			if(nW > m_pVideoSurface->w) 
			{
				nW = m_pVideoSurface->w;
				nH = ((int)(nW / fRatio)) & -3;
			}
			int nX = (m_pVideoSurface->w - nW) / 2;
			int nY = (m_pVideoSurface->h - nH) / 2;
			rcShow.x = nX;
			rcShow.y = nY;
			rcShow.w = nW;
			rcShow.h = nH;
		}
		
		if(m_bRefresh)
		{
			SDL_FillRect(m_pVideoSurface,NULL,SDL_MapRGB(m_pVideoSurface->format, 0, 0, 0));
			SDL_UpdateRect(m_pVideoSurface, 0, 0, 0, 0);
			m_bRefresh = false;
		}
		SDL_DisplayYUVOverlay(m_VideoPic.bmp, &rcShow);
	}

	return nRet;
}

// 我们在这里做了很多检查：首先，我们保证现在的时间戳和上一个时间戳之间的处以dbDelay是有意义的。
// 如果不是的话，我们就猜测着用上次的延迟。接着，我 们有一个同步阈值，因为在同步的时候事情并不总是那么完美的。
// 在ffplay中使用0.01作为它的值。我们也保证阈值不会比时间戳之间的间隔短。最后， 我们把最小的刷新值设置为10毫秒。
// 我们简单的添加新的帧定时器延时，把它和电脑的系统时间进行比较，然后使用那个值来调度下一次刷新
// 使视频同步到音频  
void CVideoPlayer::RefrshVideo()
{
	if(m_nVideoPicCnt > 0)
	{
		m_dCurrentPts = m_VideoPic.pts;
		m_nCurrentPtsTime = av_gettime();

		double dDelay = m_dCurrentPts - m_dVideoFrameLastPts;
		if(dDelay <= 0 || dDelay >= 1.0)
			dDelay = m_dVideoFrameLastDelay;
		
		m_dVideoFrameLastDelay = dDelay;
		m_dVideoFrameLastPts = m_dCurrentPts;
		if(m_nSyncType != AV_SYNC_VIDEO_MASTER) 
		{
			double diff = m_VideoPic.pts - GetMaterClock();
			if(fabs(diff) < AV_NOSYNC_THRESHOLD) 
			{
				double sync_threshold = (dDelay > AV_SYNC_THRESHOLD) ? dDelay : AV_SYNC_THRESHOLD;
				if(diff <= -sync_threshold)
					dDelay = 0;
				else if (diff >= sync_threshold)
					dDelay = 2 * dDelay;
			}
		}

		m_dVideoFrameTime += dDelay;
		double dActualDelay = (m_dVideoFrameTime - (av_gettime()/1000000.0));
		if(dActualDelay < 0.010)
			dActualDelay = 0.010;
		
		SDL_AddTimer(dActualDelay*1000+0.5, SdlTimerCallback, this);
		ShowVideoPic();

		if(m_pListener)
		{
			int nCurTime = global_video_pkt_pts/1000;
			m_pListener->HandlePlayerMsg(PlayerMsg_Pos, nCurTime, m_nDuration);
		}

		SDL_LockMutex(m_pMutex);
		m_nVideoPicCnt--;
		SDL_CondSignal(m_pCond);
		SDL_UnlockMutex(m_pMutex);
	}
	else
	{
		SDL_AddTimer(1, SdlTimerCallback, this);
	}
}

int CVideoPlayer::HandleVideoPic(AVFrame *pFrame, double pts)
{
	int nRet = 0;

	SDL_LockMutex(m_pMutex);
	while(m_nVideoPicCnt >= 1 && m_nState)
		SDL_CondWait(m_pCond, m_pMutex);
	SDL_UnlockMutex(m_pMutex);

	if(m_nState)
	{
		if(m_bResize)
		{
			m_bResize = false;
			m_pVideoSurface = SDL_SetVideoMode(m_szSize.cx, m_szSize.cy, 0, SDL_HWSURFACE|SDL_RESIZABLE|SDL_ASYNCBLIT|SDL_HWACCEL);
			SDL_FillRect(m_pVideoSurface,NULL,SDL_MapRGB(m_pVideoSurface->format, 0, 0, 0));
			SDL_UpdateRect(m_pVideoSurface, 0, 0, 0, 0);
		}

		SDL_LockYUVOverlay(m_VideoPic.bmp);
		if(m_pSwsCtx)
		{
			AVPicture pict;
			pict.data[0] = m_VideoPic.bmp->pixels[0];
			pict.data[1] = m_VideoPic.bmp->pixels[2];
			pict.data[2] = m_VideoPic.bmp->pixels[1];
			pict.linesize[0] = m_VideoPic.bmp->pitches[0];
			pict.linesize[1] = m_VideoPic.bmp->pitches[2];
			pict.linesize[2] = m_VideoPic.bmp->pitches[1];
			sws_scale(m_pSwsCtx, (const uint8_t **)pFrame->data, pFrame->linesize,
						0, m_pFmtCtx->streams[m_nVideoIndex]->codec->height, pict.data, pict.linesize);
		}
		SDL_UnlockYUVOverlay(m_VideoPic.bmp);

		SDL_LockMutex(m_pMutex);
		m_nVideoPicCnt++;
		SDL_UnlockMutex(m_pMutex);
	}

	return nRet;
}

double CVideoPlayer::SyncVideo(AVFrame *pFrame, double pts)
{
	if(pts != 0) 
		m_dVideoClock = pts;
	else 
		pts = m_dVideoClock;

	double dFrameDelay = av_q2d(m_pFmtCtx->streams[m_nVideoIndex]->codec->time_base);
	dFrameDelay += pFrame->repeat_pict * (dFrameDelay * 0.5);
	m_dVideoClock += dFrameDelay;

	return pts;
}

void CVideoPlayer::AudioPro(void *pUserdata, Uint8 *pStream, int nLen)
{
	CVideoPlayer *pThis = (CVideoPlayer *)pUserdata;
	if(pThis)
	{
		double dPts;
		while(nLen > 0)
		{
			if(pThis->m_nAudioBufIndex >= pThis->m_nAudioBufSize) 
			{
				int nSize = pThis->HandleAudio(dPts);
				if(nSize < 0) 
				{
					pThis->m_nAudioBufSize = 1024;
					memset(pThis->m_pAudioBuf, 0, pThis->m_nAudioBufSize);
				} 
				else 
				{
					pThis->m_nAudioBufSize = nSize;
				}
				pThis->m_nAudioBufIndex = 0;
			}

			int nLenHandle = pThis->m_nAudioBufSize - pThis->m_nAudioBufIndex;
			if(nLenHandle > nLen)
				nLenHandle = nLen;

			SDL_MixAudio(pStream, (uint8_t *)pThis->m_pAudioBuf + pThis->m_nAudioBufIndex, nLenHandle, pThis->m_nVolume);
			nLen -= nLenHandle;
			pStream += nLenHandle;
			pThis->m_nAudioBufIndex += nLenHandle;
		}
	}
}

int CVideoPlayer::VideoPro(void *pArg)
{
	CVideoPlayer *pThis = (CVideoPlayer *)pArg;
	if(pThis)
	{
		AVFrame *pFrame = av_frame_alloc();
		if(pFrame)
		{
			AVStream *pStream = pThis->m_pFmtCtx->streams[pThis->m_nVideoIndex];
			AVPacket Pkt;
			while(pThis->m_nState)
			{
				if(pThis->PktQueGet(&pThis->m_qVideoPkt, &Pkt, 1) == 1)
				{
					if(Pkt.data == PktFlush.data)
					{
						avcodec_flush_buffers(pStream->codec);
					}
					else
					{
						global_video_pkt_pts = Pkt.pts;
						double pts = 0;
						int nLen = 0, nFinished = 0;
						nLen = avcodec_decode_video2(pStream->codec, pFrame, &nFinished, &Pkt);
						if(nLen >= 0)
						{
							if(Pkt.dts == AV_NOPTS_VALUE && pFrame->opaque && *(uint64_t*)pFrame->opaque != AV_NOPTS_VALUE)
								pts = *(uint64_t *)pFrame->opaque;
							else if(Pkt.dts != AV_NOPTS_VALUE) 
								pts = Pkt.dts;
							pts *= av_q2d(pStream->time_base);

							if(nFinished)
							{
								pts = pThis->SyncVideo(pFrame, pts);
								if(pThis->HandleVideoPic(pFrame, pts) < 0)
									break;
							}
						}
						av_free_packet(&Pkt);
					}
				}
			}

			av_frame_free(&pFrame);
		}
	}
	return 0;
}

int CVideoPlayer::DecodeThreadPro(void *pArg)
{
	CVideoPlayer *pThis = (CVideoPlayer *)pArg;
	if(pThis)
	{
		while(pThis->m_nState)
		{
			if(pThis->m_nState == 1) //playing
			{
				if(/*pThis->m_qAudioPkt.size>MAX_AUDIOQ_SIZE ||*/ pThis->m_qVideoPkt.size>MAX_VIDEOQ_SIZE) 
				{
					SDL_Delay(160);
					continue;
				}
				else if(pThis->m_qAudioPkt.size>MAX_AUDIOQ_SIZE /*|| pThis->m_qVideoPkt.size>MAX_VIDEOQ_SIZE*/) 
				{
					SDL_Delay(20);
					continue;
				}
				else
				{
					AVPacket Pkt;
					if(av_read_frame(pThis->m_pFmtCtx, &Pkt) >= 0) 
					{
						if(Pkt.stream_index == pThis->m_nVideoIndex)
							pThis->PktQuePut(&pThis->m_qVideoPkt, &Pkt);
						else if (Pkt.stream_index == pThis->m_nAudioIndex) 
							pThis->PktQuePut(&pThis->m_qAudioPkt, &Pkt);
						else 
							av_free_packet(&Pkt);
					}
					else
					{
						if(pThis->m_pFmtCtx->pb->error == 0)
						{
							SDL_Delay(1000); 
							if(pThis->m_pListener)
								pThis->m_pListener->HandlePlayerMsg(PlayerMsg_End);
						}
						else 
						{
							if(pThis->m_pListener)
								pThis->m_pListener->HandlePlayerMsg(PlayerMsg_Error);
						}
						break;
					}
				}
			}
			else if(pThis->m_nState == 2) //pause
			{
				SDL_Delay(200);
			}
		}
	}
	return 0;
}

int CVideoPlayer::EventPro(void *pArg)
{
	CVideoPlayer *pThis = (CVideoPlayer *)pArg;
	if(pThis)
	{
		SDL_Event event;
		while(pThis->m_nState)
		{
			SDL_WaitEvent(&event);
			switch(event.type) 
			{
			case REFRESH_EVENT:
				{
					pThis->RefrshVideo();
				}
				break;
			case SDL_VIDEORESIZE:
				{
					if(pThis->m_nState == 1)
					{
						pThis->m_szSize.cx = event.resize.w;
						pThis->m_szSize.cy = event.resize.h;
						pThis->m_bResize = true;
					}
					else if(pThis->m_nState == 2)
					{
						pThis->m_pVideoSurface = SDL_SetVideoMode(event.resize.w, event.resize.h, 0, SDL_HWSURFACE|SDL_RESIZABLE|SDL_ASYNCBLIT|SDL_HWACCEL);
						pThis->Refresh();
						pThis->ShowVideoPic();
					}
				}
				break;
			case SDL_KEYDOWN:
				{
					switch(event.key.keysym.sym) 
					{
					case SDLK_ESCAPE:
						break;
					case SDLK_SPACE:
						break;
					}
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				{
					switch(event.button.button)
					{
					case SDL_BUTTON_LEFT:
						break;
					case SDL_BUTTON_RIGHT:
						break;
					}
				}
				break;
			default:
				break;
			}
		}
	}
	return 0;
}
