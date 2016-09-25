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

namespace Ui {
class Timestamp;
}

class Timestamp : public QDialog
{
    Q_OBJECT

public:
    explicit Timestamp(QWidget *parent = 0);
    ~Timestamp();

private:
    Ui::Timestamp *ui;

public slots:
    void handlePacket(void *);
};

#endif // TIMESTAMP_H
