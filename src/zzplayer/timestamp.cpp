#include "timestamp.h"
#include "ui_timestamp.h"

Timestamp::Timestamp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Timestamp)
{
    ui->setupUi(this);
}

Timestamp::~Timestamp()
{
    delete ui;
}
