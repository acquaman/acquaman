#include "AM0DAccumulatorAB.h"

AM0DAccumulatorAB::AM0DAccumulatorAB(const QString &outputName, QObject *parent) : AMStandardAnalysisBlock(outputName, parent)
{
    axes_ << AMAxisInfo("invalid", 0, "No input data");
    setState(AMDataSource::InvalidFlag);

    dataStored_ = QList<double>();
    dataMax_ = 10;
}

AM0DAccumulatorAB::~AM0DAccumulatorAB()
{

}

QList<double> AM0DAccumulatorAB::dataStored() const
{
    return dataStored_;
}

int AM0DAccumulatorAB::dataStoredCount() const
{
    return dataStored_.size();
}

int AM0DAccumulatorAB::dataStoredCountMax() const
{
    return dataMax_;
}

bool AM0DAccumulatorAB::areInputDataSourcesAcceptable(const QList<AMDataSource *> &dataSources) const
{
    // null input is acceptable.
    if (dataSources.isEmpty())
        return true;

    // we expect to receive maximum one input data source.
    if (dataSources.size() > 1)
        return false;

    // the data source must have rank zero.
    if (dataSources.at(0)->rank() != 0)
        return false;

    return true;
}

void AM0DAccumulatorAB::setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources)
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
        setDescription(QString("Model for %1").arg(sources_.at(0)->name()));

        connect( sources_.at(0)->signalSource(), SIGNAL(valuesChanged(AMnDIndex, AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex, AMnDIndex)) );
        connect( sources_.at(0)->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()) );
    }

    reviewState();

    emitSizeChanged();
    emitValuesChanged();
    emitAxisInfoChanged();
    emitInfoChanged();
}

AMNumber AM0DAccumulatorAB::value(const AMnDIndex &indexes) const
{
    if (indexes.rank() != 1)
        return AMNumber(AMNumber::DimensionError);

    if (!isValid())
        return AMNumber(AMNumber::InvalidError);

    int index = indexes.i();

    if (index < 0 || index >= dataStored_.size())
        return AMNumber(AMNumber::OutOfBoundsError);

    return dataStored_.at(index);
}

bool AM0DAccumulatorAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
    if (indexStart.rank() != 1 || indexEnd.rank() != 1)
        return false;

    if (!isValid())
        return false;

    int start = indexStart.i();
    int end = indexEnd.i();

    if (start > end)
        return false;

    if (start < 0 || end < 0)
        return false;

    if (start >= dataStored_.size() || end >= dataStored_.size())
        return false;

    // set output values to point to the subset of points saved for the data source.
    for (int i = 0; i <= end - start; i++) {
        outputValues[i] = dataStored_.at(start + i);
    }

    return true;
}

AMNumber AM0DAccumulatorAB::axisValue(int axisNumber, int index) const
{
    if (!isValid())
        return AMNumber(AMNumber::InvalidError);

    if (axisNumber != 0)
        return AMNumber(AMNumber::DimensionError);

    if (index < 0 || index >= dataStored_.size())
        return AMNumber(AMNumber::OutOfBoundsError);

    return index;
}

bool AM0DAccumulatorAB::loadFromDb(AMDatabase *db, int id)
{
    bool success = AMDbObject::loadFromDb(db, id);

    if (success) {
        AMDataSource::name_ = AMDbObject::name();
    }

    return success;
}

void AM0DAccumulatorAB::onInputSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end)
{
    Q_UNUSED(start)
    Q_UNUSED(end)

    // update the values stored for this source. We assume the source is 0D, so the latest value is always recovered with an empty AMnDIndex.
    double newValue = sources_.at(0)->value(AMnDIndex());

    // if we are less than the data stored max, append the new value to the end of the data stored list.
    if (dataStored_.size() <= dataMax_) {
        dataStored_.append(newValue);

    } else {
        dataStored_.removeFirst();
        dataStored_.append(newValue);
    }

    // update the axes info for this data source to reflect the (possibly) new number of points to plot.
    axes_[0] = AMAxisInfo(sources_.at(0)->name(), dataStored_.size());

    emitValuesChanged();
    emitSizeChanged();
}

void AM0DAccumulatorAB::onInputSourceStateChanged()
{
    reviewState();
}

void AM0DAccumulatorAB::reviewState()
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

    // does the data source have rank 0?
    if (sources_.at(0)->rank() != 0) {
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
