#include "zplayer.h"

ZPlayer::ZPlayer(QObject *parent) :
    QObject(parent)
{
    qDebug()<<"ZPlayer::ZPlayer()";
}

void ZPlayer::init()
{
    qDebug()<<"ZPlayer::init()";

    BUFFSIZE = 0;

    //avcodec_register_all();
    av_register_all();
    av_log_set_level(AV_LOG_DEBUG);

    qDebug()<<"ZPlayer::av_register_all()";

    pFormatCtx = avformat_alloc_context();
    //pCodecCtx = avcodec_alloc_context3(pCodec);

    qDebug()<<"ZPlayer::FileName:" << FileName;
    if(avformat_open_input(&pFormatCtx,FileName,NULL,NULL)!=0){
        qDebug()<<"OpenFail";
    }
    qDebug()<<"Open ok";

    if(avformat_find_stream_info(pFormatCtx,NULL)<0){
        qDebug()<<"FindFail";
    }
    qDebug()<<"Find ok";

    videoindex = -1;
    for(int i=0;pFormatCtx->nb_streams;i++){
        if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
            videoindex = i;
            break;
        }
    }
    if(videoindex == -1){
        qDebug()<<"Din't find video stream";
    }
    pCodecCtx = pFormatCtx->streams[videoindex]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec == NULL){
        qDebug()<<"codec not find";
    }
    if(avcodec_open2(pCodecCtx,pCodec,NULL)<0){
        qDebug()<<"can't open codec";
    }
    pFrame = av_frame_alloc();
    pFrameRGB = av_frame_alloc();
    play();

}
void ZPlayer::play()
{
    int got_picture;

    av_new_packet(&packet, pCodecCtx->width*pCodecCtx->height);

    uint8_t *out_buffer;

    qDebug()<<"width:" << pCodecCtx->width<< "height:" << pCodecCtx->height;

    out_buffer = new uint8_t[avpicture_get_size(AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height)];//分配AVFrame所需内存
    avpicture_fill((AVPicture *)pFrameRGB, out_buffer, AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height);//填充AVFrame
    avpicture_fill((AVPicture *)pFrame, out_buffer, AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height);//填充AVFrame

    int out_width = 400;
    int out_height = 200;
    SwsContext *convertCtx = NULL;
    while(1){
        QThread::msleep(40);
        if(BUFFSIZE>51) continue;
        if(!(av_read_frame(pFormatCtx,&packet)>=0)) break;
        if(packet.stream_index == videoindex){
            int rec = avcodec_decode_video2(pCodecCtx,pFrame,&got_picture,&packet);
            if(rec>0){
                if(convertCtx == NULL){
                    convertCtx = sws_getContext(pCodecCtx->width,pCodecCtx->height
                                                        ,pCodecCtx->pix_fmt,out_width,out_height
                                                        ,AV_PIX_FMT_RGB32,SWS_BICUBIC,NULL,NULL,NULL);
                }

                sws_scale(convertCtx,(const uint8_t*  const*)pFrame->data,pFrame->linesize,0
                          ,pCodecCtx->height,pFrameRGB->data,pFrameRGB->linesize);
                QImage Img((uchar *)pFrameRGB->data[0],out_width,out_height,QImage::Format_RGB32);
                VideoImg.append(Img.copy());
                BUFFSIZE++;
                sws_freeContext(convertCtx);
            }

            av_free_packet(&packet);
        }
    }

}

