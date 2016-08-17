#include "zcapture.h"

ZCapture::ZCapture(QObject *parent) :
    QThread(parent)
{
    qDebug()<<"ZCapture::ZCapture()";

    startFlag = 0;
    memset(streamUrl, 0, sizeof(streamUrl));
    pFormatCtx = NULL;

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
    int ret;
    while(1){
        qDebug()<<"capture run";

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

            //startFlag = 0; // after open stream set flag
        }

        if(pFormatCtx){
            av_init_packet(&pkt);
            pkt.data = NULL;
            pkt.size = 0;

            ret = av_read_frame(pFormatCtx, &pkt);
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

            //printf("index=%d, pts=%lld, dts=%lld.\n", pkt.stream_index, pkt.pts, pkt.dts);
            qDebug()<<"index="<<pkt.stream_index<<" pts="<<pkt.pts<< " dts=" << pkt.dts;

            sendPacket(&pkt);
        }

        QThread::msleep(500);
    }
}

int ZCapture::setUrl(char * url)
{
    strcpy(streamUrl, url);
    startFlag = 1;
    qDebug()<<"ZCapture::setUrl, streamUrl" << streamUrl;
    return 0;
}
