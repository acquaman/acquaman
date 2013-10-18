#include "StripToolPV.h"

StripToolPV::StripToolPV(const QString &pvName, const QString &pvDescription, const QString &pvUnits, QObject *parent)
    : QObject(parent)
{
    updateIndex_ = 0;
    valuesDisplayed_ = 10;
    dataVectorSize_ = 100;

    pvName_ = pvName;
    pvDescription_ = pvDescription;
    xUnits_ = "Update number";
    yUnits_ = pvUnits;
    isUpdating_ = true;
    checkState_ = Qt::Checked;

    pvUpdateIndex_ = QVector<double>(dataVectorSize_);
    pvDataTotal_ = QVector<double>(dataVectorSize_);

    pvData_ = new MPlotVectorSeriesData();

    pvSeries_ = new MPlotSeriesBasic();
    pvSeries_->setModel(pvData_);
    pvSeries_->setDescription(" ");
    pvSeries_->setMarker(MPlotMarkerShape::None);

    pvControl_ = new AMReadOnlyPVControl(pvName_, pvName_, this);
    connect( pvControl_, SIGNAL(valueChanged(double)), this, SLOT(onPVValueChanged(double)) );
}



StripToolPV::~StripToolPV()
{
}



QString StripToolPV::pvName()
{
    return pvName_;
}



QString StripToolPV::pvDescription()
{
    return pvDescription_;
}



QString StripToolPV::xUnits()
{
    return xUnits_;
}



QString StripToolPV::yUnits()
{
    return yUnits_;
}



Qt::CheckState StripToolPV::checkState()
{
    return checkState_;
}



MPlotVectorSeriesData* StripToolPV::data()
{
    return pvData_;
}



MPlotSeriesBasic* StripToolPV::series()
{
    return pvSeries_;
}



int StripToolPV::valuesDisplayed()
{
    return valuesDisplayed_;
}



void StripToolPV::setDescription(const QString &newDescription)
{
    pvDescription_ = newDescription;
}



void StripToolPV::setUnits(const QString &newUnits)
{
    yUnits_ = newUnits;
}



void StripToolPV::setPVUpdating(bool isUpdating)
{
    isUpdating_ = isUpdating;
}



void StripToolPV::setValuesDisplayed(int newValuesDisplayed)
{
    valuesDisplayed_ = newValuesDisplayed;
    xValuesDisplayed_ = QVector<double>(valuesDisplayed_);
    yValuesDisplayed_ = QVector<double>(valuesDisplayed_);
}



void StripToolPV::setCheckState(Qt::CheckState isChecked)
{
    checkState_ = isChecked;
}



void StripToolPV::onPVValueChanged(double newValue)
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

    //  if the pv is updating on the plot, display the correct updated information.
    if (isUpdating_)
    {
        //  if we haven't collected a lot of points yet, display all we've got.
        if (updateIndex_ < valuesDisplayed_)
        {
            xValuesDisplayed_ = pvUpdateIndex_.mid(0, updateIndex_);
            yValuesDisplayed_ = pvDataTotal_.mid(0, updateIndex_);

        //  otherwise, show the latest "valuesDisplayed" points.
        } else {

            xValuesDisplayed_ = pvUpdateIndex_.mid(updateIndex_ - valuesDisplayed_, valuesDisplayed_);
            yValuesDisplayed_ = pvDataTotal_.mid(updateIndex_ - valuesDisplayed_, valuesDisplayed_);
        }
    }

    //  update the displayed data with the new vectors and increment update counter.
    pvData_->setValues(xValuesDisplayed_, yValuesDisplayed_);
    updateIndex_++;
}
