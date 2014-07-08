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


#include "AM0DTimestampAB.h"

AM0DTimestampAB::AM0DTimestampAB(const QString &outputName, QObject *parent) :
    AMStandardAnalysisBlock(outputName, parent)
{
    dataStored_ = QList<QDateTime>();
    dataMax_ = 50;

    timeValue_ = 10;
    timeUnits_ = Seconds;

    timeFilteringEnabled_ = false;

    axes_ << AMAxisInfo("invalid", 0, "No input data");
    setState(AMDataSource::InvalidFlag);

}

AM0DTimestampAB::~AM0DTimestampAB()
{

}

QList<QDateTime> AM0DTimestampAB::dataStored() const
{
    return dataStored_;
}

int AM0DTimestampAB::dataStoredCount() const
{
    return dataStored_.size();
}

int AM0DTimestampAB::dataStoredCountMax() const
{
    return dataMax_;
}

AM0DTimestampAB::TimeUnits AM0DTimestampAB::timeUnits() const
{
    return timeUnits_;
}

int AM0DTimestampAB::timeValue() const
{
    return timeValue_;
}

bool AM0DTimestampAB::timeFilteringEnabled() const
{
    return timeFilteringEnabled_;
}

bool AM0DTimestampAB::areInputDataSourcesAcceptable(const QList<AMDataSource *> &dataSources) const
{
    // null input is acceptable.
    if (dataSources.isEmpty())
        return true;

    // we expect to receive maximum one input data source.
    if (dataSources.size() > 1)
        return false;

    // we expect the source to be of rank 0.
    if (dataSources.at(0)->rank() != 0)
        return false;

    return true;
}

void AM0DTimestampAB::setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources)
{
    // disconnect connections from old sources, if they exist.
    if (!sources_.isEmpty()) {
        disconnect( sources_.at(0)->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)) );
        disconnect( sources_.at(0)->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourcesStateChanged()) );
    }

    // set to invalid if new dataSources is empty.
    if (dataSources.isEmpty()) {
        sources_.clear();
        axes_[0] = AMAxisInfo("invalid", 0, "No input data");
        setDescription("-- No input data --");

    // if data source is valid, set sources_, axis info, description, and connections.
    } else {

        sources_ = dataSources;
        axes_[0] = sources_.at(0)->axisInfoAt(0);
        setDescription(QString("Timestamps for %1 value updates").arg(sources_.at(0)->name()));

        connect( sources_.at(0)->signalSource(), SIGNAL(valuesChanged(AMnDIndex, AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex, AMnDIndex)) );
        connect( sources_.at(0)->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourcesStateChanged()) );
    }

    reviewState();

    emitSizeChanged(0);
    emitValuesChanged();
    emitAxisInfoChanged(0);
    emitInfoChanged();
}

AMNumber AM0DTimestampAB::value(const AMnDIndex &indexes) const
{
    if(indexes.rank() != 1)
        return AMNumber(AMNumber::DimensionError);

    if(!isValid())
        return AMNumber(AMNumber::InvalidError);

    if (indexes.i() < 0 || indexes.i() >= dataStored_.size())
        return AMNumber(AMNumber::OutOfBoundsError);

    return convertMS(latestUpdate_.msecsTo(dataStored_.at(indexes.i())), timeUnits_);
}

bool AM0DTimestampAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
    if(!isValid())
        return false;

    if (indexStart.i() < 0 || indexStart.i() >= dataStored_.size())
        return false;

    if (indexEnd.i() < 0 || indexEnd.i() >= dataStored_.size())
        return false;

    if (indexStart.i() > indexEnd.i())
        return false;

    if(indexStart.rank() != 1 || indexEnd.rank() != 1)
        return false;

    int totalSize = indexStart.totalPointsTo(indexEnd);

    for (int i = 0; i < totalSize; i++) {
        outputValues[i] = (double)value(AMnDIndex(indexStart.i() + i));
    }

    return true;
}

