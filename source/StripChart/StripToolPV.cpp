#include "StripToolPV.h"

StripToolPV::StripToolPV(QObject *parent)
    : QObject(parent)
{
    updateIndex_ = 0;
    updateGranularity_ = 2;
    timeFactor_ = 0.001; // for conversion from different time units (initially ms) into seconds.

    defaultTimeDisplayed_ = 10;
    timeDisplayed_ = defaultTimeDisplayed_;

    dataVectorSize_ = 100;
    savePoint_ = 100;

    pvName_ = "";
    pvDescription_ = "";
    xUnits_ = "";
    yUnits_ = "";
    yAxisLabel_ = "";

    isUpdating_ = true;
    checkState_ = Qt::Checked;
    pvColor_ = QColor(Qt::red);

    isSelected_ = false;

    dateCreated_ = QDateTime::currentDateTime().toString();

    waterfall_ = 0.00; // all pvs begin centered on the plot view.

    masterUpdateTimes_ = QVector<QTime>(dataVectorSize_);
    masterUpdateValues_ = QVector<double>(dataVectorSize_);

    defaultDisplayedYMin_ = 0.0; // this will always be updated to reflect the min/max of the pv's displayed values!
    defaultDisplayedYMax_ = 1.0;

    defaultYMaxDisplayed_ = true;
    defaultYMinDisplayed_ = true;


    customDisplayedYMin_ = ""; // if not using the defaultDisplayWindow, the user can specify a range they would like to view.
    customDisplayedYMax_ = "";

    pvControl_ = 0;

    pvData_ = new MPlotVectorSeriesData();

    pvSeries_ = new MPlotSeriesBasic();
    pvSeries_->setModel(pvData_);
    pvSeries_->setDescription(" ");
    pvSeries_->setMarker(MPlotMarkerShape::None);
    pvSeries_->setLinePen(QPen(pvColor_));

    forceUpdate_ = false;

    setMetaDataHeaders();

    // we can force the master times and values lists to update with the most recent value (say if we wanted all pvs to have roughly the same number of points displayed) by emitting this pv's forceUpdate signal. See 'toForceUpdateValue' slot.
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



int StripToolPV::updateGranularity()
{
    return updateGranularity_;
}


double StripToolPV::waterfall()
{
    return waterfall_;
}



double StripToolPV::displayedYMin()
{
    if (defaultYMinDisplayed_)
        return defaultDisplayedYMin_;
    else
        return customDisplayedYMin_.toDouble();
}



double StripToolPV::displayedYMax()
{
    if (defaultYMaxDisplayed_)
        return defaultDisplayedYMax_;
    else
        return customDisplayedYMax_.toDouble();
}


bool StripToolPV::isSelected()
{
    return isSelected_;
}


void StripToolPV::setSelected(bool selected)
{
    isSelected_ = selected;

    emit updateYAxisLabel(yAxisLabel_);
    emit updateWaterfallDisplay(waterfall_);
}



MPlotVectorSeriesData* StripToolPV::data()
{
    return pvData_;
}



MPlotSeriesBasic* StripToolPV::series()
{
    return pvSeries_;
}



int StripToolPV::timeDisplayed()
{
    return timeDisplayed_;
}



QVector<QString> StripToolPV::saveMasterTimes()
{
    int position = updateIndex_ - savePoint_; // the index at which we begin saving data.
    int amount; // the number of values to save.

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
        amount = savePoint_; // otherwise, return a vector that contains the latest savePoint_ data values.
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
    headers_ << "Color ";
    headers_ << "Granularity ";
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
    metaData << color().name();
    metaData << QString::number(updateGranularity());
    metaData << dateCreated_;

    return metaData;
}



QList<QString> StripToolPV::editPVDialogData()
{
    QList<QString> editPVData;

    editPVData << pvName();
    editPVData << dateCreated_;
    editPVData << pvDescription();
    editPVData << yUnits();
    editPVData << QString::number(updateGranularity());
    editPVData << color().name();
    editPVData << customDisplayedYMax_;
    editPVData << customDisplayedYMin_;

    qDebug() << "PV information to be displayed in EditPVDialog : " << editPVData;


    return editPVData;
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
        qDebug() << "The number of meta data entries" << QString::number(metaData.size()) << "and the number of headers" << QString::number(metaDataHeaders().size()) << "don't match!";
        return false;
    }

    QString description = metaData.at(1);
    QString yUnits = metaData.at(2);
    QString color = metaData.at(3);
    int granularity = metaData.at(4).toInt();

    setDescription(description);
    setYUnits(yUnits);
    setSeriesColor(color);

    if (granularity > 0) {
        setUpdateGranularity(granularity);
    } else {
        setUpdateGranularity(2);
    }

    return true;
}



