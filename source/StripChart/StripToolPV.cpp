#include "StripToolPV.h"

StripToolPV::StripToolPV(const QString &pvName, const QString &pvDescription, const QString &pvUnits, QObject *parent)
    : QObject(parent)
{
    updateIndex_ = 0;
    defaultValuesDisplayed_ = 10;
    valuesDisplayed_ = defaultValuesDisplayed_;
    dataVectorSize_ = 100;
    savePoint_ = 10;

    pvName_ = pvName;
    pvDescription_ = pvDescription;
    xUnits_ = "Update number";
    yUnits_ = pvUnits;
    isUpdating_ = true;
    checkState_ = Qt::Checked;
    pvColor_ = QColor(Qt::red);

    pvUpdateIndex_ = QVector<double>(dataVectorSize_);
    pvDataTotal_ = QVector<double>(dataVectorSize_);

    pvControl_ = 0;

    pvData_ = new MPlotVectorSeriesData();

    pvSeries_ = new MPlotSeriesBasic();
    pvSeries_->setModel(pvData_);
    pvSeries_->setDescription(" ");
    pvSeries_->setMarker(MPlotMarkerShape::None);
    pvSeries_->setLinePen(QPen(pvColor_));
    pvSeries_->enableYAxisNormalization(true, MPlotAxisRange(0, 100));
}



StripToolPV::~StripToolPV()
{
}



QString StripToolPV::pvName() const
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



QColor StripToolPV::color()
{
    return pvColor_;
}



MPlotVectorSeriesData* StripToolPV::data()
{
    return pvData_;
}



MPlotSeriesBasic* StripToolPV::series()
{
    return pvSeries_;
}



//QList<QVector<double> >* StripToolPV::dataToSave()
//{
//    QList<QVector<double> > *toSave = new QList<QVector<double> >();
//    toSave[0] = pvUpdateIndex_.mid(updateIndex_ - savePoint_, -1);
//    toSave[1] = pvDataTotal_.mid(updateIndex_ - savePoint_, -1);

//    return toSave;
//}



int StripToolPV::valuesDisplayed()
{
    return valuesDisplayed_;
}



QVector<double> StripToolPV::saveIndexes()
{
    int position = updateIndex_ - savePoint_;
    int amount;

    if (position < 0)
        position = 0;

    if (pvUpdateIndex_.size() != pvDataTotal_.size())
    {
        amount = 0;
        qDebug() << "The number of indices do not match the number of data points for : " << pvName();

    } else {

        if (pvUpdateIndex_.size() < savePoint_)
        {
            amount = pvUpdateIndex_.size(); // if the number of data points is less than savePoint_, then return a vector containing all the values we have.

        } else {
            amount = savePoint_; // otherwise, return a vector that contains the latest savePoint_ data values.
        }
    }

    QVector<double> toSave = pvUpdateIndex_.mid(position, amount);

    if (toSave.size() != savePoint_)
        qDebug() << "mismatched sizes : should be saving " << savePoint_ << " values, but are actually saving " << toSave.size();

    return toSave;
}



QVector<double> StripToolPV::saveData()
{
    int position = updateIndex_ - savePoint_;
    int amount = savePoint_;

    if (position < 0)
        position = 0;

    if (pvDataTotal_.size() != pvUpdateIndex_.size())
    {
        amount = 0;
        qDebug() << "The number of indices do not match the number of data points for : " << pvName();

    } else {

        if (pvDataTotal_.size() < savePoint_)
        {
            amount = pvDataTotal_.size(); // if the number of data points is less than savePoint_, then return a vector containing all the values we have.

        } else {
            amount = savePoint_; // otherwise, return a vector that contains the latest savePoint_ data values.
        }
    }

    QVector<double> toSave = pvDataTotal_.mid(position, amount);
    return toSave;
}



//MPlotAxisRange* StripToolPV::axisRangeLeft()
//{
//    qreal min = ;
//    qreal max;
//    return new MPlotAxisRange(min, max);
//}



MPlotAxisRange StripToolPV::axisBottomRange() const
{
    qreal min = pvUpdateIndex_.at(updateIndex_ - valuesDisplayed_ - 1);
    qreal max = pvUpdateIndex_.at(updateIndex_ - 1);

    return MPlotAxisRange(min, max);
}



QList<QString> StripToolPV::metaDataHeader()
{
    QList<QString> metaDataHeaders;

    metaDataHeaders << "Name";
    metaDataHeaders << "Color";

    return metaDataHeaders;
}



QList<QString> StripToolPV::metaData()
{
    QList<QString> metaData;

    metaData << pvName();
    metaData << color().name();

    return metaData;
}



void StripToolPV::setControl(AMControl *newPV)
{
    pvControl_ = newPV;
    pvControl_->setParent(this);
    connect( pvControl_, SIGNAL(valueChanged(double)), this, SLOT(onPVValueChanged(double)) );
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



void StripToolPV::setValuesDisplayed(int points)
{
    if (points <= 0)
    {
        // do nothing?
        // I don't want to be able to hide a pv this way.

    } else {
        valuesDisplayed_ = points;
    }
}



void StripToolPV::incrementValuesDisplayed(int diff)
{
    int newPoints = valuesDisplayed_ + diff;
    setValuesDisplayed(newPoints);
}



void StripToolPV::setCheckState(Qt::CheckState isChecked)
{
    checkState_ = isChecked;
}



void StripToolPV::setSeriesColor(const QColor &color)
{
    pvColor_ = color;
    pvSeries_->setLinePen( QPen(pvColor_) );
}



bool StripToolPV::operator== (const StripToolPV &anotherPV)
{
    return (this->pvName() == anotherPV.pvName());
}



void StripToolPV::onPVValueChanged(double newValue)
{
    if (updateIndex_ > 0 && updateIndex_ % savePoint_ == 0)
        emit savePV();

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
        //  if we want to display more points than exist yet, display all we've got.
        if (updateIndex_ < valuesDisplayed_)
        {
            xValuesDisplayed_ = pvUpdateIndex_.mid(0, updateIndex_);
            yValuesDisplayed_ = pvDataTotal_.mid(0, updateIndex_);

        }

        //  otherwise, show the latest "valuesDisplayed" points.
        else {
            xValuesDisplayed_ = pvUpdateIndex_.mid(updateIndex_ - valuesDisplayed_, valuesDisplayed_);
            yValuesDisplayed_ = pvDataTotal_.mid(updateIndex_ - valuesDisplayed_, valuesDisplayed_);
        }
    }

    //  update the displayed data with the new vectors.
    pvData_->setValues(xValuesDisplayed_, yValuesDisplayed_);

    // increment the update counter.
    updateIndex_++;
}
