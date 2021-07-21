
#pragma once

#include "ChangeLog.h"
#include <vfw.h>


#ifdef __cplusplus
extern "C" {
#endif 

#ifdef HAVE_AV_CONFIG_H
#undef HAVE_AV_CONFIG_H
#endif

#include "./include/libavcodec/avcodec.h"
//#include "./include/libavutil/mathematics.h"
#include "./include/libavutil/avutil.h"
#include "./include/libswscale/swscale.h"
//#include "./include/libavutil/fifo.h"
//#include "./include/libavutil/samplefmt.h"
//#include "./include/libavformat/avformat.h"

#ifdef __cplusplus
}
#endif 

#pragma comment( lib, "vfw32.lib" )
#pragma comment( lib, "Winmm.lib" )
#pragma comment( lib, "Iphlpapi.lib" )

#pragma comment(lib, "Dxva2.lib")
#pragma comment( lib, "avcodec.lib" )
#pragma comment( lib, "avutil.lib" )
#pragma comment( lib, "swscale.lib" )

//#define	SAVE_TO_PS
//#define	TEST_SAVE_FILE

#define	COLOR_BKGROUND	RGB(10,10,10)
#define	MSG_BASE_RTSP	(WM_USER+100)

#define	MAX_DRAW_BUFFER		4
#define	MAX_RECTS_NUMBER	4
#ifdef	CH_20120918_ZHENGZHUO
#define	MAX_TS__COUNT		20000
#define	MAX_TS_BUFFER_LEN	3760000L	// 188*20000
#else
#define	MAX_TS_BUFFER		8
#define	MAX_TS_BUFFER_LEN	2097152	// 1024*1024*2
#endif

#define	PTZ_SIZE_X			100
#define	PTZ_SIZE_Y			350
#define	PTZ_ACT_NUM			34
#define	PTZ_PRESET_OFFSET_X	50
#define	PTZ_PRESET_OFFSET_Y	296

typedef struct __DRAW_BUFFER_OBJECT__
{
	int		fill;
	UINT	pts;
	BYTE	*buff;
	struct __DRAW_BUFFER_OBJECT__	*pNext;
}DRAW_BUFFER_OBJECT,*PDRAW_BUFFER_OBJECT;

typedef struct
{
	//HDRAWDIB			hdd;
	//HDC					hdc;
	LPBITMAPINFOHEADER	lbmi;
	int					SrcWidth;
	int					SrcHeight;
	int					YuvWidth;
	int					YuvHeight;
	int					YuvLinesize;
	int					DstWidth;
	int					DstHeight;
}REAL_DRAW_PARAM,*PREAL_DRAW_PARAM;

typedef struct
{
	int		left;
	int		top;
	int		width;
	int		height;
	int		color;
}RECTS_DRAW,*PRECTS_DRAW;

typedef struct
{
	int			update;
	RECTS_DRAW	rect[MAX_RECTS_NUMBER];
	RECTS_DRAW	rect_bak[MAX_RECTS_NUMBER];
	int			update_drawing;
	RECTS_DRAW	rect_drawing;
	RECTS_DRAW	rect_drawing_bak;
}RECTS_DRAW_OBJECT,*PRECTS_DRAW_OBJECT;

class CDxva2Decode;

typedef struct
{
	int		RunStatus;

	//
	BYTE	*pParamVideo;
	int		ParamVideoLen;
	//
	BOOL	g_DrawPause;
	HDC		hdc;
	BYTE	*pYuv;
	//
	UINT	FrameTotals;
	UINT	DataByteTotals;
	UINT	DataRate;
	UINT	VSsrc;
	UINT	VPts;
	//
	int		DrawThreadRun;
	//__int64				draw_timeoffset;
	int					draw_running;

	REAL_DRAW_PARAM		m_Rdp;

	UINT	            DelayTick;

	CDxva2Decode		*g_pDxva2Decode;
	BOOL				g_HWDecodeMode;

	BOOL				m_SoundEnable;
	HWND				g_DrawHwnd;
	UINT				DecodeTotals;


}THREAD_RTP_OBJECT,*PTHREAD_RTP_OBJECT;

typedef struct
{
	int		RunStatus;
	LPVOID	pMainCtrl;
	//
	UINT	UrlIp;
	USHORT	UrlPort;
	//
	USHORT	RtpPort;
	USHORT	RtcpPort;
	USHORT	ARtpPort;
	USHORT	ARtcpPort;
	//
	char	RtspUrl[256];
	char	UserName[64];
	char	UserPassword[64];
	//
	PTHREAD_RTP_OBJECT	pTro;
}THREAD_RTSP_OBJECT,*PTHREAD_RTSP_OBJECT;

typedef struct
{
	BYTE	CsrcCount:4;		// CSRC计数包括紧接在固定头后CSRC标识符个数。
	BYTE	X:1;				// 扩展位，头后面带扩展
	BYTE	P:1;				// 填充位，设置1表示包尾带填充数据，最后一个字节表示填充数据的长度
	BYTE	Version:2;			// RTP版本号，2

	BYTE	PT:7;				// 记录后面资料使用哪种Codec
	BYTE	M:1;				// 标志

	USHORT	Seq;				// 序列号

	UINT	Timestamp;			// 时标

	UINT	SSRC;				// SSRC段标识同步源

	UINT	CSRC[1];			// CSRC列表表示包内的对载荷起作用的源。标识数量由CC段给出
}RTP_HEAD,*PRTP_HEAD;

typedef struct
{
	BYTE	RC:5;				// 包内块计数
	BYTE	P:1;				// 填充位，设置1表示包尾带填充数据，最后一个字节表示填充数据的长度
	BYTE	Version:2;			// RTP版本号，2
	BYTE	PT;					// 包类型，200-RTCP-SR包
	USHORT	Len;				// 长度
	UINT	SSRC;				// SSRC段标识同步源

	UINT	TimeNtpH;			// NTP时间高位
	UINT	TimeNtpL;			// NTP时间低位
	UINT	TimeRtp;			// 与Rtp对应的时间
	UINT	RtpCounts;			// Rtp包发送数据计数
	UINT	RtpByteCounts;		// Rtp包发送的字节数
}RTCP_HEAD,*PRTCP_HEAD;

#ifdef	CH_20120911_ZHENGZHUO
class CColumbusCode;
#endif
typedef struct
{
	USHORT	RtpSeq;
	UINT	RtpPts;
	UINT	RtpSsrc;
	int		RtpFirst;
	int		CodecType;
	int		BufLen;
	int		FrameEnd;
	int		PtsPerframe;
	BYTE	*pBuff;
#ifdef	CH_20120911_ZHENGZHUO
	CColumbusCode	*pSlice;
	int				FrameType,PreFrameType;
#endif
#ifdef	CH_20121030_ZHENGZHUO
	int		PacketLostFlag;		// 标记当前帧前面是否丢帧
#endif
}RTP_H264_OBJECT,*PRTP_H264_OBJECT;

typedef struct
{
	BYTE	Type:5;
	BYTE	NRI:2;
	BYTE	F:1;
}NALU_HEAD,*PNALU_HEAD,FU_INDICATOR,*PFU_INDICATOR;

typedef struct
{
	BYTE	Type:5;
	BYTE	R:1;
	BYTE	E:1;
	BYTE	S:1;
}FU_HEAD,*PFU_HEAD;

typedef struct
{
	int		Act;				// 是否动作，1-正在动作
	int		ActUp;				// 弹起的动作编号
	int		ActDown;			// 按下的动作编号
	RECT	rect;				// 矩形框，动作范围
}PTZ_ACTION_RECT,*PPTZ_ACTION_RECT;
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
