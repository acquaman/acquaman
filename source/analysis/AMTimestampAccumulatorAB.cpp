#include "AMTimestampAccumulatorAB.h"

AMTimestampAccumulatorAB::AMTimestampAccumulatorAB(const QString &outputName, QObject *parent) : AMStandardAnalysisBlock(outputName, parent)
{
    axes_ << AMAxisInfo("invalid", 0, "No input data");
    setState(AMDataSource::InvalidFlag);

    dataCount_ = 0;

    accumulator_ = new AM0DAccumulatorAB("Accumulated 0D data", this);

    timestamp_ = new AMTimestampAB("Timestamp data", this);
    timestamp_->setInputDataSources(QList<AMDataSource*>() << accumulator_);

    sources_ = QList<AMDataSource*>() << timestamp_;

    connect( timestamp_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)) );
    connect( timestamp_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()) );
}

AMTimestampAccumulatorAB::~AMTimestampAccumulatorAB()
{

}

bool AMTimestampAccumulatorAB::areInputDataSourcesAcceptable(const QList<AMDataSource *> &dataSources) const
{
    // null input is acceptable.
    if (dataSources.isEmpty())
        return true;

    // we expect one data source.
    if (dataSources.size() > 1)
        return false;

    // the data source must have rank 0.
    if (dataSources.at(0)->rank() != 0)
        return false;

    return true;
}

void AMTimestampAccumulatorAB::setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources)
{
    // set to invalid if the new dataSources is empty.
    if (dataSources.isEmpty()) {
        setDescription("-- No input data --");
    }

    // if data source is valid, give the new source to the accumulator and set the description.
    else {
        setDescription( QString("Timestamped %1 data").arg(sources_.at(0)->name()) );
        accumulator_->setInputDataSources(QList<AMDataSource*>() << dataSources.at(0));
    }

    reviewState();

    emitSizeChanged();
    emitValuesChanged();
    emitAxisInfoChanged();
    emitInfoChanged();
}

AMNumber AMTimestampAccumulatorAB::value(const AMnDIndex &indexes) const
{
    return timestamp_->value(indexes);
}

bool AMTimestampAccumulatorAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
    return timestamp_->values(indexStart, indexEnd, outputValues);
}

AMNumber AMTimestampAccumulatorAB::axisValue(int axisNumber, int index) const
{
    return timestamp_->axisValue(axisNumber, index);
}

bool AMTimestampAccumulatorAB::loadFromDb(AMDatabase *db, int id)
{
    bool success = AMDbObject::loadFromDb(db, id);

    if (success) {
        AMDataSource::name_ = AMDbObject::name();
    }

    return success;
}

void AMTimestampAccumulatorAB::onInputSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end)
{
    Q_UNUSED(start)
    Q_UNUSED(end)

    // update the axes info for the data source to reflect the (possibly) new number of points to plot.
    axes_[0] = accumulator_->axisInfoAt(0);

    emitValuesChanged();
    emitSizeChanged();
}

void AMTimestampAccumulatorAB::onInputSourceStateChanged()
{
    reviewState();
}

void AMTimestampAccumulatorAB::reviewState()
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

    // check that data sources are valid.
    foreach (AMDataSource *source, sources_) {
        if (!source->isValid()) {
            setState(AMDataSource::InvalidFlag);
            return;
        }
    }

    setState(0);
}
