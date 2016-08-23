#include "zplayer.h"

ZPlayer::ZPlayer(QObject *parent) :
    QObject(parent)
{
    qDebug()<<"ZPlayer::ZPlayer()";

    captureThread = new ZCapture;
    videoDecoderThread = new ZVideoDecoder;
    displayThread = new ZDisplay;

    //captureThread->setUrl("D:/video/FCL_1080p.mp4");

    connect(captureThread,SIGNAL(sendPacket(void *)),this,SLOT(getPacket(void *)));
    captureThread->start();

    connect(captureThread,SIGNAL(sendVideoPacket(void *)),videoDecoderThread,SLOT(handleVideoPacket(void *)));
    connect(captureThread,SIGNAL(sendVideoCtx(void *)),videoDecoderThread,SLOT(handleCodecContext(void *)));
    videoDecoderThread->start();

    connect(videoDecoderThread,SIGNAL(sendVideoFrame(void *)),displayThread,SLOT(handleVideoFrame(void *)));
    displayThread->start();

}

void ZPlayer::getPacket(void *pkt)
{
    //qDebug()<<"getPacket";
    AVPacket *packet = (AVPacket *)pkt;
    qDebug()<<"getPacket, index="<<packet->stream_index << " pts=" << packet->pts << " dts=" << packet->dts;

}

int ZPlayer::setUrl(char * url)
{
    captureThread->setUrl(url);
    return 0;
}

void ZPlayer::init()
{
    qDebug()<<"ZPlayer::init()";

}
void ZPlayer::play()
{
    qDebug()<<"ZPlayer::play()";
}

