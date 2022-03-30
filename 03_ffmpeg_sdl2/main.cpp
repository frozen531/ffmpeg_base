#include "sdldisplay.h"
#include <QApplication>

#undef main

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SdlDisplay w;
    w.show();

    return a.exec();
}
