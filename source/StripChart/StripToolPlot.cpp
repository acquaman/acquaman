#include "StripToolPlot.h"

StripToolPlot::StripToolPlot(QWidget *parent) : MPlotWidget(parent)
{
    selector_ = new MPlotPlotSelectorTool();
    connect( selector_, SIGNAL(itemSelected(MPlotItem*)), this, SLOT(onSeriesSelected(MPlotItem*)) );
    connect( selector_, SIGNAL(deselected()), this, SLOT(onSeriesDeselected()) );

    plot_ = new MPlot();
    plot_->addTool(selector_);
//    plot_->setAcceptDrops(true);
    plot_->axisLeft()->showAxisName(false);
    plot_->axisBottom()->showAxisName(false);

//    plot_->axisLeft()->setEnabled(false);
//    plot_->axisLeft()->hide();
//    plot_->axisLeft()->setVisible(false);
//    plot_->axisScaleLeft()->set;

    setPlot(plot_);
    enableAntiAliasing(true);
}



StripToolPlot::~StripToolPlot()
{
}



bool StripToolPlot::contains(MPlotItem *series)
{
    return plot_->plotItems().contains(series);
}



void StripToolPlot::setSeriesSelection(MPlotItem *modelSelection)
{
    if (contains(modelSelection))
    {

    }
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



void StripToolPlot::setPlotAxesLabels(const QString &bottomLabel, const QString &leftLabel)
{
    plot_->axisBottom()->setAxisName(bottomLabel);
    plot_->axisBottom()->showAxisName(true);

    plot_->axisLeft()->setAxisName(leftLabel);
    plot_->axisLeft()->showAxisName(true);
}



void StripToolPlot::onSeriesSelected(MPlotItem *plotSelection)
{
    plotSelection_ = plotSelection;
    emit seriesSelected(plotSelection_, true);
}



void StripToolPlot::onSeriesDeselected()
{
    emit seriesSelected(plotSelection_, false);
    plotSelection_ = 0;
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


