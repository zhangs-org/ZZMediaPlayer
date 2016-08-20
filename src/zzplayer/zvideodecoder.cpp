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

    AVPacket * pPkt = NULL;
    AVFrame * pFrame = NULL;
    int ret;
    int got_frame;

    while(1){
        //qDebug()<< "ZVideoDecoder::run()";

        // need to set decoder ?
        if(codecContextChange()){
            if(resetCodecContext()){
                QThread::msleep(500);
                continue;
            }
        }


        // get the packet from queue
        if(!packetQueue.isEmpty()){
            pPkt = (AVPacket *)packetQueue.dequeue();
            qDebug()<< "ZVideoDecoder::run(), get Packet index=" << pPkt->stream_index << " pts=" << pPkt->pts;

            // To decode when decoder has been inited
            if(pCodecCtx){


                if(pFrame == NULL){
                    pFrame = av_frame_alloc();
                    if(!pFrame){
                        qWarning("ZVideoDecoder::run(), error for av_frame_alloc.\n");
                        break;
                    }
                }

                // decode the frame
                got_frame = 0;
                ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_frame, pPkt);
                qDebug()<< "ZVideoDecoder::run(), avcodec_decode_video2 return:"<<ret<<" got_frame="<<got_frame;

                // send frame
                if(got_frame){
                    // malloc for frame
                    emit sendVideoFrame(pFrame);
                    //pFrame = NULL;
                }


            }

            // free the packet
            av_packet_unref(pPkt);
            free(pPkt);
        }




        QThread::msleep(100);

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

    // push to queue
    packetQueue.enqueue(pkt);
}

void ZVideoDecoder::handleCodecContext(void *ctx)
{
    pCodecCtxNew = (AVCodecContext *)ctx;
    getNewCodecContext = 1;
    qDebug()<<"ZVideoDecoder::handleCodecContext()";



}







