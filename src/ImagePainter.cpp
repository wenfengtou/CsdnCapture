//
// ImagePainter.cpp
//


#include "stdafx.h"
#include "ImagePainter.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
CImagePainter::CImagePainter()
{
    memset(&m_bmpInfo, 0, sizeof(m_bmpInfo));
	m_hVideoWindow = NULL;
	m_WindowDC    = NULL;
	m_bNeedStretch = TRUE;

	SetRectEmpty(&m_TargetRect);
    SetRectEmpty(&m_SourceRect);

	m_pRgb24 = NULL;
	m_nRgbSize = 0;
	img_convert_ctx = NULL;	
}

CImagePainter::~CImagePainter()
{
	// Close the wave output device
	Close();
}

//设置显示视频的窗口句柄
void CImagePainter::SetVideoWindow(HWND inWindow)
{
	m_hVideoWindow = inWindow;
}

//设置视频源格式
// width -- 图像宽带
// height -- 图像高度
// bpp -- 图像的位深，默认为24
// bReverseShow -- 是否颠倒显示图像，某些位图像素排列是颠倒的，需要把该参数设置为TRUE才能令图像正立显示
//
BOOL CImagePainter::SetSrcFormat(long width, long height, int bpp, BOOL bReverseShow)
{
	m_bmpInfo.bmiHeader.biBitCount = bpp;
	m_bmpInfo.bmiHeader.biPlanes = 1;
	m_bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFO);
	m_bmpInfo.bmiHeader.biWidth = width;
	m_bmpInfo.bmiHeader.biHeight = bReverseShow ?(-height):height;
	m_bmpInfo.bmiHeader.biSizeImage = width*height*bpp/8;

	// Fill source rectangle
	m_SourceRect.left   = 0;
	m_SourceRect.top    = 0;
	m_SourceRect.right  = width;
	m_SourceRect.bottom = height;
	return TRUE;
}

//初始化
BOOL CImagePainter::Open(void)
{
	// Get attributes of video window
	m_WindowDC = ::GetDC(m_hVideoWindow);
	::GetClientRect(m_hVideoWindow, &m_TargetRect);

	::SetStretchBltMode(m_WindowDC,STRETCH_DELETESCANS);

	// Calculate the overall rectangle dimensions
	LONG sourceWidth = m_SourceRect.right - m_SourceRect.left;
	LONG targetWidth = m_TargetRect.right - m_TargetRect.left;
	LONG sourceHeight = m_SourceRect.bottom - m_SourceRect.top;
	LONG targetHeight = m_TargetRect.bottom - m_TargetRect.top;

	if (sourceWidth == targetWidth && sourceHeight == targetHeight) 
	{
		m_bNeedStretch = FALSE;
    }

	//m_Image.Destroy();
	//m_Image.Create(sourceWidth, sourceHeight, 24);
	//long lpPitch = m_Image.GetPitch();

	m_pRgb24 = NULL;
	m_nRgbSize = 0;
	img_convert_ctx = NULL;	

	return TRUE;
}

BOOL CImagePainter::Close(void)
{
	//m_Image.Destroy();

	if(m_WindowDC)
	{
		::ReleaseDC(m_hVideoWindow, m_WindowDC);
		m_WindowDC = NULL;

		::InvalidateRect(m_hVideoWindow, NULL, 1);
	}
	memset(&m_bmpInfo, 0, sizeof(m_bmpInfo));

	if(m_pRgb24)
	{
		delete m_pRgb24;
		m_pRgb24 = NULL;
	}

	m_nRgbSize = 0;
	img_convert_ctx = NULL;	

	return TRUE;
}

//是否缩放图像显示图像，如果为TRUE，则填充窗口显示，如果为FALSE，则图像以原始分辨率显示到窗口中
void CImagePainter:: SetStretch(BOOL bStretch)
{
	m_bNeedStretch = bStretch;
}

