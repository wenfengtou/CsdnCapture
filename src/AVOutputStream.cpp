/**
** ���ߣ�pengweizhi
** ���䣺1795387053@qq.com
**/

#include "stdafx.h"
#include "AVOutputStream.h"

CAVOutputStream::CAVOutputStream(void)
{
	m_video_codec_id = AV_CODEC_ID_NONE;
	m_audio_codec_id = AV_CODEC_ID_NONE;
    m_out_buffer = NULL;
	m_width = 320;
	m_height = 240;
	m_framerate = 25;
	m_video_bitrate = 500000;
	m_samplerate = 0;
	m_channels = 1;
	m_audio_bitrate = 32000;
	video_st = NULL;
    audio_st = NULL;
	ofmt_ctx = NULL;
	pCodecCtx = NULL;
	pCodecCtx_a = NULL;
	pCodec = NULL;
	pCodec_a = NULL;
    m_fifo = NULL;
	pFrameYUV = NULL;
	img_convert_ctx = NULL;
	aud_convert_ctx = NULL;
	m_fifo = NULL;
	m_vid_framecnt = 0;
	m_nb_samples = 0;
	m_converted_input_samples = NULL;

	m_output_path = "";

	m_vid_framecnt = 0;
	m_aud_framecnt = 0;
    m_first_vid_time1 = m_first_vid_time2 = -1;
	m_first_aud_time = -1;
	m_next_vid_time = 0;
	m_next_aud_time = 0;

	m_nLastAudioPresentationTime = 0;

	avcodec_register_all();
    av_register_all();
}

CAVOutputStream::~CAVOutputStream(void)
{
}



//��ʼ����Ƶ������
void CAVOutputStream::SetVideoCodecProp(AVCodecID codec_id, int framerate, int bitrate, int gopsize, int width, int height)
{
	 m_video_codec_id = codec_id;
	 m_width = width;
	 m_height = height;
	 m_framerate = ((framerate == 0) ? 10 : framerate);
	 m_video_bitrate = bitrate;
	 m_gopsize = gopsize;
}

//��ʼ����Ƶ������
void CAVOutputStream::SetAudioCodecProp(AVCodecID codec_id, int samplerate, int channels, int bitrate)
{
	m_audio_codec_id = codec_id;
	m_samplerate = samplerate;
	m_channels = channels;
	m_audio_bitrate = bitrate;
}


