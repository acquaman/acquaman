#include "StripToolItem.h"



StripToolItem::StripToolItem(QString pvName, QString pvDescription, QString pvUnits, QObject *parent) :
    QObject(parent)
{
    updateIndex_ = 0;
    valuesDisplayed_ = 10;
    dataVectorSize_ = 100;

    pvName_ = pvName;
    pvDescription_ = pvDescription;
    xUnits_ = "Update number";
    yUnits_ = pvUnits;

    pvUpdateIndex_ = QVector<double>(dataVectorSize_);
    pvDataTotal_ = QVector<double>(dataVectorSize_);

    pvDataDisplay_ = new MPlotVectorSeriesData();

    pvSeries_ = new MPlotSeriesBasic();
    pvSeries_->setDescription(pvName_);
    pvSeries_->setModel(pvDataDisplay_);

    pvControl_ = new AMReadOnlyPVControl(pvName_, pvName_, this);
    connect( pvControl_, SIGNAL(valueChanged(double)), this, SLOT(onPVValueChanged(double)) );
}



StripToolItem::~StripToolItem()
{
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
//  check to see if the size of the data vectors allows for a new addition.
    if (dataVectorSize_ < updateIndex_ + 1)
    {
        dataVectorSize_ += 100;
        pvUpdateIndex_.resize(dataVectorSize_);
        pvDataTotal_.resize(dataVectorSize_);
    }

//  vectors are now the correct size, add the new data.
    pvUpdateIndex_[updateIndex_] = updateIndex_;
    pvDataTotal_[updateIndex_] = newValue;

//  define the vectors containing the information to be displayed.
    QVector<double> xValues;
    QVector<double> yValues;

    //  if we haven't collected a lot of points yet, display all we've got.
    if (updateIndex_ < valuesDisplayed_)
    {
        xValues = pvUpdateIndex_.mid(0, updateIndex_);
        yValues = pvDataTotal_.mid(0, updateIndex_);

    //  otherwise, show the latest "valuesDisplayed" points.
    } else {

        xValues = pvUpdateIndex_.mid(updateIndex_ - valuesDisplayed_, valuesDisplayed_);
        yValues = pvDataTotal_.mid(updateIndex_ - valuesDisplayed_, valuesDisplayed_);
    }

    //  update the displayed data with the new vectors and increment update counter.
    pvDataDisplay_->setValues(xValues, yValues);
    updateIndex_++;
}
