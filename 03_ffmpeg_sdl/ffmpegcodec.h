#ifndef FFMPEGCODEC_H
#define FFMPEGCODEC_H

// 自定义
#include "ffmpeg_error.h"

extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}

// c++库函数
#include <fstream>

class FFmpegCodec
{
public:
    FFmpegCodec(){};

    /**
     * @brief FFmpegCodec_Process
     */
    void FFmpegCodec_Process();

    /**
     * @brief FFmpegCodec_SetSaveFile
     * @param outfilename
     */
    FFMPEG_ERROR FFmpegCodec_SetSaveFile(char* outfilename);

    /**
     * @brief FFmpegCodec_Init
     * @return 错误码
     */
    int FFmpegCodec_Init(char *infilename);

    /**
     * @brief FFmpegCodec_GetW
     * @return 返回解码视频流的宽
     */
    int FFmpegCodec_GetW(AVCodecContext &codecCtx) const{return codecCtx.width;}

    /**
     * @brief FFmpegCodec_GetH
     * @return 返回解码视频流的高
     */
    int FFmpegCodec_GetH(AVCodecContext &codecCtx) const{return codecCtx.height;}

private:
    // 文件相关
    std::ifstream       m_infile;
    std::ofstream       m_outfile;

    // 解码相关
    AVFormatContext     *m_pFormatCtx = nullptr;
    int                 m_videoIndex_inForamt;
    AVCodecContext      *m_pCodecCtx = nullptr;
};

#endif // FFMPEGCODEC_H