//�����������ͻ����
bool CAVOutputStream::OpenOutputStream(const char* out_path)
{
	m_output_path = out_path;

	//output initialize
	avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_path);

	if(m_video_codec_id != 0)
	{
		//output video encoder initialize
		pCodec = avcodec_find_encoder(m_video_codec_id);
		if (!pCodec)
		{
			ATLTRACE("Can not find output video encoder! (û���ҵ����ʵı�������)\n");
			return false;
		}
		pCodecCtx = avcodec_alloc_context3(pCodec);
		pCodecCtx->pix_fmt = PIX_FMT_YUV420P;
		pCodecCtx->width = m_width;
		pCodecCtx->height = m_height;
		pCodecCtx->time_base.num = 1;
		pCodecCtx->time_base.den = m_framerate;
		pCodecCtx->bit_rate = m_video_bitrate;
		pCodecCtx->gop_size = m_gopsize;
		/* Some formats want stream headers to be separate. */
		if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
			pCodecCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;


		AVDictionary *param = 0;

		//set H264 codec param
		if(m_video_codec_id == AV_CODEC_ID_H264)
		{
			//pCodecCtx->me_range = 16;
			//pCodecCtx->max_qdiff = 4;
			//pCodecCtx->qcompress = 0.6;
			pCodecCtx->qmin = 10;
			pCodecCtx->qmax = 51;
			//Optional Param
			pCodecCtx->max_b_frames = 0;
			

#if 1
			 //����������������Ӱ�������ʱ����������ã�������Ĭ�ϻỺ��ܶ�֡
			// Set H264 preset and tune
			av_dict_set(&param, "preset", "fast", 0);
			av_dict_set(&param, "tune", "zerolatency", 0);
#else
			 /**
			  * ultrafast,superfast, veryfast, faster, fast, medium
			  * slow, slower, veryslow, placebo.��
			  ע�⣺����x264�����ٶȵ�ѡ� ���øò������Խ��ͱ�����ʱ
			  */
			av_opt_set(pCodecCtx->priv_data,"preset","superfast",0);
#endif

		}

		if (avcodec_open2(pCodecCtx, pCodec, &param) < 0)
		{
			ATLTRACE("Failed to open output video encoder! (��������ʧ�ܣ�)\n");
			return false;
		}

		//Add a new stream to output,should be called by the user before avformat_write_header() for muxing
		video_st = avformat_new_stream(ofmt_ctx, pCodec);
		if (video_st == NULL)
		{
			return false;
		}
		video_st->time_base.num = 1;
		video_st->time_base.den = m_framerate;
		video_st->codec = pCodecCtx;

		//Initialize the buffer to store YUV frames to be encoded.
		pFrameYUV = av_frame_alloc();
		m_out_buffer = (uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
		avpicture_fill((AVPicture *)pFrameYUV, m_out_buffer, PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
	}

	if(m_audio_codec_id != 0)
	{
		//output audio encoder initialize
		pCodec_a = avcodec_find_encoder(m_audio_codec_id);
		if (!pCodec_a)
		{
			ATLTRACE("Can not find output audio encoder! (û���ҵ����ʵı�������)\n");
			return false;
		}
		pCodecCtx_a = avcodec_alloc_context3(pCodec_a);
		pCodecCtx_a->channels = m_channels;
		pCodecCtx_a->channel_layout = av_get_default_channel_layout(m_channels);
		pCodecCtx_a->sample_rate = m_samplerate;
		pCodecCtx_a->sample_fmt = pCodec_a->sample_fmts[0];
		pCodecCtx_a->bit_rate = m_audio_bitrate;
		pCodecCtx_a->time_base.num = 1;
		pCodecCtx_a->time_base.den = pCodecCtx_a->sample_rate;

		if(m_audio_codec_id == AV_CODEC_ID_AAC)
		{
			/** Allow the use of the experimental AAC encoder */
			pCodecCtx_a->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
		}

		/* Some formats want stream headers to be separate. */
		if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
			pCodecCtx_a->flags |= CODEC_FLAG_GLOBAL_HEADER;

		if (avcodec_open2(pCodecCtx_a, pCodec_a, NULL) < 0)
		{
			ATLTRACE("Failed to open ouput audio encoder! (��������ʧ�ܣ�)\n");
			return false;
		}

		//Add a new stream to output,should be called by the user before avformat_write_header() for muxing
		audio_st = avformat_new_stream(ofmt_ctx, pCodec_a);
		if (audio_st == NULL)
		{
			return false;
		}
		audio_st->time_base.num = 1;
		audio_st->time_base.den = pCodecCtx_a->sample_rate;
		audio_st->codec = pCodecCtx_a;

		//Initialize the FIFO buffer to store audio samples to be encoded. 

		m_fifo = av_audio_fifo_alloc(pCodecCtx_a->sample_fmt, pCodecCtx_a->channels, 1);

		//Initialize the buffer to store converted samples to be encoded.
		m_converted_input_samples = NULL;
		/**
		* Allocate as many pointers as there are audio channels.
		* Each pointer will later point to the audio samples of the corresponding
		* channels (although it may be NULL for interleaved formats).
		*/
		if (!(m_converted_input_samples = (uint8_t**)calloc(pCodecCtx_a->channels, sizeof(**m_converted_input_samples)))) 
		{
			ATLTRACE("Could not allocate converted input sample pointers\n");
			return false;
		}
		m_converted_input_samples[0] = NULL;
	}

	//Open output URL,set before avformat_write_header() for muxing
	if (avio_open(&ofmt_ctx->pb, out_path, AVIO_FLAG_READ_WRITE) < 0)
	{
		ATLTRACE("Failed to open output file! (����ļ���ʧ�ܣ�)\n");
		return false;
	}

	//Show some Information
	av_dump_format(ofmt_ctx, 0, out_path, 1);

	//Write File Header
	avformat_write_header(ofmt_ctx, NULL);

	m_vid_framecnt = 0;
	m_aud_framecnt = 0;
    m_nb_samples = 0;
    m_nLastAudioPresentationTime = 0;
    m_next_vid_time = 0;
	m_next_aud_time = 0;
    m_first_vid_time1 = m_first_vid_time2 = -1;
	m_first_aud_time = -1;

	return true;
}

//input_st -- ����������Ϣ
//input_frame -- ������Ƶ֡����Ϣ
//lTimeStamp -- ʱ�����ʱ�䵥λΪ1/1000000
//
int CAVOutputStream::write_video_frame(AVStream * input_st, enum PixelFormat pix_fmt, AVFrame *pframe, INT64 lTimeStamp)
{
	if(video_st == NULL)
	   return -1;

	//ATLTRACE("Video timestamp: %ld \n", lTimeStamp);

   if(m_first_vid_time1 == -1)
   {
	   TRACE("First Video timestamp: %ld \n", lTimeStamp);
	   m_first_vid_time1 = lTimeStamp;
   }

	AVRational time_base_q = { 1, AV_TIME_BASE };

	if(img_convert_ctx == NULL)
	{
		//camera data may has a pix fmt of RGB or sth else,convert it to YUV420
		img_convert_ctx = sws_getContext(m_width, m_height,
			pix_fmt, pCodecCtx->width, pCodecCtx->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	}

    sws_scale(img_convert_ctx, (const uint8_t* const*)pframe->data, pframe->linesize, 0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);
    pFrameYUV->width = pframe->width;
    pFrameYUV->height = pframe->height;
    pFrameYUV->format = PIX_FMT_YUV420P;

    enc_pkt.data = NULL;
    enc_pkt.size = 0;
    av_init_packet(&enc_pkt);

	int ret;
	int enc_got_frame = 0;
    ret = avcodec_encode_video2(pCodecCtx, &enc_pkt, pFrameYUV, &enc_got_frame);

    if (enc_got_frame == 1)
	{
        //printf("Succeed to encode frame: %5d\tsize:%5d\n", framecnt, enc_pkt.size);
       
		if(m_first_vid_time2 == -1)
		{
			m_first_vid_time2 = lTimeStamp;
		}

        enc_pkt.stream_index = video_st->index;						

#if 0
        //Write PTS
		AVRational time_base = video_st->time_base;//{ 1, 1000 };
        AVRational r_framerate1 = input_st->r_frame_rate;//{ 50, 2 }; 
        //Duration between 2 frames (us)
       // int64_t calc_duration = (double)(AV_TIME_BASE)*(1 / av_q2d(r_framerate1));	//�ڲ�ʱ���
		int64_t calc_pts = (double)m_vid_framecnt * (AV_TIME_BASE)*(1 / av_q2d(r_framerate1));

        //Parameters
        enc_pkt.pts = av_rescale_q(calc_pts, time_base_q, time_base);  //enc_pkt.pts = (double)(framecnt*calc_duration)*(double)(av_q2d(time_base_q)) / (double)(av_q2d(time_base));
        enc_pkt.dts = enc_pkt.pts;
        //enc_pkt.duration = av_rescale_q(calc_duration, time_base_q, time_base); //(double)(calc_duration)*(double)(av_q2d(time_base_q)) / (double)(av_q2d(time_base));
        //enc_pkt.pos = -1;
#else
	
		//enc_pkt.pts= av_rescale_q(lTimeStamp, time_base_q, video_st->time_base);
		enc_pkt.pts = (INT64)video_st->time_base.den * lTimeStamp/AV_TIME_BASE;

#endif

        m_vid_framecnt++;

        ////Delay
		//int64_t pts_time = av_rescale_q(enc_pkt.pts, time_base, time_base_q);
		//int64_t now_time = av_gettime() - start_time;						
		//if ((pts_time > now_time) && ((vid_next_pts + pts_time - now_time)<aud_next_pts))
		//	av_usleep(pts_time - now_time);
		
        ret = av_interleaved_write_frame(ofmt_ctx, &enc_pkt);
		if(ret < 0)	
		{
			char tmpErrString[128] = {0};
			ATLTRACE("Could not write video frame, error: %s\n", av_make_error_string(tmpErrString, AV_ERROR_MAX_STRING_SIZE, ret));
			av_packet_unref(&enc_pkt);
			return ret;
		}

        av_free_packet(&enc_pkt);
	}
	else if(ret == 0)
	{
		ATLTRACE("Buffer video frame, timestamp: %I64d.\n", lTimeStamp); //����������֡
	}

	return 0;
}

//input_st -- ����������Ϣ
//input_frame -- ������Ƶ֡����Ϣ
//lTimeStamp -- ʱ�����ʱ�䵥λΪ1/1000000
//
int  CAVOutputStream::write_audio_frame(AVStream *input_st, AVFrame *input_frame, INT64 lTimeStamp)
{
	if(audio_st == NULL)
		return -1;

	if(m_first_aud_time == -1)
	{
		TRACE("First Audio timestamp: %ld \n", lTimeStamp);
		m_first_aud_time = lTimeStamp;
	}

	const int output_frame_size = pCodecCtx_a->frame_size;

	AVRational time_base_q = { 1, AV_TIME_BASE };
	int ret;

	//if((INT64)(av_audio_fifo_size(m_fifo) + input_frame->nb_samples) * AV_TIME_BASE /(INT64)(input_st->codec->sample_rate) - lTimeStamp > AV_TIME_BASE/10)
	//{
	//	TRACE("audio data is overflow \n");
	//	return 0;
	//}

	int nFifoSamples = av_audio_fifo_size(m_fifo);
	INT64 timeshift = (INT64)nFifoSamples * AV_TIME_BASE /(INT64)(input_st->codec->sample_rate); //��ΪFifo����֮ǰδ��������ݣ����Դ�Fifo��������ȡ���ĵ�һ����Ƶ����ʱ������ڵ�ǰʱ��������岿�ֵ�ʱ��


	TRACE("audio time diff: %I64d \n", lTimeStamp - timeshift - m_nLastAudioPresentationTime); //�����ϸò�ֵ�ȶ���һ��ˮƽ�������ֵһֱ�����ĳЩ�ɼ��豸�Ϸ����д����󣩣����������Ƶ��ͬ�������⣬���������ԭ�����
    m_aud_framecnt += input_frame->nb_samples;


	if(aud_convert_ctx == NULL)
	{
		// Initialize the resampler to be able to convert audio sample formats
		aud_convert_ctx = swr_alloc_set_opts(NULL,
			av_get_default_channel_layout(pCodecCtx_a->channels),
			pCodecCtx_a->sample_fmt,
			pCodecCtx_a->sample_rate,
			av_get_default_channel_layout(input_st->codec->channels),
			input_st->codec->sample_fmt,
			input_st->codec->sample_rate,
			0, NULL);

		/**
		* Perform a sanity check so that the number of converted samples is
		* not greater than the number of samples to be converted.
		* If the sample rates differ, this case has to be handled differently
		*/
		ATLASSERT(pCodecCtx_a->sample_rate == input_st->codec->sample_rate);

		swr_init(aud_convert_ctx);
	}

	/**
	* Allocate memory for the samples of all channels in one consecutive
	* block for convenience.
	*/

	if ((ret = av_samples_alloc(m_converted_input_samples, NULL, pCodecCtx_a->channels, input_frame->nb_samples, pCodecCtx_a->sample_fmt, 0)) < 0)
	{
		ATLTRACE("Could not allocate converted input samples\n");
		av_freep(&(*m_converted_input_samples)[0]);
		free(*m_converted_input_samples);
		return ret;
	}
	

	/**
	* Convert the input samples to the desired output sample format.
	* This requires a temporary storage provided by converted_input_samples.
	*/
	/** Convert the samples using the resampler. */
	if ((ret = swr_convert(aud_convert_ctx,
		m_converted_input_samples, input_frame->nb_samples,
		(const uint8_t**)input_frame->extended_data, input_frame->nb_samples)) < 0)
	{
		ATLTRACE("Could not convert input samples\n");
		return ret;
	}

	/** Add the converted input samples to the FIFO buffer for later processing. */
	/**
	* Make the FIFO as large as it needs to be to hold both,
	* the old and the new samples.
	*/
	if ((ret = av_audio_fifo_realloc(m_fifo, av_audio_fifo_size(m_fifo) + input_frame->nb_samples)) < 0)
	{
		ATLTRACE("Could not reallocate FIFO\n");
		return ret;
	}

	/** Store the new samples in the FIFO buffer. */
	if (av_audio_fifo_write(m_fifo, (void **)m_converted_input_samples, input_frame->nb_samples) < input_frame->nb_samples) 
	{
		ATLTRACE("Could not write data to FIFO\n");
		return AVERROR_EXIT;
	}


	INT64 timeinc = (INT64)pCodecCtx_a->frame_size * AV_TIME_BASE /(INT64)(input_st->codec->sample_rate);
    
    //��ǰ֡��ʱ�������С����һ֡��ֵ 
	if(lTimeStamp - timeshift > m_nLastAudioPresentationTime )
	{
		m_nLastAudioPresentationTime = lTimeStamp - timeshift; 
	}
	
	while (av_audio_fifo_size(m_fifo) >= output_frame_size)
		/**
		* Take one frame worth of audio samples from the FIFO buffer,
		* encode it and write it to the output file.
		*/
	{
		/** Temporary storage of the output samples of the frame written to the file. */
		AVFrame *output_frame = av_frame_alloc();
		if (!output_frame)
		{
			ret = AVERROR(ENOMEM);
			return ret;
		}
		/**
		* Use the maximum number of possible samples per frame.
		* If there is less than the maximum possible frame size in the FIFO
		* buffer use this number. Otherwise, use the maximum possible frame size
		*/
		const int frame_size = FFMIN(av_audio_fifo_size(m_fifo), pCodecCtx_a->frame_size);


		/** Initialize temporary storage for one output frame. */
		/**
		* Set the frame's parameters, especially its size and format.
		* av_frame_get_buffer needs this to allocate memory for the
		* audio samples of the frame.
		* Default channel layouts based on the number of channels
		* are assumed for simplicity.
		*/
		output_frame->nb_samples = frame_size;
		output_frame->channel_layout = pCodecCtx_a->channel_layout;
		output_frame->format = pCodecCtx_a->sample_fmt;
		output_frame->sample_rate = pCodecCtx_a->sample_rate;

		/**
		* Allocate the samples of the created frame. This call will make
		* sure that the audio frame can hold as many samples as specified.
		*/
		if ((ret = av_frame_get_buffer(output_frame, 0)) < 0) 
		{
			ATLTRACE("Could not allocate output frame samples\n");
			av_frame_free(&output_frame);
			return ret;
		}

		/**
		* Read as many samples from the FIFO buffer as required to fill the frame.
		* The samples are stored in the frame temporarily.
		*/
		if (av_audio_fifo_read(m_fifo, (void **)output_frame->data, frame_size) < frame_size) 
		{
			ATLTRACE("Could not read data from FIFO\n");
			return AVERROR_EXIT;
		}

		/** Encode one frame worth of audio samples. */
		/** Packet used for temporary storage. */
		AVPacket output_packet;
		av_init_packet(&output_packet);
		output_packet.data = NULL;
		output_packet.size = 0;

		int enc_got_frame_a = 0;

		/**
		* Encode the audio frame and store it in the temporary packet.
		* The output audio stream encoder is used to do this.
		*/
		if ((ret = avcodec_encode_audio2(pCodecCtx_a, &output_packet, output_frame, &enc_got_frame_a)) < 0) 
		{
			ATLTRACE("Could not encode frame\n");
			av_packet_unref(&output_packet);
			return ret;
		}


		/** Write one audio frame from the temporary packet to the output file. */
		if (enc_got_frame_a)
		{
			//output_packet.flags |= AV_PKT_FLAG_KEY;
			output_packet.stream_index = audio_st->index;

#if 0
			AVRational r_framerate1 = { input_st->codec->sample_rate, 1 };// { 44100, 1};
			//int64_t calc_duration = (double)(AV_TIME_BASE)*(1 / av_q2d(r_framerate1));  //�ڲ�ʱ���
			int64_t calc_pts = (double)m_nb_samples * (AV_TIME_BASE)*(1 / av_q2d(r_framerate1));

			output_packet.pts = av_rescale_q(calc_pts, time_base_q, audio_st->time_base);
			//output_packet.dts = output_packet.pts;
			//output_packet.duration = output_frame->nb_samples;
#else
			output_packet.pts = av_rescale_q(m_nLastAudioPresentationTime, time_base_q, audio_st->time_base);

#endif

			//ATLTRACE("audio pts : %ld\n", output_packet.pts);

			//int64_t pts_time = av_rescale_q(output_packet.pts, time_base, time_base_q);
			//int64_t now_time = av_gettime() - start_time;
			//if ((pts_time > now_time) && ((aud_next_pts + pts_time - now_time)<vid_next_pts))
			//	av_usleep(pts_time - now_time);

			if ((ret = av_interleaved_write_frame(ofmt_ctx, &output_packet)) < 0) 
			{
				char tmpErrString[128] = {0};
				ATLTRACE("Could not write audio frame, error: %s\n", av_make_error_string(tmpErrString, AV_ERROR_MAX_STRING_SIZE, ret));
				av_packet_unref(&output_packet);
				return ret;
			}

			av_packet_unref(&output_packet);
		}//if (enc_got_frame_a)


		m_nb_samples += output_frame->nb_samples;

		m_nLastAudioPresentationTime += timeinc;

		av_frame_free(&output_frame);		
	}//while


	return 0;
}

void  CAVOutputStream::CloseOutput()
{
	if(ofmt_ctx != NULL)
	{
		if(video_st != NULL || audio_st != NULL)
		{
		 //Write file trailer
		  av_write_trailer(ofmt_ctx);
		}
	}

    if (video_st)
        avcodec_close(video_st->codec);
    if (audio_st)
        avcodec_close(audio_st->codec);

    if(m_out_buffer)
	{
		av_free(m_out_buffer);
		m_out_buffer = NULL;
	}

	if (m_converted_input_samples) 
	{
		av_freep(&m_converted_input_samples[0]);
		//free(converted_input_samples);
		m_converted_input_samples = NULL;
	}

	if (m_fifo)
	{
		av_audio_fifo_free(m_fifo);
		m_fifo = NULL;
	}
	if(ofmt_ctx)
        avio_close(ofmt_ctx->pb);

    avformat_free_context(ofmt_ctx);

    m_video_codec_id = AV_CODEC_ID_NONE;
	m_audio_codec_id = AV_CODEC_ID_NONE;

	ofmt_ctx = NULL;
	video_st = NULL;
    audio_st = NULL;
}