void StripToolPV::setControl(AMControl *newControl)
{
    pvControl_ = newControl;
    pvName_ = newControl->name();

    if (pvDescription_ == "")
        setDescription(pvName_);

    pvControl_->setParent(this);
    connect( pvControl_, SIGNAL(valueChanged(double)), this, SLOT(onPVValueChanged(double)) );
}



void StripToolPV::setDescription(const QString &newDescription)
{
    if (newDescription != "")
        pvDescription_ = newDescription;

    else
        pvDescription_ = pvName();

    qDebug() << "Setting new description for pv" << pvName() << ":" << pvDescription_;
    emit savePVMetaData();

    yAxisLabel_ = newDescription + " [" + yUnits() + "]";
    emit updateYAxisLabel(yAxisLabel_);
}



void StripToolPV::setYUnits(const QString &newUnits)
{
    yUnits_ = newUnits;
    qDebug() << "Setting new units for pv" << pvName() << ":" << yUnits_;
    emit savePVMetaData();

    yAxisLabel_ = pvDescription() + " [" + newUnits + "]";
    emit updateYAxisLabel(yAxisLabel_);
}



void StripToolPV::setXUnits(const QString &newUnits)
{
    if (newUnits == "")
        return;

    xUnits_ = newUnits;

    if (xUnits_ == "seconds") {
        timeFactor_ = 0.001; // convert calculated millisecond values to seconds.

    } else if (xUnits_ == "minutes") {
        timeFactor_ = 0.001/60.0; // convert calculated millisecond values to minutes.

    } else if (xUnits_ == "hours") {
        timeFactor_ = 0.001/3600.0; // convert calculated millisecond values to hours!

    } else if (xUnits_ == "sec") {
        timeFactor_ = 0.001;

    } else if (xUnits_ == "min") {
        timeFactor_ = 0.001/60.0;

    } else if (xUnits_ == "hr") {
        timeFactor_ = 0.001/3600.0;

    } else {
        qDebug() << "Could not determine correct time factor for these units :" << newUnits;

    }
}



void StripToolPV::setPVUpdating(bool isUpdating)
{
    isUpdating_ = isUpdating;
}



void StripToolPV::setWaterfall(double newWaterfall)
{
    if (newWaterfall > 0.5 || newWaterfall < -0.5) {
        qDebug() << "New waterfall value falls outside acceptable range.";
        return;
    }

    waterfall_ = newWaterfall;
    qDebug() << "Waterfall for pv" << pvName() << "has been changed to" << waterfall();

    if (checkState_ == Qt::Checked && isSelected_) {
        emit updateWaterfall(waterfall_);
    }
}



void StripToolPV::setDisplayedYMax(const QString &newMax)
{
    if (newMax == "") {
        qDebug() << "Returning to automatically updating y max for pv" << pvName();
        defaultYMaxDisplayed_ = true;
        customDisplayedYMax_ = newMax;
        return;
    }

    qDebug() << "Setting upper limit on displayed y values for pv" << pvName() << "to" << newMax;
    customDisplayedYMax_ = newMax;
    defaultYMaxDisplayed_ = false;
}



void StripToolPV::setDisplayedYMin(const QString &newMin)
{
    if (newMin == "") {
        qDebug() << "Returning to automatically updating y max for pv " << pvName();
        customDisplayedYMin_ = newMin;
        defaultYMinDisplayed_ = true;
        return;
    }

    qDebug() << "Setting lower limit on displayed y values for pv" << pvName() << "to" << newMin;
    customDisplayedYMin_ = newMin;
    defaultYMinDisplayed_ = false;
}



void StripToolPV::setTimeDisplayed(int seconds)
{
    if (seconds <= 0) {
        qDebug() << "Cannot display a time less than zero seconds!";
        return;
    }

    timeDisplayed_ = seconds;
}



void StripToolPV::setCheckState(Qt::CheckState isChecked)
{
    checkState_ = isChecked;
}



void StripToolPV::setSeriesColor(const QColor &color)
{
    if (!color.isValid()) {
        qDebug() << "New color selection is invalid.";
        return;
    }

    pvColor_ = color;
    pvSeries_->setLinePen( QPen(pvColor_) );
    qDebug() << "Setting new color for pv" << pvName() << ":" << color.name();
    emit savePVMetaData();
}



