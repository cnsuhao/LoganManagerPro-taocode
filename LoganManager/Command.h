#pragma once

#include <cstdlib>
#include <iostream>
#include <iomanip>

using namespace std;

const int max_length = 7 + 0xff;

typedef unsigned char byte;

static void print(byte* p, int len)
{
	for (int i = 0; i<len; i++)
	{
		cout <<setbase(16)<< (int)p[i] << ends;
	}
	cout << endl;
}

/*______________________________________________________________________
|  start   |	address|		ctype		| command  | length	 | data	  | checksum  | end	 |
_______________________________________________________________________
|	 1B	|    1B	  |		   1B		|	   1B	     |   	1B	      |length B|		   1B	     |	 1B |
_______________________________________________________________________*/

class BaseCmdFmt //base command fromat
{
public:
	byte		begin;
	byte		address;
	byte		ctype;
	byte		cmd;
	byte		length;
	byte*		data;
	byte		check;
	byte		end;
protected:// not instance
	BaseCmdFmt();
	~BaseCmdFmt();
};

class BaseCmd :protected BaseCmdFmt
{
public:
	BaseCmd();
	virtual ~BaseCmd();
	virtual void setAddress(byte uaddress);
	virtual void setType(byte utype);
	virtual void setCmd(byte ucmd);
	virtual void setData(byte* udata, int len);
	virtual void calcCheck();
	int getCmdLength();
	virtual void update();
};



class Command : public BaseCmd
{
public:
	Command();
	~Command();
	byte* getCommand();
	void update();

	virtual void testOut() sealed // just use test
	{
		for (int i = 0; i < length + 7; i++)
		{
			cout << setbase(16) << (int)cmdset[i] << " ";
		}
		cout << endl;
	}
	void parseCommand(byte address, byte type, byte cmd, byte* data, int length);

private:
	byte cmdset[max_length];
	void makecmd();
};

/*be a sealed class cannot to be inherit*/
class Cmd sealed 
{
private:
	Cmd(){}
public:
	static Command CreateCmd();
	static void Init();
	//øÿ÷∆÷∏¡Ó
	static Command CenterCtrlFlag;
	static Command SysClose;
	static Command SysOpen;
	static Command ProjectorOpen;
	static Command ProjectorClose;
	static Command ScreenUp;
	static Command ScreenDown;
	//…˘“Ù÷∏¡Ó
	static Command SoundUp;
	static Command SoundDown;
	static Command HighSoundUp;
	static Command HighSoundDown;
	static Command LowSoundUp;
	static Command LowSoundDown;
	static Command MikeSoundUp;
	static Command MikeSoundDown;
	static Command Mute;
	static Command GetCurrentSound(byte address,byte data/* 0<-->÷˜“Ù   1<-->ª∞Õ≤  2<-->µÕ“Ù   3<-->∏ﬂ“Ù*/);
	static Command SetCurrentSound(byte address,byte* data/* 0|1|2|3 + soundValue*/);

private:
	static bool bInit;
	/*Ctrl*/
	static void initCenterCtrlFlag();
	static void initSysClose();
	static void initSysOpen();
	static void initProjectorOpen();
	static void initProjectorClose();
	static void initScreenUp();
	static void initScreenDown();
	/*Sound*/
	static void initSoundUp();
	static void initSoundDown();
	static void initHighSoundUp();
	static void initHighSoundDown();
	static void initLowSoundUp();
	static void initLowSoundDown();
	static void initMikeSoundUp();
	static void initMikeSoundDown();
	static void initMute();
};

