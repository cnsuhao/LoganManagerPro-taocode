#include "StdAfx.h"
#include "Command.h"


/*******************************************/
BaseCmdFmt::BaseCmdFmt()
{
	data = NULL;
}

BaseCmdFmt::~BaseCmdFmt()
{
	data = NULL;
}

/**********************************************/

BaseCmd::BaseCmd()
{
	begin = 0xfe;
	address = 0x1;
	ctype = 0xf0;
	cmd = 0x0;
	length = 0x0;
	data = NULL;
	check = 0xef;
	end = 0xff;
}

BaseCmd::~BaseCmd()
{
	delete[] data;
	data = NULL; 
}

void BaseCmd::setAddress(byte uaddress)
{
	address = uaddress;
	update();
}

void BaseCmd::setType(byte utype)
{
	ctype = utype;
	update();
}

void BaseCmd::setCmd(byte ucmd)
{
	cmd = ucmd;
	update();
}
void BaseCmd::setData(byte* udata, int len)
{
	if (len<0)
		return;
	length = len;
	if (length > 0xff)
	{
		// if udata > 255 make length =255
		length = 0xff;
	}
	if (data)
	{
		delete[] data;
		data = NULL;
	}
	data = new byte[length];
	memcpy(data, udata, length);
	if (length == 0)
		data = NULL;
	update();
}

void BaseCmd::calcCheck()
{
	//   max= 255*(255+7)=66810
	int res = 0;
	res =  address + ctype + cmd + length;
	for (int i = 0; i < length; i++)
	{
		res += *(data + i);
	}
	check = (byte)(res&0xff);
}
int BaseCmd::getCmdLength()
{
	return length + 7;
}
void BaseCmd::update()
{
}
/*****************************************************/


Command::Command()
{
	memset(cmdset, 0x0, max_length);
	makecmd();

}
Command::~Command()
{

}
byte* Command::getCommand()
{
	return cmdset;
}
void Command::parseCommand(byte address,byte type, byte cmd,byte* data,int length)
{
	setAddress(address);
	setType(type);
	setCmd(cmd);
	setData(data, length);
}
void Command::update()
{
	calcCheck();
	makecmd();
}

void Command::makecmd()
{
	cmdset[0] = begin;
	cmdset[1] = address;
	cmdset[2] = ctype;
	cmdset[3] = cmd;
	cmdset[4] = length;
	for (int i = 5; i < 5 + length; i++)
	{
		cmdset[i] = data[i - 5];
	}
	cmdset[length + 5] = check;
	cmdset[length + 6] = end;
}

/****************************************/

Command Cmd::CenterCtrlFlag;
Command Cmd::SysClose;
Command Cmd::SysOpen;
Command Cmd::ProjectorOpen;
Command Cmd::ProjectorClose;
Command Cmd::ScreenUp;
Command Cmd::ScreenDown;
Command Cmd::SoundUp;
Command Cmd::SoundDown;
Command Cmd::HighSoundUp;
Command Cmd::HighSoundDown;
Command Cmd::LowSoundUp;
Command Cmd::LowSoundDown;
Command Cmd::MikeSoundUp;
Command Cmd::MikeSoundDown;
Command Cmd::Mute;
bool    Cmd::bInit = false;

void Cmd::Init()
{
	if (!bInit)
	{
		bInit = true;
		initCenterCtrlFlag();
		initSysClose();
		initSysOpen();
		initProjectorOpen();
		initProjectorClose();
		initScreenUp();
		initScreenDown();
		/*sound*/
		initSoundUp();
		initSoundDown();
		initHighSoundUp();
		initHighSoundDown();
		initLowSoundUp();
		initLowSoundDown();
		initMikeSoundUp();
		initMikeSoundDown();
		initMute();
	}
	else
	{
		return;
	}


}

void Cmd::initCenterCtrlFlag()
{

	CenterCtrlFlag.parseCommand(0x00,0x03,0x90,NULL,0);
}

void Cmd::initSysClose()
{
	byte cls = 0x00;
	SysClose.parseCommand(0x00, 0x02, 0x82, &cls, 1);
}
void Cmd::initSysOpen()
{
	byte opn = 0x01;
	SysOpen.parseCommand(0x00, 0x02, 0x82, &opn, 1);
}
void Cmd::initProjectorOpen()
{
	byte pro[] = {0x00,0x01};
	ProjectorOpen.parseCommand(0x00, 0x02, 0x68, pro, 2);
}
void Cmd::initProjectorClose()
{
	byte pro[] = { 0x00, 0x00 };
	ProjectorClose.parseCommand(0x00, 0x02, 0x68, pro, 2);
}
void Cmd::initScreenUp()
{
	byte scr = 0x23;
	ScreenUp.parseCommand(0x00, 0x02, 0x0b, &scr, 1);
}
void Cmd::initScreenDown()
{
	byte scr = 0x32;
	ScreenDown.parseCommand(0x00, 0x02, 0x0b, &scr, 1);
}

void Cmd::initSoundUp()
{
	byte sound[] = { 0x00, 0x01 };
	SoundUp.parseCommand(0x00, 0x02, 0x44, sound, 2);
}
void Cmd::initSoundDown()
{
	byte sound[] = { 0x00, 0xff };
	SoundDown.parseCommand(0x00, 0x02, 0x44, sound, 2);
}
void Cmd::initHighSoundUp()
{
	byte sound[] = { 0x03, 0x01 };
	HighSoundUp.parseCommand(0x00, 0x02, 0x44, sound, 2);
}
void Cmd::initHighSoundDown()
{
	byte sound[] = { 0x03, 0xff };
	HighSoundDown.parseCommand(0x00, 0x02, 0x44, sound, 2);
}
void Cmd::initLowSoundUp()
{
	byte sound[] = {0x02,0x01};
	LowSoundUp.parseCommand(0x00, 0x02, 0x44, sound, 2);

}
void Cmd::initLowSoundDown()
{
	byte sound[] = { 0x02, 0xff };
	LowSoundDown.parseCommand(0x00, 0x02, 0x44, sound, 2);
}
void Cmd::initMikeSoundUp()
{
	byte sound[] = { 0x01, 0x01 };
	MikeSoundUp.parseCommand(0x00, 0x02, 0x44, sound, 2);
}
void Cmd::initMikeSoundDown()
{
	byte sound[] = { 0x01, 0xff };
	MikeSoundDown.parseCommand(0x00, 0x02, 0x44, sound, 2);
}
void Cmd::initMute()
{
	byte sound[] = { 0x00, 0x00 };
	Mute.parseCommand(0x00, 0x02, 0x44, sound, 2);
}

Command Cmd::GetCurrentSound(byte address,byte data)
{
	Command cmd;
	cmd.parseCommand(address, 0x02, 0x40, &data, 1);
	return cmd;
}

Command Cmd::SetCurrentSound(byte address,byte* data)
{
	Command cmd;
	cmd.parseCommand(address,0x02,0x42,data,2);
	return cmd;
}
