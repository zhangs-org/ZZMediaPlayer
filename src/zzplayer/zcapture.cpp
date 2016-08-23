#include "zcapture.h"

ZCapture::ZCapture(QObject *parent) :
    QThread(parent)
{
    qDebug()<<"ZCapture::ZCapture()";

    startFlag = 0;
    memset(streamUrl, 0, sizeof(streamUrl));
    pFormatCtx = NULL;
    bufferSize = 100;
}

void ZCapture::clean()
{
    qDebug()<<"ZCapture::clean()";
    if(pFormatCtx){
        avformat_close_input(&pFormatCtx);
    }

    startFlag = 0;
}

void ZCapture::run()
{
    av_register_all();
    av_log_set_level(AV_LOG_DEBUG);

    AVPacket pkt;
    AVPacket * pPkt = NULL;
    AVPacket * pSendPkt = NULL;
    int ret;
    int i;

    while(1){
        //qDebug()<<"capture run";

        // start new stream
        if(startFlag && strlen(streamUrl) > 0){
            clean();
            qDebug()<<"ZCapture::run(), open stream:" << streamUrl;

            // open input file, and allocate format context
            if (avformat_open_input(&pFormatCtx, streamUrl, NULL, NULL) < 0) {
                qWarning("Could not open source file %s\n", streamUrl);
                clean();
                continue ;
            }

            // get media info
            if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
                qWarning("Could not find stream information\n");
                clean();
                continue ;
            }

            for(i = 0; i < pFormatCtx->nb_streams; i++){
                if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
                    emit sendVideoCtx(pFormatCtx->streams[i]->codec);
                }
            }
            // dump input information to stderr
            av_dump_format(pFormatCtx, 0, streamUrl, 0);

        }



        if(pFormatCtx){

            if(pPkt == NULL){
                pPkt = (AVPacket *)malloc(sizeof(AVPacket));
                if(!pPkt){
                    qWarning("ZCapture::run(), error for malloc.\n");
                    break;
                }
            }

            // initialize packet, set data to NULL, let the demuxer fill it
            av_init_packet(pPkt);
            pPkt->data = NULL;
            pPkt->size = 0;

            ret = av_read_frame(pFormatCtx, pPkt);
            if (ret == AVERROR(EAGAIN)){
                qWarning("av_read_frame ret=%d, EAGAIN means try again.\n", ret);
                continue ;
            }

            if (ret == AVERROR_EOF){
                qWarning("av_read_frame ret=AVERROR_EOF, break.\n");
                clean();
                continue ;
            }

            if (ret < 0){
                qWarning("av_read_frame ret=%d, break.\n", ret);
                clean();
                continue ;
            }

            qDebug()<<"ZCapture::run(),index="<<pPkt->stream_index<<" pts="<<pPkt->pts<< " dts=" << pPkt->dts;

            // push the packet to queue
            packetsQueue.enqueue((void *)pPkt);
            pPkt = NULL;

            // fixme: add if the packet should be sent
            if(packetSouldSend()){
                pSendPkt = (AVPacket *)packetsQueue.dequeue();
                if(pFormatCtx->streams[pSendPkt->stream_index]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
                    emit sendVideoPacket(pSendPkt);
                }else{
                    // free the packet
                    av_packet_unref(pSendPkt);
                    free(pSendPkt);
                }
            }

        }

        if(packetsQueue.size() > bufferSize){
            qDebug()<<"ZCapture,packetsQueue.size()=" << packetsQueue.size() << ", bufferSize=" << bufferSize;
            QThread::msleep(10);
        }
    }
}

int ZCapture::setUrl(char * url)
{
    strcpy(streamUrl, url);
    startFlag = 1;
    qDebug()<<"ZCapture::setUrl, streamUrl" << streamUrl;
    return 0;
}

int ZCapture::packetSouldSend()
{
    AVPacket * pPkt = NULL;

    // pick one packet queue
    if(!packetsQueue.isEmpty()){
        pPkt = (AVPacket *)packetsQueue.at(0);
        qDebug()<<"ZCapture::packetSouldSend(), pPkt dts:" << pPkt->dts;

        // fixme: add code for stream time and system time
        return 1;
    }


    //

    return 0;
}



