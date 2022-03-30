#ifndef SDLDISPLAY_H
#define SDLDISPLAY_H

#include <QMainWindow>
#include <QLabel>
#include <QIcon>

#include "ffmpegcodec.h"
#include "sdltimer.h"
#include "sdlshow.h"

namespace Ui {
class SdlDisplay;
}

class SdlDisplay : public QMainWindow
{
    Q_OBJECT

public:
    explicit SdlDisplay(QWidget *parent = nullptr);
    ~SdlDisplay();

signals:
    void emit_decode();

private slots:
    /**
     * @brief on_actOpen_triggered
     * @function 打开文件槽函数
     */
    void on_actOpen_triggered();

    /**
     * @brief on_actSave_triggered
     * @function 当需要保存文件时，设置文件名
     */
    void on_actSave_triggered();

    /**
     * @brief on_actPlay_triggered
     * @function 视频播放
     */
    void on_actPlay_triggered();


    void on_emitDisplay();

    void on_emitDisplayFinish();


private:
    // ui界面
    Ui::SdlDisplay *ui;
    QLabel *m_statusLabel;

    SdlTimer m_timer;
    SdlShow  m_sdl;
    FFmpegCodec m_ffmpegCodec;
};

#endif // SDLDISPLAY_H
