/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


//#include "StripToolPV.h"

//StripToolPV::StripToolPV(StripTool0DVariableInfo *info, AMDataSource *dataSource, QObject *parent)
//    : StripToolBasicVariable(info, dataSource, parent)
//{
//    int defaultGranularity = 2;

//    info_ = info;

//    setGranularity(defaultGranularity);

//    updateIndex_ = 0;
//    updateGranularity_ = 2;
//    timeFactor_ = 0.001; // for conversion from different time units (initially ms) into seconds.

//    defaultTimeDisplayed_ = 10; // seconds
//    timeDisplayed_ = defaultTimeDisplayed_;

//    dataVectorSize_ = 100;
//    savePoint_ = 100;

//    xUnits_ = "";

//    isUpdating_ = true;
//    checkState_ = Qt::Checked;
//    color_ = QColor(Qt::red);

//    maxTimeBetweenUpdates_ = 1500; // ms

//    updateIntervalTimer_ = new QTimer(this);
//    connect( updateIntervalTimer_, SIGNAL(timeout()), this, SLOT(toManuallyUpdatePV()) );

//    isSelected_ = false;

//    masterUpdateTimes_ = QVector<QTime>(dataVectorSize_);
//    masterUpdateValues_ = QVector<double>(dataVectorSize_);

//    pvData_ = new MPlotVectorSeriesData();

//    pvSeries_ = new StripToolSeries();
//    pvSeries_->setModel(pvData_, true);
//    pvSeries_->setDescription(" ");
//    pvSeries_->setMarker(MPlotMarkerShape::None);
//    pvSeries_->setLinePen(QPen(color_));

////    setMetaDataHeaders();

////    connect( this, SIGNAL(dataSourceUpdated(double)), this, SLOT(toUpdateValues(double)) );
////    connect( this, SIGNAL(manuallyUpdatePV(double)), this, SLOT(toUpdateValues(double)) );

//    connect( this, SIGNAL(colorNameChanged(QString)), this, SLOT(toSetColor(QString)) );

//    qDebug() << "Instance of StripToolPV created.";
//}



//StripToolPV::~StripToolPV()
//{
//}



//StripTool0DVariableInfo* StripToolPV::info() const {
//    if (isValid())
//        return info_;
//    else
//        return 0;
//}



//QString StripToolPV::xUnits() const
//{
//    return xUnits_;
//}



//Qt::CheckState StripToolPV::checkState()
//{
//    return checkState_;
//}



//QColor StripToolPV::color()
//{
//    return color_;
//}



//int StripToolPV::updateGranularity()
//{
//    return updateGranularity_;
//}



//QString StripToolPV::customDisplayedYMax()
//{
//    if (series()->customMaxDefined()) {
//        return QString::number(series()->customMax());

//    } else {
//        return "";
//    }
//}



//QString StripToolPV::customDisplayedYMin()
//{
//    if (series()->customMinDefined()) {
//        return QString::number(series()->customMin());

//    } else {
//        return "";
//    }
//}



//double StripToolPV::displayedYMin()
//{
//    return series()->displayedMin();
//}



//double StripToolPV::displayedYMax()
//{
//    return series()->displayedMax();
//}



//double StripToolPV::maxTimeBetweenUpdates()
//{
//    return maxTimeBetweenUpdates_;
//}



//bool StripToolPV::isSelected()
//{
//    return isSelected_;
//}


//void StripToolPV::setSelected(bool selected)
//{
//    isSelected_ = selected;
//}



//MPlotVectorSeriesData* StripToolPV::data()
//{
//    return pvData_;
//}



//StripToolSeries* StripToolPV::series() const {
//    return pvSeries_;
//}



//MPlotItem* StripToolPV::plotItem() const
//{
//    return series();
//}



//int StripToolPV::timeDisplayed()
//{
//    return timeDisplayed_;
//}



