// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// ��Ŀ�ض��İ����ļ�

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// �� Windows ��ͷ���ų�����ʹ�õ�����
#endif

#define  WINVER 0x0501


#ifndef WINVER				// ����ʹ�� Windows 95 �� Windows NT 4 ����߰汾���ض����ܡ�
#define WINVER 0x0501		//Ϊ Windows98 �� Windows 2000 �����°汾�ı�Ϊ�ʵ���ֵ��
#endif

#ifndef _WIN32_WINNT		// ����ʹ�� Windows NT 4 ����߰汾���ض����ܡ�
#define _WIN32_WINNT 0x0501		//Ϊ Windows98 �� Windows 2000 �����°汾�ı�Ϊ�ʵ���ֵ��
#endif						

#ifndef _WIN32_WINDOWS		// ����ʹ�� Windows 98 ����߰汾���ض����ܡ�
#define _WIN32_WINDOWS 0x0501 //Ϊ Windows Me �����°汾�ı�Ϊ�ʵ���ֵ��
#endif

#ifndef _WIN32_IE			// ����ʹ�� IE 4.0 ����߰汾���ض����ܡ�
#define _WIN32_IE 0x0501	//Ϊ IE 5.0 �����°汾�ı�Ϊ�ʵ���ֵ��
#endif


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ��������������ȫ���Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ���ĺͱ�׼���
#include <afxext.h>         // MFC ��չ

#include <afxdtctl.h>		// Internet Explorer 4 �����ؼ��� MFC ֧��
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// Windows �����ؼ��� MFC ֧��
#endif // _AFX_NO_AFXCMN_SUPPORT


#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000


typedef struct _decode_statis_
{
	UINT  nVideoFormat; //1--MPEG1, 2--MPEG2, 3--MPEG4, 4--H264
   	UINT  nVideoFrames;
	UINT  nAudioFrames;
    UINT  nFramerate;
	int  nWidth;
	int  nHeight;
    DWORD dwBPS; 
}DECODE_STATIS;



//#include <afxsock.h>		// MFC socket extensions
#include <Winsock2.h>
#include "./include/stdint.h"

#include <atltrace.h>

//#include <streams.h>
#include <string>
using namespace std;

#ifdef __cplusplus
extern "C" {
#endif 

#ifdef HAVE_AV_CONFIG_H
#undef HAVE_AV_CONFIG_H
#endif

#include "./include/libavcodec/avcodec.h"
#include "./include/libavutil/mathematics.h"
#include "./include/libavutil/avutil.h"
#include "./include/libswscale/swscale.h"
#include "./include/libavutil/fifo.h"
#include "./include/libavformat/avformat.h"
#include "./include/libavutil/opt.h"
#include "./include/libavutil/error.h"
#include "./include/libswresample/swresample.h"
#include "./include/libavutil/audio_fifo.h"
#include "./include/libavutil/time.h"
#include "./include/libavdevice/avdevice.h"
#ifdef __cplusplus
}
#endif

#pragma comment( lib, "avcodec.lib")
#pragma comment( lib, "avutil.lib")
#pragma comment( lib, "avformat.lib")
#pragma comment(lib,   "swresample.lib")
#pragma comment(lib,  "swscale.lib" )
#pragma comment(lib, "avdevice.lib")

#ifndef CodecID
#define CodecID AVCodecID
#endif

#define _USE_NEW_FFMPEG_API_

#define nullptr NULL


#define  WM_COMM_MESSAGE         WM_USER + 111