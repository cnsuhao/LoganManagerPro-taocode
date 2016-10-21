#pragma once
#include "stdafx.h"
#include "LoganPlayer.h"
#include "HttpRequest.h"
#include "MainWndCfg.h"
class Router;



typedef struct
{
	string									ip;
	string									port;
	string									token;
	ServerModel						model;
	map<string,string>				URL;
}_videoURL;
namespace DuiLib
{
	enum CamWndTimer
	{
		LBDOWNTIMER = 100,
		REFRESH_REMOTE_SCREEN,
		AddCamCtrlTimer
	};

	class CCamCtrolUI;
	class CtrlWnd : public CWindowWnd
	{
	public:
		LPCTSTR GetWindowClassName()const
		{
			return "CtrlWmd";
		}
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		void PainBkColor();
		void FullScreen();
		void Init(CCamCtrolUI* p);
		RECT CalPos();
	private:
		HWND hparent;
		WINDOWPLACEMENT media_oldplace;
		HWND			media_parent;
	protected:
		CCamCtrolUI *power;

	};
	class CCamWnd:public CWindowWnd
	{
	public:
		CCamWnd();
		CDuiString GetClass() const;
		void Init(CCamCtrolUI* pOwner);
		void SetWndStytle();
		virtual RECT CalPos();
		void PainBkImage();
		virtual void PainBkColor();

		virtual LPCTSTR GetWindowClassName() const;
		void    OnFinalMessage(HWND hWnd);
		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		void			SetDevID(CDuiString sID){m_sDevID = sID;};
		CDuiString		GetDevID(){return m_sDevID;};

	protected:
		CCamCtrolUI*	m_pOwner;
		int				m_iDownCount;
		CDuiString		m_sDevID;
	};
	////////////////////////////////////////////////////
	//
	class CVNCWnd:public CWindowWnd
	{
	public:
		CVNCWnd();
		CDuiString GetClass() const;
		void Init(CCamCtrolUI* pOwner);
		void SetWndStytle();
		virtual RECT CalPos();
		void PainBkImage();
		void PainBkColor();

		virtual LPCTSTR GetWindowClassName() const;
		void    OnFinalMessage(HWND hWnd);
		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		void			SetDevID(CDuiString sID){m_sDevID = sID;};
		CDuiString		GetDevID(){return m_sDevID;};

	public:
		bool            m_bConnect;
		CDuiString		m_strIP;
		int				m_nPort;

	protected:
		CCamCtrolUI*	m_pOwner;
		int				m_iDownCount;
		CDuiString		m_sDevID;

	private:
		//////////////////////////////////////////////////////////////////////////
		double		dScaleX;
		double		dScaleY;
		bool		m_bEnableControl;
		//////////////////////////////////////////////////////////////////////////

	};

	////////////////////////////////////////////////////
	//
	class CCamWndParent:public CCamWnd
	{
	public:
		CCamWndParent();
		void Init(CCamCtrolUI* pOwner);
		RECT CalPos();

		LPCTSTR GetWindowClassName() const;
		void    OnFinalMessage(HWND hWnd);
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		void  FullScreen();
		bool  GetFullFlag();

	protected:
		HWND	m_hParent;
		bool	m_bFull;
		RECT	m_rcNomal;
	};

	////////////////////////////////////////////////////
	//
	enum SetPosType
	{
		NOSET = -1,
		PIP,
		TILE,
		SINGLE
	};

	class CCamCtrolUI: public CContainerUI
	{
		friend class CCamWnd;
		friend class CVNCWnd;
		friend class CCamWndParent;
		friend class CtrlWnd;
	public:
		CCamCtrolUI();
		~CCamCtrolUI();
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT   GetControlFlags() const;
		void Init();
		void InitSubWnd();
		void DeleteSbuWnd();
		void SetPos(RECT rc);
		void SetPosPIP();
		void SetPosTile();
		void SetPosSingle(bool bUp=false);
		void DoEvent(TEventUI& event);
		void PaintBorder(HDC hDC);
		void PaintStatusImage(HDC hDC);
		void SetVisible(bool bVisible = true);

		HWND GetHwnd();
		HWND GetHwndUp();
		int  GetSelect();
		BOOL GetHasWndFlag();
		bool GetSingleFlag();
		bool GetFullFlag();
		bool GetRecordFlag();
		void SetRecordFlag(bool bRecord);
		int  GetPosType();
		bool IsConnect(){return m_bConnect;};
		bool FullScreen(bool bShowToolbar = true);

		void SetText(LPCTSTR pstrText);
		CDuiString GetText(){return m_sTitle;};
		void UpdateText();
		void SetCourseFile(LPCTSTR filename){m_sCourseFile = filename;};
		CDuiString	GetID(){return m_sID;};
		void    SetID(CDuiString sID){m_sID = sID;};

