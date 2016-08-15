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
    zplayer.cpp

HEADERS  += mainwindow.h \
    zplayer.h

FORMS    += mainwindow.ui

LIBS += -L../lib -lavcodec -lavformat -lavfilter -lavutil -lswresample -lswscale -lpostproc

INCLUDEPATH += ../include
DEPENDPATH += ../include
