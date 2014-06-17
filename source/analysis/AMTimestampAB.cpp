#include "AMTimestampAB.h"

AMTimestampAB::AMTimestampAB(const QString &outputName, QObject *parent) :
    AMStandardAnalysisBlock(outputName, parent)
{
    axes_ << AMAxisInfo("invalid", 0, "No input data");
    setState(AMDataSource::InvalidFlag);

    dataStored_ = QVector<QDateTime>();

    setTimeUnits(msec);
}

AMTimestampAB::~AMTimestampAB()
{

}

QVector<QDateTime> AMTimestampAB::dataStored() const
{
    return dataStored_;
}

int AMTimestampAB::dataCount() const
{
    return dataStored_.size();
}

AMTimestampAB::TimeUnits AMTimestampAB::timeUnits() const
{
    return units_;
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
        connect( sources_.at(0)->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()) );
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

    return msecTo(latestUpdate_.msecsTo(dataStored_.at(index)), units_);
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
    if (units_ != newUnits) {
        units_ = newUnits;
        emit timeUnitsChanged(units_);
    }
}

void AMTimestampAB::onInputSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end)
{
    Q_UNUSED(start)
    Q_UNUSED(end)

    latestUpdate_ = QDateTime::currentDateTime();
    dataStored_.append(latestUpdate_);

    // update the axes info for this data source to reflect the (possibly) new number of points to plot.
    axes_[0] = AMAxisInfo(sources_.at(0)->name(), dataStored_.size());

    emitValuesChanged();
    emitSizeChanged(0);
}

void AMTimestampAB::onInputSourceStateChanged()
{
    reviewState();
}

double AMTimestampAB::msecTo(double msecVal, TimeUnits newUnit) const
{
    double result = 0;

    if (newUnit == msec) {
        result = msecVal;

    } else if (newUnit == sec) {
        result = msecVal / 1000;

    } else if (newUnit == min) {
        result = msecVal / 1000 / 60;

    } else if (newUnit == hour) {
        result = msecVal / 1000 / 60 / 60;
    }

    return result;
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
