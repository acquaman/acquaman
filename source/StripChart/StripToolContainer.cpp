#include "StripToolContainer.h"

StripToolContainer::StripToolContainer(QObject *parent) :
    QObject(parent)
{
    itemCount_ = 0;
}


void StripToolContainer::addItem(const QString &pvName, const QString &pvDescription, const QString &pvUnits)
{
    StripToolItem *newPVItem = new StripToolItem(pvName, pvDescription, pvUnits, this);
    connect( newPVItem, SIGNAL(pvConnected(QString)), this, SLOT(updateSeriesInfo(QString)) );
    connect( newPVItem, SIGNAL(pvDataUpdate(QString,QVector<double>,QVector<double>)), this, SLOT(onPVDataUpdate(QString, QVector<double>, QVector<double>)) );
    pvNameToItemMap[pvName] = newPVItem;

    MPlotVectorSeriesData *newPVData = new MPlotVectorSeriesData();
    pvNameToDataMap[pvName] = newPVData;

    MPlotSeriesBasic *newPVSeries = new MPlotSeriesBasic();
    newPVSeries->setDescription(" ");
    newPVSeries->setModel(newPVData);

    pvNameToSeriesMap[pvName] = newPVSeries;

    itemCount_++;
}


void StripToolContainer::removeItem(const QString &pvName)
{
    pvNameToItemMap.remove(pvName);
    pvNameToDataMap.remove(pvName);
    pvNameToSeriesMap.remove(pvName);

    itemCount_--;
}


bool StripToolContainer::contains(const QString &pvName)
{
    return pvNameToItemMap.contains(pvName);
}


void StripToolContainer::setValuesDisplayed(const QString &pvName, const int newValuesDisplayed)
{
    StripToolItem *item = pvNameToItemMap[pvName];
    item->setValuesDisplayed(newValuesDisplayed);
}


void StripToolContainer::onPVDataUpdate(const QString &pvName, QVector<double> xValues, QVector<double> yValues)
{    
    MPlotVectorSeriesData *pvData = pvNameToDataMap[pvName];
    pvData->setValues(xValues, yValues);
}


MPlotItem *StripToolContainer::getSeries(const QString &pvName)
{
    return pvNameToSeriesMap[pvName];
}


QString StripToolContainer::getAxisLeft(MPlotItem *plotSelection)
{
    QString pvName = pvNameToSeriesMap.key(plotSelection);
    StripToolItem *pvItem = pvNameToItemMap[pvName];
    return pvItem->getYUnits();
}


QString StripToolContainer::getAxisBottom(MPlotItem *plotSelection)
{
    QString pvName = pvNameToSeriesMap.key(plotSelection);
    StripToolItem *pvItem = pvNameToItemMap[pvName];
    return pvItem->getXUnits();
}
