#ifndef SDLSHOW_H
#define SDLSHOW_H

#include <QThread>

extern "C"
{
#include "SDL.h"
}

class SdlShow : public QThread
{
    Q_OBJECT
public:
    explicit SdlShow();

    void SdlSetW(int w) {m_screen_w = w;}
    void SdlSetH(int h) {m_screen_h = h;}
    void SdlSetData(char *data) {m_data = data;}

    int SdlInit(void* winId);

    void run() Q_DECL_OVERRIDE;

    void test();
signals:
    void emit_displayFinish();
public slots:
    void on_emitShow();
    void on_emitDecodeFinish();
private:
    // sdl相关
    int m_screen_w;
    int m_screen_h;
    SDL_Renderer  *m_sdlRenderer;    // 渲染器
    SDL_Texture   *m_sdlTexture;      // 纹理
    char *m_data;

    int m_sdl_show_stop = false;
    int m_sdl_show_pause = false;
};

#endif // SDLSHOW_H