		void OnBtnCtrl();
		void OnBtnFull();
		void OnBtnEscFull();
		void OnMainWndEscFull();
		void OnUpdateToolBar();
		void OnChangeWnd();

	public:
		CCamWnd*   m_pWindow;
		CVNCWnd*   m_pWndUp;
		CCamWndParent* m_pWndParent;
		CtrlWnd				*ctrlwnd;
		IMediaPlayer		*player;//video
		
		_videoURL			videosrc;
		Router				*router;
		bool	   m_bCtrlOpen;
		bool	   m_bDoorOpen;
		bool	   m_bProjectorOpen;
		bool	   m_bSelect;
		CComboUI *menu;
	protected:
		RECT	   m_rcBtnRes;
		RECT      m_rcBtnVoice;
		UINT       m_uResState;
		UINT       m_uVoiceState;
		bool	   m_bResSelect;
		bool       m_bResIng;
		BOOL       m_bVoiceSelect;
		CDuiString m_sTitle;
		CDuiString m_sCourseFile;
		int        m_iSelect;//setpos type
		bool       m_bHasWnd;
		bool	   m_bSingelUp;
		CDuiString m_sID;
		CDuiString m_sLBtnDownClass;
		bool       m_bVNCUp;
		bool       m_bConnect;

		RECT  rcDropBtn;

		
	};

	////////////////////////////////////////////////////
	//
	class CCamLayoutUI: public CContainerUI
	{
	public:
		CCamLayoutUI();
		~CCamLayoutUI(){m_bCheckCtrl = false;};
		LPCTSTR GetClass() const;
		void Init();
		void SetItemCount(int count);
		void SetParentHwnd(HWND hwnd);
		HWND GetParentHwnd();
		void DoEvent(TEventUI& event);
		void SetPos(RECT rc);
		void SetPos1(RECT rc,int iStartPos=0,bool bRe = true);
		void SetPos4(RECT rc,int iStartPos=0,bool bRe = true);
		void SetPos9(RECT rc,int iStartPos=0,bool bRe = true);
		void SetPos16(RECT rc,int iStartPos=0,bool bRe = true);
		void SetPos25(RECT rc,int iStartPos=0,bool bRe = true);
		void SetPos36(RECT rc,int iStartPos=0,bool bRe = true);
		void SetPos64(RECT rc,int iStartPos=0,bool bRe = true);
		void SetPosSquare(int nSquareRoot,RECT rc,int iStartPos=0,bool bRe = true);
		void SetPosCount(int count);
		int  GetPosCount();
		void SetFullFlag(bool bFull);
		bool GetFullFlag();

		bool Add(CDuiString sID);
		bool AddAt(CDuiString sID,int iIndex);
		bool Remove(CDuiString sID);
		void RemoveAll();
		CCamCtrolUI* GetItem(CDuiString sID);
		CCamCtrolUI* GetFullScreenItem();
		void SetClassTree(CFileListUI* p){m_pFileList = p;};
		void ReConnectClass();
		void RecordVisualClass(bool bRecord);
		int  GetRecordCount();
		void SelectAll(bool bSel);

	private:

	public:
		CFileListUI* m_pFileList;//方便内部窗口调用查询
		bool		 m_bSelMutil;
		bool		 m_bSelAll;
	protected:
		HWND m_hParent;
		int  m_iPosCount;
		int  m_iStartPos;
		bool m_bFull;
		bool m_bCheckCtrl;
	};

	////////////////////////////////////////////////////
	//
	class CBtnCalendarUI: public CButtonUI
	{
	public:
		CBtnCalendarUI(){};
		~CBtnCalendarUI(){};
		void PaintText(HDC hDC);
	};

	////////////////////////////////////////////////////
	//
	class CComboListUI:public CComboUI
	{
	public:
		CComboListUI(){};
		~CComboListUI(){};
		LPCTSTR GetClass(){return _T("CComboListUI");}
		void PaintText(HDC hDC);
		CDuiString m_sImage;
	};

	////////////////////////////////////////////////////
	//
	class CComboBtnUI: public CButtonUI
	{
	public:
		CComboBtnUI(){};
		~CComboBtnUI(){};
		LPCTSTR GetClass(){return _T("CComboBtnUI");}
		void DoEvent(TEventUI& event);
	};

	////////////////////////////////////////////////////
	//
	class COptionElement: public COptionUI
	{
	public:
		COptionElement();
		void SetNorImg(CDuiString sImg);
		void SetHotImg(CDuiString sImg);
		void SetSelectNorImg(CDuiString sImg);	
		void SetSelectHotImg(CDuiString sImg);

	private:
		void PaintText(HDC hDC);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	private:
		CDuiString  m_sNorImg;
		CDuiString  m_sHotImg;
		CDuiString	m_sSelNorImg;
		CDuiString	m_sSelHotImg;	
	};

}
