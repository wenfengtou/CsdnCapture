// MainFrm.cpp : CMainFrame ���ʵ��
//
/**
** ���ߣ�pengweizhi
** ���䣺1795387053@qq.com
**/

#include "stdafx.h"

#include "NSPlayer.h"
//#include "Mydef.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include <mmsystem.h>
#include "MainFrm.h"
#include "StatisticsViewDlg.h"
#include "PProfile.h"
#include <math.h>
//#include "StreamMuxer.h"
#include "SelectCaptureDeviceDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define  WM_UPDATE_CLIENT_STATE  WM_USER + 102

#define  WM_START_STREAM         WM_USER + 104
#define  WM_STOP_STREAM          WM_USER + 105


#define  TIMER_CLIENT       20


ULONG  StartTime = 0;
BOOL   g_border = 1;


CMainFrame         *  gpMainFrame = NULL;

//CStatisticsViewDlg  *  m_pViewStatisDlg = NULL;


//LRESULT CALLBACK OnDisplayVideo(int devNum,PBYTE pRgb, int dwSize); //ͼ��RGB24���ݻص�

//�ɼ�������Ƶͼ��ص�
LRESULT CALLBACK VideoCaptureCallback(AVStream * input_st, enum PixelFormat pix_fmt, AVFrame *pframe, INT64 lTimeStamp);

//�ɼ�������Ƶ���ݻص�
LRESULT CALLBACK AudioCaptureCallback(AVStream * input_st, AVFrame *pframe, INT64 lTimeStamp);

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	//ON_WM_SETFOCUS()
	ON_WM_TIMER()
	ON_WM_DESTROY()
    ON_COMMAND_RANGE(32770,32880, OnClickMenu)
    ON_UPDATE_COMMAND_UI_RANGE(32770, 32880, OnUpdateMenuUI)
	ON_MESSAGE(WM_COMM_MESSAGE, OnProcessCommnMessage)
	ON_MESSAGE(WM_START_STREAM, OnStartStream)
	ON_MESSAGE(WM_STOP_STREAM, OnStopStream)
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_MOVING()
END_MESSAGE_MAP()


// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	gpMainFrame = this;
	//m_pViewStatisDlg = NULL;

	memset(m_szFilePath, 0, sizeof(m_szFilePath));
   
	m_bPreview  = TRUE;
	m_nChannelNum = -1;
	m_nFPS = 0;
		                 
	m_fp = INVALID_HANDLE_VALUE;

	 /* register all codecs, demux and protocols */
    avcodec_register_all();
    av_register_all();

	avdevice_register_all();  
}

CMainFrame::~CMainFrame()
{

}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndView.Create(NULL, NULL, WS_CHILD|WS_VISIBLE,
		CRect(0, 0, 0, 0), this, -1))
	{
		TRACE0("δ�ܴ�����ƵԤ������\n");
		return -1;
	}
	

    SetWindowPos(NULL, 0, 0, 400, 350, SWP_NOZORDER|SWP_NOMOVE);
	

	//OnStartStream(0, 0);

	m_strAppTitle = GetTitle();

	return 0;
}

void CMainFrame::OnDestroy()
{
	KillTimer(TIMER_CLIENT);

    OnStopStream(0, 0);

	//if(m_pViewStatisDlg)
	//	delete m_pViewStatisDlg;

	Sleep(100);
	
	CFrameWnd::OnDestroy();

	TRACE("OnDestroy end.\n");
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	// TODO: �ڴ˴�ͨ���޸� CREATESTRUCT cs ���޸Ĵ��������ʽ

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	cs.style |= WS_CLIPCHILDREN;

	return TRUE;
}


// CMainFrame ���

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG



void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// ������ǰ�Ƶ���ͼ����
	//m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	//if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
	//	return TRUE;

	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CMainFrame::OnClickMenu(UINT uMenuId)
{
	// Parse the menu command
	switch (uMenuId)
	{
		case IDM_START_CAP: //��ʼ�ɼ�
		{
			CSelectCaptureDeviceDlg dlg;
			if(dlg.DoModal() == IDOK)
			{
				USES_CONVERSION;

				m_InputStream.SetVideoCaptureDevice(T2A((LPTSTR)(LPCTSTR)dlg.m_strVideoDevice));
				m_InputStream.SetAudioCaptureDevice(T2A((LPTSTR)(LPCTSTR)dlg.m_strAudioDevice));

			    LRESULT lResult = OnStartStream(0, 0);

				if(lResult == 0)
				   SetWindowText(_T("���ڲɼ�..."));
			}
		}
		break;
		
		case IDM_STOP_CAP: //ֹͣ�ɼ�
		{
			OnStopStream(0, 0);
			SetWindowText(m_strAppTitle);
		}
		break;
		
		case IDM_PREVIEW_VIDEO: //Ԥ��
		{
			m_bPreview = !m_bPreview;
		}
        break;

		case IDM_MENU1:
		case IDM_MENU2:
		case IDM_MENU3:
			{
				HandleMenuID(uMenuId, m_ptMenuClickPoint);
			}
			break;

	}//switch	 
}


