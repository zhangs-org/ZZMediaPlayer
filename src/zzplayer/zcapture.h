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

}
#include <QObject>
#include <QImage>
#include <QThread>
#include <QDebug>
#include <QList>

class ZCapture : public QObject
{
    Q_OBJECT
public:
    explicit ZCapture(QObject *parent = 0);


    char streamUrl[1024];  // the stream url

    /* funtions */
    int  setUrl(char * url);
    // start
    // restart
    // stop
    // pause
    // seek


signals:
    void sendPacket(void * packet);

public slots:
    void init();
    void play();


private:
    char runtimeStreamUrl[1024];  // the runtime stream url
    QQueue<void *> packetQueue; // the queue for the packets after demux
    QQueue<void *> frameQueue;  // the queue for the frames after decode

    /* funtions */

};


#endif // ZCAPTURE_H
