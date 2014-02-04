#include "StripToolPV.h"

StripToolPV::StripToolPV(QObject *parent)
    : StripToolVariable(parent)
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

    isUpdating_ = true;
    checkState_ = Qt::Checked;
    pvColor_ = QColor(Qt::red);

    maxTimeBetweenUpdates_ = 1500; // ms

    updateIntervalTimer_ = new QTimer(this);
    connect( updateIntervalTimer_, SIGNAL(timeout()), this, SLOT(toManuallyUpdatePV()) );
    connect( this, SIGNAL(stopUpdateIntervalTimer()), updateIntervalTimer_, SLOT(stop()) );
    updateIntervalTimer_->start(maxTimeBetweenUpdates_);

    isSelected_ = false;

    dateCreated_ = QDateTime::currentDateTime().toString();

    masterUpdateTimes_ = QVector<QTime>(dataVectorSize_);
    masterUpdateValues_ = QVector<double>(dataVectorSize_);

    pvControl_ = 0;

    pvData_ = new MPlotVectorSeriesData();

    pvSeries_ = new StripToolSeries();
    pvSeries_->setModel(pvData_, true);
    pvSeries_->setDescription(" ");
    pvSeries_->setMarker(MPlotMarkerShape::None);
    pvSeries_->setLinePen(QPen(pvColor_));

    setMetaDataHeaders();

    connect( this, SIGNAL(manuallyUpdatePV(double)), this, SLOT(onPVValueChanged(double)) );
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



QString StripToolPV::customDisplayedYMax()
{
    if (series()->customMaxDefined()) {
        return QString::number(series()->customMax());

    } else {
        return "";
    }
}



QString StripToolPV::customDisplayedYMin()
{
    if (series()->customMinDefined()) {
        return QString::number(series()->customMin());

    } else {
        return "";
    }
}



double StripToolPV::displayedYMin()
{
    return series()->displayedMin();
}



double StripToolPV::displayedYMax()
{
    return series()->displayedMax();
}



double StripToolPV::shiftAmount()
{
    return shiftAmount_;
}



double StripToolPV::maxTimeBetweenUpdates()
{
    return maxTimeBetweenUpdates_;
}



bool StripToolPV::isSelected()
{
    return isSelected_;
}


void StripToolPV::setSelected(bool selected)
{
    isSelected_ = selected;
}



MPlotVectorSeriesData* StripToolPV::data()
{
    return pvData_;
}



StripToolSeries* StripToolPV::series()
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
        qDebug() << "StripToolPV :: The number of time entries do not match the number of data points for : " << pvName();

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
    editPVData << customDisplayedYMax();
    editPVData << customDisplayedYMin();
    editPVData << QString::number(shiftAmount_);

    qDebug() << "StripToolPV :: PV information to be displayed in EditPVDialog : " << editPVData;


    return editPVData;
}



bool StripToolPV::setMetaData(QList<QString> metaData)
{
    qDebug() << "StripToolPV :: Attempting to set meta data for pv named" << metaData.at(0);

    if (metaData.at(0) != pvName())
    {
        qDebug() << "StripToolPV :: The meta data name" << metaData.at(0) << "and pv name" << pvName() << "don't match!";
        return false;
    }

    if (metaData.size() != metaDataHeaders().size())
    {
        qDebug() << "StripToolPV :: The number of meta data entries" << QString::number(metaData.size()) << "and the number of headers" << QString::number(metaDataHeaders().size()) << "don't match!";
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
        setUpdateGranularity(QString::number(granularity));
    } else {
        setUpdateGranularity(QString::number(2));
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
    connect( pvControl_, SIGNAL(valueChanged(double)), this, SLOT(toRestartUpdateIntervalTimer()) );
}



void StripToolPV::setDescription(const QString &newDescription)
{
    if (newDescription != "")
        pvDescription_ = newDescription;

    else
        pvDescription_ = pvName();

    qDebug() << "StripToolPV :: Setting new description for pv" << pvName() << ":" << pvDescription_;

    emit savePVMetaData();
    emit descriptionChanged(pvDescription_);
}



void StripToolPV::setYUnits(const QString &newUnits)
{
    qDebug() << "StripToolPV :: Setting new units for pv" << pvName() << ":" << newUnits;

    yUnits_ = newUnits;

    emit savePVMetaData();
    emit unitsChanged(yUnits_);
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
        qDebug() << "StripToolPV :: Could not determine correct time factor for these units :" << newUnits;

    }
}



void StripToolPV::setMaxTimeBetweenUpdates(double seconds)
{
    maxTimeBetweenUpdates_ = seconds * 1000; // ms
    emit maxTimeBetweenUpdatesChanged(maxTimeBetweenUpdates_);
}



void StripToolPV::setPVUpdating(bool isUpdating)
{
    isUpdating_ = isUpdating;
}



