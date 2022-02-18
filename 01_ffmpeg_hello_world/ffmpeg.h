#ifndef FFMPEG_H
#define FFMPEG_H

#include <QWidget>

namespace Ui {
class ffmpeg;
}

class ffmpeg : public QWidget
{
    Q_OBJECT

public:
    explicit ffmpeg(QWidget *parent = nullptr);
    ~ffmpeg();

private slots:
    /**
     * @brief on_btn_clear_clicked
     * @attention 清空QPlainTextEdit
     */
    void on_btn_clear_clicked();

    /**
     * @brief on_btn_print_clicked
     * @attention 将信息输出到QPlainTextEdit
     */
    void on_btn_print_clicked();

    /**
     * @attention 下面5个为QRadioButton将对应信息写入info中
     */
    void on_rb_protocol_clicked();
    void on_rb_avformat_clicked();
    void on_rb_avcodec_clicked();
    void on_rb_filter_clicked();
    void on_rb_configure_clicked();

    /**
     * @brief on_btn_clear_2_clicked
     * @attention 退出程序
     */
    void on_btn_clear_2_clicked();

private:
    Ui::ffmpeg *ui;
    char *info = nullptr;

    /**
     * @brief protocol_info
     * @return
     * @attention ffmpeg类库支持的协议
     */
    char* protocol_info();

    /**
     * @brief avformat_info
     * @return
     * @attention ffmpeg类库支持的封装格式
     */
    char* avformat_info();

    /**
     * @brief avcodec_info
     * @return
     * @attention ffmpeg类库支持的编解码器
     */
    char* avcodec_info();

    /**
     * @brief avfilter_info
     * @return
     * @attention ffmpeg类库支持的滤镜
     */
    char* avfilter_info();

    /**
     * @brief ffmpeg_configure
     * @return
     * @attention ffmpeg类库的配置信息
     */
    char* ffmpeg_configure();
};

#endif // FFMPEG_H
