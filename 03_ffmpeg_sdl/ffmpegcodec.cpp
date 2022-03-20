#include "ffmpegcodec.h"

// ffmpeg相关
extern "C"
{
#include "libavutil/frame.h"
}

#include <iostream>

int FFmpegCodec::FFmpegCodec_Init(char *infilename)
{
    int ret = 0;

    // 分配一个AVFormatContext
    if(!(m_pFormatCtx = avformat_alloc_context()))
    {
        std::cout << "avformat_alloc_context error" << std::endl;
        return AVFORMAT_ALLOC_CONTEXT_FAILED;
    }

    // 打开avFormat
    ret = avformat_open_input(&m_pFormatCtx, infilename, nullptr, nullptr);
    std::cout <<"avformat_open_input return : " << ret <<std::endl;
    if(ret < 0)
    {
        std::cout << "avformat_open_input error" << std::endl;
        return AVFORMAT_OPEN_INPUT_FAILED;
    }


    // 读取流信息，获取输入流中的视频codec类型
    if(avformat_find_stream_info(m_pFormatCtx, nullptr) < 0)
    {
        std::cout << "avformat_find_stream_info error" << std::endl;
        return AVFORMAT_FIND_STREAM_INFO_FAILED;
    }


    std::cout << "num = " << m_pFormatCtx->nb_streams <<std::endl;
    for(int i=0; i<m_pFormatCtx->nb_streams; i++)
    {
        std::cout << "codec:" << m_pFormatCtx->streams[i]->codec->codec_type << "id:" << m_pFormatCtx->streams[i]->codec->codec_id << std::endl;

        if(m_pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
        {
            m_videoIndex_inForamt = i;
            std::cout << "video" << std::endl;
        }
        if(m_pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO)
        {
            std::cout << "audio" << std::endl;
        }
    }

    m_pCodecCtx = m_pFormatCtx->streams[m_videoIndex_inForamt]->codec;
    AVCodec *pCodec;
    pCodec = avcodec_find_decoder(m_pCodecCtx->codec_id);
    if(!pCodec)
    {
        std::cout << "codec not find" << std::endl;
        return AVFORMAT_NO_VIDEO_STREAM;
    }

    // 打开解码器
    if(avcodec_open2(m_pCodecCtx, pCodec, nullptr) < 0)
    {
        std::cout << "avcodec_open2 failed" << std::endl;
        return AVCODEC_OPEN_FAILED;
    }

    std::cout << "w:" << m_pCodecCtx->width << " h:" <<m_pCodecCtx->height << std::endl;

    return FFMPEG_OK;
}

void FFmpegCodec::FFmpegCodec_Process()
{
    int ret = 0;
    AVPacket    *pPacket;
    AVFrame     *pFrame, *pFrameYUV;
    pPacket = av_packet_alloc();
    if(!pPacket)
    {
        av_log(nullptr, AV_LOG_ERROR, "Can't allocate frame\n");
        return;
    }
    pFrame = av_frame_alloc();
    if (!pFrame) {
        av_log(nullptr, AV_LOG_ERROR, "Cannot allocate packet\n");
        return;
    }

    while(1)
    {
        // 只读取m_videoIndex_inForamt类型对应的数据
        ret = av_read_frame(m_pFormatCtx, pPacket);
        if(ret >= 0)
        {
            if(m_videoIndex_inForamt != pPacket->stream_index)  // 只有读取到的是视频帧时才解码
                continue;
        }
        else
        {
            break;
        }

        // 解码
        ret = avcodec_send_packet(m_pCodecCtx, pPacket);  // 送入一帧进行解码
        if(0 == ret)
        {
            ret = avcodec_receive_frame(m_pCodecCtx, pFrame); // 获取一帧解码数据
            if(0 == ret)
            {
                for(int row = 0; row < pFrame->height; ++row)
                    m_outfile.write((char*)(pFrame->data[0] + pFrame->linesize[0] * row), pFrame->width);
                for(int row = 0; row < pFrame->height / 2; ++row)
                    m_outfile.write((char*)(pFrame->data[1] + pFrame->linesize[1] * row), pFrame->width / 2);
                for(int row = 0; row < pFrame->height / 2; ++row)
                    m_outfile.write((char*)(pFrame->data[2] + pFrame->linesize[2] * row), pFrame->width / 2);

            }
        }
    }

    // flush
    while(1)
    {
        ret = avcodec_send_packet(m_pCodecCtx, nullptr);  // 送入一帧进行解码
        if(0 == ret)
        {
            ret = avcodec_receive_frame(m_pCodecCtx, pFrame); // 获取一帧解码数据
            if(0 == ret)
            {
                for(int row = 0; row < pFrame->height; ++row)
                    m_outfile.write((char*)(pFrame->data[0] + pFrame->linesize[0] * row), pFrame->width);
                for(int row = 0; row < pFrame->height / 2; ++row)
                    m_outfile.write((char*)(pFrame->data[1] + pFrame->linesize[1] * row), pFrame->width / 2);
                for(int row = 0; row < pFrame->height / 2; ++row)
                    m_outfile.write((char*)(pFrame->data[2] + pFrame->linesize[2] * row), pFrame->width / 2);

            }
        }
        else
        {
            break;
        }
    }

    m_infile.close();
    m_outfile.close();

    av_packet_free(&pPacket);
    av_frame_free(&pFrame);
    avformat_close_input(&m_pFormatCtx);
    avcodec_free_context(&m_pCodecCtx);

    return;
}

FFMPEG_ERROR FFmpegCodec::FFmpegCodec_SetSaveFile(char *outfilename)
{
    m_outfile.open(outfilename, std::ios::out | std::ios::binary);
    if(!m_outfile)
    {
        std::cout << "out file:" << outfilename << " open failed!" << std::endl;
        return FILE_OPEN_FAILED;
    }
}
