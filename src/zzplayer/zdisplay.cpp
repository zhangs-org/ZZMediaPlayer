#include "zdisplay.h"

ZDisplay::ZDisplay(QObject *parent) :
    QThread(parent)
{
    qDebug()<<"ZDisplay::ZDisplay()";

    convertCtx = NULL;
}

void ZDisplay::run()
{
    while(1){
        //qDebug()<<"ZDisplay::run()";
        QThread::msleep(100);
    }
}


void ZDisplay::handleVideoFrame(void * pFrame)
{
    qDebug()<<"ZDisplay::handleVideoFrame";

    // push to queue
    //av_frame_unref((AVFrame *)pFrame); // for test
}



