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

#include "zcapture.h"


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
    char *FileName = "D:/video/Christmas.mp4";
    //char *FileName = "D:/Desert.jpg";
    QList<QImage> VideoImg;




    char streamUrl[1024];  // the stream url


    /* funtions */
    int  setUrl(char * url);
    // start
    // restart
    // stop
    // pause


signals:
    void SendImage(QImage img);

public slots:
    void init();
    void play();
    void getPacket(void *pkt);


private:
    char runtimeStreamUrl[1024];  // the runtime stream url
    //QQueue<void *> packetQueue; // the queue for the packets after demux
    //QQueue<void *> frameQueue;  // the queue for the frames after decode

    ZCapture * captureThread;


    /* funtions */

};

#endif // ZPLAYER_H
