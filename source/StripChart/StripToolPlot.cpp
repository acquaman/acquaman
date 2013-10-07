#include "StripToolPlot.h"

StripToolPlot::StripToolPlot(QObject *parent) : QObject(parent)
{
    selector_ = new MPlotPlotSelectorTool();
    connect( selector_, SIGNAL(itemSelected(MPlotItem*)), this, SLOT(showItemInfo(MPlotItem*)) );

    basePlot_ = new MPlot();
    basePlot_->addTool(selector_);
}



StripToolPlot::~StripToolPlot()
{
}



MPlot* StripToolPlot::plot()
{
    return basePlot_;
}



void StripToolPlot::addItem(const QString &pvName, MPlotVectorSeriesData *pvData)
{
    MPlotSeriesBasic *newItem = new MPlotSeriesBasic();
    newItem->setDescription(" ");
    newItem->setModel(pvData);

    basePlot_->addItem(newItem);
    pvNameToItemMap_[pvName] = newItem;
}



void StripToolPlot::removeItem(const QString &pvName, MPlotItem *removeItem)
{
    //  remove the item from the plot and from any data structures it may be saved in.
    basePlot_->removeItem(removeItem);
    pvNameToItemMap_.remove(pvName);
}



void StripToolPlot::showItemInfo(MPlotItem* plotSelection)
{
    QString axisLeftLabel = "This is broken!";
    QString axisBottomLabel = "This is broke too. :(";

    updatePlotAxesLabels(axisLeftLabel, axisBottomLabel);
    updatePlotAxesScale(plotSelection);

    emit showItemValuesDisplayed(pvNameToItemMap_.key(plotSelection));
}



void StripToolPlot::updatePlotAxesLabels(const QString &axisLeftLabel, const QString &axisBottomLabel)
{
    basePlot_->axisLeft()->setAxisName(axisLeftLabel);
    basePlot_->axisLeft()->showAxisName(true);

    basePlot_->axisBottom()->setAxisName(axisBottomLabel);
    basePlot_->axisBottom()->showAxisName(true);
}



void StripToolPlot::updatePlotAxesScale(MPlotItem* plotSelection)
{
    Q_UNUSED(plotSelection);
}