void CMainFrame:: OnUpdateMenuUI(CCmdUI* pCmdUI)
{
  switch(pCmdUI->m_nID)
 {
  case IDM_START_CAP:
	  {	 
		//pCmdUI->Enable(!m_bCaptureing); 
	  }
     break;
  case IDM_STOP_CAP:
	  {
		//pCmdUI->Enable(m_bCaptureing);	 
	  }
	 break;
  case IDM_SET_NOBORDER:
	  {
        pCmdUI->SetCheck(!g_border);
	  }
	  break;

  case IDM_PREVIEW_VIDEO: //�Ƿ�Ԥ��
	 {
		 pCmdUI->SetCheck(m_bPreview); 
	 }
	 break;

 }//switch

}

static  BOOL g_bCaptureCursor = FALSE;
POINT   g_pCapturePt;


// We use  "Esc" key to restore from fullscreen mode
BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	
	switch(pMsg->message)
	{
 
		case WM_LBUTTONDBLCLK:
		{
			//CRect rcVideo;
			//::GetWindowRect(m_Decoder.m_hWnd, rcVideo);
			////ScreenToClient(rcVideo);

			//POINT pt;
			//::GetCursorPos(&pt);
			//if (rcVideo.PtInRect(pt))
			//{
			//  ::SendMessage(m_hWnd, WM_SHOW_MAXIMIZED, 0, 0);
			//  m_bFullScreen = TRUE;
			//}	
			//    
			//return TRUE;

		}
		break;
		
		case WM_RBUTTONDOWN:
			{
				LONG style = GetWindowLong(m_hWnd, GWL_STYLE);
				if(style & (WS_DLGFRAME | WS_THICKFRAME | WS_BORDER))
				{
					CMenu menu;
					menu.CreatePopupMenu();
					menu.AppendMenu(MF_STRING, IDM_MENU1, "Test1");
					menu.AppendMenu(MF_STRING, IDM_MENU2, "Test2");
					menu.AppendMenu(MF_STRING, IDM_MENU3, "Test3");

					POINT pt;
					::GetCursorPos(&pt);
					menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);

	                ScreenToClient(&pt);
					m_ptMenuClickPoint = pt;
				}

			}
			break;
			
		case WM_LBUTTONDOWN:
		{
			LONG style = GetWindowLong(m_hWnd, GWL_STYLE);
			if(style & (WS_DLGFRAME | WS_THICKFRAME | WS_BORDER))
			{
				return TRUE;
			}

			SetCapture();
			g_bCaptureCursor = TRUE;
			//TRACE("SetCapture() \n");
				 
			POINT pt;
			pt = pMsg->pt;
			ScreenToClient(&pt);

			int nCaptionHeight = 0;
			if(g_border)
				nCaptionHeight = GetSystemMetrics(SM_CYSMCAPTION);

			g_pCapturePt.x   = pt.x;
			g_pCapturePt.y   = pt.y + nCaptionHeight;

		}
		break;

		case WM_LBUTTONUP:
		{
			ReleaseCapture();
			g_bCaptureCursor = FALSE;
			//TRACE("ReleaseCapture() \n");
		}
		break;

		case WM_MOUSEMOVE:
		{
			if(!g_bCaptureCursor)
				return TRUE;

				SetWindowPos(NULL, 
					pMsg->pt.x - g_pCapturePt.x,
					pMsg->pt.y - g_pCapturePt.y, 
					0, 0, SWP_NOZORDER|SWP_NOSIZE);
			  
			return TRUE;
		}
		break;

		case WM_KEYDOWN:
		{
			if(::GetAsyncKeyState(VK_F5))
			{
				//int nTitleHeight  = GetSystemMetrics(SM_CYCAPTION);
				//int nBorderWidth  = GetSystemMetrics(SM_CXBORDER);
				//int nBorderHeight = GetSystemMetrics(SM_CYBORDER);

				RECT rcWin, rcClient;
				GetWindowRect(&rcWin);
				GetClientRect(&rcClient);

				int nBorderWidth = (rcWin.right-rcWin.left) - (rcClient.right - rcClient.left);
				int nBorderHeight = (rcWin.bottom - rcWin.top) - (rcClient.bottom - rcClient.top);

				//CSize size;
	           // GetVideoSize(size);

				//if(size.cx > 0 && size.cy > 0)
				//{
				//	SetWindowPos(NULL, 0, 0, size.cx+nBorderWidth, size.cy+nBorderHeight, SWP_NOMOVE);
				//}
			}
	       
		}
		break;

	}//switch

	return CFrameWnd::PreTranslateMessage(pMsg);
}



void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	CRect rect;
	GetClientRect(&rect);

	m_wndView.MoveWindow(rect);
}


void CMainFrame::OnMove(int x, int y)
{
	CFrameWnd::OnMove(x, y);
	//TRACE("OnMove() \n");
}

