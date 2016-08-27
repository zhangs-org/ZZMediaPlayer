#ifndef ZDISPLAY_H
#define ZDISPLAY_H

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

#include <QAudioOutput>
#include <QByteArray>


class ZDisplay : public QThread
{
    Q_OBJECT
public:
    explicit ZDisplay(QObject *parent = 0);
    int displayWidth;
    int displayHeight;


protected:
    void run();

signals:
    void sendPicture(QImage);
    void sendVoice(void *);

public slots:
    void handleVideoFrame(void *);
    void handleAudioFrame(void *);


private:
    QQueue<void *> videoFrameQueue;
    QQueue<void *> audioFrameQueue;

    QList<void *> videoFrameList;
    QList<void *> audioFrameList;
    SwsContext *imgConvertCtx; // for video

    QAudioOutput* audioOutput; // for audio
    QIODevice * streamOut; // for audio
    QByteArray tempBuffer;

    int setAudioOutput();
    void sendAudio(AVFrame * pFrame);
};

#endif // ZDISPLAY_H
