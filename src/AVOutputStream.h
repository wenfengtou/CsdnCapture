#pragma once

#include <string>
using namespace std;

class CAVOutputStream
{
public:
	CAVOutputStream(void);
	~CAVOutputStream(void);

public:

	//��ʼ����Ƶ������
	void SetVideoCodecProp(AVCodecID codec_id, int framerate, int bitrate, int gopsize, int width, int height);

	//��ʼ����Ƶ������
	void SetAudioCodecProp(AVCodecID codec_id, int samplerate, int channels, int bitrate);

	//�����������ͻ�����������
	bool  OpenOutputStream(const char* out_path);

	//д��һ֡ͼ��
	int   write_video_frame(AVStream *st, enum PixelFormat pix_fmt, AVFrame *pframe, INT64 lTimeStamp);

	//д��һ֡��Ƶ
	int   write_audio_frame(AVStream *st, AVFrame *pframe, INT64 lTimeStamp);

	//�ر����
	void  CloseOutput();

protected:
	//AVFormatContext *m_pVidFmtCtx;
   // AVFormatContext *m_pAudFmtCtx;
    //AVInputFormat* m_pInputFormat;

    AVStream* video_st;
    AVStream* audio_st;
    AVFormatContext *ofmt_ctx;

    AVCodecContext* pCodecCtx;
    AVCodecContext* pCodecCtx_a;
    AVCodec* pCodec;
    AVCodec* pCodec_a;
    AVPacket enc_pkt;
    AVPacket enc_pkt_a;
    AVFrame *pFrameYUV;
    struct SwsContext *img_convert_ctx;
    struct SwrContext *aud_convert_ctx;

	AVAudioFifo * m_fifo;

	int  m_vid_framecnt;
	int  m_aud_framecnt;

	int  m_nb_samples;

	int64_t m_first_vid_time1, m_first_vid_time2; //ǰ���ǲɼ���Ƶ�ĵ�һ֡��ʱ�䣬�����Ǳ���������ĵ�һ֡��ʱ��
	int64_t m_first_aud_time; //��һ����Ƶ֡��ʱ��

	int64_t m_next_vid_time;
	int64_t m_next_aud_time;

	int64_t  m_nLastAudioPresentationTime; //��¼��һ֡����Ƶʱ���

	uint8_t ** m_converted_input_samples;
	uint8_t * m_out_buffer;

public:
	string     m_output_path; //���·��

    AVCodecID  m_video_codec_id;
    AVCodecID  m_audio_codec_id;

	int m_width, m_height;
	int m_framerate;
	int m_video_bitrate;
	int m_gopsize;

	int m_samplerate;
	int m_channels;
	int m_audio_bitrate;

};
