#pragma once

#include "wxutil2.h"

#include <string>
using namespace std;


typedef LRESULT (CALLBACK* VideoCaptureCB)(AVStream * input_st, enum PixelFormat pix_fmt, AVFrame *pframe, INT64 lTimeStamp);
typedef LRESULT (CALLBACK* AudioCaptureCB)(AVStream * input_st, AVFrame *pframe, INT64 lTimeStamp);

class CAVInputStream
{
public:
	CAVInputStream(void);
	~CAVInputStream(void);

public:
	void  SetVideoCaptureDevice(string device_name);
	void  SetAudioCaptureDevice(string device_name);

	bool  OpenInputStream();
	void  CloseInputStream();

	bool  StartCapture();

	void  SetVideoCaptureCB(VideoCaptureCB pFuncCB);
	void  SetAudioCaptureCB(AudioCaptureCB pFuncCB);

	bool  GetVideoInputInfo(int & width, int & height, int & framerate, AVPixelFormat & pixFmt);
	bool  GetAudioInputInfo(AVSampleFormat & sample_fmt, int & sample_rate, int & channels);

protected:
	static DWORD WINAPI CaptureVideoThreadFunc(LPVOID lParam);
	static DWORD WINAPI CaptureAudioThreadFunc(LPVOID lParam);

	int  ReadVideoPackets();
	int  ReadAudioPackets();

protected:
	string  m_video_device;
	string  m_audio_device;

	int     m_videoindex;
    int     m_audioindex;

    AVFormatContext *m_pVidFmtCtx;
    AVFormatContext *m_pAudFmtCtx;
    AVInputFormat  *m_pInputFormat;

    AVPacket *dec_pkt;

	HANDLE m_hCapVideoThread, m_hCapAudioThread; //线程句柄
	bool   m_exit_thread; //退出线程的标志变量

	VideoCaptureCB  m_pVideoCBFunc; //视频数据回调函数指针
	AudioCaptureCB  m_pAudioCBFunc; //音频数据回调函数指针

	CCritSec     m_WriteLock;

	int64_t     m_start_time; //采集的起点时间
};
