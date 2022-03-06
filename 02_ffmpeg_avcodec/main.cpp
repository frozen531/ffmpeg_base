#include "ffmpeg_avcodec.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ffmpeg_avcodec w;
    w.show();

    return a.exec();
}
