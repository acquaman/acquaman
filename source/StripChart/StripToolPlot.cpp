#include "StripToolPlot.h"

StripToolPlot::StripToolPlot(QObject *parent) : QObject(parent)
{
    selector_ = new MPlotPlotSelectorTool();
    connect( selector_, SIGNAL(itemSelected(MPlotItem*)), this, SLOT(showItemInfo(MPlotItem*)) );

    basePlot_ = new MPlot();
    basePlot_->addTool(selector_);
    basePlot_->setAcceptDrops(true);
}



StripToolPlot::~StripToolPlot()
{
}



MPlot* StripToolPlot::plot()
{
    return basePlot_;
}



QList<QString> StripToolPlot::getActivePVList()
{
    return pvNameToSeriesMap_.keys();
}



bool StripToolPlot::contains(const QString &pvName)
{
    return pvNameToSeriesMap_.contains(pvName);
}



void StripToolPlot::addSeries(const QString &pvName, const QString &pvUnits, MPlotVectorSeriesData* pvData)
{
    if (!contains(pvName))
    {
        StripToolSeries *newSeries = new StripToolSeries();
        newSeries->setPVName(pvName);
        newSeries->setPVUnits(pvUnits);
        newSeries->setDescription(" ");
        newSeries->setModel(pvData);

        basePlot_->addItem(newSeries);
        pvNameToSeriesMap_[pvName] = newSeries;
    }
}



void StripToolPlot::showSeries(const QString &pvName)
{
    Q_UNUSED(pvName);
}



void StripToolPlot::hideSeries(const QString &pvName)
{
    Q_UNUSED(pvName);
}



void StripToolPlot::deleteSeries(const QString &pvName)
{
    if (contains(pvName))
    {
        MPlotItem *toDelete = pvNameToSeriesMap_[pvName];
        basePlot_->removeItem(toDelete);
        delete toDelete;

        pvNameToSeriesMap_.remove(pvName);
    }
}



void StripToolPlot::showItemInfo(MPlotItem* plotSelection)
{
    //  use the plot selection provided to determine the properties to display.

    StripToolSeries *seriesSelection = (StripToolSeries*) plotSelection;

    QString axisLeftLabel = seriesSelection->pvUnits();
    QString axisBottomLabel = "Update number";

    updatePlotAxesLabels(axisLeftLabel, axisBottomLabel);
    updatePlotAxesScale(seriesSelection);
}



void StripToolPlot::updatePlotAxesLabels(const QString &axisLeftLabel, const QString &axisBottomLabel)
{
    basePlot_->axisLeft()->setAxisName(axisLeftLabel);
    basePlot_->axisLeft()->showAxisName(true);

    basePlot_->axisBottom()->setAxisName(axisBottomLabel);
    basePlot_->axisBottom()->showAxisName(true);
}



void StripToolPlot::updatePlotAxesScale(StripToolSeries* plotSelection)
{
    Q_UNUSED(plotSelection);
}



void StripToolPlot::toShowCheckedPV(const QString &pvName)
{
    showSeries(pvName);
}



void StripToolPlot::toRemoveUncheckedPV(const QString &pvName)
{
    hideSeries(pvName);
}
