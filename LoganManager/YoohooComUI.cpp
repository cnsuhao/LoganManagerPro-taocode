#include "StdAfx.h"
#include "YoohooComUI.h"

YoohooComUI::YoohooComUI(void)
{
	videoList.resize(9);// 
	for (int i=0;i<videoList.size();i++)
	{
		videoList[i] = new YoohooVideo();
		//videoList[i]->SetFloat(true);
		if(i!=0)
		{
			waitVideoStackList.push(videoList[i]);
		}
	}
	currentVideoStackList.push(videoList[0]);// default self-video
}

YoohooComUI::~YoohooComUI(void)
{
	/*for(int i=0;i<videoList.size();i++)
	{
		delete videoList[i];
	}*/
	videoList.clear();
	
	while(!currentVideoStackList.empty())
	{
		currentVideoStackList.pop();
	}
}
YoohooVideo* YoohooComUI::getVideo(int index)
{
	if(index<0||index>currentVideoStackList.size())
	{
		throw "index out of the range";
		return NULL;
	}
	else
	{
		stack<YoohooVideo*>tempVideo=currentVideoStackList;
		YoohooVideo* res=NULL;
		int i=0;
		while(i<=index)
		{
			res=tempVideo.top();
			tempVideo.pop();
			++i;
		}
		while(!tempVideo.empty())
		{
			tempVideo.pop();
		}
		return res;
	}
}
YoohooVideo* YoohooComUI::operator [](int index)
{
	if(index<0||index>currentVideoStackList.size())
	{
		throw "index out of the range";
		return NULL;
	}
	else
	{
		stack<YoohooVideo*>tempVideo=currentVideoStackList;
		YoohooVideo* res=NULL;
		int i=0;
		while(i<=index)
		{
			res=tempVideo.top();
			tempVideo.pop();
		}
		while(!tempVideo.empty())
		{
			tempVideo.pop();
		}
		return res;
	}
}

int YoohooComUI::getVideoCount()const
{
	return currentVideoStackList.size();
}

bool YoohooComUI::addVideo()
{
	if(waitVideoStackList.empty())
	{
		return false;
	}
	else
	{
		currentVideoStackList.push(waitVideoStackList.top());
		waitVideoStackList.pop();
		return true;
	}
}

bool YoohooComUI::removeVideo()
{
	if(currentVideoStackList.size()==1)
	{
		return false;
	}
	else
	{
		waitVideoStackList.push(currentVideoStackList.top());
		currentVideoStackList.pop();
		return true;
	}
}

void YoohooComUI::SetPos(RECT rc)
{
	__super::SetPos(rc);

	RECT c= this->GetPos();
	switch(currentVideoStackList.size())
	{
	case 1:video_1(rc);break;
	case 2:video_2(rc);break;
	case 3:video_3(rc);break;
	case 4:video_4(rc);break;
	case 5:video_5(rc);break;
	case 6:video_6(rc);break;
	case 7:video_7(rc);break;
	case 8:video_8(rc);break;
	case 9:video_9(rc);break;
	case 10:video_10(rc);break;
	}
}

void YoohooComUI::VideoVisibleChange()
{
	stack<YoohooVideo*> tempNOVisible=waitVideoStackList;
	stack<YoohooVideo*> tempVisible=currentVideoStackList;
	while(!tempVisible.empty())
	{
		tempVisible.top()->SetVisible(true);
		tempVisible.pop();
	}

	while(!tempNOVisible.empty())
	{
		tempNOVisible.top()->SetVisible(false);
		tempNOVisible.pop();
	}
}

