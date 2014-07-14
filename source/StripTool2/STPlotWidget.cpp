#include "STPlotWidget.h"

STPlotWidget::STPlotWidget(QWidget *parent) : MPlotWidget(parent)
{    
    // Set up variables.

    plotName_ = "";
    plotNameVisible_ = false;
    timeFilteringEnabled_ = false;

    time_ = new STTime(10, STTime::Seconds, this);

    MPlot *plot = new MPlot();
    setPlot(plot);

    selector_ = new STPlotSelectorTool();
    plot_->addTool(selector_);

    // Get current settings.

    setBottomAxisName("Time [" + time_->unitsToString(time_->units()) + "]");

    // Make connections.

    connect( time_, SIGNAL(unitsChanged(STTime::Units)), this, SLOT(onTimeUnitsChanged(STTime::Units)) );
    connect( selector_, SIGNAL(itemSelected(MPlotItem*)), this, SLOT(onPlotSelectionChanged()) );
    connect( selector_, SIGNAL(deselected()), this, SLOT(onPlotSelectionChanged()) );

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

bool STPlotWidget::timeFilteringEnabled() const
{
    return timeFilteringEnabled_;
}

STTime* STPlotWidget::time() const
{
    return time_;
}

STPlotSelectorTool* STPlotWidget::selector() const
{
    return selector_;
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

void STPlotWidget::enableTimeFiltering(bool enabled)
{
    if (timeFilteringEnabled_ != enabled) {
        timeFilteringEnabled_ = enabled;
        emit timeFilteringEnabled(timeFilteringEnabled_);
    }
}

void STPlotWidget::setLeftAxisName(const QString &newName)
{
    plot()->axisLeft()->setAxisName(newName);
    emit leftAxisNameChanged(newName);
}

void STPlotWidget::setBottomAxisName(const QString &newName)
{
    plot()->axisBottom()->setAxisName(newName);
    emit bottomAxisNameChanged(newName);
}

void STPlotWidget::onTimeUnitsChanged(STTime::Units newUnits)
{
    setBottomAxisName("Time [" + STTime::unitsToString(newUnits) + "]");
}

void STPlotWidget::onPlotSelectionChanged()
{
    emit plotSelectionChanged(selector_->selectedItem());
}