BOOL CImagePainter::Play(BYTE * inData, DWORD inLength)
{
	::GetClientRect(m_hVideoWindow, &m_TargetRect);
	INT targetWidth  = m_TargetRect.right - m_TargetRect.left;
	INT targetHeight = m_TargetRect.bottom - m_TargetRect.top;
	
    if(m_bNeedStretch)
	{
	// Stretch the image when copying to the window
	 StretchDIBits(
			(HDC) m_WindowDC,                             // Target device HDC
			0,                        // X sink position
			0,                         // Y sink position
			targetWidth,                             // Destination width
			targetHeight,                            // Destination height
			m_SourceRect.left,                        // X source position
			m_SourceRect.top,                         // Y source position
			m_SourceRect.right - m_SourceRect.left,    // Source width
			m_SourceRect.bottom - m_SourceRect.top,    // Source height
			inData,                                  // Image data
			(BITMAPINFO *) &m_bmpInfo.bmiHeader,   // DIB header
			DIB_RGB_COLORS,                          // Type of palette
			SRCCOPY);  
	}
	else
	{

     SetDIBitsToDevice(
			(HDC) m_WindowDC,                            
			0,                       
			0,                        
			m_SourceRect.right - m_SourceRect.left,                        
			m_SourceRect.bottom - m_SourceRect.top,                           
			m_SourceRect.left,                      
			m_SourceRect.top,                       
			(UINT) 0,                             
			abs(m_bmpInfo.bmiHeader.biHeight),         
			inData,                                
			(BITMAPINFO *) &m_bmpInfo.bmiHeader, 
			DIB_RGB_COLORS); 
	}

	//BYTE* image_data = (BYTE *)m_Image.GetBits()+(m_Image.GetPitch()*(m_Image.GetHeight()-1));
	//memcpy(image_data, inData, inLength);
	//m_Image.Draw(mWindowDC, 0, 0, targetWidth, targetHeight);

	return TRUE;
}

BOOL CImagePainter::Play(HBITMAP inputBitmap)
{
	INT targetWidth  = m_TargetRect.right - m_TargetRect.left;
	INT targetHeight = m_TargetRect.bottom - m_TargetRect.top;
	INT sourceWidth  = m_SourceRect.right - m_SourceRect.left;
	INT sourceHeight = m_SourceRect.bottom - m_SourceRect.top;
	
	// To display video smoothly, use a memory DC
	HDC memDC = CreateCompatibleDC(m_WindowDC);

	HBITMAP oldbmp = (HBITMAP) SelectObject(memDC, inputBitmap);
    
	::StretchBlt(m_WindowDC, 0, 0, targetWidth, targetHeight, memDC, 0, 0, sourceWidth, sourceHeight, SRCCOPY);

	SelectObject(memDC, oldbmp);

	DeleteDC(memDC);

	return TRUE;
}

//显示FFmpeg的AVFrame类型的数据
BOOL CImagePainter::Play(AVStream * st, AVFrame * pFrame)
{

	if(m_pRgb24 == NULL || m_nRgbSize == 0)
	{
		int nRgbSize = st->codec->width * st->codec->height*4; //24位位图占用的大小理论上应为st->codec->width * st->codec->height*3， 但是考虑到每行像素宽带4字节对齐的问题，所以故意分配多一点空间
		m_pRgb24 = new BYTE[nRgbSize];	
		memset(m_pRgb24, 0, nRgbSize);

		m_nRgbSize = nRgbSize;
	}

    if(st->codec->pix_fmt != PIX_FMT_BGR24)
	{
		if(img_convert_ctx == NULL)
		{
			img_convert_ctx = sws_getContext(st->codec->width, st->codec->height,
			/*PIX_FMT_YUV420P*/st->codec->pix_fmt,
			st->codec->width, st->codec->height,
			PIX_FMT_BGR24,
			SWS_BICUBIC, NULL, NULL, NULL);

			if (img_convert_ctx == NULL)
			{
				TRACE("sws_getContext() failed \n");
				return FALSE;		
			}
		}

		uint8_t *rgb_src[3]= {m_pRgb24, NULL, NULL};
		int rgb_stride[3] = {st->codec->width*3, 0, 0};

		//转成RGB格式
		sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize,  0, st->codec->height, rgb_src, rgb_stride);

		//显示图像
		Play(m_pRgb24, m_nRgbSize);
	}
	else
	{
		ASSERT(pFrame->linesize[0] ==  st->codec->width * st->codec->height*3);

		Play(pFrame->data[0], pFrame->linesize[0]);
	}

	return TRUE;
}
