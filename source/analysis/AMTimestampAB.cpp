#include "AMTimestampAB.h"

AMTimestampAB::AMTimestampAB(const QString &outputName, QObject *parent) :
    AMStandardAnalysisBlock(outputName, parent)
{
    axes_ << AMAxisInfo("invalid", 0, "No input data");
    setState(AMDataSource::InvalidFlag);

    dataStored_ = QList<QDateTime>();
    dataMax_ = 10;

    setTimeUnits(mSeconds);
}

AMTimestampAB::~AMTimestampAB()
{

}

QList<QDateTime> AMTimestampAB::dataStored() const
{
    return dataStored_;
}

int AMTimestampAB::dataStoredCount() const
{
    return dataStored_.size();
}

int AMTimestampAB::dataStoredCountMax() const
{
    return dataMax_;
}

AMTimestampAB::TimeUnits AMTimestampAB::timeUnits() const
{
    return timeUnits_;
}

bool AMTimestampAB::areInputDataSourcesAcceptable(const QList<AMDataSource *> &dataSources) const
{
    // null input is acceptable.
    if (dataSources.isEmpty())
        return true;

    // we expect to receive maximum one input data source.
    if (dataSources.size() > 1)
        return false;

    return true;
}

void AMTimestampAB::setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources)
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

    emitSizeChanged(0);
    emitValuesChanged();
    emitAxisInfoChanged(0);
    emitInfoChanged();
}

AMNumber AMTimestampAB::value(const AMnDIndex &indexes) const
{
    return sources_.at(0)->value(indexes);
}

bool AMTimestampAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
    return sources_.at(0)->values(indexStart, indexEnd, outputValues);
}

AMNumber AMTimestampAB::axisValue(int axisNumber, int index) const
{
    if (!isValid())
        return AMNumber(AMNumber::InvalidError);

    if (axisNumber != 0)
        return AMNumber(AMNumber::DimensionError);

    if (index < 0 || index >= dataStored_.size())
        return AMNumber(AMNumber::OutOfBoundsError);

    if (!latestUpdate_.isValid())
        return AMNumber(AMNumber::InvalidError);

    return msecTo(latestUpdate_.msecsTo(dataStored_.at(index)), timeUnits_);
}

bool AMTimestampAB::loadFromDb(AMDatabase *db, int id)
{
    bool success = AMDbObject::loadFromDb(db, id);

    if (success) {
        AMDataSource::name_ = AMDbObject::name();
    }

    return success;
}

void AMTimestampAB::setTimeUnits(TimeUnits newUnits)
{
    if (timeUnits_ != newUnits) {
        timeUnits_ = newUnits;
        emit timeUnitsChanged(timeUnits_);
    }
}

void AMTimestampAB::setDataStoredCountMax(int newMax)
{
    if (dataMax_ != newMax && newMax > 0) {

        if (newMax < dataStored_.size()) {
            dataStored_ = dataStored_.mid(dataStored_.size() - 1 - newMax, newMax);
        }

        dataMax_ = newMax;
        emit dataStoredMaxChanged(dataMax_);
    }
}

void AMTimestampAB::onInputSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end)
{
    Q_UNUSED(start)
    Q_UNUSED(end)

    latestUpdate_ = QDateTime::currentDateTime();
    appendToDataStored(latestUpdate_);
}

void AMTimestampAB::onInputSourcesStateChanged()
{
    reviewState();
}

void AMTimestampAB::appendToDataStored(QDateTime newDateTime)
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
    emitSizeChanged(0);
}

double AMTimestampAB::msecTo(double msecVal, TimeUnits newUnit) const
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

int AMTimestampAB::pointsInTimeWindow(int time, TimeUnits units)
{
    int i;

    for (i = 0; i < dataStored_.size(); i++) {
        if ( msecTo(latestUpdate_.msecsTo(dataStored_.at(i)), units) <= time ) {
            break;
        }
    }

    return dataStored_.mid(i).size();
}

void AMTimestampAB::reviewState()
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
