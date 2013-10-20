#include "StripToolContainer.h"

StripToolContainer::StripToolContainer(QObject *parent) : QObject(parent)
{
}


StripToolContainer::~StripToolContainer()
{
}



bool StripToolContainer::contains(const QString &pvName)
{
    return pvNameToItemMap.contains(pvName);
}



void StripToolContainer::addItem(const QString &pvName, const QString &pvDescription, const QString &pvUnits)
{
    if (contains(pvName))
    {
        // if we don't want to display two copies of the same pv, display warning message.
    } else {
        StripToolItem *newItem = new StripToolItem(pvName, pvDescription, pvUnits);
        pvNameToItemMap[pvName] = newItem;
    }
}


void StripToolContainer::deleteItem(const QString &pvName)
{
    if (contains(pvName))
    {
        StripToolItem *itemToDelete = pvNameToItemMap[pvName];
        pvNameToItemMap.remove(pvName);
        delete itemToDelete;
    }
}



int StripToolContainer::valuesDisplayed(const QString &pvName)
{
    if (contains(pvName))
        return pvNameToItemMap[pvName]->valuesDisplayed();
    else
        return 0;
}


void StripToolContainer::setValuesDisplayed(const QString &pvName, const int newValuesDisplayed)
{
    if (contains(pvName))
    {
        StripToolItem *item = pvNameToItemMap[pvName];
        item->setValuesDisplayed(newValuesDisplayed);
    }
}


void StripToolContainer::setPVUpdating(const QString &pvName, bool isUpdating)
{
    if (contains(pvName))
    {
        StripToolItem *item = pvNameToItemMap[pvName];
        item->setPVUpdating(isUpdating);
    }
}



MPlotVectorSeriesData* StripToolContainer::data(const QString &pvName)
{
    if (contains(pvName))
        return pvNameToItemMap[pvName]->data();
    else
        return new MPlotVectorSeriesData();
}



QString StripToolContainer::description(const QString &pvName)
{
    if (contains(pvName))
        return pvNameToItemMap[pvName]->pvDescription();
    else
        return "";
}



QString StripToolContainer::xUnits(const QString &pvName)
{
    if (contains(pvName))
        return pvNameToItemMap[pvName]->xUnits();
    else
        return "";
}



QString StripToolContainer::yUnits(const QString &pvName)
{
    if (contains(pvName))
        return pvNameToItemMap[pvName]->yUnits();
    else
        return "";
}



QList<QString> StripToolContainer::getPVList()
{
    return pvNameToItemMap.keys();
}
