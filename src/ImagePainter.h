//
// ImagePainter.h
//


#ifndef __H_CImagePainter__
#define __H_CImagePainter__

//#include <atlImage.h>


class CImagePainter 
{
private:
    BITMAPINFO      m_bmpInfo;
	HWND			m_hVideoWindow;
	HDC				m_WindowDC;
	RECT			m_TargetRect;
	RECT			m_SourceRect;
	BOOL			m_bNeedStretch;

	//CImage          m_Image;

    int     m_nRgbSize;
	PBYTE   m_pRgb24;

	struct SwsContext *img_convert_ctx;	

public:
	CImagePainter();
	~CImagePainter();

	void SetVideoWindow(HWND inWindow);
	BOOL SetSrcFormat(long width, long height, int bpp, BOOL bReverseShow);

	void  SetStretch(BOOL bStretch);
	BOOL  IsStretchShow() { return m_bNeedStretch; }

	BOOL Open(void);
	BOOL Close(void);
	BOOL Play(BYTE * inData, DWORD inLength);
	BOOL Play(HBITMAP inputBitmap);
	BOOL Play(AVStream * st, AVFrame * pFrame);

	BOOL  IsOpened() { return (m_bmpInfo.bmiHeader.biSizeImage > 0); }


};

#endif // __H_CImagePainter__