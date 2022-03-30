#-------------------------------------------------
#
# Project created by QtCreator 2022-02-18T21:05:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 01_ffmpeg_hello_world
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

INCLUDEPATH += $$PWD/../00_ffmpeg_4_4_1/include

LIBS += -L$$PWD/../00_ffmpeg_4_4_1/lib -lavcodec \
        -L$$PWD/../00_ffmpeg_4_4_1/lib -lavfilter \
        -L$$PWD/../00_ffmpeg_4_4_1/lib -lavformat \
        -L$$PWD/../00_ffmpeg_4_4_1/lib -lpostproc \
#        -L$$PWD/"ffmpeg_4_4_1/lib" -lavdevice \
#        -L$$PWD/"ffmpeg_4_4_1/lib" -lavutil \
#        -L$$PWD/"ffmpeg_4_4_1/lib" -lswresample \
#        -L$$PWD/"ffmpeg_4_4_1/lib" -lswscale \

SOURCES += \
        main.cpp \
        ffmpeg.cpp

HEADERS += \
        ffmpeg.h

FORMS += \
        ffmpeg.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
