#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QThread>
#include <QWidget>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QList>
#include <QTimer>
#include <QFile>
#include <QFileDialog>
#include "zplayer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QThread *VideoDecThread;
    ZPlayer *Player;
    QString FileName;
    QTimer *readTimer;
    int BUFFINDEX;

    void on_play_clicked();

signals:
    void init_s();
    void play_s();
public slots:
    void showVideo();
    void getInfo(void *pkt);
    void showPicture(QImage);

private slots:
    void on_VideoPlay_clicked();
    void on_btnOpenFile_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
