#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    VideoDecThread = new QThread(this);
    Player = new ZPlayer;
    readTimer = new QTimer(this);
    BUFFINDEX = 0;
    Player->moveToThread(VideoDecThread);
    qDebug()<<"moveToThread";

    VideoDecThread->start();
    qDebug()<<"moveToThread start ";
    connect(this,SIGNAL(init_s()),Player,SLOT(init()));
    connect(this,SIGNAL(play_s()),Player,SLOT(play()));
    //connect(OVideoDec,SIGNAL(SendImage(QImage)),this,SLOT(showVideo(QImage)));
    //connect(OVideoDec,SIGNAL(SendImage(QImage)),this,SLOT(showVideo(QImage)));
    connect(readTimer,SIGNAL(timeout()),this,SLOT(showVideo())); // by me
    qDebug()<<"MainWindow::MainWindow() over ";

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_play_clicked()
{
    emit init_s();
    readTimer->start(40);
    qDebug()<<"readTimer->start ";
}

void MainWindow::on_VideoPlay_clicked()
{
    emit init_s();
    readTimer->start(40);
    qDebug()<<"readTimer->start ";
}

void MainWindow::showVideo()
{
    qDebug()<<"MainWindow::showVideo()";
#if 1
    QPixmap pix;
    if(Player->VideoImg.isEmpty()) return;

    qDebug()<<"MainWindow::showVideo, VideoImg.count=" << Player->VideoImg.count() << "BUFFINDEX=" << BUFFINDEX;

    if(Player->VideoImg.count() > 0){

        pix = pix.fromImage(Player->VideoImg.at(0));//.scaledToWidth(ui->VideoShow->width());
        ui->videoShow->setPixmap(pix);
        Player->VideoImg.removeAt(0);

    }
#endif
}

void MainWindow::on_btnOpenFile_clicked()
{
#if 0
    FileName = QFileDialog::getOpenFileName(this,"Open Video","","video file(*.mp4 *.rmvb)");
    QByteArray byte = FileName.toLocal8Bit();
    OVideoDec->FileName = byte.data();
#endif
}