//QVector<QString> StripToolPV::saveMasterTimes()
//{
//    int position = updateIndex_ - savePoint_; // the index at which we begin saving data.
//    int amount; // the number of values to save.

//    if (position < 0)
//        position = 0;

//    if (masterUpdateTimes_.size() != masterUpdateValues_.size())
//    {
//        amount = 0;
//        qDebug() << "StripToolPV :: The number of time entries do not match the number of data points for : " << name();

//    } else {

//        if (masterUpdateTimes_.size() < savePoint_)
//        {
//            amount = masterUpdateTimes_.size(); // if the number of time entries is less than savePoint_, then return a vector containing all the times we have.

//        } else {
//            amount = savePoint_; // otherwise, return a vector that contains the latest savePoint_ times.
//        }
//    }

//    QVector<QString> toSave;

//    foreach (QTime time, masterUpdateTimes_.mid(position, amount))
//    {
//        toSave.append(time.toString());
//    }

//    return toSave;
//}



//QVector<double> StripToolPV::saveMasterValues()
//{
//    int position = updateIndex_ - savePoint_;
//    int amount = savePoint_;

//    if (position < 0)
//        position = 0;

//    if (masterUpdateValues_.size() != masterUpdateTimes_.size())
//    {
//        amount = 0;
//        qDebug() << "The number of time entries do not match the number of data points for : " << name();

//    } else {
//        amount = savePoint_; // otherwise, return a vector that contains the latest savePoint_ data values.
//    }

//    QVector<double> toSave = masterUpdateValues_.mid(position, amount);
//    return toSave;
//}



//bool StripToolPV::setMetaData(QList<QString> metaData)
//{
////    qDebug() << "StripToolPV :: Attempting to set meta data for pv named" << metaData.at(0);

////    if (metaData.at(0) != name())
////    {
////        qDebug() << "StripToolPV :: The meta data name" << metaData.at(0) << "and pv name" << name() << "don't match!";
////        return false;
////    }

////    if (metaData.size() != metaDataHeaders().size())
////    {
////        qDebug() << "StripToolPV :: The number of meta data entries" << QString::number(metaData.size()) << "and the number of headers" << QString::number(metaDataHeaders().size()) << "don't match!";
////        return false;
////    }

////    QString description = metaData.at(1);
////    QString yUnits = metaData.at(2);
////    QString color = metaData.at(3);
////    int granularity = metaData.at(4).toInt();

////    setDescription(description);
////    setUnits(yUnits);
////    setColor(color);

////    if (granularity > 0) {
////        setUpdateGranularity(QString::number(granularity));
////    } else {
////        setUpdateGranularity(QString::number(2));
////    }

//    return true;
//}



//void StripToolPV::setXUnits(const QString &newUnits)
//{
//    if (newUnits == "")
//        return;

//    xUnits_ = newUnits;

//    if (xUnits_ == "seconds") {
//        timeFactor_ = 0.001; // convert calculated millisecond values to seconds.

//    } else if (xUnits_ == "minutes") {
//        timeFactor_ = 0.001/60.0; // convert calculated millisecond values to minutes.

//    } else if (xUnits_ == "hours") {
//        timeFactor_ = 0.001/3600.0; // convert calculated millisecond values to hours!

//    } else if (xUnits_ == "sec") {
//        timeFactor_ = 0.001;

//    } else if (xUnits_ == "min") {
//        timeFactor_ = 0.001/60.0;

//    } else if (xUnits_ == "hr") {
//        timeFactor_ = 0.001/3600.0;

//    } else {
//        qDebug() << "StripToolPV :: Could not determine correct time factor for these units :" << newUnits;

//    }
//}



//void StripToolPV::setMaxTimeBetweenUpdates(double seconds)
//{
//    maxTimeBetweenUpdates_ = seconds * 1000; // ms
//    emit maxTimeBetweenUpdatesChanged(maxTimeBetweenUpdates_);
//}



