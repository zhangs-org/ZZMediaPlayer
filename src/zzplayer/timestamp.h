#ifndef TIMESTAMP_H
#define TIMESTAMP_H

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
};

#endif // TIMESTAMP_H
