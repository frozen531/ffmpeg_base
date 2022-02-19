#include "ffmpeg.h"
#include "ui_ffmpeg.h"

#ifdef _WIN32
//Windows
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
};
#else
//Linux...
#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#ifdef __cplusplus
};
#endif
#endif

ffmpeg::ffmpeg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ffmpeg)
{
    ui->setupUi(this);
    info = (char*)malloc(40000);
}

ffmpeg::~ffmpeg()
{
    free(info);
    delete ui;
}

char *ffmpeg::protocol_info()
{
    memset(info, 0, 40000);
    sprintf(info, "%s\n", "<<URLProtocol>>");

    struct URLProtocol *pup = nullptr;
    //Input
    struct URLProtocol **p_temp = &pup;
    avio_enum_protocols((void **)p_temp, 0);
    while ((*p_temp) != nullptr){
        sprintf(info, "%s[In ][%10s]\n", info, avio_enum_protocols((void **)p_temp, 0));
    }
    pup = nullptr;
    //Output
    avio_enum_protocols((void **)p_temp, 1);
    while ((*p_temp) != nullptr){
        sprintf(info, "%s[Out][%10s]\n", info, avio_enum_protocols((void **)p_temp, 1));
    }

    return info;
}

char *ffmpeg::avformat_info()
{
    memset(info,0,40000);
    sprintf(info, "%s\n", "<<AVFormat>>");

    //av_register_all();

    AVInputFormat *if_temp = av_iformat_next(nullptr);
    AVOutputFormat *of_temp = av_oformat_next(nullptr);
    //Input
    while(if_temp!=nullptr){
        sprintf(info, "%s[In ] %10s\n", info, if_temp->name);
        if_temp=if_temp->next;
    }
    //Output
    while (of_temp != nullptr){
        sprintf(info, "%s[Out] %10s\n", info, of_temp->name);
        of_temp = of_temp->next;
    }
    return info;
}

char *ffmpeg::avcodec_info()
{
    memset(info,0,40000);
    sprintf(info, "%s\n", "<<AVCodec>>");

    //av_register_all();

    AVCodec *c_temp = av_codec_next(nullptr);

    while(c_temp!=nullptr){
        if (c_temp->decode!=nullptr){
            sprintf(info, "%s[Dec]", info);
        }
        else{
            sprintf(info, "%s[Enc]", info);
        }
        switch (c_temp->type){
        case AVMEDIA_TYPE_VIDEO:
            sprintf(info, "%s[Video]", info);
            break;
        case AVMEDIA_TYPE_AUDIO:
            sprintf(info, "%s[Audio]", info);
            break;
        default:
            sprintf(info, "%s[Other]", info);
            break;
        }

        sprintf(info, "%s %10s\n", info, c_temp->name);

        c_temp=c_temp->next;
    }
    return info;
}

char *ffmpeg::avfilter_info()
{
    memset(info,0,40000);
    sprintf(info, "%s\n", "<<AVFilter>>");

    //avfilter_register_all(); // 同av_register_all可被移除或替换

    AVFilter *f_temp = (AVFilter *)avfilter_next(nullptr);

    while (f_temp != nullptr){
        sprintf(info, "%s[%15s]\n", info, f_temp->name);
        f_temp=f_temp->next;
    }
    return info;
}

char *ffmpeg::ffmpeg_configure()
{
    memset(info,0,40000);
    sprintf(info, "%s\n", "<<Configuration>>");

    // 新版本使用全局数组muxer_list[]等替换链表first_oformat等完成解复用器等的注册
    //av_register_all();

    sprintf(info, "%s %s\n", info, avcodec_configuration());

    return info;
}

void ffmpeg::on_btn_clear_clicked()
{
    ui->plainTextEdit->clear();
}

void ffmpeg::on_btn_print_clicked()
{
    ui->plainTextEdit->clear();
    QString context(info);
    ui->plainTextEdit->appendPlainText(context);
}

void ffmpeg::on_rb_configure_clicked()
{
    info = ffmpeg_configure();
}

void ffmpeg::on_rb_protocol_clicked()
{
    info = protocol_info();
}

void ffmpeg::on_rb_avformat_clicked()
{
    info = avformat_info();
}

void ffmpeg::on_rb_avcodec_clicked()
{
    info = avcodec_info();
}

void ffmpeg::on_rb_filter_clicked()
{
    info = avfilter_info();
}

void ffmpeg::on_btn_clear_2_clicked()
{
    close();
}
