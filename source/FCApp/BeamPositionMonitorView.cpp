#include "BeamPositionMonitorView.h"

#include <QVBoxLayout>

#include <QDebug>

BeamPositionMonitorView::BeamPositionMonitorView(QWidget *parent)
    : QWidget(parent){


    bpmXY_ = new BeamPositionMonitor("BPM:XRMS", "BPM:YRMS", this);
    bpmDataSource_ = new MPlotVectorSeriesData;

    connect(bpmXY_, SLOT(sendValues(int,int)), this, SLOT(addItemToDataSource(qreal, qreal)));

    setupPlot();

    QVBoxLayout *plotLayout = new QVBoxLayout;
    plotLayout->addWidget(viewWidget_);
    setLayout(plotLayout);



    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateValues()));
    timer->start();

}

void BeamPositionMonitorView::updateValues(){

    int xIndex = bpmXY_->xValueCount();
    int yIndex = bpmXY_->yValueCount();

    qDebug() << xIndex;

    bpmXY_->SendValues(xIndex - 1, yIndex - 1 );

}

void BeamPositionMonitorView::addItemToDataSource(qreal x, qreal y){

    int xIndex = bpmXY_->xValueCount();
    int yIndex = bpmXY_->yValueCount();

    bpmDataSource_->setXValue(xIndex, x);
    bpmDataSource_->setYValue(yIndex, y);


}



void BeamPositionMonitorView::setupPlot(){

    //Create plot window
    viewWidget_ = new MPlotWidget;

    //Create the plot and setup axis
    plot_ = new MPlot;
    plot_->axisBottom()->setAxisNameFont(QFont("Helvetica", 6));
    plot_->axisBottom()->setTickLabelFont(QFont("Helvetica", 6));
    plot_->axisBottom()->setAxisName("X");

    plot_->axisLeft()->setAxisNameFont(QFont("Helvetica", 6));
    plot_->axisLeft()->setTickLabelFont(QFont("Helvetica", 6));
    plot_->axisLeft()->setAxisName("Y");

    plot_->setMarginLeft(10);
    plot_->setMarginBottom(15);
    plot_->setMarginRight(2);
    plot_->setMarginTop(2);

    //Creates a series to be viewed using the XY scatter data source object
    //from MPlotVectorSeriesData.
    scatter_ = new MPlotSeriesBasic;
    scatter_->setModel(bpmDataSource_, true);
    scatter_->setMarker(MPlotMarkerShape::CrossCircle, 14);
    scatter_->setDescription("Beam Position");


    //Add series to plot
    plot_->addItem(scatter_);

    //Put the plot into a plot window, the figures used here will be subject to change as I sort out how I want it to look
    viewWidget_->setPlot(plot_);
    viewWidget_->setMinimumSize(640, 480);
    plot_->axisScaleBottom()->setDataRangeAndDisableAutoScaling(MPlotAxisRange(-0.2, 0.1));
    plot_->axisScaleLeft()->setDataRangeAndDisableAutoScaling(MPlotAxisRange(-0.1, 0.2));
    plot_->axisBottom()->setTicks(4, MPlotAxis::Middle);
    plot_->axisTop()->setTicks(4, MPlotAxis::Middle);
    plot_->axisLeft()->setTicks(4, MPlotAxis::Middle);
    plot_->axisRight()->setTicks(4, MPlotAxis::Middle);


    viewWidget_->show();


}
