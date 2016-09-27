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
#include "timestamp.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    ZPlayer *Player;
    QString FileName;
    QTimer *readTimer;
    Timestamp *TimestampDialog;
signals:
    void init_s();
    void play_s();
public slots:
    void getInfo(void *pkt);
    void showPicture(QImage);

private slots:
    void on_play_clicked();
    void OpenFileClicked();
    void ToolTimestamp();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
