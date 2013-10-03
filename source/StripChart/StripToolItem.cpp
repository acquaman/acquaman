#include "StripToolItem.h"



StripToolItem::StripToolItem(QString pvName, QString pvDescription, QString pvUnits, QObject *parent) :
    QObject(parent)
{
    updateIndex_ = 0;
    valuesDisplayed_ = 10;
    pvName_ = pvName;
    pvDescription_ = pvDescription;
    xUnits_ = "Update number";
    yUnits_ = pvUnits;

    pvDataDisplay_ = new MPlotVectorSeriesData();

    pvSeries_ = new MPlotSeriesBasic();
    pvSeries_->setDescription(pvName_);
    pvSeries_->setModel(pvDataDisplay_);

    pvControl_ = new AMReadOnlyPVControl(pvName_, pvName_, this);
    connect( pvControl_, SIGNAL(valueChanged(double)), this, SLOT(onPVValueChanged(double)) );
}



QString StripToolItem::pvName()
{
    return pvName_;
}



QString StripToolItem::pvDescription()
{
    return pvDescription_;
}



QString StripToolItem::xUnits()
{
    return xUnits_;
}



QString StripToolItem::yUnits()
{
    return yUnits_;
}



MPlotItem* StripToolItem::series()
{
    return pvSeries_;
}



void StripToolItem::setValuesDisplayed(const int newValuesDisplayed)
{
    valuesDisplayed_ = newValuesDisplayed;
}



void StripToolItem::onPVValueChanged(double newValue)
{
    //  update the vectors containing all pv data, and increment the update counter.
    pvUpdateIndex_.append(updateIndex_);
    pvDataTotal_.append(newValue);
    updateIndex_++;

    //  create new vectors containing the values to be displayed.
    QVector<double> xValues, yValues;

    if (updateIndex_ < valuesDisplayed_)
    {
        xValues = pvUpdateIndex_;
        yValues = pvDataTotal_;

    } else {

        xValues = pvUpdateIndex_.mid(updateIndex_ - valuesDisplayed_);
        yValues = pvDataTotal_.mid(updateIndex_ - valuesDisplayed_);
    }

    //  update the displayed data with the new vectors.
    pvDataDisplay_->setValues(xValues, yValues);
}
