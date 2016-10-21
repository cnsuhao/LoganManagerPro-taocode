#ifndef WIN_IMPL_BASE_HPP
#define WIN_IMPL_BASE_HPP


namespace DuiLib
{

	enum UILIB_RESOURCETYPE
	{
		UILIB_FILE=1,		// 来自磁盘文件
		UILIB_ZIP,			// 来自磁盘zip压缩包
		UILIB_RESOURCE,		// 来自资源
		UILIB_ZIPRESOURCE,	// 来自资源的zip压缩包(参考QQdemo)
	};

	enum DOCK_STATUS
	{
		DOCK_IDLE,
		DOCK_LEFT,
		DOCK_TOP,
		DOCK_RIGHT
	};

	const UINT nTimerClose = 6;

	class UILIB_API WindowImplBase
		: public CWindowWnd
		, public CNotifyPump
		, public INotifyUI
		, public IMessageFilterUI
		, public IDialogBuilderCallback
	{
		DUI_DECLARE_MESSAGE_MAP()
		virtual void OnClick(TNotifyUI& msg);

	public:
		WindowImplBase();
		virtual ~WindowImplBase(){};
		virtual void Init(){m_bInit = true;};
		virtual void OnFinalMessage( HWND hWnd );
		virtual void Notify(TNotifyUI& msg);
		virtual void SetKeepToSide(bool bSide=true){m_bKeepToSide = bSide;};
		virtual bool GetKeepToSide(){return m_bKeepToSide;};
		virtual void SetOnToolBar(bool bShow=false){m_bOnToolBar = bShow;};
		virtual void AnimateClose(){SetTimer(m_hWnd,nTimerClose,50,NULL);};
		//新建右键菜单
		virtual CMenuWnd* CreateMenuWnd(CDuiString sXML,CPoint pt, map<CDuiString,bool>* pMenuCheckInfo = NULL, bool bShowOnTop = true);
		//文件拖拽
		virtual void SetFileDrop(bool bDrop=true);

	protected:
		virtual CDuiString GetSkinFolder()= 0;
		virtual CDuiString GetSkinFile() = 0;
		virtual UILIB_RESOURCETYPE GetResourceType() const;
		virtual LPCTSTR GetResourceID() const;
		virtual void ReloadZipResource(int ID); // ZIP资源包换肤函数 

		virtual LPCTSTR GetWindowClassName(void) const = 0 ;
		LRESULT ResponseDefaultKeyEvent(WPARAM wParam);

		bool		m_bInit;
		CPaintManagerUI m_PaintManager;
		static LPBYTE m_lpResourceZIPBuffer;

	public:
		virtual UINT GetClassStyle() const;
		virtual CDuiString GetZIPFileName() const;
		virtual CControlUI* CreateControl(LPCTSTR pstrClass);
		virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, bool& /*bHandled*/);
		virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

#if defined(WIN32) && !defined(UNDER_CE)
		virtual LRESULT OnNcButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnNcActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnNcPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
		virtual LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnMouseWheel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
#endif
		virtual LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnKeyUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT OnDropFile(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);//自定义消息
		virtual LONG	GetStyle();
		virtual LRESULT OnMenu(UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:
		void KeepTOSide_Show();
		void KeepTOSide_Hide();
		bool OnAnimateClose();
		//文件拖曳回调
		static void OnDropFiles(WindowImplBase* pObj, HDROP hDrop);
		friend void OnDropFiles(WindowImplBase* pObj, HDROP hDrop);

	private:
		CDuiRect	m_rcCurRect;		//窗口当前位置及大小
		BOOL		m_bHided;			//是否隐藏
		int			m_iDockState;		//当前停靠状态
		bool		m_bKeepToSide;		//靠边隐藏与否
		bool		m_bOnToolBar;		//是否在任务栏显示
		bool        m_bDrop;			//是否要支持拖放
		CDropTargetEx * m_pDropTarget;  //使窗体支持拖放操作
	};
}

#endif // WIN_IMPL_BASE_HPP
