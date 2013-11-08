#include "StripToolPlot.h"

StripToolPlot::StripToolPlot(QWidget *parent) : MPlotWidget(parent)
{
    plotSelection_ = 0;
    model_ = 0;

    selector_ = new MPlotPlotSelectorTool();
    connect( selector_, SIGNAL(itemSelected(MPlotItem*)), this, SLOT(onSeriesSelected(MPlotItem*)) );
    connect( selector_, SIGNAL(deselected()), this, SLOT(onSeriesDeselected()) );

    plot_ = new MPlot();

    plot_->addTool(selector_);

//    plot_->setAcceptDrops(true);

    plot_->axisLeft()->showAxisName(false);
    plot_->axisBottom()->showAxisName(false);

    plot_->axisLeft()->showTickLabels(false);
    plot_->axisBottom()->showTickLabels(false);

    setPlot(plot_);
    enableAntiAliasing(true);

    connect( this, SIGNAL(updatePlotSelection(MPlotItem*)), this, SLOT(toUpdatePlotSelection(MPlotItem*)) );
}



StripToolPlot::~StripToolPlot()
{
}



void StripToolPlot::setModel(StripToolModel *model)
{
    model_ = model;

    connect( this, SIGNAL(seriesSelected(MPlotItem*, bool)), model_, SLOT(toChangeModelSelection(MPlotItem*,bool)) );

//    connect( model_, SIGNAL(setPlotSelection(MPlotItem*)), this, SLOT(toSetPlotSelection(MPlotItem*)) );
    connect( model_, SIGNAL(seriesChanged(Qt::CheckState, MPlotItem*)), this, SLOT(onSeriesChanged(Qt::CheckState, MPlotItem*)) );
    connect( model_, SIGNAL(setPlotAxesLabels(QString,QString)), this, SLOT(setPlotAxesLabels(QString, QString)) );
    connect( model_, SIGNAL(setPlotTicksVisible(bool)), this, SLOT(setTicksVisible(bool)) );
}



bool StripToolPlot::contains(MPlotItem *series)
{
    return plot_->plotItems().contains(series);
}



void StripToolPlot::toSetPlotSelection(MPlotItem *modelSelection)
{
//    if (modelSelection != 0)
//    {
//        foreach(MPlotItem *series, plot_->plotItems())
//        {
//            if (series == modelSelection)
//            {
//                emit updatePlotSelection(series);
//                break;
//            }
//        }
//    }
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



void StripToolPlot::setPlotAxesRanges(const MPlotAxisRange &axisBottom)
{
    plot_->axisScaleBottom()->setDataRange(axisBottom);
}



void StripToolPlot::setTicksVisible(bool isShown)
{
    plot_->axisLeft()->showTickLabels(isShown);
    plot_->axisBottom()->showTickLabels(isShown);
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



void StripToolPlot::toUpdatePlotSelection(MPlotItem *newSelection)
{
    plotSelection_ = newSelection;
    plotSelection_->setSelected();
}


