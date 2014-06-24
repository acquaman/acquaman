#include "AM1DTimedDataAB.h"

AM1DTimedDataAB::AM1DTimedDataAB(const QString &outputName, QObject *parent) : AMStandardAnalysisBlock(outputName, parent)
{
    updateOffset_ = 0;

    data_ = 0;
    timestamps_ = 0;

    axes_ << AMAxisInfo("invalid", 0, "No input data");
    setState(AMDataSource::InvalidFlag);
}

AM1DTimedDataAB::~AM1DTimedDataAB()
{

}

bool AM1DTimedDataAB::areInputDataSourcesAcceptable(const QList<AMDataSource *> &dataSources) const
{
    // null input always acceptable.
    if (dataSources.isEmpty())
        return true;

    // we expect two input sources.
    if (dataSources.count() != 2)
        return false;

    // both input sources must have rank 1.
    foreach (AMDataSource* source, dataSources)
        if (source->rank() != 1)
            return false;

    return true;
}

void AM1DTimedDataAB::setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources)
{
    // disconnect old connections, if they exist.
    if (data_){
        disconnect( data_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onDataSourceValuesChanged(AMnDIndex,AMnDIndex)) );
        disconnect( data_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()) );
        data_ = 0;
    }

    if (timestamps_){

        disconnect( timestamps_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onTimeSourceValuesChanged(AMnDIndex,AMnDIndex)));
        disconnect( timestamps_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
        timestamps_ = 0;
    }

    if (dataSources.isEmpty()){

        data_ = 0;
        timestamps_ = 0;
        sources_.clear();

        axes_[0] = AMAxisInfo("invalid", 0, "No input data");
        setDescription("-- No input data --");
    }

    else if (dataSources.count() == 2) {
        data_ = dataSources.at(0);
        timestamps_ = dataSources.at(1);
        sources_ = dataSources;

        axes_[0] = data_->axisInfoAt(0);
        setDescription(QString("Timed updates for %1").arg(data_->name()));

        connect(data_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onDataSourceValuesChanged(AMnDIndex,AMnDIndex)));
        connect(data_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

        connect(timestamps_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onTimeSourceValuesChanged(AMnDIndex,AMnDIndex)));
        connect(timestamps_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
    }

    else {
        sources_ = dataSources;
    }

    reviewState();

    emitSizeChanged();
    emitValuesChanged();
    emitAxisInfoChanged();
    emitInfoChanged();
}


AMNumber AM1DTimedDataAB::value(const AMnDIndex &indexes) const
{
    if (data_)
        return data_->value(indexes);

    return AMNumber(AMNumber::InvalidError);
}

bool AM1DTimedDataAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
    if (data_)
        return data_->values(indexStart, indexEnd, outputValues);

    outputValues = 0;
    return false;
}

AMNumber AM1DTimedDataAB::axisValue(int axisNumber, int index) const
{
    if (!isValid())
        return AMNumber(AMNumber::InvalidError);

    if (axisNumber != 0)
        return AMNumber(AMNumber::DimensionError);

    if (timestamps_)
        return timestamps_->value(AMnDIndex(index));

    return AMNumber(AMNumber::InvalidError);
}

bool AM1DTimedDataAB::loadFromDb(AMDatabase *db, int id)
{
    bool success = AMDbObject::loadFromDb(db, id);

    if (success) {
        AMDataSource::name_ = AMDbObject::name();
    }

    return success;
}

void AM1DTimedDataAB::onDataSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end)
{
    Q_UNUSED(start)
    Q_UNUSED(end)

    updateOffset_++;
    qDebug() << "AM1DTimedDataAB : data source update.";
}

void AM1DTimedDataAB::onTimeSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end)
{
    updateOffset_--;
    qDebug() << "AM1DTimedDataAB : time source update.";
    reviewValuesChanged(start, end);
}

void AM1DTimedDataAB::onInputSourceStateChanged()
{
    reviewState();
}

void AM1DTimedDataAB::reviewValuesChanged(const AMnDIndex &start, const AMnDIndex &end)
{
    if (updateOffset_ != 0) {
        qDebug() << "Sources are not updating properly--not in lockstep.";
        return;
    }

    qDebug() << "AM1DTimedDataAB :" << end.i() - start.i() << "values changed.";

    axes_[0] = AMAxisInfo(sources_.at(0)->name(), end.i() - start.i());

    emitValuesChanged(start, end);
    emitAxisInfoChanged(0);
    emitSizeChanged(0);
}

void AM1DTimedDataAB::reviewState()
{
    // are there data sources for this AB?
    if (sources_.isEmpty()) {
        setState(AMDataSource::InvalidFlag);
        return;
    }

    // are there two sources?
    if (sources_.count() != 2) {
        setState(AMDataSource::InvalidFlag);
        return;
    }

    // are the two sources identified appropriately?
    if (!data_ || !timestamps_) {
        setState(AMDataSource::InvalidFlag);
        return;
    }

    // are both sources valid?
    foreach (AMDataSource *source, sources_)
        if (!source->isValid()) {
            setState(AMDataSource::InvalidFlag);
            return;
        }

    // otherwise, this AB is valid.
    setState(0);
}


