#-------------------------------------------------
#
# Project created by QtCreator 2016-08-15T22:42:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = zzplayer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    zplayer.cpp \
    zcapture.cpp

HEADERS  += mainwindow.h \
    zplayer.h \
    zcapture.h

FORMS    += mainwindow.ui

LIBS += -LD:/work/ffmpeg-20160718-450cf40-win32-dev/lib -lavcodec -lavformat -lavfilter -lavutil -lswresample -lswscale -lpostproc

INCLUDEPATH += D:/work/ffmpeg-20160718-450cf40-win32-dev/include
DEPENDPATH += D:/work/ffmpeg-20160718-450cf40-win32-dev/include
