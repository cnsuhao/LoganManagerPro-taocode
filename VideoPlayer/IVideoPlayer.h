//#ifndef __IVIDEOPLAYER_H__
//#define __IVIDEOPLAYER_H__
//
//#include <Windows.h>
//#include <string>
//using namespace std;
//
//typedef signed __int64   int64_t;
//
//#ifdef  IVIDEOPLAYER_API
//#else
//#define IVIDEOPLAYER_API __declspec(dllimport)
//#endif
//
//#define interface class __declspec(novtable)
//
//
////enum enScale
////{
////	Scale_Src = 0,
////	Scale_Full,
////	Scale_1_1,
////	Scale_4_3,
////	Scale_16_9,
////};
////
////enum enPlayerMsg
////{
////	PlayerMsg_Pos = 0,
////	PlayerMsg_End,
////	PlayerMsg_Error,
////};
//
//interface IVideoPlayerListener
//{
//public:
//	virtual void HandlePlayerMsg(int nMsg, WPARAM wParam=0, LPARAM lParam=0) = 0;
//};
//
//
//class IVIDEOPLAYER_API IVideoPlayer
//{
//public:
//	static IVideoPlayer *GetInstance();
//	static void ReleaseInstance(IVideoPlayer *pInst);
//
//	virtual void SetListener(IVideoPlayerListener *pListener) = 0;
//	virtual void SetHWND(HWND hwnd) = 0;
//	virtual bool Load(string strPath) = 0;
//
//	virtual void Play() = 0;
//	virtual void Pause() = 0;
//	virtual void Stop() = 0;
//
//	virtual void SetVolume(int nVolume) = 0;
//	virtual void SetMute(bool bMute) = 0;
//
//	virtual void SetScale(enScale eScale) = 0;
//
//	virtual void Refresh() = 0;
//
//	virtual bool Seek(int64_t nPos) = 0;
//
//protected:
//	virtual ~IVideoPlayer(){};
//};
//
//#endif