void StripToolPV::setUpdateGranularity(int newVal)
{
    if (newVal <= 0) {
        qDebug() << "Cannot display a pv with update granularity of zero or less! Must be positive, nonzero integer.";
        return;
    }

    updateGranularity_ = newVal;
    qDebug() << "Setting new update granularity for pv" << pvName() << ":" << updateGranularity();
    emit savePVMetaData();
}



bool StripToolPV::operator== (const StripToolPV &anotherPV)
{
    return (this->pvName() == anotherPV.pvName());
}



void StripToolPV::saveCheck()
{
    if (updateIndex_ > 0 && updateIndex_ % savePoint_ == 0) {

        emit savePVData();
    }
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
    // check to see if it's time to save the pv data to file.
    saveCheck();

    //  check to see if the size of the data vectors allows for a new addition, resize if not.
    dataVectorSizeCheck();

    //  vectors are now the correct size, add the new data!
    QTime latestTime = QTime::currentTime();
    double latestValue = newValue;

    masterUpdateTimes_[updateIndex_] = latestTime;
    masterUpdateValues_[updateIndex_] = latestValue;

//    qDebug() << "PV" << pvName() << "value update :" << latestValue;

    //  if the pv is updating on the plot, display the correct updated information.
    if (isUpdating_ && (updateIndex_ % updateGranularity() == 0))
    {
        // set a 'now' time that will be used to generate the x-axis display values.
        QTime nowish = QTime::currentTime();

        // clear the display arrays to prep for new values.
        displayedTimes_.clear();
        displayedValues_.clear();

        // copy all values of masterUpdateTimes_ that are less than timeDisplayed_ to the display vectors.
        int startIndex = updateIndex_;
        int index = startIndex;
        bool copyComplete = false;

        while (index >= 0 && index < masterUpdateTimes_.size() && !copyComplete)
        {   
            double relativeTime = nowish.msecsTo(masterUpdateTimes_.at(index)) * timeFactor_; // relative time is initially in seconds, but changes depending on the x axis units.

            if (qAbs(relativeTime) < qAbs(timeDisplayed_))
            {
                double newTime = relativeTime;
                double newVal = masterUpdateValues_.at(index);

                displayedTimes_.append(newTime);
                displayedValues_.append(newVal);

                index--;

            } else {

                copyComplete = true;
            }
        }
    }

    //  update the displayed data with the new vectors.
    pvData_->setValues(displayedTimes_, displayedValues_);

    // increment the internal update counter.
    updateIndex_++;

    // if the model forced this update to happen (as opposed to the pv itself updating), do not tell the model that this pv updated!
    if (forceUpdate_)
        forceUpdate_ = false;
    else
        emit pvValueUpdated();

    // if the pv is selected (and plotted) then the axis labels should reflect the data of this pv.
    if (isSelected() && checkState() == Qt::Checked) {
        double min, max;

        for (int i = 0; i < displayedValues_.size(); i++) {

            if (i == 0) {
                min = displayedValues_.at(i);
                max = displayedValues_.at(i);

            } else {
                if (min > displayedValues_.at(i))
                    min = displayedValues_.at(i);

                if (max < displayedValues_.at(i))
                    max = displayedValues_.at(i);
            }
        }

        defaultDisplayedYMin_ = min;
        defaultDisplayedYMax_ = max;

        double displayedYMax, displayedYMin;

        if (defaultYMaxDisplayed_) {
            displayedYMax = defaultDisplayedYMax_;
        } else {
            displayedYMax = customDisplayedYMax_.toDouble();
        }

        if (defaultYMinDisplayed_) {
            displayedYMin = defaultDisplayedYMin_;
        } else {
            displayedYMin = customDisplayedYMin_.toDouble();
        }

        MPlotAxisRange *newRange = new MPlotAxisRange(displayedYMin, displayedYMax); // get the vertical range of the selected data.
        emit updateYAxisRange(newRange);

        emit updateYDisplayMax(defaultDisplayedYMax_);
        emit updateYDisplayMin(defaultDisplayedYMin_);
    }

}



void StripToolPV::toForceUpdateValue(const QString &toIgnore)
{
    if (this->pvName() != toIgnore)
    {
        forceUpdate_ = true;
        emit forceUpdate(pvControl_->value());
    }
}



void StripToolPV::toUpdateTime(int newTime)
{
    setTimeDisplayed(newTime);
}



void StripToolPV::toUpdateTimeUnits(const QString &newUnits)
{
    qDebug() << "Updating time units to" << newUnits << "for pv " << this->pvName();
    setXUnits(newUnits);
}
