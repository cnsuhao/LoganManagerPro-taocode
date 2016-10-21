#ifndef WIN_IMPL_BASE_HPP
#define WIN_IMPL_BASE_HPP


namespace DuiLib
{

	enum UILIB_RESOURCETYPE
	{
		UILIB_FILE=1,		// ���Դ����ļ�
		UILIB_ZIP,			// ���Դ���zipѹ����
		UILIB_RESOURCE,		// ������Դ
		UILIB_ZIPRESOURCE,	// ������Դ��zipѹ����(�ο�QQdemo)
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
		//�½��Ҽ��˵�
		virtual CMenuWnd* CreateMenuWnd(CDuiString sXML,CPoint pt, map<CDuiString,bool>* pMenuCheckInfo = NULL, bool bShowOnTop = true);
		//�ļ���ק
		virtual void SetFileDrop(bool bDrop=true);

	protected:
		virtual CDuiString GetSkinFolder()= 0;
		virtual CDuiString GetSkinFile() = 0;
		virtual UILIB_RESOURCETYPE GetResourceType() const;
		virtual LPCTSTR GetResourceID() const;
		virtual void ReloadZipResource(int ID); // ZIP��Դ���������� 

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
		virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);//�Զ�����Ϣ
		virtual LONG	GetStyle();
		virtual LRESULT OnMenu(UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:
		void KeepTOSide_Show();
		void KeepTOSide_Hide();
		bool OnAnimateClose();
		//�ļ���ҷ�ص�
		static void OnDropFiles(WindowImplBase* pObj, HDROP hDrop);
		friend void OnDropFiles(WindowImplBase* pObj, HDROP hDrop);

	private:
		CDuiRect	m_rcCurRect;		//���ڵ�ǰλ�ü���С
		BOOL		m_bHided;			//�Ƿ�����
		int			m_iDockState;		//��ǰͣ��״̬
		bool		m_bKeepToSide;		//�����������
		bool		m_bOnToolBar;		//�Ƿ�����������ʾ
		bool        m_bDrop;			//�Ƿ�Ҫ֧���Ϸ�
		CDropTargetEx * m_pDropTarget;  //ʹ����֧���ϷŲ���
	};
}

#endif // WIN_IMPL_BASE_HPP
