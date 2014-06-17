#include "AM0DAccumulatorAB.h"

AM0DAccumulatorAB::AM0DAccumulatorAB(const QString &outputName, QObject *parent) : AMStandardAnalysisBlock(outputName, parent)
{
    axes_ << AMAxisInfo("invalid", 0, "No input data");
    setState(AMDataSource::InvalidFlag);

    dataStored_ = QVector<double>();

    updatesStored_ = 0;
    setMaximumStored(10);
}

AM0DAccumulatorAB::~AM0DAccumulatorAB()
{

}

QVector<double> AM0DAccumulatorAB::dataStored() const
{
    return dataStored_;
}

int AM0DAccumulatorAB::updatesStored() const
{
    return updatesStored_;
}

int AM0DAccumulatorAB::maximumStored() const
{
    return dataStored_.size();
}

int AM0DAccumulatorAB::updateCount() const
{
    return updateCount_;
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

        sources_ = QList<AMDataSource*>() << dataSources.at(0);
        axes_[0] = sources_.at(0)->axisInfoAt(0);
        setDescription(QString("Model for %1").arg(sources_.at(0)->name()));

        connect( sources_.at(0)->signalSource(), SIGNAL(valuesChanged(AMnDIndex, AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex, AMnDIndex)) );
        connect( sources_.at(0)->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()) );
    }

    reviewState();

    emitSizeChanged(0);
    emitValuesChanged();
    emitAxisInfoChanged(0);
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
    outputValues = dataStored_.mid(start, end - start).data();

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

void AM0DAccumulatorAB::setMaximumStored(int newMax)
{
    int storedSize = dataStored_.size();

    // in order to update the number of points stored, the new number must be different than the old one and must be greater than zero.
    if (storedSize != newMax && newMax > 0) {

        // if we are growing the number of points stored, we are resizing dataStored_. The number of updates actually stored does not change.
        if (storedSize < newMax) {
            dataStored_.resize(newMax);

        // if we are shrinking the number of points stored, we want to fill a new vector with the biggest number of updates possible.
        } else {

            if (newMax < updatesStored_) {
                dataStored_ = dataStored_.mid(0, updatesStored_ - newMax);
                updatesStored_ = newMax;

            } else {
                dataStored_ = dataStored_.mid(0, updatesStored_ + (newMax - updatesStored_));
            }
        }

        emit maximumStoredChanged(dataStored_.size());
    }
}

void AM0DAccumulatorAB::onInputSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end)
{
    Q_UNUSED(start)
    Q_UNUSED(end)

    // update the values stored for this source. We assume the source is 0D, so the latest value is always recovered with an empty AMnDIndex.
    double newValue = sources_.at(0)->value(AMnDIndex());

    if (updateCount_ >= dataStored_.size()) {
        dataStored_.remove(0);
        dataStored_.squeeze();
        dataStored_.append(newValue);

    } else {
        dataStored_[updatesStored_] = newValue;
        updatesStored_++;
    }

    updateCount_++;

    // update the axes info for this data source to reflect the (possibly) new number of points to plot.
    axes_[0] = AMAxisInfo(sources_.at(0)->name(), dataStored_.size());

    emitValuesChanged();
    emitSizeChanged(0);
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
