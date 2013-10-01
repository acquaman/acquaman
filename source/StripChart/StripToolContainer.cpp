#include "StripToolContainer.h"

StripToolContainer::StripToolContainer(QObject *parent) :
    QObject(parent)
{
    itemCount_ = 0;
}


void StripToolContainer::addItem(const QString &pvName, const QString &pvDescription)
{
    StripToolItem *newPVItem = new StripToolItem(pvName, pvDescription, this);
    connect(newPVItem, SIGNAL(pvDataUpdate(QString,QVector<double>,QVector<double>)), this, SLOT(onPVDataUpdate(QString, QVector<double>, QVector<double>)) );
    pvNameToItemMap[pvName] = newPVItem;
    itemCount_++;
}


void StripToolContainer::removeItem(const QString &pvName)
{
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


void StripToolContainer::onPVDataUpdate(const QString &pvName, QVector<double> xValues, QVector<double> yValues)
{
    emit pvDataUpdate(pvName, xValues, yValues);
}
