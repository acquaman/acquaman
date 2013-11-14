#include "StripToolPlot.h"

StripToolPlot::StripToolPlot(QWidget *parent) : MPlotWidget(parent)
{
    model_ = 0;

    selector_ = new StripToolSelector();

    connect( this, SIGNAL(setPlotSelection(MPlotItem*)), selector_, SLOT(setSelection(MPlotItem*)) );

    connect( selector_, SIGNAL(itemSelected(MPlotItem*)), this, SIGNAL(seriesSelected(MPlotItem*)) );
    connect( selector_, SIGNAL(deselected()), this, SIGNAL(seriesDeselected()) );

    plot_ = new MPlot();

    plot_->addTool(selector_);

//    plot_->setAcceptDrops(true);

    plot_->axisLeft()->showAxisName(false);
    plot_->axisBottom()->showAxisName(false);

    plot_->axisLeft()->showTickLabels(false);
    plot_->axisBottom()->showTickLabels(false);

    setPlot(plot_);

    enableAntiAliasing(true);

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
    connect( model_, SIGNAL(setPlotAxesLabels(QString,QString)), this, SLOT(setPlotAxesLabels(QString, QString)) );
    connect( model_, SIGNAL(setPlotTicksVisible(bool)), this, SLOT(setTicksVisible(bool)) );

    connect( model_, SIGNAL(modelSelectionChange()), this, SLOT(onModelSelectionChange()) );
}



void StripToolPlot::createActions()
{
    toggleControls_ = new QAction("Control Panel", this);
    connect( toggleControls_, SIGNAL(triggered()), this, SLOT(toToggleControls()) );
}



bool StripToolPlot::contains(MPlotItem *series)
{
    return plot_->plotItems().contains(series);
}



void StripToolPlot::toAddSeries(const QModelIndex &parent, int rowStart, int rowFinish)
{
    Q_UNUSED(parent);

    int row = rowStart;

    while (row <= rowFinish)
    {
        bool success = addSeriesToPlot(model_->series(row));

        if (success == false)
            qDebug() << "\nError adding series to plot! \n";

        row++;
    }
}



bool StripToolPlot::addSeriesToPlot(MPlotItem *newSeries)
{
    bool success = false;

    if (!contains(newSeries))
    {
        plot_->addItem(newSeries);

        if (contains(newSeries))
            success = true;

    } else {
        qDebug() << "Series already a member of plot. Cannot add duplicate.";
    }

    return success;
}



void StripToolPlot::toRemoveSeries(const QModelIndex &parent, int rowStart, int rowFinish)
{
    Q_UNUSED(parent);

    int row = rowStart;

    while(row <= rowFinish)
    {
        bool success = removeSeriesFromPlot(model_->series(row));

        if (success == false)
            qDebug() << "\nERROR removing series from plot!\n";

        row++;
    }
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



void StripToolPlot::onSeriesChanged(Qt::CheckState newState, int rowChanged)
{
    //  if the series is already plotted and the new state indicates it should be hidden, remove it!
    if (newState == Qt::Unchecked)
        emit removeSeries(QModelIndex(), rowChanged, rowChanged);

    //  if the series is NOT plotted and the new state indicates it should be shown, add it!
    else if (newState == Qt::Checked)
        emit addSeries(QModelIndex(), rowChanged, rowChanged);
}



void StripToolPlot::onModelSelectionChange()
{
    MPlotItem *modelSelection = model_->selectedSeries();

    if (modelSelection && contains(modelSelection))
    {
        if (modelSelection != selector_->selectedItem())
            emit setPlotSelection(modelSelection);

    } else {
        emit setPlotSelection(0);
    }
}



void StripToolPlot::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(toggleControls_);
    menu.exec(event->globalPos());
}



void StripToolPlot::toToggleControls()
{

}


