#include "StripToolPV.h"

StripToolPV::StripToolPV(QObject *parent)
    : QObject(parent)
{
    updateIndex_ = 0;
    defaultValuesDisplayed_ = 10;
    valuesDisplayed_ = defaultValuesDisplayed_;
    dataVectorSize_ = 100;
    savePoint_ = 10;

    pvName_ = "";
    pvDescription_ = "";
    xUnits_ = "Update number";
    yUnits_ = "";
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

    setMetaDataHeaders();

    connect( this, SIGNAL(savePVMetaData()), this, SLOT(saveMetaDataTest()) );
}



StripToolPV::~StripToolPV()
{
}



QString StripToolPV::pvName() const
{
    return pvName_;
}



QString StripToolPV::pvDescription() const
{
    return pvDescription_;
}



QString StripToolPV::xUnits() const
{
    return xUnits_;
}



QString StripToolPV::yUnits() const
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
        qDebug() << "Mismatched sizes : should be saving " << savePoint_ << " values, but are actually saving " << toSave.size();

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



void StripToolPV::setMetaDataHeaders()
{
    headers_.clear();

    headers_ << "Name ";
    headers_ << "Description ";
    headers_ << "Units ";
    headers_ << "Displayed ";
    headers_ << "Color ";
}



QList<QString> StripToolPV::metaDataHeaders()
{
    return headers_;
}



QList<QString> StripToolPV::metaData()
{
    QList<QString> metaData;

    metaData << pvName();
    metaData << pvDescription();
    metaData << yUnits();
    metaData << QString::number(valuesDisplayed());
    metaData << color().name();

    return metaData;
}



bool StripToolPV::setMetaData(QList<QString> metaData)
{
    qDebug() << "Attempting to set meta data for pv named" << metaData.at(0);

    if (metaData.at(0) != pvName())
    {
        qDebug() << "The meta data name" << metaData.at(0) << "and pv name" << pvName() << "don't match!";
        return false;
    }

    if (metaData.size() != metaDataHeaders().size())
    {
        qDebug() << "The meta data size" << QString::number(metaData.size()) << "and the number of pv headers" << QString::number(metaDataHeaders().size()) << "don't match!";
        return false;
    }

    setDescription(metaData.at(1));
    setUnits(metaData.at(2));
    setValuesDisplayed(metaData.at(3).toInt());
    setSeriesColor(metaData.at(4));

    return true;
}



void StripToolPV::setControl(AMControl *newControl)
{
    pvControl_ = newControl;
    pvName_ = newControl->name();
    pvControl_->setParent(this);
    connect( pvControl_, SIGNAL(valueChanged(double)), this, SLOT(onPVValueChanged(double)) );
}



void StripToolPV::setDescription(const QString &newDescription)
{
    pvDescription_ = newDescription;
    emit savePVMetaData();
}



void StripToolPV::setUnits(const QString &newUnits)
{
    yUnits_ = newUnits;
    emit savePVMetaData();
}



void StripToolPV::setPVUpdating(bool isUpdating)
{
    isUpdating_ = isUpdating;
}



void StripToolPV::setValuesDisplayed(int points)
{
    if (points <= 0)
        return;

    valuesDisplayed_ = points;
    emit savePVMetaData();
}



void StripToolPV::setCheckState(Qt::CheckState isChecked)
{
    checkState_ = isChecked;
}



void StripToolPV::setSeriesColor(const QColor &color)
{
    pvColor_ = color;
    pvSeries_->setLinePen( QPen(pvColor_) );
    emit savePVMetaData();
}



bool StripToolPV::operator== (const StripToolPV &anotherPV)
{
    return (this->pvName() == anotherPV.pvName());
}



void StripToolPV::onPVValueChanged(double newValue)
{
    if (updateIndex_ > 0 && updateIndex_ % savePoint_ == 0)
        emit savePVData();

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



void StripToolPV::saveMetaDataTest()
{
    qDebug() << "The signal for saving changed meta data was emitted.";
}
