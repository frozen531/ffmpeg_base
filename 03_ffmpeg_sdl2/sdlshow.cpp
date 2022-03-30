#include "sdlshow.h"

#include <iostream>
#include <fstream>

#include <QDebug>

SdlShow::SdlShow()
{

}

int SdlShow::SdlInit(void * winId)
{
    // 初始化
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        std::cout << "Could not initialize SDL" << SDL_GetError() << std::endl;
        return -1;
    }

    // 窗口创建
    SDL_Window *screen = SDL_CreateWindowFrom(winId);
    if(!screen) {
        std::cout << "SDL: could not create window - exiting: " << SDL_GetError() << std::endl;
        return -1;
    }
    SDL_SetWindowSize(screen, m_screen_w, m_screen_h);

    // 创建渲染器
    m_sdlRenderer = SDL_CreateRenderer(screen, -1, 0);
    if(!m_sdlRenderer)
    {
        std::cout << "SDL: could not create Renderer - exiting: " << SDL_GetError() << std::endl;
        return -1;
    }

    // 创建纹理
    //IYUV: Y + U + V  (3 planes)
    //YV12: Y + V + U  (3 planes)
    m_sdlTexture = SDL_CreateTexture(m_sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, m_screen_w, m_screen_h);
    if(!m_sdlTexture)
    {
        std::cout << "SDL: could not create Texture - exiting: " << SDL_GetError() << std::endl;
        return -1;
    }

    m_sdl_show_stop = 0;
    m_sdl_show_pause = 1;

    return 0;
}

void SdlShow::run()
{
    SDL_Rect sdlRect;
    sdlRect.x = 0;
    sdlRect.y = 0;
    sdlRect.w = m_screen_w;
    sdlRect.h = m_screen_h;

    while(!m_sdl_show_stop)
    {
        if(!m_sdl_show_pause)
        {
            std::cout << "display" << std::endl;


            SDL_UpdateTexture(m_sdlTexture, nullptr, m_data, m_screen_w);
            SDL_RenderClear(m_sdlRenderer);
            SDL_RenderCopy( m_sdlRenderer, m_sdlTexture, nullptr, &sdlRect);
            SDL_RenderPresent(m_sdlRenderer);
            m_sdl_show_pause = true;
        }
    }
    std::cout <<"sdl stop" << std::endl;
    //SDL_Quit();   // 如果不将这个注释掉，再次打开文件，可以正常解码并发送通知sdlshow显示，但是没有画面
    emit_displayFinish();
    std::cout <<"sdl stop2" << std::endl;
}

void SdlShow::test()
{
    // 读取YUV文件
    std::ifstream file;
    file.open("E://00_media_Data//1.yuv", std::ios::out | std::ios::binary);
    if(!file)
    {
        std::cout << "out file: E://00_media_Data// 1.yuv open failed!" << std::endl;
        return;
    }

    const int size = m_screen_h*m_screen_w*1.5;
    char *data;
    data = new char[size];

    SDL_Rect sdlRect;
    sdlRect.x = 0;
    sdlRect.y = 0;
    sdlRect.w = m_screen_w;
    sdlRect.h = m_screen_h;

    int i = 0;
    //SDL End----------------------
    while(file.readsome(data, size)>0)
    {
        std::cout << "index:" <<i++ << std::endl;

        SDL_UpdateTexture(m_sdlTexture, nullptr, data, m_screen_w);
        SDL_RenderClear( m_sdlRenderer );
        SDL_RenderCopy( m_sdlRenderer, m_sdlTexture, nullptr, &sdlRect);
        SDL_RenderPresent( m_sdlRenderer );
        SDL_Delay(40);
    }
    SDL_Quit();
}

void SdlShow::on_emitShow()
{
    std::cout << "on_emitShow" << std::endl;
    m_sdl_show_pause = false;
}

void SdlShow::on_emitDecodeFinish()
{
    std::cout << "on_emitDecodeFinish" << std::endl;
    m_sdl_show_stop = true;
}
