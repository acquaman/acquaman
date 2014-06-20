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

        disconnect(timestamps_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
        disconnect(timestamps_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
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

        connect(timestamps_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
        connect(timestamps_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
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
    Q_UNUSED(indexes)
    return AMNumber(AMNumber::InvalidError);
}

bool AM1DTimedDataAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
    Q_UNUSED(indexStart)
    Q_UNUSED(indexEnd)
    Q_UNUSED(outputValues)

    return false;
}

AMNumber AM1DTimedDataAB::axisValue(int axisNumber, int index) const
{
    Q_UNUSED(axisNumber)
    Q_UNUSED(index)

    return AMNumber(AMNumber::InvalidError);
}

bool AM1DTimedDataAB::loadFromDb(AMDatabase *db, int id)
{
    Q_UNUSED(db)
    Q_UNUSED(id)

    return false;
}

void AM1DTimedDataAB::onInputSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end)
{
    Q_UNUSED(start)
    Q_UNUSED(end)
}

void AM1DTimedDataAB::onInputSourceSizeChanged()
{
    return;
}

void AM1DTimedDataAB::onInputSourceStateChanged()
{
    reviewState();
}

void AM1DTimedDataAB::reviewState()
{
    return;
}


