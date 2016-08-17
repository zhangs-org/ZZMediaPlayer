#include "zplayer.h"

ZPlayer::ZPlayer(QObject *parent) :
    QObject(parent)
{
    qDebug()<<"ZPlayer::ZPlayer()";
}

int ZPlayer::setUrl(char * url)
{
    strcpy(streamUrl, url);
    return 0;
}

void ZPlayer::init()
{
    qDebug()<<"ZPlayer::init()";

    BUFFSIZE = 0;

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
    uint8_t *out_buffer2;

    qDebug()<<"width:" << pCodecCtx->width<< "height:" << pCodecCtx->height;
    int out_width =  pCodecCtx->width;
    int out_height = pCodecCtx->height;

    out_buffer = new uint8_t[avpicture_get_size(AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height)];//分配AVFrame所需内存
    out_buffer2 = new uint8_t[avpicture_get_size(AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height)];
    avpicture_fill((AVPicture *)pFrameRGB, out_buffer, AV_PIX_FMT_RGB32, out_width, out_height);//填充AVFrame
    avpicture_fill((AVPicture *)pFrame, out_buffer2, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);//填充AVFrame


    SwsContext *convertCtx = NULL;
    while(1){
        QThread::msleep(25);
        BUFFSIZE = VideoImg.count();
        if(BUFFSIZE>100){
            qDebug()<<"BUFFSIZE(" << BUFFSIZE << ")>100";
            continue;
        }
        if(!(av_read_frame(pFormatCtx,&packet)>=0)){
            qDebug()<<"av_read_frame error";
            break;
        }

        if(packet.stream_index == videoindex){
            got_picture = 0;
            int rec = avcodec_decode_video2(pCodecCtx,pFrame,&got_picture,&packet);

            qDebug()<<"avcodec_decode_video2 rec=" << rec << "got_picture=" << got_picture;

            if(rec > 0 && got_picture>0){

#if 1
                if(convertCtx == NULL){
                    convertCtx = sws_getContext(pCodecCtx->width,pCodecCtx->height,pCodecCtx->pix_fmt,
                                                out_width, out_height,AV_PIX_FMT_RGB32,
                                                SWS_BICUBIC,NULL,NULL,NULL);
                }

                sws_scale(convertCtx,(const uint8_t*  const*)pFrame->data,pFrame->linesize,0
                          ,pCodecCtx->height,pFrameRGB->data,pFrameRGB->linesize);
                QImage Img((uchar *)pFrameRGB->data[0],out_width, out_height,QImage::Format_RGB32);
                VideoImg.append(Img.copy());

#endif
            }

            av_free_packet(&packet);
        }
    }
    sws_freeContext(convertCtx);

}

