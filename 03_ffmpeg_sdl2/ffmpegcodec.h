#ifndef FFMPEGCODEC_H
#define FFMPEGCODEC_H

#include <QThread>

// c++标准库
#include <fstream>
#include <string>

// ffmpeg库
extern "C"
{
    #include "libavformat/avformat.h"
}

class FFmpegCodec : public QThread
{
    Q_OBJECT
public:
    explicit FFmpegCodec();

    /**
     * @brief FFmpegInit
     * @param infilename
     * @return 返回错误码
     * @function 读入文件并初始化解码器
     */
    int FFmpegInit(char* infilename);

    int FFmpegGetVideoW() const {return m_pCodecCtx->width;}
    int FFmpegGetVideoH() const {return m_pCodecCtx->height;}
    const char* FFmpegGetVideoType() const {return m_pCodecCtx->codec->name;}
    char *FFmpegCodec_GetData() {return m_pVideoData;}

    /**
     * @brief FFmpegCodec_SetSaveFile
     * @param outfilename
     * @return 错误码
     */
    int FFmpegCodec_SetSaveFile(char* outfilename);

    /**
     * @brief FFmpegCodec_WriteFile
     * @function 解码并保存文件
     */
    void FFmpegCodec_WriteFile();

    /**
     * @brief FFmpegCodec_Free
     * @function 资源释放
     */
    void FFmpegCodec_Free();

    void run() Q_DECL_OVERRIDE;

signals:
    void emit_display();
    void emit_decode_finish();

public slots:
    void on_emitDecode();

private:
    // 文件相关
    std::ifstream m_inFile;
    std::ofstream m_outFile;

    // 视频信息
    char *m_pVideoData; // 存放一帧解码后的YUV数据

    // 解码相关
    AVFormatContext *m_pFormatCtx;
    AVCodecContext *m_pCodecCtx;
    AVCodecContext *m_pCodecAudioCtx;
    int m_videoIndex_inForamt;

    // 控制
    bool m_codec_pause = false;
    bool m_codec_stop = false;
};

#endif // FFMPEGCODEC_H
