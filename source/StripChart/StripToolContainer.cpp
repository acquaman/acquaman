#include "StripToolContainer.h"

StripToolContainer::StripToolContainer(QObject *parent) :
    QObject(parent)
{
    itemCount_ = 0;
}


StripToolContainer::~StripToolContainer()
{
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
    delete itemToDelete;
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


MPlotItem *StripToolContainer::series(const QString &pvName)
{
    return pvNameToItemMap[pvName]->series();
}



QString StripToolContainer::description(const QString &pvName)
{
    return pvNameToItemMap[pvName]->pvDescription();
}


QString StripToolContainer::xUnits(const QString &pvName)
{
    return pvNameToItemMap[pvName]->xUnits();
}


QString StripToolContainer::yUnits(const QString &pvName)
{
    return pvNameToItemMap[pvName]->yUnits();
}


QString StripToolContainer::getAxisLeft(MPlotItem *plotSelection)
{
    Q_UNUSED(plotSelection);
//    QString pvName = plotSelection->description();

//    StripToolItem *pvItem = pvNameToItemMap[pvName];
//    return pvItem->yUnits();
    return "This is broken right now!";
}


QString StripToolContainer::getAxisBottom(MPlotItem *plotSelection)
{
    Q_UNUSED(plotSelection);
//    QString pvName = plotSelection->description();
//    StripToolItem *pvItem = pvNameToItemMap[pvName];
//    return pvItem->xUnits();
    return "This is broke too. :(";
}
