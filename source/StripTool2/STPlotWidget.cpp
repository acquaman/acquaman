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

QString STPlotWidget::plotName() const
{
    return plotName_;
}

bool STPlotWidget::plotNameVisible() const
{
    return plotNameVisible_;
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
    qDebug() << "Showing plot name : " << (show ? "True" : "False");
    plot_->axisTop()->showAxisName(show);
}
