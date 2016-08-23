#include "zdisplay.h"

ZDisplay::ZDisplay(QObject *parent) :
    QThread(parent)
{
    qDebug()<<"ZDisplay::ZDisplay()";

    imgConvertCtx = NULL;
}

void ZDisplay::run()
{
    AVFrame * pFrame = NULL;
    AVFrame * pFrameRGB = av_frame_alloc();
    uint8_t * outBuffer;

    while(1){
        //qDebug()<<"ZDisplay::run()";

        if(!frameQueue.isEmpty()){
            pFrame = (AVFrame *)frameQueue.dequeue();
            qDebug()<<"ZDisplay::run(), Get frame width:"
                   <<pFrame->width<<" height:"<<pFrame->height<<" format:"<<pFrame->format;


            if(!imgConvertCtx){
                imgConvertCtx = sws_getContext(pFrame->width,pFrame->height,(enum AVPixelFormat)pFrame->format,
                                               640,360,AV_PIX_FMT_RGB32,
                                               SWS_BICUBIC,NULL,NULL,NULL);

                //outBuffer = new uint8_t[avpicture_get_size(AV_PIX_FMT_RGB32, pFrame->width,pFrame->height)];
                //avpicture_fill((AVPicture *)pFrameRGB, outBuffer, AV_PIX_FMT_RGB32,  pFrame->width,pFrame->height);
                outBuffer = new uint8_t[avpicture_get_size(AV_PIX_FMT_RGB32, 640,360)];
                avpicture_fill((AVPicture *)pFrameRGB, outBuffer, AV_PIX_FMT_RGB32, 640,360);
            }


            sws_scale(imgConvertCtx,(const uint8_t*  const*)pFrame->data, pFrame->linesize,0
                      ,pFrame->height,pFrameRGB->data,pFrameRGB->linesize); // fixme:

            av_frame_unref((AVFrame *)pFrame);

            QImage Img((uchar *)pFrameRGB->data[0], 640,360, QImage::Format_RGB32);

            emit sendPicture(Img.copy());
        }

        QThread::msleep(10);
    }
}


void ZDisplay::handleVideoFrame(void * pFrame)
{
    qDebug()<<"ZDisplay::handleVideoFrame";

    // push to queue
    if(pFrame)
        frameQueue.enqueue(pFrame);
    //av_frame_unref((AVFrame *)pFrame); // for test
}



