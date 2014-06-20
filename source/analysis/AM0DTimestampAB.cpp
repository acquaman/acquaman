#include "AM0DTimestampAB.h"

AM0DTimestampAB::AM0DTimestampAB(const QString &outputName, QObject *parent) :
    AMStandardAnalysisBlock(outputName, parent)
{
    axes_ << AMAxisInfo("invalid", 0, "No input data");
    setState(AMDataSource::InvalidFlag);

    dataStored_ = QList<QDateTime>();
    dataMax_ = 50;

    setTimeUnits(mSeconds);
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

bool AM0DTimestampAB::areInputDataSourcesAcceptable(const QList<AMDataSource *> &dataSources) const
{
    // null input is acceptable.
    if (dataSources.isEmpty())
        return true;

    // we expect to receive maximum one input data source.
    if (dataSources.size() > 1)
        return false;

    // we expect the source to be of rank 0 (though maybe this shouldn't be a requirement?)
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

        sources_ = QList<AMDataSource*>() << dataSources.at(0);
        axes_[0] = sources_.at(0)->axisInfoAt(0);
        setDescription(QString("Timestamps for %1 value updates").arg(sources_.at(0)->name()));

        connect( sources_.at(0)->signalSource(), SIGNAL(valuesChanged(AMnDIndex, AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex, AMnDIndex)) );
        connect( sources_.at(0)->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourcesStateChanged()) );
    }

    reviewState();

    emitSizeChanged();
    emitValuesChanged();
    emitAxisInfoChanged();
    emitInfoChanged();
}

AMNumber AM0DTimestampAB::value(const AMnDIndex &indexes) const
{
    if(indexes.rank() != 1)
        return AMNumber(AMNumber::DimensionError);

    if(!isValid())
        return AMNumber(AMNumber::InvalidError);

    for (int i = 0; i < sources_.size(); i++)
        if (indexes.i() >= dataStored_.size())
            return AMNumber(AMNumber::OutOfBoundsError);

    return msecTo(latestUpdate_.msecsTo(dataStored_.at(indexes.i())), timeUnits_);
}

bool AM0DTimestampAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
    if(indexStart.rank() != 1 || indexEnd.rank() != 1)
        return false;

    if(!isValid())
        return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
    for (int i = 0; i < sources_.size(); i++)
        if ((unsigned)indexEnd.i() >= (unsigned)axes_.at(0).size)
            return false;

    if ((unsigned)indexStart.i() > (unsigned)indexEnd.i())
        return false;
#endif

    int totalSize = indexStart.totalPointsTo(indexEnd);

    for (int i = 0; i < totalSize; i++) {
        outputValues[i] = value(AMnDIndex(i + indexStart.i()));
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

void AM0DTimestampAB::onInputSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end)
{
    Q_UNUSED(start)
    Q_UNUSED(end)

    latestUpdate_ = QDateTime::currentDateTime();
    appendToDataStored(latestUpdate_);
}

void AM0DTimestampAB::onInputSourcesStateChanged()
{
    reviewState();
}

void AM0DTimestampAB::appendToDataStored(QDateTime newDateTime)
{
    // if we are less than the data stored max, append the new value to the end of the data stored list.
    if (dataStored_.size() <= dataMax_) {
        dataStored_.append(newDateTime);

    } else {
        dataStored_.removeFirst();
        dataStored_.append(newDateTime);
    }

    // update the axes info for this data source to reflect the (possibly) new number of points to plot.
    axes_[0] = AMAxisInfo(sources_.at(0)->name(), dataStored_.size());

    emitValuesChanged();
    emitAxisInfoChanged();
    emitSizeChanged();
}

double AM0DTimestampAB::msecTo(double msecVal, TimeUnits newUnit) const
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

int AM0DTimestampAB::pointsInTimeWindow(int time, TimeUnits units)
{
    int i;

    for (i = 0; i < dataStored_.size(); i++) {
        if ( msecTo(latestUpdate_.msecsTo(dataStored_.at(i)), units) <= time ) {
            break;
        }
    }

    return dataStored_.mid(i).size();
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
