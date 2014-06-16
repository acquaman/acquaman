#include "AM0DAccumulatorAB.h"

AM0DAccumulatorAB::AM0DAccumulatorAB(const QString &outputName, QObject *parent) :
    AMStandardAnalysisBlock(outputName, parent)
{
    axes_ << AMAxisInfo("invalid", 0, "No input data");
    setState(AMDataSource::InvalidFlag);

    dataStored_ = QVector<double>();
}

AM0DAccumulatorAB::~AM0DAccumulatorAB()
{

}

QVector<double> AM0DAccumulatorAB::dataStored() const
{
    return dataStored_;
}

int AM0DAccumulatorAB::dataCount() const
{
    return dataStored_.size();
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

    // if data sources are valid, set sources_, axis info, description, and connections.
    // note that only one data source is necessary.
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

    if (indexes.i() < 0 || indexes.i() >= dataStored_.size())
        return AMNumber(AMNumber::OutOfBoundsError);

    return dataStored_.at(indexes.i());
}

bool AM0DAccumulatorAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
    int start;
    int end;

    if (indexStart == AMnDIndex())
        start = 0;
    else
        start = indexStart.i();

    if (indexEnd == AMnDIndex())
        end = dataStored_.size() - 1;
    else
        end = indexEnd.i();

    if (indexStart.rank() != 1 || indexEnd.rank() != 1)
        return false;

    if (!isValid())
        return false;

    if ((unsigned)indexStart.i() > (unsigned)indexEnd.i())
        return false;

    if (indexStart.i() < 0 || indexEnd.i() < 0)
        return false;

    if (indexStart.i() >= dataStored_.size() || indexEnd.i() >= dataStored_.size())
        return false;

    // set output values to point to the subset of points saved for the data source.
    int totalSize = indexStart.totalPointsTo(indexEnd);
    QVector<double> update = dataStored_.mid(start, end - start);
    outputValues = update.data();

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

    dataStored_.append(newValue);

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