//void StripToolPV::setPVUpdating(bool isUpdating)
//{
//    isUpdating_ = isUpdating;
//}



//void StripToolPV::setDisplayedYMax(const QString &newMax)
//{
//    if (newMax == "") {
//        qDebug() << "StripToolPV :: Returning to automatically updating y max for pv" << name();
//        series()->eraseCustomMax();

//    } else if (newMax.toDouble() < displayedYMin()) {
//        qDebug() << "StripToolPV :: New displayed y max for" << name() << "is less than the current displayed y min! No change made.";

//    } else {
//        qDebug() << "StripToolPV :: Setting upper limit on displayed y values for pv" << name() << "to" << newMax;
//        series()->setCustomMax(newMax.toDouble());
//    }
//}



//void StripToolPV::setDisplayedYMin(const QString &newMin)
//{
//    if (newMin == "") {
//        qDebug() << "StripToolPV :: Returning to automatically updating y min for pv " << name();
//        series()->eraseCustomMin();

//    } else if (newMin.toDouble() > displayedYMax()) {
//        qDebug() << "StripToolPV :: New displayed y min for" << name() << "is greater than the current displayed y max! No change made.";

//    } else {
//        qDebug() << "StripToolPV :: Setting lower limit on displayed y values for pv" << name() << "to" << newMin;
//        series()->setCustomMin(newMin.toDouble());
//    }
//}



//void StripToolPV::setTimeDisplayed(int seconds)
//{
//    if (seconds <= 0) {
//        qDebug() << "StripToolPV :: Cannot display a time less than zero seconds! No change made.";
//        return;
//    }

//    timeDisplayed_ = seconds;
//}



//void StripToolPV::setColor(const QString &colorName)
//{
//    if (isValid())
//        this->info()->setColorName(colorName);

//    if (this->info()->hasColor())
//        emit savePVMetaData();

////    color_ = QColor(colorName);
////    series()->setLinePen( QPen(color_) );

////    emit savePVMetaData();
//}



//void StripToolPV::setGranularity(const QString &newVal)
//{
//    if (isValid())
//        info()->setGranularity(newVal.toInt());

////    if (newVal.toInt() <= 0) {
////        qDebug() << "StripToolPV :: Cannot display a pv with update granularity of zero or less! Must be positive, nonzero integer. No change made.";
////        return;
////    }

////    updateGranularity_ = newVal.toInt();
////    qDebug() << "StripToolPV :: Setting new update granularity for pv" << name() << ":" << updateGranularity();
////    emit savePVMetaData();
//}



//void StripToolPV::setGranularity(int newVal) {
//    if (isValid())
//        info()->setGranularity(newVal);
//}


//void StripToolPV::toUpdateTime(int newTime)
//{
//    setTimeDisplayed(newTime);
//}



//void StripToolPV::toUpdateTimeUnits(const QString &newUnits)
//{
//    qDebug() << "StripToolPV :: Updating time units to" << newUnits << "for pv " << name();
//    setXUnits(newUnits);
//}



//void StripToolPV::toManuallyUpdatePV()
//{
//    qDebug() << "Manually updating pv" << name();

//    double newValue;

//    if (isValid())
//        newValue = masterUpdateValues_.at(updateIndex_ - 1);
//    else
//        newValue = 0.0;

////    emit manuallyUpdatePV(newValue);
//}



//void StripToolPV::onInfoChanged() {
//    qDebug() << "The information for" << name() << "has changed.";
//}



//void StripToolPV::saveCheck()
//{
//    if (updateIndex_ > 0 && updateIndex_ % savePoint_ == 0) {

//        emit savePVData();
//    }
//}



//void StripToolPV::dataVectorSizeCheck()
//{
//    if (dataVectorSize_ < updateIndex_ + 1)
//    {
//        dataVectorSize_ += 100;
//        masterUpdateTimes_.resize(dataVectorSize_);
//        masterUpdateValues_.resize(dataVectorSize_);
//    }
//}



