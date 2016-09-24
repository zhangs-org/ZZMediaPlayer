#-------------------------------------------------
#
# Project created by QtCreator 2016-08-15T22:42:47
#
#-------------------------------------------------

QT      += core gui multimedia


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = zzplayer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    zplayer.cpp \
    zcapture.cpp \
    zvideodecoder.cpp \
    zdisplay.cpp \
    zaudiodecoder.cpp \
    timestamp.cpp

HEADERS  += mainwindow.h \
    zplayer.h \
    zcapture.h \
    zvideodecoder.h \
    zdisplay.h \
    zaudiodecoder.h \
    timestamp.h

FORMS    += mainwindow.ui \
    timestamp.ui


LIBS += -LD:/work/ffmpeg-20160718-450cf40-win32-dev/lib -lavcodec -lavformat -lavfilter -lavutil -lswresample -lswscale -lpostproc

INCLUDEPATH += D:/work/ffmpeg-20160718-450cf40-win32-dev/include
DEPENDPATH += D:/work/ffmpeg-20160718-450cf40-win32-dev/include

#qwt设置
#include(D:/Qt/qtctrl/qwt-6.1.3/qwtfunctions.pri )
#DEFINES *= QWT_DLL
#CONFIG += qwt
#LIBS += -LD:/Qt/Qt5.7.0/5.7/mingw53_32/lib -lqwt
#INCLUDEPATH += D:/Qt/Qt5.7.0/5.7/mingw53_32/include/qwt
#DEPENDPATH += D:/Qt/qtctrl/qwt-6.1.3/src
#qwtAddLibrary(qwt)

#end qwt


#qwt设置
QWT_BUILD = D:/Qt/qtctrl/build-qwt-Desktop_Qt_5_7_0_MinGW_32bit-Debug
QWT_ROOT = D:/Qt/qtctrl/qwt-6.1.3
QWT_CONFIG  += QwtDll
include( $${QWT_ROOT}/qwtfunctions.pri )
INCLUDEPATH += $${QWT_ROOT}/src
DEPENDPATH  += $${QWT_ROOT}/src
LIBS      += -L$${QWT_BUILD}/lib -lqwt

#qwtAddLibrary(qwt)

#contains(QWT_CONFIG, QwtOpenGL ) {
#    QT += opengl
#}
#else {
#    DEFINES += QWT_NO_OPENGL
#}
#contains(QWT_CONFIG, QwtSvg) {
#    QT += svg
#}
#else {
#    DEFINES += QWT_NO_SVG
#}
#win32 {
#    contains(QWT_CONFIG, QwtDll) {
#        DEFINES    += QT_DLL QWT_DLL
#    }
#}
#end qwt set

