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
    curve = new QwtPlotCurve();

    grid->attach(ui->qwtPlot);


    curve->setTitle( "Some Points" );
    curve->setPen( Qt::blue, 1 ),
    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    //QwtSymbol *symbol = new QwtSymbol( QwtSymbol::Ellipse,
    //    QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 4,4 ) );
    //curve->setSymbol( symbol );

    QPolygonF points;
    points << QPointF( 0.0, 4.4 ) << QPointF( 1.0, 3.0 )
        << QPointF( 2.0, 4.5 ) << QPointF( 3.0, 6.8 )
        << QPointF( 4.0, 7.9 ) << QPointF( 5.0, 7.1 )<< QPointF( 6.0, 7.9 ) << QPointF( 7.0, 7.1 )<< QPointF( 8.0, 7.9 ) << QPointF( 9.0, 7.1 );
    curve->setSamples( points );

    curve->attach(ui->qwtPlot);
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

    tsQueue.enqueue(packet->dts);
    if( tsQueue.count() > 100){
        tsQueue.dequeue();
    }

    qDebug()<<"Timestamp::handlePacket()  tsQueue.count()="<<tsQueue.count();
}

void Timestamp::handTimestamps(int index, int timebase, qint64 pts, qint64 dts)
{

    qDebug()<<"Timestamp::handTimestamps(), index="<<index << " pts=" << pts << " dts=" << dts;

    //qDebug()<<"Timestamp::handTimestamps(), dts=" << dts;

    tsQueue.enqueue(dts);
    if( tsQueue.count() > 20){
        tsQueue.dequeue();
    }

    //qDebug()<<"Timestamp::handTimestamps()  tsQueue.count()="<<tsQueue.count();
}
/*
void Timestamp::handTimestamps(int index, int timebase, int64_t pts, int64_t dts)
{

    qDebug()<<"Timestamp::handTimestamps(), index="<<index << " pts=" << pts << " dts=" << dts;

    tsQueue.enqueue(dts);
    if( tsQueue.count() > 100){
        tsQueue.dequeue();
    }

    qDebug()<<"Timestamp::handTimestamps()  tsQueue.count()="<<tsQueue.count();
}*/

void Timestamp::showFlots()
{
    QPolygonF points;

    int i;
    int count = tsQueue.count();

    for(i = 0; i < count; i++){
        points << QPointF(QPoint((int)i, (int)tsQueue.at(i)));
        //qDebug()<<"Timestamp::showFlots()  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx, points.count()="<< points.count()<<"point("<<i<<","<<tsQueue.at(i)<<")";
    }


    //grid->attach(ui->qwtPlot);

    //qDebug()<<"Timestamp::showFlots()  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx, points.count()="<< points.count();

    curve->setTitle( "Some Points" );
    curve->setPen( Qt::blue, 1 ),
    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    curve->setSamples( points );
    //curve->attach(ui->qwtPlot);
    ui->qwtPlot->replot();

    showTimer->start(500);
}



