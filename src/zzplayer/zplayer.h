#ifndef ZPLAYER_H
#define ZPLAYER_H

extern "C"{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include "libavutil/avutil.h"
    #include "libavutil/mem.h"
    #include "libavutil/fifo.h"
    #include "libswscale/swscale.h"
    #include <libavutil/dict.h>

}
#include <QObject>
#include <QImage>
#include <QThread>
#include <QDebug>
#include <QList>


class ZPlayer : public QObject
{
    Q_OBJECT
public:
    explicit ZPlayer(QObject *parent = 0);
    struct ImgPacket{
        QImage PImage;
        struct ImgPacket *next;
    };
    int videoindex;
    int BUFFSIZE;
    AVFormatContext *pFormatCtx;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVPacket packet;
    AVFrame *pFrame,*pFrameRGB;
    char *FileName = "D:/video/trans.mp4";
    QList<QImage> VideoImg;


signals:
    void SendImage(QImage img);

public slots:
    void init();
    void play();

};

#endif // ZPLAYER_H