YoohooVideo* YoohooComUI::getLocalVideo()
{
	return getVideo(currentVideoStackList.size()-1);
}
void YoohooComUI::Init()
{
	for(int i=0;i<videoList.size();i++)
	{
		this->Add(videoList[i]);
	}
}
void YoohooComUI::video_1(RECT rc)
{
	currentVideoStackList.top()->SetPos(rc);
	VideoVisibleChange();
}
void YoohooComUI::video_2(RECT rc)
{
	stack<YoohooVideo*> temp=currentVideoStackList;
	RECT LeftRect=rc;
	LeftRect.right-=200;
	LeftRect.bottom-=150;
	temp.top()->SetPos(LeftRect);
	temp.pop();
	RECT RightRect=rc;
	RightRect.left=rc.right-200;
	RightRect.top=rc.bottom-150;
	::InvalidateRect(temp.top()->getHwnd(),&RightRect,false);
	temp.top()->SetPos(RightRect);
	temp.pop();
	VideoVisibleChange();
}
void YoohooComUI::video_3(RECT rc)
{
	stack<YoohooVideo*> temp=currentVideoStackList;
	int width=(rc.right-rc.left)/2;
	int height=(rc.bottom-rc.top)/2;
	int dif=((rc.right-rc.left)-width)/2;
	stack<RECT>rect;
	RECT R1=rc;
	R1.right=R1.left+width-1;
	R1.bottom=R1.top+height-1;

	RECT R2=rc;
	R2.left=R1.right+1;
	R2.bottom=R1.bottom;

	RECT R3=rc;
	R3.left=rc.left+dif;
	R3.top=R1.bottom+2;
	R3.right=rc.right-dif;

	rect.push(R3);
	rect.push(R2);
	rect.push(R1);
	while(!temp.empty())
	{
		temp.top()->SetPos(rect.top());
		temp.pop();
		rect.pop();
	}
	VideoVisibleChange();

}
void YoohooComUI::video_4(RECT rc)
{
	stack<YoohooVideo*> temp=currentVideoStackList;
	int width=(rc.right-rc.left)/3;
	int height=(rc.bottom-rc.top)/3;
	stack<RECT>rect;
	RECT R1=rc;
	R1.bottom=rc.bottom-height-1;

	RECT R2=rc;
	R2.top=R1.bottom+2;
	R2.right=rc.left+width-1;

	RECT R3=rc;
	R3.left=R2.right+2;
	R3.top=R1.bottom+2;
	R3.right=R3.left+width-1;

	RECT R4=rc;
	R4.top=R1.bottom+2;
	R4.left=R3.right+2;
	rect.push(R4);
	rect.push(R3);
	rect.push(R2);
	rect.push(R1);
	while(!temp.empty())
	{
		temp.top()->SetPos(rect.top());
		temp.pop();
		rect.pop();
	}
	VideoVisibleChange();

}
void YoohooComUI::video_5(RECT rc)
{
	stack<YoohooVideo*> temp=currentVideoStackList;
	int width=(rc.right-rc.left)/4;
	int height=(rc.bottom-rc.top)/3;
	stack<RECT>rect;
	RECT R1=rc;
	R1.bottom=rc.bottom-height-1;

	RECT R2=rc;
	R2.top=R1.bottom+2;
	R2.right=rc.left+width-1;

	RECT R3=rc;
	R3.left=R2.right+2;
	R3.top=R1.bottom+2;
	R3.right=R3.left+width-1;

	RECT R4=rc;
	R4.top=R1.bottom+2;
	R4.left=R3.right+2;
	R4.right=R4.left+width;

	RECT R5=rc;
	R5.top=R1.bottom+2;
	R5.left=R4.right+2;

	rect.push(R5);
	rect.push(R4);
	rect.push(R3);
	rect.push(R2);
	rect.push(R1);
	while(!temp.empty())
	{
		temp.top()->SetPos(rect.top());
		temp.pop();
		rect.pop();
	}
	VideoVisibleChange();

}
void YoohooComUI::video_6(RECT rc)
{
	stack<YoohooVideo*> temp=currentVideoStackList;
	int width=(rc.right-rc.left)/3;
	int height=(rc.bottom-rc.top)/3;
	stack<RECT>rect;
	RECT R1=rc;
	R1.bottom=rc.bottom-height-1;
	R1.right=rc.right-width-1;

	RECT R2=rc;
	R2.left = R1.right+2;
	R2.bottom=rc.top+height-1;

	RECT R3=rc;
	R3.left = R1.right+2;
	R3.top=R2.bottom+2;
	R3.bottom=R3.top+height-1;

	RECT R4=rc;
	R4.top=R1.bottom+2;
	R4.right=rc.left+width-1;

	RECT R5=rc;
	R5.top=R1.bottom+2;
	R5.left=R4.right+2;
	R5.right=R5.left+width-1;

	RECT R6=rc;
	R6.top=R1.bottom+2;
	R6.left=R5.right+2;

	rect.push(R6);
	rect.push(R5);
	rect.push(R4);
	rect.push(R3);
	rect.push(R2);
	rect.push(R1);
	while(!temp.empty())
	{
		temp.top()->SetPos(rect.top());
		temp.pop();
		rect.pop();
	}
	VideoVisibleChange();

}
void YoohooComUI::video_7(RECT rc)
{
	stack<YoohooVideo*> temp=currentVideoStackList;
	int width=(rc.right-rc.left)/2;
	int height=(rc.bottom-rc.top)/2;
	int cellWidth=width/2;
	int cellHeight=height/2;
	stack<RECT>rect;

	RECT R1=rc;
	R1.bottom=rc.bottom-height-1;
	R1.right=rc.right-width-1;

	RECT R2=rc;
	R2.left = R1.right+2;
	R2.bottom=rc.top+height-1;

	RECT R3=rc;
	R3.top=R1.bottom+2;
	R3.right=R3.left+cellWidth-1;
	R3.bottom=R3.top+cellHeight-1;

	RECT R4=rc;
	R4.top=R1.bottom+2;
	R4.left=R3.right+2;
	R4.bottom=R3.bottom;
	R4.right=R4.left+cellWidth-1;

	RECT R5=rc;
	R5.top=R3.bottom+2;
	R5.right=R5.left+cellWidth-1;

	RECT R6=rc;
	R6.top=R4.bottom+2;
	R6.left=R5.right+2;
	R6.right=R6.left+cellWidth-1;

	RECT R7=rc;
	R7.left=R6.right+2;
	R7.top=R2.bottom+2;

	rect.push(R7);
	rect.push(R6);
	rect.push(R5);
	rect.push(R4);
	rect.push(R3);
	rect.push(R2);
	rect.push(R1);
	while(!temp.empty())
	{
		temp.top()->SetPos(rect.top());
		temp.pop();
		rect.pop();
	}
	VideoVisibleChange();
}
void YoohooComUI::video_8(RECT rc)
{
	stack<YoohooVideo*> temp=currentVideoStackList;
	int width=(rc.right-rc.left)/4;
	int height=(rc.bottom-rc.top)/4;
	stack<RECT>rect;
	RECT R1=rc;
	R1.bottom=rc.bottom-height-1;
	R1.right=rc.right-width-1;

	RECT R2=rc;
	R2.left = R1.right+2;
	R2.bottom=rc.top+height-1;

	RECT R3=rc;
	R3.left = R1.right+2;
	R3.top=R2.bottom+2;
	R3.bottom=R3.top+height-1;

	RECT R4=rc;
	R4.left=R1.right+2;
	R4.top=R3.bottom+2;
	R4.bottom=R4.top+height-1;

	RECT R5=rc;
	R5.top=R1.bottom+2;
	R5.right=R5.left+width-1;

	RECT R6=rc;
	R6.top=R1.bottom+2;
	R6.left=R5.right+2;
	R6.right=R6.left+width-1;

	RECT R7=rc;
	R7.top=R1.bottom+2;
	R7.left=R6.right+2;
	R7.right=R7.left+width-1;

	RECT R8=rc;
	R8.left=R7.right+2;
	R8.top=R4.bottom+2;

	rect.push(R8);
	rect.push(R7);
	rect.push(R6);
	rect.push(R5);
	rect.push(R4);
	rect.push(R3);
	rect.push(R2);
	rect.push(R1);
	while(!temp.empty())
	{
		temp.top()->SetPos(rect.top());
		temp.pop();
		rect.pop();
	}
	VideoVisibleChange();
}
void YoohooComUI::video_9(RECT rc)
{
	stack<YoohooVideo*> temp=currentVideoStackList;
	int width=(rc.right-rc.left)/4;
	int height=(rc.bottom-rc.top)/4;
	stack<RECT>rect;

	RECT R1=rc;
	R1.bottom=rc.bottom-2*height-1;

	RECT R2=rc;
	R2.top=R1.bottom+2;
	R2.right=R2.left+width-1;
	R2.bottom=R2.top+height-1;

	RECT R3=rc;
	R3.left = R2.right+2;
	R3.top=R1.bottom+2;
	R3.bottom=R3.top+height-1;
	R3.right=R3.left+width-1;

	RECT R4=rc;
	R4.top=R1.bottom+2;
	R4.left=R3.right+2;
	R4.right=R4.left+width-1;
	R4.bottom =R4.top+height-1;

	RECT R5=rc;
	R5.top=R1.bottom+2;
	R5.left=R4.right+2;
	R5.bottom=R5.top+height-1;

	RECT R6=rc;
	R6.top=R2.bottom+2;
	R6.right=R6.left+width-1;

	RECT R7=rc;
	R7.top=R3.bottom+2;
	R7.left=R6.right+2;
	R7.right=R7.left+width-1;

	RECT R8=rc;
	R8.top=R4.bottom+2;
	R8.left=R7.right+2;
	R8.right=R8.left+width-1;

	RECT R9=rc;
	R9.left=R8.right+2;
	R9.top=R5.bottom+2;

	rect.push(R9);
	rect.push(R8);
	rect.push(R7);
	rect.push(R6);
	rect.push(R5);
	rect.push(R4);
	rect.push(R3);
	rect.push(R2);
	rect.push(R1);
	while(!temp.empty())
	{
		temp.top()->SetPos(rect.top());
		temp.pop();
		rect.pop();
	}
	VideoVisibleChange();
}
void YoohooComUI::video_10(RECT rc)
{
}