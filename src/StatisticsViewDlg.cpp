// StatisticsViewDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "NSPlayer.h"
#include "Resource.h"
#include "StatisticsViewDlg.h"
#include "MainFrm.h"
//#include "Mydef.h"



#define  TIMER_UPDATE_STATIS  3


enum 
{
	_VIDEO_MPEG1 = 0,
	_VIDEO_MPEG2 = 1,
	_VIDEO_MPEG4 = 2,
	_VIDEO_H264 = 3
};

// CStatisticsViewDlg 对话框

IMPLEMENT_DYNAMIC(CStatisticsViewDlg, CDialog)
CStatisticsViewDlg::CStatisticsViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStatisticsViewDlg::IDD, pParent)
{
	m_nChannel = 0;
	m_pwndParent = pParent;
}

CStatisticsViewDlg::~CStatisticsViewDlg()
{
}

void CStatisticsViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStatisticsViewDlg, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)

END_MESSAGE_MAP()


// CStatisticsViewDlg 消息处理程序

BOOL CStatisticsViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_nTimer = -1;

	return TRUE;  

}

void CStatisticsViewDlg::OnTimer(UINT nIDEvent)
{
	CDialog::OnTimer(nIDEvent);

	if(!IsWindowVisible())
		return;

	//if(m_nChannel < 0)
	//	return;


	DECODE_STATIS  statis;

	if(!((CMainFrame*)m_pwndParent)->GetDecodeStatis(statis))
	{
         return;
	}


	CString str;

	switch(statis.nVideoFormat)
	{
	case _VIDEO_MPEG1:
		str.Format("Video Format: MPEG1");
		break;
	case _VIDEO_MPEG2:
		str.Format("Video Format: MPEG2");
		break;
	case _VIDEO_MPEG4:
		str.Format("Video Format: MPEG4");
		break;
	case _VIDEO_H264:
		str.Format("Video Format: H264");
		break;

	}

	GetDlgItem(IDC_STATIC_VF)->SetWindowText(str);


	str.Format("Picture Size: %d x %d", statis.nWidth, statis.nHeight);

	GetDlgItem(IDC_STATIC_WH)->SetWindowText(str);

		
	str.Format("Bitrate: %ld kb/s", statis.dwBPS);
	GetDlgItem(IDC_STATIC_BPS)->SetWindowText(str);


	str.Format("Framerate: %d", statis.nFramerate);
	GetDlgItem(IDC_STATIC_FPS)->SetWindowText(str);

	str.Format("Buffered Video Frames: %d", statis.nVideoFrames);
	GetDlgItem(IDC_STATIC_BUFFERED_FRAMES)->SetWindowText(str);



	//TRACE("CStatisticsViewDlg::OnTimer()...\n");

}

void  CStatisticsViewDlg::StartTimer()
{
	if(m_nTimer > 0)
	{
		KillTimer(TIMER_UPDATE_STATIS);
		m_nTimer = -1;
	}

    m_nTimer = SetTimer(TIMER_UPDATE_STATIS, 2000, NULL);

   	CString str;


	str.Format("Framerate: %d", 0);
	GetDlgItem(IDC_STATIC_2)->SetWindowText(str);

	str.Format("Picture size: %d * %d",0, 0);
	GetDlgItem(IDC_STATIC_3)->SetWindowText(str);

	str.Format("Bitrate: %ld kb/s", 0);
	GetDlgItem(IDC_STATIC_4)->SetWindowText(str);
}

void CStatisticsViewDlg::OnBnClickedOk()
{
	KillTimer(TIMER_UPDATE_STATIS);
    m_nTimer = -1;

    ShowWindow(SW_HIDE);
	//OnOK();
}


