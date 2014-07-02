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


//#include "StripTool0DVariable.h"
//#include <QVector>

//StripTool0DVariable::StripTool0DVariable(StripTool0DVariableInfo *info, AMDataSource *dataSource, QObject *parent) :
//    StripToolBasicVariable(info, dataSource, parent)
//{
//    info_ = info;

//    updateCount_ = 0;
//    displayedTime_ = 10000; // milliseconds
//    totalVectorSizeCheck();

//    data_ = new MPlotVectorSeriesData();

//    series_ = new StripToolSeries();
//    series_->setModel(data_, true);
//    series_->setDescription(" ");
//    series_->setMarker(MPlotMarkerShape::None);

//    qDebug() << "StripTool0DVariable object created for" << name();
//}



//StripTool0DVariableInfo* StripTool0DVariable::info() const {
//    if (isValid())
//        return info_;
//    else
//        return 0;
//}



//MPlotVectorSeriesData* StripTool0DVariable::data() const {
//    if (isValid())
//        return data_;
//    else
//        return 0;
//}



//StripToolSeries* StripTool0DVariable::series() const {
//    if (isValid())
//        return series_;
//    else
//        return 0;
//}



//MPlotItem* StripTool0DVariable::plotItem() const {
//    MPlotItem* plotItem = series();
//    return plotItem;
//}



//QString StripTool0DVariable::colorName() const {
//    if (isValid())
//        return info()->colorName();
//    else
//        return "";
//}



//int StripTool0DVariable::granularity() const {
//    if (isValid())
//        return info()->granularity();
//    else
//        return -1;
//}



//void StripTool0DVariable::setColorName(const QString &newColor) {
//    if (isValid()) {
//        info()->setColorName(newColor);
//        series()->setLinePen(QPen(QColor(info()->colorName())));
//    }
//}



//void StripTool0DVariable::setGranularity(int newGranularity) {
//    if (isValid())
//        info()->setGranularity(newGranularity);
//}



//void StripTool0DVariable::onDataSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end) {
//    Q_UNUSED(start)
//    Q_UNUSED(end)

//    double newValue = (double) dataSource()->value(AMnDIndex());
//    QTime measurementTime = QTime::currentTime();
////    qDebug() << "Value update for" << name() << ":" << newValue;

//    updateTotalValues(measurementTime, newValue);
//    updateDisplayValues();
//}



//void StripTool0DVariable::totalVectorSizeCheck() {
//    int timesSize = totalUpdateTimes_.size();
//    int valuesSize = totalUpdateValues_.size();
//    int delta = 10;

//    // if the arrays happen to be of different sizes (not sure how this would happen... but just in case), we can fix it.

//    if (timesSize != valuesSize) {
//        qDebug() << "The time and value arrays have become different sizes. Resizing the smaller to match the size of the larger.";

//        if (timesSize > valuesSize) {
//            totalUpdateValues_.resize(timesSize);
//            qDebug() << "Value array adjusted.";

//        } else {
//            totalUpdateTimes_.resize(valuesSize);
//            qDebug() << "Time array adjusted.";
//        }
//    }

//    // we assume now the arrays are of the same length.

//    if (timesSize < updateCount_ + 1) {
//        qDebug() << "Resizing time/data vectors for" << name();
//        totalUpdateTimes_.resize(timesSize + delta);
//        totalUpdateValues_.resize(valuesSize + delta);
//    }
//}



//void StripTool0DVariable::updateTotalValues(QTime latestTime, double latestValue) {
//    updateCount_++;

//    // check that the totalUpdateTimes_/Values_ vectors are sufficiently large to accomodate a new value, resize if not.
//    totalVectorSizeCheck();

//    // we assume now the vectors are large enough. Add new values!
//    totalUpdateTimes_[updateCount_ - 1] = latestTime;
//    totalUpdateValues_[updateCount_ - 1] = latestValue;

//    emit totalVectorsUpdated();

////    // if this variable is the selected variable, its values determine the range of the y axis.
////    if (info()->selectionState() && info()->checkState()) {
////        emit dataRangeUpdated(series()->dataRange());

////        // we check that the display range values for this variable make sense, just in case.
////        qreal max = series()->displayedRange()->max();
////        qreal min = series()->displayedRange()->min();

////        if (max == min) {

////            if (min == 0) {
////                qDebug() << "StripTool0DVariable :: the max and min values of the displayed range retrieved from StripToolSeries are both equal to zero! Setting each +/- 2.";
////                max = 2;
////                min = -2;
////                series()->setCustomLimits(min, max);

////            } else {
////                qDebug() << "StripTool0DVariable :: the max and min values of the displayed range retrieved from StripToolSeries are identical. Scaling each by +/- 5%.";
////                min *= 0.95;
////                max *= 1.05;
////                series()->setCustomLimits(min, max);
////            }
////        }

////        // emit signal containing the range used for the y axis.
////        emit displayRangeUpdated(series()->displayedRange());
////    }

//}



//void StripTool0DVariable::updateDisplayValues() {
//    QTime latestTime = totalUpdateTimes_.at(updateCount_ - 1);

//    // we generate two new vectors to house the subset of values that will be displayed.
//    QVector<double> displayedTimes(0);
//    QVector<double> displayedValues(0);

//    int startIndex = updateCount_ - 1;
//    int index = startIndex;
//    bool displayComplete = false;

//    while (index >= 0 && index <= totalUpdateTimes_.size() && !displayComplete) {
//        QTime measurementTime = totalUpdateTimes_.at(index);
//        double measurement = totalUpdateValues_.at(index);

//        // we want to ignore 'filler' values that don't represent actual measurements.
//        if (measurementTime != QTime()) {
//            double displayTime = latestTime.msecsTo(measurementTime);

//            // if the display time falls within the range we'd like to see on the plot, add the time and value to the display vectors.
//            if (qAbs(displayTime) <= qAbs(displayedTime_)) {
//                displayedTimes.prepend(displayTime);
//                displayedValues.prepend(measurement);

//            } else {
//                // we want to stop considering values that are outside the display range.
//                displayComplete = true;
//            }
//        }

//        index--;
//    }

//    // update the series values plotted on the graph.
//    data()->setValues(displayedTimes, displayedValues);
//}
