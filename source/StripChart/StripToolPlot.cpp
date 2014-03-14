#include "StripToolPlot.h"

StripToolPlot::StripToolPlot(QWidget *parent) : MPlotWidget(parent)
{
    selector_ = 0;
    plot_ = 0;

    buildComponents();
    makeConnections();
    defaultSettings();

    qDebug() << "StripToolPlot object created.";
}



StripToolPlot::~StripToolPlot()
{
}



bool StripToolPlot::contains(MPlotItem *series)
{
    return plot_->plotItems().contains(series);
}



MPlotItem* StripToolPlot::selectedItem() const {
    return selector_->selectedItem();
}



bool StripToolPlot::addPlotItem(MPlotItem *newSeries)
{    
    if (newSeries == 0) {
        qDebug() << "StripToolPlot :: cannot add a series with an empty pointer! No changes made.";
        return false;
    }

    if (contains(newSeries)) {
        qDebug() << "StripToolPlot :: cannot add duplicate series to plot. No changes made.";
        return false;
    }

    int oldItemNum = plot_->plotItems().size();
    plot()->addItem(newSeries);
    int newItemNum = plot_->plotItems().size();

    if (contains(newSeries) && newItemNum == oldItemNum + 1) {
        qDebug() << "StripToolPlot :: added new series to plot successfully.";
        return true;

    } else {
        qDebug() << "StripToolPlot :: adding new series to plot failed.";
        return false;
    }

}



bool StripToolPlot::removePlotItem(MPlotItem *toRemove)
{
    if (toRemove == 0) {
        qDebug() << "StripToolPlot :: cannot remove a series with an empty pointer! No changes made.";
        return false;
    }

    if (!contains(toRemove)) {
        qDebug() << "StripToolPlot :: cannot remove a series that the plot doesn't contain! No changes made.";
        return false;
    }

    int oldItemNum = plot_->plotItems().size();
    plot()->removeItem(toRemove);
    int newItemNum = plot_->plotItems().size();

    if (!contains(toRemove) && newItemNum == oldItemNum - 1) {
        return true;

    } else {
        qDebug() << "StripToolPlot :: removing series failed.";
        return false;
    }

}



void StripToolPlot::setSelectedItem(MPlotItem *newSelection) {
    selector_->setSelection(newSelection);
}



void StripToolPlot::setLeftAxisName(const QString &newName) {
    plot_->axisLeft()->setAxisName(newName);
    plot_->axisLeft()->showAxisName(true);
}



void StripToolPlot::setLeftAxisRange(const MPlotAxisRange *newAxisRange) {
    qDebug() << "StripToolPlot :: Updating left axis range...";
    qreal rangeMin = newAxisRange->min();
    qreal rangeMax = newAxisRange->max();

    plot_->axisScaleLeft()->setDataRange(newAxisRange->normalized(), true);

    int itemCount = plot_->plotItems().size();

    for (int i = 0; i < itemCount; i++) {
        StripToolSeries* series = qgraphicsitem_cast<StripToolSeries*>(plot_->plotItems().at(i));
        series->enableWaterfall(waterfallOn_, i, itemCount);
        series->enableYNormalization(true, rangeMin, rangeMax);
    }
}



void StripToolPlot::setBottomAxisName(const QString &newName) {
    plot_->axisBottom()->setAxisName(newName);
}



void StripToolPlot::setWaterfall(bool waterfallOn)
{
    waterfallOn_ = waterfallOn;

    if (waterfallOn) {
        qDebug() << "StripToolPlot :: waterfall feature is turning on:";
    } else {
        qDebug() << "StripToolPlot :: waterfall feature is turning off:";
    }

    int count = plot_->plotItems().size();

    for (int i = 0; i < count; i++) {
        StripToolSeries* series = qgraphicsitem_cast<StripToolSeries*>(plot_->plotItems().at(i));
        series->enableWaterfall(waterfallOn_, i, count);
    }
}



//void StripToolPlot::toUpdateLeftAxisRange(MPlotAxisRange *newAxisRange)
//{
//    qDebug() << "Updating left axis range...";
//    qreal rangeMin = newAxisRange->min();
//    qreal rangeMax = newAxisRange->max();

//    plot_->axisScaleLeft()->setDataRange(newAxisRange->normalized(), true);

////    int itemCount = plot_->plotItems().size();

////    for (int i = 0; i < itemCount; i++) {
////        StripToolSeries* series = qgraphicsitem_cast<StripToolSeries*>(plot_->plotItems().at(i));
////        series->enableWaterfall(waterfallOn_, i, itemCount);
////        series->enableYNormalization(true, rangeMin, rangeMax);
////    }
//}




void StripToolPlot::onSelectionChange()
{
    if (selector_->selectedItem() == 0) {
        plot_->axisLeft()->showTickLabels(false);
        plot_->axisLeft()->showAxisName(false);

    } else {
        plot_->axisLeft()->showTickLabels(true);
        plot_->axisLeft()->showAxisName(true);
    }

    emit selectionChanged(selector_->selectedItem());
}



void StripToolPlot::buildComponents()
{
    MPlotDragZoomerTool *dragZoomer = new MPlotDragZoomerTool();
    selector_ = new StripToolSelector();
    plot_ = new MPlot();

    plot_->addTool(dragZoomer);
    plot_->addTool(selector_);

    setPlot(plot_);
}



void StripToolPlot::makeConnections()
{
    connect( selector_, SIGNAL(itemSelected(MPlotItem*)), this, SLOT(onSelectionChange()) );
}



void StripToolPlot::defaultSettings()
{
    waterfallOn_ = false;

    plot_->axisBottom()->setAxisName("Time");
    plot_->axisBottom()->showAxisName(true);
    plot_->axisBottom()->showTickLabels(true);

    plot_->axisLeft()->showAxisName(false);
    plot_->axisLeft()->showTickLabels(false);

    plot_->enableAxisNormalization(MPlot::Left, false);
    plot_->axisScaleLeft()->setAutoScaleEnabled(false);
}


