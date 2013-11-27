#include "StripToolPV.h"

StripToolPV::StripToolPV(QTime modelStart, QObject *parent)
    : QObject(parent)
{
    modelStartTime_ = modelStart;
    pvCreationTime_ = QTime::currentTime();

    updateIndex_ = 0;

    defaultValuesDisplayed_ = 10;
    valuesDisplayed_ = defaultValuesDisplayed_;
    dataVectorSize_ = 100;
    savePoint_ = 10;

    pvName_ = "";
    pvDescription_ = "";
    xUnits_ = "Time [ms]";
    yUnits_ = "";
    isUpdating_ = true;
    checkState_ = Qt::Checked;
    pvColor_ = QColor(Qt::red);

    masterUpdateTimes_ = QVector<QTime>(dataVectorSize_);
    masterUpdateValues_ = QVector<double>(dataVectorSize_);
    displayedTimes_ = QVector<double>(valuesDisplayed_);
    displayedValues_ = QVector<double>(valuesDisplayed_);

    pvControl_ = 0;

    pvData_ = new MPlotVectorSeriesData();

    pvSeries_ = new MPlotSeriesBasic();
    pvSeries_->setModel(pvData_);
    pvSeries_->setDescription(" ");
    pvSeries_->setMarker(MPlotMarkerShape::None);
    pvSeries_->setLinePen(QPen(pvColor_));
    pvSeries_->enableYAxisNormalization(true, MPlotAxisRange(0, 1));

    forceUpdate_ = false;

    setMetaDataHeaders();

    connect( this, SIGNAL(forceUpdate(double)), this, SLOT(onPVValueChanged(double)) );

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



QVector<QString> StripToolPV::saveMasterTimes()
{
    int position = updateIndex_ - savePoint_;
    int amount;

    if (position < 0)
        position = 0;

    if (masterUpdateTimes_.size() != masterUpdateValues_.size())
    {
        amount = 0;
        qDebug() << "The number of time entries do not match the number of data points for : " << pvName();

    } else {

        if (masterUpdateTimes_.size() < savePoint_)
        {
            amount = masterUpdateTimes_.size(); // if the number of time entries is less than savePoint_, then return a vector containing all the times we have.

        } else {
            amount = savePoint_; // otherwise, return a vector that contains the latest savePoint_ times.
        }
    }

    QVector<QString> toSave;

    foreach (QTime time, masterUpdateTimes_.mid(position, amount))
    {
        toSave.append(time.toString());
    }

    if (toSave.size() != savePoint_)
        qDebug() << "Mismatched sizes : should be saving " << savePoint_ << " values, but are actually saving " << toSave.size();

    return toSave;
}



QVector<double> StripToolPV::saveMasterValues()
{
    int position = updateIndex_ - savePoint_;
    int amount = savePoint_;

    if (position < 0)
        position = 0;

    if (masterUpdateValues_.size() != masterUpdateTimes_.size())
    {
        amount = 0;
        qDebug() << "The number of time entries do not match the number of data points for : " << pvName();

    } else {

        if (masterUpdateValues_.size() < savePoint_)
        {
            amount = masterUpdateValues_.size(); // if the number of data points is less than savePoint_, then return a vector containing all the values we have.

        } else {
            amount = savePoint_; // otherwise, return a vector that contains the latest savePoint_ data values.
        }
    }

    QVector<double> toSave = masterUpdateValues_.mid(position, amount);
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
    headers_ << "Date ";
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
    metaData << QDateTime::currentDateTime().toString();

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



void StripToolPV::saveCheck()
{
    if (updateIndex_ > 0 && updateIndex_ % savePoint_ == 0)
        emit savePVData();
}



void StripToolPV::dataVectorSizeCheck()
{
    if (dataVectorSize_ < updateIndex_ + 1)
    {
        dataVectorSize_ += 100;
        masterUpdateTimes_.resize(dataVectorSize_);
        masterUpdateValues_.resize(dataVectorSize_);
    }
}



void StripToolPV::onPVValueChanged(double newValue)
{
//    qDebug() << "PV" << pvName() << "value changed";

    // check to see if it's time to save the pv data to file.
    saveCheck();

    //  check to see if the size of the data vectors allows for a new addition, resize if not.
    dataVectorSizeCheck();

    //  vectors are now the correct size, add the new data to master lists.
    masterUpdateTimes_[updateIndex_] = QTime::currentTime();
    masterUpdateValues_[updateIndex_] = newValue;

    // set a 'now' time that will be used to generate the x-axis display values.
    QTime nowish = QTime::currentTime();

    //  if the pv is updating on the plot, display the correct updated information.
    if (isUpdating_)
    {
        // clear the display arrays to prep for new values.
        displayedTimes_.clear();
        displayedValues_.clear();

        //  if we want to display more points than exist yet, show all we've got.
        if (updateIndex_ < valuesDisplayed_)
        {
            displayedTimes_ = QVector<double>(updateIndex_ + 1);
            displayedValues_ = QVector<double>(updateIndex_ + 1);

            for (int i = 0; i < updateIndex_ + 1; i++)
            {
                displayedTimes_[i] = nowish.msecsTo(masterUpdateTimes_.at(i));
                displayedValues_[i] = masterUpdateValues_.at(i);
            }
        }

        //  otherwise, show the latest "valuesDisplayed" points.
        else {

            displayedTimes_ = QVector<double>(valuesDisplayed_);
            displayedValues_ = QVector<double>(valuesDisplayed_);

            for (int i = 0; i < valuesDisplayed_; i++)
            {
                displayedTimes_[i] = nowish.msecsTo(masterUpdateTimes_.at(updateIndex_ - valuesDisplayed_ + i));
                displayedValues_[i] = masterUpdateValues_.at(updateIndex_ - valuesDisplayed_ + i);
            }
        }

//        qDebug() << "Displayed times : " << displayedTimes_;
//        qDebug() << "Displayed values : " << displayedValues_;
//        qDebug() << "Displayed values for" << this->pvName() << "updated.";
    }

    //  update the displayed data with the new vectors.
    pvData_->setValues(displayedTimes_, displayedValues_);

    // increment the internal update counter.
    updateIndex_++;

    // if the model forced this update to happen (as opposed to the pv updating), do not tell the model that this pv updated!
    if (forceUpdate_)
        forceUpdate_ = false;
    else
        emit pvValueUpdated();

//    qDebug() << "PV value change complete.";

}



void StripToolPV::toForceUpdateValue(const QString &toIgnore)
{
    if (this->pvName() != toIgnore)
    {
//        qDebug() << "PV" << this->pvName() << "is undergoing forced update.";
        forceUpdate_ = true;
        emit forceUpdate(pvControl_->value());
    }
}
