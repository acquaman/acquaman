#include "AM1DTimedDataAB.h"

AM1DTimedDataAB::AM1DTimedDataAB(const QString &outputName, QObject *parent) : AMStandardAnalysisBlock(outputName, parent)
{
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

        disconnect(data_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
        disconnect(data_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
        disconnect(data_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
        data_ = 0;
    }

    if (timestamps_){

//        disconnect(timestamps_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
//        disconnect(timestamps_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
        disconnect(timestamps_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
        timestamps_ = 0;
    }

    if (dataSources.isEmpty()){

        data_ = 0;
        timestamps_ = 0;
        sources_.clear();

        axes_[0] = AMAxisInfo("invalid", 0, "No input data");
        setDescription("Timed 1D Data Source");
    }

    else if (dataSources.count() == 2) {
        data_ = dataSources.at(0);
        timestamps_ = dataSources.at(1);
        sources_ = dataSources;

        axes_[0] = data_->axisInfoAt(0);
        setDescription(QString("Timed updates for %1").arg(data_->name()));

        connect(data_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
        connect(data_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
        connect(data_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

//        connect(timestamps_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
//        connect(timestamps_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
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

    if (index >= axes_.at(axisNumber).size)
        return AMNumber(AMNumber::DimensionError);

    if (timestamps_)
        return timestamps_->value(AMnDIndex(index));

    return AMNumber(AMNumber::InvalidError);
}

bool AM1DTimedDataAB::loadFromDb(AMDatabase *db, int id)
{
    bool success = AMDbObject::loadFromDb(db, id);
    if(success)
        AMDataSource::name_ = AMDbObject::name();	/// \todo This might change the name of a data-source in mid-life, which is technically not allowed.
    return success;
}

void AM1DTimedDataAB::onInputSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end)
{
    // attempting to find a way to make sure AM1DTimedDataAB doesn't announce an update until both sources have updated (not just data_).

    QVector<double> temp;
    if (timestamps_->values(start, end, temp.data())) {
        emitValuesChanged(start, end);

    } else if (end.i() > 0) {
        emitValuesChanged(start, AMnDIndex(end.i() - 1));

    }
}

void AM1DTimedDataAB::onInputSourceSizeChanged()
{
    if(axes_.at(0).size != data_->size(0)){

        axes_[0].size = data_->size(0);
        emitSizeChanged(0);
    }
}

void AM1DTimedDataAB::onInputSourceStateChanged()
{
    // just in case the size has changed while the input source was invalid, and now it's going valid.  Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
    onInputSourceSizeChanged();
    reviewState();
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


