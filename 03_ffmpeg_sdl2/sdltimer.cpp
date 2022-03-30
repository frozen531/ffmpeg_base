#include "sdltimer.h"

extern "C"
{
#include "SDL.h"
}

#include <iostream>

SdlTimer::SdlTimer()
{

}

void SdlTimer::run()
{
    while (!m_time_stop)
    {
        std::cout << "sdlTimer emit decode" <<std::endl;
        emit_decode();
        SDL_Delay(40);
    }
    std::cout <<"timer stop" << std::endl;
}
