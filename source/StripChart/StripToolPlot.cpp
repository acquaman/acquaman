#include "StripToolPlot.h"

StripToolPlot::StripToolPlot(QObject *parent) : QObject(parent)
{
    selector_ = new MPlotPlotSelectorTool();
    connect( selector_, SIGNAL(itemSelected(MPlotItem*)), this, SLOT(showItemInfo(MPlotItem*)) );
    connect( selector_, SIGNAL(deselected()), this, SLOT(hideItemInfo()) );

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
    return namesToShownSeries_.keys();
}



bool StripToolPlot::contains(const QString &pvName)
{
    return pvShown(pvName) || pvHidden(pvName);
}



bool StripToolPlot::pvShown(const QString &pvName)
{
    return namesToShownSeries_.contains(pvName);
}



bool StripToolPlot::pvHidden(const QString &pvName)
{
    return namesToHiddenSeries_.contains(pvName);
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
        namesToShownSeries_[pvName] = newSeries;
    }
}



void StripToolPlot::showSeries(const QString &pvName)
{
    if (!pvShown(pvName) && pvHidden(pvName))
    {
        MPlotItem *toShow = namesToHiddenSeries_[pvName];
        basePlot_->addItem(toShow);
        namesToShownSeries_[pvName] = toShow;
        namesToHiddenSeries_.remove(pvName);
    }
}



void StripToolPlot::hideSeries(const QString &pvName)
{
    if (pvShown(pvName) && !pvHidden(pvName))
    {
        MPlotItem *toHide = namesToShownSeries_[pvName];
        basePlot_->removeItem(toHide);
        namesToShownSeries_.remove(pvName);
        namesToHiddenSeries_[pvName] = toHide;
    }
}



void StripToolPlot::deleteSeries(const QString &pvName)
{
    if (contains(pvName))
    {
        MPlotItem *toDelete;

        if (pvShown(pvName))
        {
            toDelete = namesToShownSeries_[pvName];
            basePlot_->removeItem(toDelete);
            namesToShownSeries_.remove(pvName);
            delete toDelete;

        } else if (pvHidden(pvName))
        {
            toDelete = namesToHiddenSeries_[pvName];
            basePlot_->removeItem(toDelete);
            namesToHiddenSeries_.remove(pvName);
            delete toDelete;
        }
    }
}



void StripToolPlot::showItemInfo(MPlotItem* plotSelection)
{
    //  use the plot selection provided to determine the properties to display.

    selectedItem_ = (StripToolSeries*) plotSelection;

    QString axisLeftLabel = selectedItem_->pvUnits();
    QString axisBottomLabel = "Update number";

    showPlotAxesLabels(axisLeftLabel, axisBottomLabel);
    showSeriesDescription(selectedItem_);
    showPlotAxesScale(selectedItem_);
}



void StripToolPlot::showPlotAxesLabels(const QString &axisLeftLabel, const QString &axisBottomLabel)
{
    basePlot_->axisLeft()->setAxisName(axisLeftLabel);
    basePlot_->axisLeft()->showAxisName(true);

    basePlot_->axisBottom()->setAxisName(axisBottomLabel);
    basePlot_->axisBottom()->showAxisName(true);
}



void StripToolPlot::showPlotAxesScale(StripToolSeries* plotSelection)
{
    Q_UNUSED(plotSelection);
}



void StripToolPlot::showSeriesDescription(StripToolSeries* plotSelection)
{
    plotSelection->setDescription(plotSelection->pvName());
}



void StripToolPlot::hideItemInfo()
{
    hidePlotAxesLabels();
    hideSeriesDescription(selectedItem_);

    selectedItem_ = 0;
}



void StripToolPlot::hidePlotAxesLabels()
{
    basePlot_->axisLeft()->showAxisName(false);
    basePlot_->axisBottom()->showAxisName(false);
}



void StripToolPlot::hideSeriesDescription(StripToolSeries *series)
{
    series->setDescription(" ");
}



void StripToolPlot::toShowCheckedPV(const QString &pvName)
{
    showSeries(pvName);
}



void StripToolPlot::toRemoveUncheckedPV(const QString &pvName)
{
    hideSeries(pvName);
}