void CMainFrame::OnMoving(UINT fwSide, LPRECT pRect)
{
	CFrameWnd::OnMoving(fwSide, pRect);
	//TRACE("OnMoveing() \n");
}


LRESULT CMainFrame::OnProcessCommnMessage(WPARAM wParam, LPARAM lParam)
{
	if(wParam == 0)
	{
		::MessageBox(m_hWnd, _T("ת���ļ��ɹ�"), _T("��ʾ"), MB_OK);

		return 0;
	}
	else if(wParam == 1)
	{

	}

	return 0;
}



void CMainFrame::OnTimer(UINT nIDEvent)
{
	CFrameWnd::OnTimer(nIDEvent);

}

void   CMainFrame::HandleMenuID(UINT  nOSDMenuID, POINT pt)
{
	switch(nOSDMenuID)
	{
	case IDM_MENU1:
		break;
	case IDM_MENU2:
		break;
	case IDM_MENU3:
		break;
	}
}

LRESULT  CMainFrame:: OnStartStream(WPARAM wParam, LPARAM lParam)
{
	m_InputStream.SetVideoCaptureCB(VideoCaptureCallback);
	m_InputStream.SetAudioCaptureCB(AudioCaptureCallback);

	bool bRet;
	bRet = m_InputStream.OpenInputStream(); //��ʼ���ɼ��豸
	if(!bRet)
	{
		MessageBox(_T("�򿪲ɼ��豸ʧ��"), _T("��ʾ"), MB_OK|MB_ICONWARNING);
		return 1;
	}

	int cx, cy, fps;
	AVPixelFormat pixel_fmt;
	if(m_InputStream.GetVideoInputInfo(cx, cy, fps, pixel_fmt)) //��ȡ��Ƶ�ɼ�Դ����Ϣ
	{
		m_OutputStream.SetVideoCodecProp(AV_CODEC_ID_H264, fps, 500000, 100, cx, cy); //������Ƶ����������
	}

    int sample_rate = 0, channels = 0;
	AVSampleFormat  sample_fmt;
	if(m_InputStream.GetAudioInputInfo(sample_fmt, sample_rate, channels)) //��ȡ��Ƶ�ɼ�Դ����Ϣ
	{
		m_OutputStream.SetAudioCodecProp(AV_CODEC_ID_AAC, sample_rate, channels, 32000); //������Ƶ����������
	}

	//��Config.INI�ļ��ж�ȡ¼���ļ�·��
	P_GetProfileString(_T("Client"), "file_path", m_szFilePath, sizeof(m_szFilePath));

	bRet  = m_OutputStream.OpenOutputStream(m_szFilePath); //�������·��
	if(!bRet)
	{
		MessageBox(_T("��ʼ�����ʧ��"), _T("��ʾ"), MB_OK|MB_ICONWARNING);
		return 1;
	}

	m_Painter.SetVideoWindow(m_wndView.GetSafeHwnd()); //������ƵԤ������
	m_Painter.SetSrcFormat(cx, cy, 24, TRUE); //��4��������ʾ�Ƿ�תͼ��
	m_Painter.SetStretch(TRUE); //�Ƿ�������ʾͼ��
	m_Painter.Open();

	bRet = m_InputStream.StartCapture(); //��ʼ�ɼ�

	StartTime = timeGetTime();

	m_frmCount = 0;
	m_nFPS = 0;

	return 0;
}

LRESULT  CMainFrame:: OnStopStream(WPARAM wParam, LPARAM lParam)
{
	m_InputStream.CloseInputStream();
	m_OutputStream.CloseOutput();
    m_Painter.Close();

	TRACE("�ɼ���ʱ��%d ��\n", (timeGetTime() - StartTime)/1000);

	StartTime = 0;

	return 0;
} 


void CMainFrame::CalculateFPS()
{
	DWORD newtick;
	newtick = timeGetTime();

	m_frmCount++;
	int costtime = newtick - StartTime;
	if(costtime < 2000)
		return;
	{
		//TRACE("Frames input speed: %d\n", m_frmCount*1000/costtime);
		m_nFPS = m_frmCount*1000/costtime;
	}
	m_frmCount = 0;
	StartTime = newtick;
}



//�ɼ�������Ƶͼ��ص�
LRESULT CALLBACK VideoCaptureCallback(AVStream * input_st, enum PixelFormat pix_fmt, AVFrame *pframe, INT64 lTimeStamp)
{
	if(gpMainFrame->IsPreview())
	{
	   gpMainFrame->m_Painter.Play(input_st, pframe);
	}

	gpMainFrame->m_OutputStream.write_video_frame(input_st, pix_fmt, pframe, lTimeStamp);
	return 0;
}

//�ɼ�������Ƶ���ݻص�
LRESULT CALLBACK AudioCaptureCallback(AVStream * input_st, AVFrame *pframe, INT64 lTimeStamp)
{
	gpMainFrame->m_OutputStream.write_audio_frame(input_st, pframe, lTimeStamp);
	return 0;
}




