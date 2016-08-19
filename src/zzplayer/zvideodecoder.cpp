#include "zvideodecoder.h"


/* functios */
ZVideoDecoder::ZVideoDecoder(QObject *parent) :
    QThread(parent)
{
    qDebug()<<"ZVideoDecoder::ZVideoDecoder()";

    // init
    pCodecCtx = NULL;
    getNewCodecContext = 0;
}

void ZVideoDecoder::run()
{

    while(1){

        // need to set decoder ?
        if(codecContextChange()){
            if(resetCodecContext()){
                QThread::msleep(500);
                continue;
            }
        }


        // get the packet from queue

        // To decode when decoder has been inited

        // free the


        QThread::msleep(500);

    }
}

int ZVideoDecoder::codecContextChange()
{
    if(getNewCodecContext > 0){
        getNewCodecContext = 0;
        return 1;
    }

    return 0;
}

int ZVideoDecoder::resetCodecContext()
{
    int ret;
    qDebug()<<"ZVideoDecoder::resetCodecContext()";

    if(!pCodecCtxNew){
        return -1;
    }
    // free the older ctx

    pCodec = NULL;
    pCodecCtx = NULL;

    // create new ctx
    pCodec = avcodec_find_decoder(pCodecCtxNew->codec_id);
    if(!pCodec){
        qWarning() << "Can't find decoder:" << pCodecCtxNew->codec_id ;
        return -1;
    }

    pCodecCtx = avcodec_alloc_context3(pCodec);
    if(!pCodecCtx){
        qWarning() << "Can't allocate decoder context";
        return -1;
    }

    ret = avcodec_copy_context(pCodecCtx, pCodecCtxNew);
    if (ret) {
        qWarning() << "Can't copy decoder context";
        return ret;
    }

    ret = avcodec_open2(pCodecCtx, pCodec, NULL);
    if (ret < 0) {
        qWarning() << "Can't open decoder";
        return ret;
    }

    qDebug()<<"ZVideoDecoder::resetCodecContext() ok ";
    return 0;
}


/* slots */
void ZVideoDecoder::handleVideoPacket(void *pkt)
{
    AVPacket *packet = (AVPacket *)pkt;
    qDebug()<<"handleVideoPacket, index="<<packet->stream_index << " pts=" << packet->pts << " dts=" << packet->dts;

}

void ZVideoDecoder::handleCodecContext(void *ctx)
{
    pCodecCtxNew = (AVCodecContext *)ctx;
    getNewCodecContext = 1;
    qDebug()<<"ZVideoDecoder::handleCodecContext()";
}