void StripToolPV::setDisplayedYMax(const QString &newMax)
{
    if (newMax == "") {
        qDebug() << "StripToolPV :: Returning to automatically updating y max for pv" << pvName();
        series()->eraseCustomMax();

    } else if (newMax.toDouble() < displayedYMin()) {
        qDebug() << "StripToolPV :: New displayed y max for" << pvName() << "is less than the current displayed y min! No change made.";

    } else {
        qDebug() << "StripToolPV :: Setting upper limit on displayed y values for pv" << pvName() << "to" << newMax;
        series()->setCustomMax(newMax.toDouble());
    }
}



void StripToolPV::setDisplayedYMin(const QString &newMin)
{
    if (newMin == "") {
        qDebug() << "StripToolPV :: Returning to automatically updating y min for pv " << pvName();
        series()->eraseCustomMin();

    } else if (newMin.toDouble() > displayedYMax()) {
        qDebug() << "StripToolPV :: New displayed y min for" << pvName() << "is greater than the current displayed y max! No change made.";

    } else {
        qDebug() << "StripToolPV :: Setting lower limit on displayed y values for pv" << pvName() << "to" << newMin;
        series()->setCustomMin(newMin.toDouble());
    }
}



void StripToolPV::setTimeDisplayed(int seconds)
{
    if (seconds <= 0) {
        qDebug() << "StripToolPV :: Cannot display a time less than zero seconds! No change made.";
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
        qDebug() << "StripToolPV :: New color selection for" << pvName() << "is invalid. No change made.";
        return;
    }

    pvColor_ = color;
    series()->setLinePen( QPen(pvColor_) );
    qDebug() << "Setting new color for pv" << pvName() << ":" << color.name();
    emit savePVMetaData();
}



void StripToolPV::setUpdateGranularity(const QString &newVal)
{
    if (newVal.toInt() <= 0) {
        qDebug() << "StripToolPV :: Cannot display a pv with update granularity of zero or less! Must be positive, nonzero integer. No change made.";
        return;
    }

    updateGranularity_ = newVal.toInt();
    qDebug() << "StripToolPV :: Setting new update granularity for pv" << pvName() << ":" << updateGranularity();
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

    qDebug() << "PV" << pvName() << "value update :" << latestValue;

    //  if the pv is updating on the plot, display the correct updated information.
    if (isUpdating_ && (updateIndex_ % updateGranularity() == 0)) {
        // set a 'now' time that will be used to generate the x-axis display values.
        QTime nowish = QTime::currentTime();

        // clear the display arrays to prep for new values.
        displayedTimes_.clear();
        displayedValues_.clear();

        // copy all values of masterUpdateTimes_ that are less than timeDisplayed_ to the display vectors.
        int startIndex = updateIndex_;
        int index = startIndex;
        bool copyComplete = false;

        while (index >= 0 && index < masterUpdateTimes_.size() && !copyComplete) {
            double relativeTime = nowish.msecsTo(masterUpdateTimes_.at(index)) * timeFactor_; // relative time is initially in seconds, but changes depending on the x axis units.

            if (qAbs(relativeTime) < qAbs(timeDisplayed_)) {
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

    qDebug() << "StripToolPV :: Displayed times : " << displayedTimes_;
    qDebug() << "StripToolPV :: Displayed values : " << displayedValues_;

    //  update the displayed data with the new vectors.
    pvData_->setValues(displayedTimes_, displayedValues_);


    // increment the internal update counter.
    updateIndex_++;

    emit pvValueUpdated();

    // if the pv is selected (and plotted) then the axis labels should reflect the data of this pv.
    if (isSelected() && checkState() == Qt::Checked) {

//        qDebug() << "StripToolPV :: dataRangeChanged emitted with lower limit" << series()->dataRange()->min() << "and upper limit" << series()->dataRange()->max();
        emit dataRangeChanged(series()->dataRange());

        qreal max = series()->displayedRange()->max();
        qreal min = series()->displayedRange()->min();

        if (max == min) {

            if (min == 0) {
                qDebug() << "StripToolPV :: the max and min values of the displayed range retrieved from StripToolSeries are both equal to zero! Setting each +/- 2.";
                max = 2;
                min = -2;
                series()->setCustomLimits(min, max);

            } else {
//                qDebug() << "StripToolPV :: the max and min values of the displayed range retrieved from StripToolSeries are identical. Scaling each by +/- 5%.";
//                min *= 0.95;
//                max *= 1.05;
//                series()->setCustomLimits(min, max);
            }
        }

        emit displayRangeChanged(new MPlotAxisRange(min, max));
    }

}



void StripToolPV::toRestartUpdateIntervalTimer()
{
    emit stopUpdateIntervalTimer();
    updateIntervalTimer_->start(maxTimeBetweenUpdates_);
}



void StripToolPV::toManuallyUpdatePV()
{
    double lastValue = masterUpdateValues_.at(updateIndex_ - 1);
    emit manuallyUpdatePV(lastValue);
}



void StripToolPV::toUpdateTime(int newTime)
{
    setTimeDisplayed(newTime);
}



void StripToolPV::toUpdateTimeUnits(const QString &newUnits)
{
    qDebug() << "StripToolPV :: Updating time units to" << newUnits << "for pv " << this->pvName();
    setXUnits(newUnits);
}
