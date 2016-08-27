#ifndef ZAUDIODECODER_H
#define ZAUDIODECODER_H

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


class ZAudioDecoder : public QThread
{
    Q_OBJECT
public:
    explicit ZAudioDecoder(QObject *parent = 0);

protected:
    void run();

signals:
    void sendFrame(void *);

public slots:
    void handlePacket(void *);
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

#endif // ZAUDIODECODER_H
