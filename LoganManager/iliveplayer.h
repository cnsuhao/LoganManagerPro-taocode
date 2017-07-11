#ifndef __ILIVEPLAYER_H__
#define __ILIVEPLAYER_H__

#include <Windows.h>
#include <string>
using namespace std;


#ifdef  ILIVEPLAYER_API
#else
#define ILIVEPLAYER_API __declspec(dllimport)
#endif

#define interface class __declspec(novtable)

enum enScale
{
	Scale_Full = 0,
	Scale_Src,
	Scale_1_1,
	Scale_4_3,
	Scale_16_9,
};
enum enPlayerMsg
{
	PlayerMsg_Volume = 0,
	PlayerMsg_End,
	PlayerMsg_Error,
};

interface ILivePlayerListener
{
public:
	virtual void HandlePlayerMsg(int nMsg, WPARAM wParam=0, LPARAM lParam=0) = 0;
};


class ILIVEPLAYER_API ILivePlayer
{
public:
	static ILivePlayer *GetInstance();
	static void ReleaseInstance(ILivePlayer *pInst);

	virtual void SetListener(ILivePlayerListener *pListener) = 0;
	virtual void SetHWND(HWND hwnd, int nSplit=0) = 0;

	virtual bool Load(string strPath) = 0;
	virtual void Stop() = 0;

	virtual void SetScale(enScale eScale) = 0;
	virtual void SetVolume(int nVolume) = 0;
	virtual void SetMute(bool bMute) = 0;

protected:
	virtual ~ILivePlayer(){};
};

#endif