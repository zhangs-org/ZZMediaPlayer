#include "timestamp.h"
#include "ui_timestamp.h"



Timestamp::Timestamp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Timestamp)
{
    ui->setupUi(this);

    showTimer = new QTimer(this);
    showTimer->start(500);
    connect(showTimer,SIGNAL(timeout()),this,SLOT(showFlots()));

    // for test
    grid = new QwtPlotGrid();
    grid->attach(ui->qwtPlot);

    int i;

    for(i = 0; i < 16; i++){
        curve[i] = new QwtPlotCurve();
        curve[i]->setTitle( "Some Points" );

        if(i%2)
            curve[i]->setPen( Qt::green, 1 ), curve[i]->setRenderHint( QwtPlotItem::RenderAntialiased, true );
        else
            curve[i]->setPen( Qt::red, 1 ), curve[i]->setRenderHint( QwtPlotItem::RenderAntialiased, true );

        //QwtSymbol *symbol = new QwtSymbol( QwtSymbol::Ellipse, QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 4,4 ) );
        //curve[i]->setSymbol( symbol );

        curve[i]->attach(ui->qwtPlot);

    }




    ui->qwtPlot->show();

}

Timestamp::~Timestamp()
{
    delete ui;
}


/* slots */
void Timestamp::handlePacket(void *pkt)
{
    AVPacket *packet = (AVPacket *)pkt;
    qDebug()<<"Timestamp::handlePacket(),handlePacket, index="<<packet->stream_index << " pts=" << packet->pts << " dts=" << packet->dts;

    tsQueue[0].enqueue(packet->dts);
    if( tsQueue[0].count() > 100){
        tsQueue[0].dequeue();
    }

    qDebug()<<"Timestamp::handlePacket()  tsQueue[0].count()="<<tsQueue[0].count();
}

void Timestamp::handTimestamps(int index, int timebase, qint64 pts, qint64 dts)
{

    qDebug()<<"Timestamp::handTimestamps(), index="<<index << " pts=" << pts << " dts=" << dts;

    //qDebug()<<"Timestamp::handTimestamps(), dts=" << dts;

    if(index >= 0 && index <= 16){
        tsQueue[index].enqueue(dts);
        if( tsQueue[index].count() > 200){
            tsQueue[index].dequeue();
        }
    }



    //qDebug()<<"Timestamp::handTimestamps()  tsQueue[index].count()="<<tsQueue[index].count();
}


void Timestamp::showFlots()
{
    QPolygonF points[16];

    int i;
    int index;
    int count;

    for(index = 0; index < 16; index++){

        count = tsQueue[index].count();

        if(count > 0){
            for(i = 0; i < count; i++){
                points[index] << QPointF(QPoint((int)i, (int)tsQueue[index].at(i)));
            }

            curve[index]->setSamples( points[index] );
        }

    }



    //curve->attach(ui->qwtPlot);
    ui->qwtPlot->replot();

    showTimer->start(500);
}



