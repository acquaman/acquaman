#include "StripToolPlot.h"

StripToolPlot::StripToolPlot(QWidget *parent) : MPlotWidget(parent)
{
    model_ = 0;

    MPlotDragZoomerTool *dragZoomer = new MPlotDragZoomerTool();
    selector_ = new StripToolSelector();

    waterfallOn_ = false;

    connect( this, SIGNAL(itemSelected(MPlotItem*)), selector_, SLOT(setSelection(MPlotItem*)) );

    connect( selector_, SIGNAL(itemSelected(MPlotItem*)), this, SIGNAL(seriesSelected(MPlotItem*)) );
    connect( selector_, SIGNAL(deselected()), this, SIGNAL(seriesDeselected()) );

    plot_ = new MPlot();

    plot_->addTool(dragZoomer);
    plot_->addTool(selector_);

    plot_->axisBottom()->setAxisName("Time");
    plot_->axisBottom()->showAxisName(true);
    plot_->axisBottom()->showTickLabels(true);

    plot_->axisLeft()->showAxisName(false);
    plot_->axisLeft()->showTickLabels(false);

    setPlot(plot_);

    plot_->enableAxisNormalization(MPlot::Left, false);
    plot_->axisScaleLeft()->setAutoScaleEnabled(false);

    connect( this, SIGNAL(removeSeries(QModelIndex, int, int)), this, SLOT(toRemoveSeries(QModelIndex,int,int)) );
    connect( this, SIGNAL(addSeries(QModelIndex, int, int)), this, SLOT(toAddSeries(QModelIndex,int,int)) );

}



StripToolPlot::~StripToolPlot()
{
}



void StripToolPlot::setModel(StripToolModel *model)
{
    model_ = model;

    connect( this, SIGNAL(seriesSelected(MPlotItem*)), model_, SLOT(seriesSelected(MPlotItem*)) );
    connect( this, SIGNAL(seriesDeselected()), model_, SLOT(seriesDeselected()) );

    connect( model_, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(toAddSeries(QModelIndex, int, int)) );
    connect( model_, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), this, SLOT(toRemoveSeries(QModelIndex, int, int)) );
    connect( model_, SIGNAL(seriesChanged(Qt::CheckState, int)), this, SLOT(onSeriesChanged(Qt::CheckState, int)) );
    connect( model_, SIGNAL(updateXAxisLabel(QString)), this, SLOT(toUpdateXAxisLabel(QString)) );
    connect( model_, SIGNAL(modelSelectionChange()), this, SLOT(onModelSelectionChange()) );

    connect( model_, SIGNAL(selectedPVDisplayRangeChanged(MPlotAxisRange*)), this, SLOT(toUpdateLeftAxisRange(MPlotAxisRange*)) );
    connect( model_, SIGNAL(selectedPVAxisLabelChanged(QString)), this, SLOT(toUpdateYAxisLabel(QString)) );
    connect( model_, SIGNAL(waterfallStateChanged(bool)), this, SLOT(onWaterfallStateChange(bool)) );
}



bool StripToolPlot::contains(MPlotItem *series)
{
    return plot_->plotItems().contains(series);
}



void StripToolPlot::toAddSeries(const QModelIndex &parent, int rowStart, int rowFinish)
{
    Q_UNUSED(parent);

    // we expect rowStart == rowFinish, only adding one series to the plot at a time!
    if (rowStart != rowFinish) {
        qDebug() << "Cannot add more than one series at a time!";
        return;
    }

    qDebug() << "Successfully added series : " << addSeriesToPlot(model_->series(rowStart));
}



bool StripToolPlot::addSeriesToPlot(MPlotItem *newSeries)
{
    int oldItemNum = plot_->plotItems().size();

    if (!contains(newSeries))
    {
        plot_->addItem(newSeries);
        int newItemNum = plot_->plotItems().size();

        if (contains(newSeries) && newItemNum == oldItemNum + 1) {
            return true;
        }
    }

    return false;
}



void StripToolPlot::toRemoveSeries(const QModelIndex &parent, int rowStart, int rowFinish)
{
    Q_UNUSED(parent);

    if (rowStart != rowFinish) {
        qDebug() << "Cannot delete more than one series at a time!";
        return;
    }

    qDebug() << "Successfully removing series from plot :" << removeSeriesFromPlot(model_->series(rowStart));
}



bool StripToolPlot::removeSeriesFromPlot(MPlotItem *toRemove)
{
    bool success = false;

    if (contains(toRemove))
    {
        plot_->removeItem(toRemove);

        if (!contains(toRemove))
            success = true;

    } else {
        qDebug() << "Series not found -- cannot remove.";
    }

    return success;
}



void StripToolPlot::toUpdateXAxisLabel(const QString &newLabel)
{
    qDebug() << "StripToolPlot :: Plot axis label changed to" << newLabel;
    plot_->axisBottom()->setAxisName(newLabel);
}



void StripToolPlot::onSeriesChanged(Qt::CheckState newState, int rowChanged)
{
    //  if the series is already plotted and the new state indicates it should be hidden, remove it!
    if (newState == Qt::Unchecked)
        emit removeSeries(QModelIndex(), rowChanged, rowChanged);

    //  if the series is NOT plotted and the new state indicates it should be shown, add it!
    else if (newState == Qt::Checked)
        emit addSeries(QModelIndex(), rowChanged, rowChanged);
}



void StripToolPlot::toUpdateLeftAxisRange(MPlotAxisRange *newDataRange)
{    
    qreal rangeMin = newDataRange->min();
    qreal rangeMax = newDataRange->max();

    MPlotAxisScale *axis = plot_->axisScaleLeft();
    axis->setDataRange(newDataRange->normalized(), true);

    int itemCount = plot_->plotItems().size();

    for (int i = 0; i < itemCount; i++) {
        StripToolSeries* series = qgraphicsitem_cast<StripToolSeries*>(plot_->plotItems().at(i));
        series->enableWaterfall(waterfallOn_, i, itemCount);
        series->enableYNormalization(true, rangeMin, rangeMax);
    }

}



void StripToolPlot::toUpdateYAxisLabel(const QString &newLabel)
{
    plot_->axisLeft()->setAxisName(newLabel);
    plot_->axisLeft()->showAxisName(true);
}



void StripToolPlot::onModelSelectionChange()
{
    MPlotItem *modelSelection = model_->selectedSeries();

    if (modelSelection && contains(modelSelection))
    {
        qDebug() << "StripToolPlot :: Setting plot selection...";
        plot_->axisLeft()->showTickLabels(true);

        emit itemSelected(modelSelection); // let selector_ tool know about the selection change.

    } else if (!modelSelection) {
        qDebug() << "Plot item deselected.";
        plot_->axisLeft()->showAxisName(false);
        plot_->axisLeft()->showTickLabels(false);
        emit itemSelected(0);

    } else {
        qDebug() << "StripToolPlot :: Attempting to select plot item that doesn't exist! No change made.";
    }
}



void StripToolPlot::onWaterfallStateChange(bool waterfallOn)
{
    waterfallOn_ = waterfallOn;

    if (waterfallOn) {
        qDebug() << "StripToolPlot :: waterfall feature is turning on:";
    } else {
        qDebug() << "StripToolPlot :: waterfall feature is turning off:";
    }
}


