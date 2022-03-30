#-------------------------------------------------
#
# Project created by QtCreator 2022-03-26T21:39:38
#
#-------------------------------------------------

QT       += core gui testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 03_ffmpeg_sdl2
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        sdldisplay.cpp \
    ffmpegcodec.cpp \
    sdltimer.cpp \
    sdlshow.cpp

HEADERS += \
        sdldisplay.h \
    ffmpegcodec.h \
    sdltimer.h \
    sdlshow.h

FORMS += \
        sdldisplay.ui

INCLUDEPATH += $$PWD/../00_ffmpeg_4_4_1/include
INCLUDEPATH += $$PWD/../00_SDL_2_0_20/include

LIBS += -L$$PWD/../00_ffmpeg_4_4_1/lib -lavcodec -lavformat -lavutil
LIBS += -L$$PWD/../00_SDL_2_0_20/lib -lSDL2

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
