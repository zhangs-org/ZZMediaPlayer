#ifndef ZCAPTURE_H
#define ZCAPTURE_H

extern "C"{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include "libavutil/avutil.h"
    #include "libavutil/mem.h"
    #include "libavutil/fifo.h"
    #include "libswscale/swscale.h"
    #include <libavutil/dict.h>
    #include <sys/time.h>
}
#include <QObject>
#include <QImage>
#include <QThread>
#include <QDebug>
#include <QList>
#include <QQueue>

class ZCapture : public QThread
{
    Q_OBJECT
public:
    explicit ZCapture(QObject *parent = 0);

    int bufferSize; // number of packets


    /* funtions */
    int  setUrl(char * url);
    // start
    // restart
    // stop
    // pause
    // seek

protected:
    void run();

signals:
    void sendPacket(void * packet);
    void sendVideoPacket(void * packet);
    void sendAudioPacket(void * packet);
    void sendVideoCtx(void *);
    void sendAudioCtx(void *);

private:
    int startFlag;
    char streamUrl[1024];  // the stream url
    AVFormatContext *pFormatCtx;
    QQueue<void *> packetsQueue;
    int64_t captureStartTime;
    int64_t streamStartTime[64];
    int64_t streamLastDTS[64];
    int CapTimebase;
    AVRational CapTimebaseQ;

    int capVideoStreamIndex; // default use first video stream
    int capAudioStreamIndex; // default use first audio stream

    /* funtions */
    void clean(); // clean the object members
    int  packetShouldSend(); // check the packet should be sent
    void getCurrentMS(int64_t *retval);

};


#endif // ZCAPTURE_H
