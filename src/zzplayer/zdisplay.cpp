#include "zdisplay.h"

ZDisplay::ZDisplay(QObject *parent) :
    QThread(parent)
{
    qDebug()<<"ZDisplay::ZDisplay()";

    imgConvertCtx = NULL;
    displayWidth  = 640;
    displayHeight = 480;

    setAudioOutput(); // fixme: just for test, need delete
}

void ZDisplay::run()
{
    AVFrame * pFrame = NULL;
    AVFrame * pFrameRGB = av_frame_alloc();
    uint8_t * outBuffer;

    while(1){
        //qDebug()<<"ZDisplay::run()";

        if(!videoFrameQueue.isEmpty()){
            pFrame = (AVFrame *)videoFrameQueue.dequeue();
            //qDebug()<<"ZDisplay::run(), Get frame width:"
            //       <<pFrame->width<<" height:"<<pFrame->height<<" format:"<<pFrame->format;


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

        // audio


        QThread::msleep(10);
    }
}

int ZDisplay::setAudioOutput()
{
    QAudioFormat audioFormat;


    audioFormat.setSampleRate(44100);
    audioFormat.setChannelCount(2);
    audioFormat.setSampleSize(32);
    audioFormat.setCodec("audio/pcm");

    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setSampleType(QAudioFormat::Float);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultOutputDevice();
    if (!info.isFormatSupported(audioFormat)) {
        qDebug()<<"default format not supported try to use nearest";
        audioFormat = info.nearestFormat(audioFormat);
    }
    audioOutput = new QAudioOutput(audioFormat, this);
    streamOut = audioOutput->start();
}

void ZDisplay::sendAudio(AVFrame * pFrame)
{
    int unpadded_linesize = pFrame->nb_samples * av_get_bytes_per_sample((enum AVSampleFormat)pFrame->format);

    // fixme: the sample should be resample

    //qDebug()<<"sendAudio, unpadded_linesize="<<unpadded_linesize<<" format="<<pFrame->format
    //       <<" linesize[0]=" << pFrame->linesize[0] << " linesize[1]=" << pFrame->linesize[1];

    tempBuffer.append((const char *)pFrame->extended_data[0],unpadded_linesize);

    if(audioOutput&&audioOutput->state()!=QAudio::StoppedState&&
            audioOutput->state()!=QAudio::SuspendedState)
    {
        int chunks = audioOutput->bytesFree()/audioOutput->periodSize();

        //qDebug()<<"sendAudio, chunks="<<chunks << " bytesFree="<<audioOutput->bytesFree() << " periodSize="<<audioOutput->periodSize();
        streamOut->write(tempBuffer);
        tempBuffer.clear();

        while (0)
        {
            if (tempBuffer.length() >= audioOutput->periodSize())
            {
                //写入到扬声器
                streamOut->write(tempBuffer.data(),audioOutput->periodSize());
                tempBuffer = tempBuffer.mid(audioOutput->periodSize());
            }
            else
            {
                break;
                //写入到扬声器
                streamOut->write(tempBuffer);
                tempBuffer.clear();
                break;
            }
            --chunks;
        }
    }


}

void ZDisplay::handleVideoFrame(void * pFrame)
{
    //qDebug()<<"ZDisplay::handleVideoFrame";

    // push to queue
    if(pFrame)
        videoFrameQueue.enqueue(pFrame);



}

void ZDisplay::handleAudioFrame(void * pFrame)
{
    //qDebug()<<"ZDisplay::handleAudioFrame";

    // push to queue
    if(pFrame)
        audioFrameQueue.enqueue(pFrame);
    sendAudio((AVFrame *)pFrame); // fixme: just for test, should be delete
}


