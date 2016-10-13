#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "timestamp.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Player = new ZPlayer;
    readTimer = new QTimer(this);
    TimestampDialog = new Timestamp;

    connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(OpenFileClicked()));
    connect(ui->actionTimestamp,SIGNAL(triggered()),this,SLOT(ToolTimestamp()));
    connect(ui->playButton, SIGNAL(clicked()),this,SLOT(PlayButtonClicked()));
    connect(ui->stopButton, SIGNAL(clicked()),this,SLOT(StopButtonClicked()));
    connect(ui->pauseButton, SIGNAL(clicked()),this,SLOT(PauseButtonClicked()));


    connect(Player->captureThread,SIGNAL(sendPacket(void *)), this,SLOT(getInfo(void *)));
    connect(Player->displayThread,SIGNAL(sendPicture(QImage)), this,SLOT(showPicture(QImage)));

    //connect(Player->captureThread,SIGNAL(sendVideoPacket(void *)), TimestampDialog,SLOT(handlePacket(void *)));
    connect(Player->captureThread,SIGNAL(sendTimestamps(int, int,qint64, qint64)),
            TimestampDialog,SLOT(handTimestamps(int, int,qint64, qint64)));

    qDebug()<<"MainWindow::MainWindow() over ";

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getInfo(void *pkt)
{
    //qDebug()<<"MainWindow::getInfo ";
    AVPacket *packet = (AVPacket *)pkt;
    qDebug()<<"getInfo, index="<<packet->stream_index << " pts=" << packet->pts << " dts=" << packet->dts;
}




void MainWindow::showPicture(QImage img)
{
    QPixmap pix;
    pix = pix.fromImage(img);
    ui->videoShow->setPixmap(pix);
}

void MainWindow::OpenFileClicked()
{

    FileName = QFileDialog::getOpenFileName(this,"Open Video","","video file(*.mp4 *.rmvb)");
    QByteArray byte = FileName.toLocal8Bit();
    qDebug()<< "MainWindow::OpenFileClicked()" << byte.data();

    Player->setUrl(byte.data());

}

void MainWindow::ToolTimestamp()
{
    TimestampDialog->show();
}

void MainWindow::PlayButtonClicked()
{
    //qDebug()<< "MainWindow::PlayButtonClicked()";
    Player->play();
}

void MainWindow::StopButtonClicked()
{
    //qDebug()<< "MainWindow::StopButtonClicked()";
    Player->stop();
}

void MainWindow::PauseButtonClicked()
{
    //qDebug()<< "MainWindow::PauseButtonClicked()";
    Player->pause();
}






