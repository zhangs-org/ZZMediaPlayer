#include "zdisplay.h"

ZDisplay::ZDisplay(QObject *parent) :
    QThread(parent)
{
    qDebug()<<"ZDisplay::ZDisplay()";

    imgConvertCtx = NULL;
    displayWidth  = 640;
    displayHeight = 480;
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


            // fixme: should add code for check the frame change

            if(!imgConvertCtx){
                imgConvertCtx = sws_getContext(pFrame->width,pFrame->height,(enum AVPixelFormat)pFrame->format,
                                               displayWidth,displayHeight,AV_PIX_FMT_RGB32,
                                               SWS_BICUBIC,NULL,NULL,NULL);

                //outBuffer = new uint8_t[avpicture_get_size(AV_PIX_FMT_RGB32, pFrame->width,pFrame->height)];
                //avpicture_fill((AVPicture *)pFrameRGB, outBuffer, AV_PIX_FMT_RGB32,  pFrame->width,pFrame->height);
                outBuffer = new uint8_t[avpicture_get_size(AV_PIX_FMT_RGB32, displayWidth,displayHeight)];
                avpicture_fill((AVPicture *)pFrameRGB, outBuffer, AV_PIX_FMT_RGB32, displayWidth,displayHeight);
            }


            sws_scale(imgConvertCtx,(const uint8_t*  const*)pFrame->data, pFrame->linesize,0
                      ,pFrame->height,pFrameRGB->data,pFrameRGB->linesize); // fixme:

            av_frame_unref((AVFrame *)pFrame);

            QImage Img((uchar *)pFrameRGB->data[0], displayWidth,displayHeight, QImage::Format_RGB32);

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

}



