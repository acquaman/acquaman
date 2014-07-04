#include "STPlotWidget.h"

STPlotWidget::STPlotWidget(QWidget *parent) : MPlotWidget(parent)
{    
    plotName_ = "Unnamed plot";
    plotNameVisible_ = false;

    // create plot and set up axes.
    MPlot *plot = new MPlot();
    plot->axisBottom()->setAxisName("Time");
    plot->axisLeft()->setAxisName("Storage ring current [mA]");

    setPlot(plot);
}

STPlotWidget::~STPlotWidget()
{

}

void STPlotWidget::setPlotName(const QString &newName)
{
    if (plotName_ != newName) {
        plotName_ = newName;
        plot_->axisTop()->setAxisName(plotName_);
        showPlotName(plotNameVisible_);

        emit plotNameChanged(plotName_);
    }
}

void STPlotWidget::showPlotName(bool show)
{
    plot_->axisTop()->showAxisName(show);
}