AMNumber AM0DTimestampAB::axisValue(int axisNumber, int index) const
{
    if (!isValid())
        return AMNumber(AMNumber::InvalidError);

    if (axisNumber != 0)
        return AMNumber(AMNumber::DimensionError);

    if (index < 0 || index >= dataStored_.size())
        return AMNumber(AMNumber::OutOfBoundsError);

    return index;
}

bool AM0DTimestampAB::loadFromDb(AMDatabase *db, int id)
{
    bool success = AMDbObject::loadFromDb(db, id);

    if (success) {
        AMDataSource::name_ = AMDbObject::name();
    }

    return success;
}

void AM0DTimestampAB::setTimeUnits(TimeUnits newUnits)
{
    if (timeUnits_ != newUnits) {
        timeUnits_ = newUnits;
        emit timeUnitsChanged(timeUnits_);
    }
}

void AM0DTimestampAB::setTimeValue(int newValue)
{
    if (timeValue_ != newValue && newValue > 0) {
        timeValue_ = newValue;
        emit timeValueChanged(timeValue_);
    }
}

void AM0DTimestampAB::setDataStoredCountMax(int newMax)
{
    if (dataMax_ != newMax && newMax > 0) {

        if (newMax < dataStored_.size()) {
            dataStored_ = dataStored_.mid(dataStored_.size() - 1 - newMax, newMax);
        }

        dataMax_ = newMax;
        emit dataStoredMaxChanged(dataMax_);
    }
}

void AM0DTimestampAB::enableTimeFiltering(bool isEnabled)
{
    if (timeFilteringEnabled_ != isEnabled) {
        timeFilteringEnabled_ = isEnabled;
        emit timeFilteringEnabled(timeFilteringEnabled_);
    }
}

void AM0DTimestampAB::onInputSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end)
{
    Q_UNUSED(start)
    Q_UNUSED(end)

    latestUpdate_ = QDateTime::currentDateTime();

    // if we are less than the data stored max, append the new value to the end of the data stored list.
    if (dataStored_.size() <= dataMax_) {
        dataStored_.append(latestUpdate_);

    } else {
        dataStored_.removeFirst();
        dataStored_.append(latestUpdate_);
    }

    reviewValuesChanged(start, end);
}

void AM0DTimestampAB::onInputSourcesStateChanged()
{
    reviewState();
}

double AM0DTimestampAB::convertMS(double msecVal, TimeUnits newUnit) const
{
    double result = 0;

    if (newUnit == mSeconds) {
        result = msecVal;

    } else if (newUnit == Seconds) {
        result = msecVal / 1000;

    } else if (newUnit == Minutes) {
        result = msecVal / 1000 / 60;

    } else if (newUnit == Hours) {
        result = msecVal / 1000 / 60 / 60;
    }

    return result;
}

void AM0DTimestampAB::reviewValuesChanged(const AMnDIndex &start, const AMnDIndex &end)
{
    Q_UNUSED(start)
    Q_UNUSED(end)

    int totalPoints = dataStored_.size();

    AMnDIndex newStart = AMnDIndex(0);
    AMnDIndex newEnd = AMnDIndex(totalPoints - 1);

    if (timeFilteringEnabled_) {

        bool newStartFound = false;
        int i = 0;

        while (!newStartFound && i < totalPoints) {
            int index = newEnd.i() - i;
            double indexValue = (double)value(AMnDIndex(index));

            if (qAbs(indexValue) > qAbs(timeValue_)) {
                newStart = AMnDIndex(index + 1);
                newStartFound = true;
            }

            i++;
        }
    }

    axes_[0] = AMAxisInfo(sources_.at(0)->name(), newEnd.i() - newStart.i());

    emitSizeChanged(0);
    emitValuesChanged(newStart, newEnd);
    emitAxisInfoChanged(0);

}

void AM0DTimestampAB::reviewState()
{
    // are there data sources?
    if (sources_.isEmpty()) {
        setState(AMDataSource::InvalidFlag);
        return;
    }

    // is there one data source?
    if (sources_.size() != 1) {
        setState(AMDataSource::InvalidFlag);
        return;
    }

    // check that data source is valid.
    if (!sources_.at(0)->isValid()) {
        setState(AMDataSource::InvalidFlag);
        return;
    }

    setState(0);
}
