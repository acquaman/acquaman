#include "StripToolVariable.h"

StripToolVariable::StripToolVariable(StripToolVariableInfo *info, QObject *parent) :
    QObject(parent)
{
    info_ = 0;
    data_ = 0;
    series_ = 0;

    setInfo(info);

    buildComponents();
    makeConnections();
    defaultSettings();

    qDebug() << "StripToolVariable object created.";
}



QModelIndex StripToolVariable::index() const
{
    return index_;
}



StripToolVariableInfo* StripToolVariable::info() const
{
    return info_;
}



MPlotVectorSeriesData* StripToolVariable::data() const
{
    return data_;
}



StripToolSeries2 *StripToolVariable::series() const
{
    return series_;
}



MPlotSeriesBasic* StripToolVariable::plotItem() const
{
    return series_->series();
}



void StripToolVariable::setIndex(const QModelIndex &index)
{
    if (index_ != QModelIndex() && index == QModelIndex()) {
        qDebug() << "StripToolVariable::setIndex(QModelIndex) : cannot overwrite a valid index with an invalid one. No change made.";
        return;
    }

    index_ = index;
}



void StripToolVariable::setInfo(StripToolVariableInfo *newInfo)
{
    if (info_ != 0) {
        disconnect(info_, SIGNAL(dataSourceValueUpdate(QTime, double)), this, SLOT(onDataSourceValuesChanged(QTime, double)) );

        info_->deleteLater();
        info_ = 0;
    }

    info_ = newInfo;
    info_->setParent(this);

    if (info_ != 0) {
        connect( info_, SIGNAL(dataSourceValueUpdate(QTime, double)), this, SLOT(onDataSourceValuesChanged(QTime, double)) );
    }

    qDebug() << "StripToolVariable :: new info set for variable " << info()->name();
}



void StripToolVariable::onDataSourceValuesChanged(QTime measurementTime, double measurementValue)
{
//    qDebug() << "StripToolVariable :: value update for " << info()->name();
    updateTotalValues(measurementTime, measurementValue);

    if (info()->checkState() == Qt::Checked)
        updateDisplayValues();
}



void StripToolVariable::onDisplayRangeUpdated(const MPlotAxisRange *newRange)
{
    Q_UNUSED(newRange)
//    if (info()->checkState() == Qt::Checked) {
//        qDebug() << "StripToolVariable :: display range updated.";
//        emit displayRangeUpdated(newRange);
//    }
}



void StripToolVariable::totalVectorSizeCheck()
{
    int timesSize = totalUpdateTimes_.size();
    int valuesSize = totalUpdateValues_.size();
    int delta = 100;

    // if the arrays happen to be of different sizes (not sure how this would happen... but just in case), we can fix it.

    if (timesSize != valuesSize) {
        qDebug() << "The time and value arrays have become different sizes. Resizing the smaller to match the size of the larger.";

        if (timesSize > valuesSize) {
            totalUpdateValues_.resize(timesSize);
            qDebug() << "Value array adjusted.";

        } else {
            totalUpdateTimes_.resize(valuesSize);
            qDebug() << "Time array adjusted.";
        }
    }

    // we assume now the arrays are of the same length.

    if (timesSize < updateCount_ + 1) {
        qDebug() << "Resizing time/data vectors for" << info()->name();
        totalUpdateTimes_.resize(timesSize + delta);
        totalUpdateValues_.resize(valuesSize + delta);
    }
}



void StripToolVariable::updateTotalValues(QTime latestTime, double latestValue)
{
    updateCount_++;

    // check that the totalUpdateTimes_/Values_ vectors are sufficiently large to accomodate a new value, resize if not.
    totalVectorSizeCheck();

    // we assume now the vectors are large enough. Add new values!
    totalUpdateTimes_[updateCount_ - 1] = latestTime;
    totalUpdateValues_[updateCount_ - 1] = latestValue;

//    emit dataRangeUpdated(series()->dataRange());
}



void StripToolVariable::updateDisplayValues()
{
    if (updateCount_ > 0) {
        QTime latestTime = totalUpdateTimes_.at(updateCount_ - 1);

        // we generate two new vectors to house the subset of values that will be displayed.
        QVector<double> displayedTimes(0);
        QVector<double> displayedValues(0);
        bool arraysComplete = false;

        int startIndex = updateCount_ - 1;
        int index = startIndex;

        while (index >= 0 && index < totalUpdateTimes_.size() && !arraysComplete) {

            QTime measurementTime = totalUpdateTimes_.at(index);
            double measurement = totalUpdateValues_.at(index);

            // we want to ignore 'filler' values that don't represent actual measurements.
            if (measurementTime != QTime()) {
                double displayTimeMS = latestTime.msecsTo(measurementTime);

                // if the display time falls within the range we'd like to see on the plot, add the time and value to the display vectors.
                if (qAbs(displayTimeMS) <= qAbs(info()->displayedTimeMS())) {
                    displayedTimes.prepend(info()->timeMSToTimeUnits(displayTimeMS)); // convert the millisecond time to amount considering current units.
                    displayedValues.prepend(measurement);

                } else {
                    // we want to stop considering values that are outside the display range.
                    arraysComplete = true;
                }
            }

            index--;
        }

        // update the series values plotted on the graph.
        data()->setValues(displayedTimes, displayedValues);
        emit displayRangeUpdated(series()->displayedRange());
    }




    //    // if this variable is the selected variable, its values determine the range of the y axis.
    //    if (info()->selectionState() && info()->checkState()) {
    //        emit dataRangeUpdated(series()->dataRange());

    //        // we check that the display range values for this variable make sense, just in case.
    //        qreal max = series()->displayedRange()->max();
    //        qreal min = series()->displayedRange()->min();

    //        if (max == min) {

    //            if (min == 0) {
    //                qDebug() << "StripTool0DVariable :: the max and min values of the displayed range retrieved from StripToolSeries are both equal to zero! Setting each +/- 2.";
    //                max = 2;
    //                min = -2;
    //                series()->setCustomLimits(min, max);

    //            } else {
    //                qDebug() << "StripTool0DVariable :: the max and min values of the displayed range retrieved from StripToolSeries are identical. Scaling each by +/- 5%.";
    //                min *= 0.95;
    //                max *= 1.05;
    //                series()->setCustomLimits(min, max);
    //            }
    //        }

    //        // emit signal containing the range used for the y axis.
    //        emit displayRangeUpdated(series()->displayedRange());
    //    }
}



void StripToolVariable::buildComponents()
{
    data_ = new MPlotVectorSeriesData();

    series_ = new StripToolSeries2(this);

    plotItem()->setModel(data_, true);
    plotItem()->setDescription(" ");
    plotItem()->setMarker(MPlotMarkerShape::None);

}



void StripToolVariable::makeConnections()
{
    // info connections made in 'setInfo'
}



void StripToolVariable::defaultSettings()
{
    index_ = QModelIndex();
    updateCount_ = 0;
//    displayedTime_ = 1000; // milliseconds

    info()->setCreationDateTime(QDateTime::currentDateTime().toString());
}
