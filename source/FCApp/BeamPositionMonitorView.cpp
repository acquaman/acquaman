#include "BeamPositionMonitorView.h"

BeamPositionMonitorView::BeamPositionMonitorView(BeamPositionMonitor *xBPM, BeamPositionMonitor *yBPM, QWidget *parent) :
    QWidget(parent)
{
    connect(xBPM, SIGNAL(rmsChanged(double)), this, SLOT( setXValue(double)));
    connect(yBPM, SIGNAL(rmsChanged(double)), this, SLOT( setYValue(double)));

    MPlotSeriesBasic *seriesX = new MPlotSeriesBasic();
    MPlotSeriesBasic *seriesY = new MPlotSeriesBasic();

    elapsedTime_ = 0;

    plotWindowX_.enableAntiAliasing(true);
    plotWindowX_.setPlot(&plotX_);
    plotX_.axisRight()->setTickPen(QPen(QBrush(QColor(Qt::black)), 0));
    plotX_.axisRight()->showTickLabels(true);
    plotX_.axisRight()->setAxisName("Y RMS");
    plotX_.axisRight()->showAxisName(true);
    plotX_.axisBottom()->setTickPen(QPen(QBrush(QColor(Qt::red)), 0));
    plotX_.axisBottom()->showTickLabels(true);
    plotX_.axisBottom()->setAxisName("time (ms)");
    plotX_.axisBottom()->showAxisName(true);
    plotX_.axisScaleRight()->setAutoScaleEnabled();
    plotX_.axisScaleBottom()->setAutoScaleEnabled();

    plotWindowY_.enableAntiAliasing(true);
    plotWindowY_.setPlot(&plotY_);
    plotY_.axisRight()->setTickPen(QPen(QBrush(QColor(Qt::black)), 0));
    plotY_.axisRight()->showTickLabels(true);
    plotY_.axisRight()->setAxisName("X RMS");
    plotY_.axisRight()->showAxisName(true);
    plotY_.axisBottom()->setTickPen(QPen(QBrush(QColor(Qt::red)), 0));
    plotY_.axisBottom()->showTickLabels(true);
    plotY_.axisBottom()->setAxisName("time (ms)");
    plotY_.axisBottom()->showAxisName(true);
    plotY_.axisScaleRight()->setAutoScaleEnabled();
    plotY_.axisScaleBottom()->setAutoScaleEnabled();

    seriesX->setModel(&dataX);
    seriesY->setModel(&dataY);

   QPen redSkinny(QBrush(QColor(Qt::red)), 1);
    seriesX->setLinePen(redSkinny);

    QPen thickBlue(QBrush(QColor(Qt::blue)), 4);
    seriesY->setLinePen(thickBlue);

    plotX_.addItem(seriesX);
    plotY_.addItem(seriesY);

    plotWindowX_.resize(400, 300);
    plotWindowY_.resize(200, 110);

    QTimer *addTimer = new QTimer(this);
    connect(addTimer, SIGNAL(timeout()), this, SLOT(updatePlot()));
    addTimer->start(100);

    QTimer *delTimer = new QTimer(this);
    connect(delTimer, SIGNAL(timeout()), this, SLOT(removePlotPoint()));
    delTimer->start(5000);
}

void BeamPositionMonitorView::removePlotPoint(){

    dataX.removePointFront();
    dataY.removePointFront();

}

void BeamPositionMonitorView::updatePlot(){

     dataX.insertPointBack(elapsedTime_, xValue_);
     dataY.insertPointBack(elapsedTime_, yValue_);

     elapsedTime_ = elapsedTime_ + 1;
}



void BeamPositionMonitorView::setXValue(double value){

    xValue_ = value;

}


void BeamPositionMonitorView::setYValue(double value){

    yValue_ = value;

}

void BeamPositionMonitorView::showBPM(){
        plotWindowX_.show();
        plotWindowY_.show();

}


