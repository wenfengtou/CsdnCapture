// NSPlayer.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "NSPlayer.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CNSPlayerApp

BEGIN_MESSAGE_MAP(CNSPlayerApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
END_MESSAGE_MAP()


// CNSPlayerApp ����

CNSPlayerApp::CNSPlayerApp()
{

}


// Ψһ��һ�� CNSPlayerApp ����

CNSPlayerApp theApp;

// CNSPlayerApp ��ʼ��

BOOL CNSPlayerApp::InitInstance()
{

	InitCommonControls();

	CWinApp::InitInstance();

	// Init WinSock
	WSADATA   data;
	int ret = WSAStartup(0x0202, &data);
	if (ret != 0)
	{
		WSACleanup();
		return FALSE;
	}

	int argc = __argc;
	char **argv = __argv;
	//processParams(argc, argv, &g_appParam);

	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;

	BOOL bSucceed = pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);

	if(!bSucceed)
		return FALSE;

	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	return TRUE;
}


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// �������жԻ����Ӧ�ó�������
void CNSPlayerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CNSPlayerApp ��Ϣ�������


int CNSPlayerApp::ExitInstance()
{
	WSACleanup();

	return CWinApp::ExitInstance();
}
