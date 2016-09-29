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

    connect(this,SIGNAL(init_s()),Player,SLOT(init()));
    connect(this,SIGNAL(play_s()),Player,SLOT(play()));

    connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(OpenFileClicked()));
    connect(ui->actionTimestamp,SIGNAL(triggered()),this,SLOT(ToolTimestamp()));

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

void MainWindow::on_play_clicked()
{
    emit init_s();
    readTimer->start(40);
    qDebug()<<"readTimer->start ";
}


void MainWindow::showPicture(QImage img)
{
    qDebug()<<"MainWindow::showPicture()";

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

