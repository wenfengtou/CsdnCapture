// MainFrm.h : CMainFrame 类的接口
//
/**
** 作者：pengweizhi
** 邮箱：1795387053@qq.com
**/

#pragma once

#include "ChildView.h"
#include <afxmt.h>
#include "VideoDisplayWnd.h"
#include "AVInputStream.h"
#include "AVOutputStream.h"
#include "ImagePainter.h"

#include <map>
using namespace std;


class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
    virtual ~CMainFrame();

public:
    void   CalculateFPS();
	BOOL   IsPreview() { return m_bPreview; }

protected: 
	DECLARE_DYNAMIC(CMainFrame)

	LRESULT  OnVideoWinMsg(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
	
	LRESULT  OnStartStream(WPARAM wParam, LPARAM lParam);
	LRESULT  OnStopStream(WPARAM wParam, LPARAM lParam);

	 void OnTimer(UINT nIDEvent);
	 void OnDestroy();

	 void OnStatisticsView();

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	void   HandleMenuID(UINT  nOSDMenuID, POINT pt);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CVideoDisplayWnd    m_wndView;

	POINT              m_ptMenuClickPoint;

protected:
	long              m_nChannelNum; //通道号

	RECT              m_paintRect;
	CRect             m_rcWin;
	CRect             m_rcVideo;


	UINT              m_frmCount;
    UINT              m_nFPS;

	HANDLE            m_fp;
	BOOL              m_bPreview;

	char              m_szFilePath[256];

	CString           m_strAppTitle;

public:
	CAVInputStream    m_InputStream;
	CAVOutputStream   m_OutputStream;
    CImagePainter     m_Painter;

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClickMenu(UINT id);
	afx_msg void OnUpdateMenuUI(CCmdUI* pCmdUI);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnMove(int x, int y);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);

	LRESULT OnProcessCommnMessage(WPARAM wParam, LPARAM lParam);

	//void   WriteVideoFrame(PBYTE pData, DWORD dwSize, INT64 lTimeStamp, int nFrameType); //输入原始图像帧

};


