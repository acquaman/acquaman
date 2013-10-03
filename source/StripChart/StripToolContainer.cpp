#include "StripToolContainer.h"

StripToolContainer::StripToolContainer(QObject *parent) :
    QObject(parent)
{
    itemCount_ = 0;
}


void StripToolContainer::addItem(const QString &pvName, const QString &pvDescription, const QString &pvUnits)
{
    StripToolItem *newItem = new StripToolItem(pvName, pvDescription, pvUnits);
    pvNameToItemMap[pvName] = newItem;
    itemCount_++;
}


void StripToolContainer::deleteItem(const QString &pvName)
{
    StripToolItem *itemToDelete = pvNameToItemMap[pvName];
    itemToDelete->~StripToolItem();
    pvNameToItemMap.remove(pvName);
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


MPlotItem *StripToolContainer::getSeries(const QString &pvName)
{
    return pvNameToItemMap[pvName]->series();
}


QString StripToolContainer::getAxisLeft(MPlotItem *plotSelection)
{
    QString pvName = plotSelection->description();
    StripToolItem *pvItem = pvNameToItemMap[pvName];
    return pvItem->yUnits();
}


QString StripToolContainer::getAxisBottom(MPlotItem *plotSelection)
{
    QString pvName = plotSelection->description();
    StripToolItem *pvItem = pvNameToItemMap[pvName];
    return pvItem->xUnits();
}
