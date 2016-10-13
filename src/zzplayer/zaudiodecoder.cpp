#include "zaudiodecoder.h"

/* functios */
ZAudioDecoder::ZAudioDecoder(QObject *parent) :
    QThread(parent)
{
    qDebug()<<"ZAudioDecoder::ZAudioDecoder()";

    // init
    pCodecCtx = NULL;
    getNewCodecContext = 0;
}

void ZAudioDecoder::run()
{

    AVPacket * pPkt = NULL;
    AVFrame * pFrame = NULL;
    AVFrame * pFrameOut = NULL;
    int ret;
    int got_frame;
    int decoded;

    while(1){
        //qDebug()<< "ZAudioDecoder::run()";

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
            //qDebug()<< "ZAudioDecoder::run(), get Packet index=" << pPkt->stream_index << " pts=" << pPkt->pts;

            // To decode when decoder has been inited
            if(pCodecCtx){


                if(pFrame == NULL){
                    pFrame = av_frame_alloc();
                    if(!pFrame){
                        qWarning("ZAudioDecoder::run(), error for av_frame_alloc.\n");
                        break;
                    }
                }

                // decode the frame
                got_frame = 0;
                ret = avcodec_decode_audio4(pCodecCtx, pFrame, &got_frame, pPkt);
                //qDebug()<< "ZAudioDecoder::run(), avcodec_decode_video2 return:"<<ret<<" got_frame="<<got_frame;

                /* Some audio decoders decode only part of the packet, and have to be
                 * called again with the remainder of the packet data.
                 * Sample: fate-suite/lossless-audio/luckynight-partial.shn
                 * Also, some decoders might over-read the packet. */

                decoded = ret > pPkt->size ? pPkt->size : ret;

                // send frame
                if(got_frame){
                    // av_frame_clone
                    pFrameOut = av_frame_clone(pFrame);

                    if(pFrameOut)
                        emit sendFrame(pFrameOut);
                    //pFrame = NULL;
                }


            }

            // free the packet
            av_packet_unref(pPkt);
            free(pPkt);
        }else{
            // no data to sleep
            QThread::msleep(10);
        }






    }
}

int ZAudioDecoder::codecContextChange()
{
    if(getNewCodecContext > 0){
        getNewCodecContext = 0;
        return 1;
    }

    return 0;
}

int ZAudioDecoder::resetCodecContext()
{
    int ret;
    qDebug()<<"ZAudioDecoder::resetCodecContext()";

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

    qDebug()<<"ZAudioDecoder::resetCodecContext() ok ";
    return 0;
}


/* slots */
void ZAudioDecoder::handlePacket(void *pkt)
{
    AVPacket *packet = (AVPacket *)pkt;
    //qDebug()<<"ZAudioDecoder::handlePacket(), handlePacket, index="<<packet->stream_index << " pts=" << packet->pts << " dts=" << packet->dts;

    // push to queue
    packetQueue.enqueue(pkt);
}

void ZAudioDecoder::handleCodecContext(void *ctx)
{
    pCodecCtxNew = (AVCodecContext *)ctx;
    getNewCodecContext = 1;
    qDebug()<<"ZAudioDecoder::handleCodecContext()";



}
