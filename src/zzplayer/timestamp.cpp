#include "timestamp.h"
#include "ui_timestamp.h"

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>

Timestamp::Timestamp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Timestamp)
{
    ui->setupUi(this);

    // for test
    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->attach(ui->qwtPlot);

    QwtPlotCurve *curve = new QwtPlotCurve();
    curve->setTitle( "Some Points" );
    curve->setPen( Qt::blue, 1 ),
    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    QwtSymbol *symbol = new QwtSymbol( QwtSymbol::Ellipse,
        QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 8, 8 ) );
    curve->setSymbol( symbol );

    QPolygonF points;
    points << QPointF( 0.0, 4.4 ) << QPointF( 1.0, 3.0 )
        << QPointF( 2.0, 4.5 ) << QPointF( 3.0, 6.8 )
        << QPointF( 4.0, 7.9 ) << QPointF( 5.0, 7.1 );
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

}

