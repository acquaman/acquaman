#include "StripToolItem.h"

StripToolItem::StripToolItem(QString pvName, QString pvDescription, QObject *parent) :
    QObject(parent)
{
    updateIndex_ = 0;
    valuesDisplayed_ = 25;
    pvName_ = pvName;
    pvDescription_ = pvDescription;

    pvControl_ = new AMReadOnlyPVControl(pvName, pvName, this);
    connect( pvControl_, SIGNAL(valueChanged(double)), this, SLOT(onPVValueChanged(double)) );
}



QString StripToolItem::getPVName()
{
    return pvName_;
}



QString StripToolItem::getPVDescription()
{
    return pvDescription_;
}



QVector<double> StripToolItem::getPVUpdateIndices()
{
    return pvUpdateIndex_;
}


QVector<double> StripToolItem::getPVData()
{
    return pvData_;
}


void StripToolItem::setValuesDisplayed(const int newValuesDisplayed)
{
    valuesDisplayed_ = newValuesDisplayed;
}


void StripToolItem::onPVValueChanged(double newValue)
{
    pvUpdateIndex_.append(updateIndex_);
    pvData_.append(newValue);

    updateIndex_++;

    QVector<double> xValues, yValues;

    if (updateIndex_ <= valuesDisplayed_)
    {
        xValues = pvUpdateIndex_;
        yValues = pvData_;

    } else {

        xValues = pvUpdateIndex_.mid(updateIndex_ - valuesDisplayed_);
        yValues = pvData_.mid(updateIndex_ - valuesDisplayed_);
    }


    emit pvDataUpdate(getPVName(), xValues, yValues);
}
