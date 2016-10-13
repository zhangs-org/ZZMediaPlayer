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

public slots:
    void getInfo(void *pkt);
    void showPicture(QImage);

private slots:
    void OpenFileClicked();
    void ToolTimestamp();
    void PlayButtonClicked();
    void StopButtonClicked();
    void PauseButtonClicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