//void StripToolPV::onDataSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end)
//{
//    Q_UNUSED(start)
//    Q_UNUSED(end)

////    qDebug() << "StripToolPV :: data source value update detected.";

//    double newValue = (double) dataSource()->value(AMnDIndex());
//    toUpdateValues(newValue);
//}



//void StripToolPV::toUpdateValues(double newValue)
//{
//    // stop the interval timer.
//    updateIntervalTimer_->stop();

//    // check to see if it's time to save the pv data to file.
//    saveCheck();

//    //  check to see if the size of the data vectors allows for a new addition, resize if not.
//    dataVectorSizeCheck();

//    //  vectors are now the correct size, add the new data!
//    QTime latestTime = QTime::currentTime();
//    double latestValue = newValue;

//    masterUpdateTimes_[updateIndex_] = latestTime;
//    masterUpdateValues_[updateIndex_] = latestValue;

//    qDebug() << "StripToolPV::toUpdateValues(...) : PV" << name() << "value update :" << latestValue;

//    //  if the pv is updating on the plot, display the correct updated information.
//    if (isUpdating_ && (updateIndex_ % updateGranularity() == 0)) {

//        // set a 'now' time that will be used to generate the x-axis display values.
//        QTime nowish = QTime::currentTime();

//        // clear the display arrays to prep for new values.
//        displayedTimes_.clear();
//        displayedValues_.clear();

//        // copy all values of masterUpdateTimes_ that are less than timeDisplayed_ to the display vectors.
//        int startIndex = updateIndex_;
//        int index = startIndex;
//        bool copyComplete = false;

//        while (index >= 0 && index < masterUpdateTimes_.size() && !copyComplete) {
//            double relativeTime = nowish.msecsTo(masterUpdateTimes_.at(index)) * timeFactor_; // relative time is initially in seconds, but changes depending on the x axis units.

//            if (qAbs(relativeTime) < qAbs(timeDisplayed_)) {
//                double newTime = relativeTime;
//                double newVal = masterUpdateValues_.at(index);

//                displayedTimes_.append(newTime);
//                displayedValues_.append(newVal);

//                index--;

//            } else {

//                copyComplete = true;
//            }
//        }
//    }

//    qDebug() << "StripToolPV :: Displayed times : " << displayedTimes_;
//    qDebug() << "StripToolPV :: Displayed values : " << displayedValues_;

//    //  update the displayed data with the new vectors.
//    pvData_->setValues(displayedTimes_, displayedValues_);


//    // increment the internal update counter.
//    updateIndex_++;

//    emit pvValueUpdated();

//    // restart the update interval timer.
//    updateIntervalTimer_->start(maxTimeBetweenUpdates_);

//    // if the pv is selected (and plotted) then the axis labels should reflect the data of this pv.
//    if (isSelected() && checkState() == Qt::Checked) {

////        qDebug() << "StripToolPV :: dataRangeChanged emitted with lower limit" << series()->dataRange()->min() << "and upper limit" << series()->dataRange()->max();
//        emit dataRangeChanged(series()->dataRange());

//        qreal max = series()->displayedRange()->max();
//        qreal min = series()->displayedRange()->min();

//        if (max == min) {

//            if (min == 0) {
//                qDebug() << "StripToolPV :: the max and min values of the displayed range retrieved from StripToolSeries are both equal to zero! Setting each +/- 2.";
//                max = 2;
//                min = -2;
//                series()->setCustomLimits(min, max);

//            } else {
//                qDebug() << "StripToolPV :: the max and min values of the displayed range retrieved from StripToolSeries are identical. Scaling each by +/- 5%.";
//                min *= 0.95;
//                max *= 1.05;
//                series()->setCustomLimits(min, max);
//            }
//        }

//        emit displayRangeChanged(new MPlotAxisRange(min, max));
//    }

//}
