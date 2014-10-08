#include "BeamPositionMonitorView.h"

#include <QVBoxLayout>

#include <QDebug>

BeamPositionMonitorView::BeamPositionMonitorView(QWidget *parent)
    : QWidget(parent){

    bpmXY_ = new BeamPositionMonitor("BPM:XRMS", "BPM:YRMS", this);
    bpmDataSource_ = new MPlotVectorSeriesData;

    setupPlot();

    QVBoxLayout *plotLayout = new QVBoxLayout;
    plotLayout->addWidget(viewWidget_);
    setLayout(plotLayout);

 connect(bpmXY_, SIGNAL(newValuesFromPV()), this, SLOT(updateVectorsFromModel()));

}
void BeamPositionMonitorView::updateVectorsFromModel(){

    tempX = bpmXY_->xList();
    tempY = bpmXY_->yList();

    xVector_ = tempX.toVector();
    yVector_ = tempY.toVector();

    bpmDataSource_->setValues(xVector_, yVector_);

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
    viewWidget_->setMinimumSize(320, 320);


    plot_->axisScaleBottom()->setAutoScaleEnabled();
    plot_->axisScaleLeft()->setAutoScaleEnabled();

   // plot_->axisScaleBottom()->setDataRangeAndDisableAutoScaling(MPlotAxisRange(-0.2, 0.1));
   // plot_->axisScaleLeft()->setDataRangeAndDisableAutoScaling(MPlotAxisRange(-0.1, 0.2));
    plot_->axisBottom()->setTicks(4, MPlotAxis::Middle);
    plot_->axisTop()->setTicks(4, MPlotAxis::Middle);
    plot_->axisLeft()->setTicks(4, MPlotAxis::Middle);
    plot_->axisRight()->setTicks(4, MPlotAxis::Middle);


    viewWidget_->show();


}
