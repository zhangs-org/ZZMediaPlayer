#ifndef TIMESTAMP_H
#define TIMESTAMP_H

extern "C"{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include "libavutil/avutil.h"
    #include "libavutil/mem.h"
    #include "libavutil/fifo.h"
    #include "libswscale/swscale.h"
    #include <libavutil/dict.h>

}

#include <QDialog>
#include <QList>
#include <QQueue>
#include <QTimer>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>



namespace Ui {
class Timestamp;
}

class Timestamp : public QDialog
{
    Q_OBJECT

public:
    explicit Timestamp(QWidget *parent = 0);
    ~Timestamp();

    QTimer *showTimer;

private:
    Ui::Timestamp *ui;

    QwtPlotGrid *grid;
    QwtPlotCurve *curve[64];

    int bufferSize;

    QQueue<int> tsQueue[64]; // timestamp queue
    QQueue<int> dtQueue[64]; // timestamp dt queue

public slots:
    void handlePacket(void *);
    void handTimestamps(int , int , qint64 , qint64 );
    void showFlots();
};

#endif // TIMESTAMP_H
