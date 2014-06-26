#include "STPlotWidget.h"

STPlotWidget::STPlotWidget(QWidget *parent) : MPlotWidget(parent)
{
    // create plot and set up axes.
    MPlot *plot = new MPlot();
    plot->axisBottom()->setAxisName("Time");
    plot->axisLeft()->setAxisName("Storage ring current [mA]");

    setPlot(plot);
}

STPlotWidget::~STPlotWidget()
{

}
