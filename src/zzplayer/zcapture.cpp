#include "zcapture.h"

ZCapture::ZCapture(QObject *parent) :
    QThread(parent)
{
    qDebug()<<"ZCapture::ZCapture()";

    startFlag = 0;
    memset(streamUrl, 0, sizeof(streamUrl));
    pFormatCtx = NULL;
    bufferSize = 500;

    memset(streamStartTime, 0, sizeof(streamStartTime));

    CapTimebase = 1000; // in ms
    CapTimebaseQ = (AVRational){1, 1000};

    capVideoStreamIndex = -1;
    capAudioStreamIndex = -1;
}

void ZCapture::clean()
{
    AVPacket * pPkt = NULL;
    AVPacket * pSendPkt = NULL;

    qDebug()<<"ZCapture::clean()";
    // flush buffer data
    while(!packetsQueue.isEmpty()){
        pSendPkt = (AVPacket *)packetsQueue.dequeue();
        if(pFormatCtx->streams[pSendPkt->stream_index]->codec->codec_type == AVMEDIA_TYPE_VIDEO
                && pSendPkt->stream_index == capVideoStreamIndex){

            emit sendVideoPacket(pSendPkt);
        }else if(pFormatCtx->streams[pSendPkt->stream_index]->codec->codec_type == AVMEDIA_TYPE_AUDIO
                 && pSendPkt->stream_index == capAudioStreamIndex){

            emit sendAudioPacket(pSendPkt);
        }else{
            // free the packet
            av_packet_unref(pSendPkt);
            free(pSendPkt);
        }
    }

    // close stream
    if(pFormatCtx){
        avformat_close_input(&pFormatCtx);
    }

    pFormatCtx = NULL;
    startFlag = 0;
    capVideoStreamIndex = -1;
    capAudioStreamIndex = -1;
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

    AVRational timebase;
    int64_t pts;
    int64_t dts;

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
                if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO && capVideoStreamIndex < 0){
                    capVideoStreamIndex = i;
                    emit sendVideoCtx(pFormatCtx->streams[i]->codec);
                }
                if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO && capAudioStreamIndex < 0){
                    capAudioStreamIndex = i;
                    emit sendAudioCtx(pFormatCtx->streams[i]->codec);
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
            if(packetShouldSend()){
                pSendPkt = (AVPacket *)packetsQueue.dequeue();
                // send timestamps
                pts = av_rescale_q(pSendPkt->pts, pFormatCtx->streams[pSendPkt->stream_index]->codec->pkt_timebase, CapTimebaseQ);
                dts = av_rescale_q(pSendPkt->dts, pFormatCtx->streams[pSendPkt->stream_index]->codec->pkt_timebase, CapTimebaseQ);

                //emit sendTimestamps(pSendPkt->stream_index, CapTimebase, pts, dts);
                emit sendTimestamps(pSendPkt->stream_index, CapTimebase, pts, dts);

                // send packets that need be decoded
                if(pFormatCtx->streams[pSendPkt->stream_index]->codec->codec_type == AVMEDIA_TYPE_VIDEO
                        && pSendPkt->stream_index == capVideoStreamIndex){

                    //emit sendVideoPacket(pSendPkt);
                }else if(pFormatCtx->streams[pSendPkt->stream_index]->codec->codec_type == AVMEDIA_TYPE_AUDIO
                         && pSendPkt->stream_index == capAudioStreamIndex){

                    //emit sendAudioPacket(pSendPkt);
                }else{
                    // free the packet(no need to decode)
                    av_packet_unref(pSendPkt);
                    free(pSendPkt);
                }
                //qDebug()<<"ZCapture,packetsQueue.size()=" << packetsQueue.size()<<" pts="<<pts<<" dts="<<dts;
            }else{
                QThread::msleep(50);
            }
        }else{
            QThread::msleep(50);
        }

        if(packetsQueue.size() > bufferSize){
            QThread::msleep(50);
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

void ZCapture::getCurrentMS(int64_t *retval)
{
    int64_t current_ms = 0ULL;
    struct timeval tp;

    if (gettimeofday(&tp, NULL) != 0) {
        //printf("gettimeofday failed: %s", strerror(errno));
        return;
    }

    current_ms = tp.tv_sec;
    current_ms *= 1000;
    current_ms += tp.tv_usec / 1000;

    *retval = current_ms;
}

int ZCapture::packetShouldSend()
{
    AVPacket * pPkt = NULL;

    int64_t now;
    int64_t dt;

    // pick one packet queue
    if(!packetsQueue.isEmpty()){
        pPkt = (AVPacket *)packetsQueue.at(0);
        //qDebug()<<"ZCapture::packetShouldSend(), stream_index" << pPkt->stream_index <<" pPkt dts:" << pPkt->dts;

        if(streamStartTime[pPkt->stream_index] == 0){
            streamStartTime[pPkt->stream_index] = pPkt->dts;
        }

        // for test
        if(pFormatCtx->streams[pPkt->stream_index]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
            return 1;

        if(captureStartTime <= 0){
            getCurrentMS(&captureStartTime);
        }

        getCurrentMS(&now);
        dt = pPkt->dts - streamStartTime[pPkt->stream_index];

        // trans to ms timebase
        dt = av_rescale_q(dt, pFormatCtx->streams[pPkt->stream_index]->codec->time_base, (AVRational){1, 1000});

        //qDebug()<<"ZCapture::packetShouldSend(), dt:" << dt << " now-startTime=" << (now - captureStartTime);

        if( dt <= (now - captureStartTime + 200) || dt <= 0 ){
            //qDebug()<<"ZCapture::packetShouldSend() true";
            return 1;
        }


        streamLastDTS[pPkt->stream_index] = pPkt->dts;
    }


    //

    return 0;
}



