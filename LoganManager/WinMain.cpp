// LoganManager.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "LoginWnd.h"
#include "MainWnd.h"
#include "ClassMateWnd.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{


	//���̻���
	HANDLE Hmutex = CreateMutex(NULL, TRUE, _T("mainWindows"));
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		HWND h = ::FindWindow(NULL, _T("mainWindows"));
		if (h)
		{
			::SetForegroundWindow(h);
		}
		CloseHandle(Hmutex);
		::MessageBox(0, _T("�ó�����������"), _T("��ʾ"), MB_OK);
		return EXIT_FAILURE;
	}

	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr))
		return 0;

	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());
	//CPaintManagerUI::SetResourceZip("loganskin.zip");
	

	CLoginWnd* Login  = new CLoginWnd();
	Login->Create(NULL, _T("��¼"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	Login->SetIcon(IDI_LOGANMANAGER);
	Login->CenterWindow();
	UINT res=Login->ShowModal();
	if(res==IDOK)
	{
		CMainWnd* mm  = new CMainWnd();
		CMainWnd::pwnd=mm;
		mm->Create(NULL, _T("������"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE|WS_EX_TOPMOST);
		mm->SetIcon(IDI_LOGANMANAGER);
		mm->CenterWindow();
		mm->ShowModal();

	}
	else if(res==10086)
	{
		ClassMateWnd *cls = new ClassMateWnd();
		cls->Create(NULL,_T("classMate"),UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
		cls->SetIcon(IDI_LOGANMANAGER);
		cls->CenterWindow();
		cls->ShowModal();
	}
	
	::CoUninitialize();
	return EXIT_SUCCESS;

}