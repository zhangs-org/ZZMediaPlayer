#ifndef ZVIDEODECODER_H
#define ZVIDEODECODER_H

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
#include <QQueue>


class ZVideoDecoder : public QThread
{
    Q_OBJECT
public:
    explicit ZVideoDecoder(QObject *parent = 0);

protected:
    void run();

signals:
    void sendVideoFrame(void *);

public slots:
    void handleVideoPacket(void *);
    void handleCodecContext(void *);

private:
    AVCodecContext *pCodecCtxNew;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    int getNewCodecContext;
    QQueue<void *> packetQueue; // the queue for the packets after demux

    int codecContextChange();
    int resetCodecContext();
    int decode();
};

#endif // ZVIDEODECODER_H
