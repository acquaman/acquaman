#include "StripToolPlot.h"

StripToolPlot::StripToolPlot(QWidget *parent) : MPlotWidget(parent)
{
    selector_ = new MPlotPlotSelectorTool();
    connect( selector_, SIGNAL(itemSelected(MPlotItem*)), this, SLOT(onSeriesSelected(MPlotItem*)) );
    connect( selector_, SIGNAL(deselected()), this, SLOT(onSeriesDeselected()) );

    plot_ = new MPlot();
    plot_->addTool(selector_);
    plot_->setAcceptDrops(true);
    plot_->axisLeft()->showAxisName(false);
    plot_->axisBottom()->showAxisName(false);

    setPlot(plot_);
}



StripToolPlot::~StripToolPlot()
{
}



bool StripToolPlot::contains(MPlotItem *series)
{
    return plot_->plotItems().contains(series);
}



void StripToolPlot::addSeriesToPlot(MPlotItem *newSeries)
{
    if (contains(newSeries))
    {
        //warn user that this is a duplicate series?
    } else {
        plot_->addItem(newSeries);
    }
}



void StripToolPlot::removeSeriesFromPlot(MPlotItem *toRemove)
{
    if (contains(toRemove))
        plot_->removeItem(toRemove);
}



void StripToolPlot::showPlotAxesLabels(const QString &axisLeftLabel, const QString &axisBottomLabel)
{
    plot_->axisLeft()->setAxisName(axisLeftLabel);
    plot_->axisLeft()->showAxisName(true);

    plot_->axisBottom()->setAxisName(axisBottomLabel);
    plot_->axisBottom()->showAxisName(true);
}



void StripToolPlot::hidePlotAxesLabels()
{
    plot_->axisLeft()->showAxisName(false);
    plot_->axisBottom()->showAxisName(false);
}



void StripToolPlot::onSeriesSelected(MPlotItem *plotSelection)
{
    Q_UNUSED(plotSelection);
}



void StripToolPlot::onSeriesDeselected()
{

}



void StripToolPlot::onSeriesChanged(Qt::CheckState newState, MPlotItem *pvSeries)
{
    //  if the series is already plotted and the new state indicates it should be hidden, remove it!
    if (contains(pvSeries) && newState == Qt::Unchecked)
    {
        removeSeriesFromPlot(pvSeries);

    }

    //  if the series is NOT plotted and the new state indicates it should be shown, add it!
    else if (!contains(pvSeries) && newState == Qt::Checked)
    {
        addSeriesToPlot(pvSeries);
    }
}


