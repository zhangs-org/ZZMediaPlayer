#include "zplayer.h"

ZPlayer::ZPlayer(QObject *parent) :
    QObject(parent)
{
    qDebug()<<"ZPlayer::ZPlayer()";

    captureThread = new ZCapture;
    videoDecoderThread = new ZVideoDecoder;
    audioDecoderThread = new ZAudioDecoder;
    displayThread = new ZDisplay;


    //captureThread->setUrl("D:/video/FCL_1080p.mp4");

    connect(captureThread,SIGNAL(sendPacket(void *)),this,SLOT(getPacket(void *)));
    captureThread->start();

    connect(captureThread,SIGNAL(sendVideoPacket(void *)),videoDecoderThread,SLOT(handlePacket(void *)));
    connect(captureThread,SIGNAL(sendVideoCtx(void *)),videoDecoderThread,SLOT(handleCodecContext(void *)));
    connect(captureThread,SIGNAL(sendAudioPacket(void *)),audioDecoderThread,SLOT(handlePacket(void *)));
    connect(captureThread,SIGNAL(sendAudioCtx(void *)),audioDecoderThread,SLOT(handleCodecContext(void *)));

    videoDecoderThread->start();
    audioDecoderThread->start();

    connect(videoDecoderThread,SIGNAL(sendFrame(void *)),displayThread,SLOT(handleVideoFrame(void *)));
    connect(audioDecoderThread,SIGNAL(sendFrame(void *)),displayThread,SLOT(handleAudioFrame(void *)));
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
    captureThread->play();
}

void ZPlayer::stop()
{
    qDebug()<<"ZPlayer::stop()";
    captureThread->stop();
}

void ZPlayer::pause()
{
    qDebug()<<"ZPlayer::pause()";
    captureThread->pause();
}

