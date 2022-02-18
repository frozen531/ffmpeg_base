#include "ffmpeg.h"
#include <QApplication>
#include "ffmpeg.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ffmpeg w;
    w.show();

    //w.print_ffmpeg_info();

    return a.exec();
}
