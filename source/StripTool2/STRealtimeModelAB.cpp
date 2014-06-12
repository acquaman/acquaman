#include "STRealtimeModelAB.h"

STRealtimeModelAB::STRealtimeModelAB(const QString &outputName, QObject *parent) : AMStandardAnalysisBlock(outputName, parent)
{
    axes_ << AMAxisInfo("invalid", 0, "No input data");
    setState(AMDataSource::InvalidFlag);

    dataCount_ = 0;
    dataDisplayed_ = 10;
    dataStored_ = new MPlotRealtimeModel(this);
}

STRealtimeModelAB::~STRealtimeModelAB()
{

}

bool STRealtimeModelAB::areInputDataSourcesAcceptable(const QList<AMDataSource *> &dataSources) const
{
    // null input is acceptable.
    if (dataSources.isEmpty())
        return true;

    // we expect to receive maximum one input data source.
    if (dataSources.size() > 1)
        return false;

    // every data source must have rank zero.
    for (int i = 0; i < dataSources.count(); i++) {
        if (dataSources.at(i)->rank() != 0)
            return false;
    }

    return true;
}

void STRealtimeModelAB::setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources)
{
    // disconnect connections from old sources, if they exist.
    if (!sources_.isEmpty()) {

        for (int i = 0; i < sources_.size(); i++) {
            disconnect( sources_.at(i)->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)) );
            disconnect( sources_.at(i)->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourcesStateChanged()) );
        }

    }

    // set to invalid if new dataSources is empty.
    if (dataSources.isEmpty()) {

        sources_.clear();
        axes_[0] = AMAxisInfo("invalid", 0, "No input data");
        setDescription("-- No input data --");

    // if data sources are valid, set sources_, axis info, description, and connections.
    // note that only one data source is necessary.
    } else {
        sources_ = dataSources.mid(0, 1);
        axes_[0] = sources_.at(0)->axisInfoAt(0);
        setDescription(QString("Model for %1").arg(sources_.at(0)->name()));

        for (int i = 0; i < sources_.size(); i++) {
            connect( sources_.at(i)->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)) );
            connect( sources_.at(i)->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()) );
        }
    }

    reviewState();

    emitSizeChanged(0);
    emitValuesChanged();
    emitAxisInfoChanged(0);
    emitInfoChanged();
}

AMNumber STRealtimeModelAB::value(const AMnDIndex &indexes) const
{
    if (indexes.rank() != 0)
        return AMNumber(AMNumber::DimensionError);

    if (!isValid())
        return AMNumber(AMNumber::InvalidError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
    for (int i = 0; i < sources_.size(); i++)
        if (indexes.i() >= sources_.at(i)->size(0))
            return AMNumber(AMNumber::OutOfBoundsError);
#endif

    return 0;
}

bool STRealtimeModelAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
    if (indexStart.rank() != 0 || indexEnd.rank() != 0)
        return false;

    if (!isValid())
        return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
    for (int i = 0; i < sources_.size(); i++)
        if ((unsigned)indexEnd.i() >= (unsigned)axes_.at(0).size)
            return false;

    if ((unsigned)indexStart.i() > (unsigned)indexEnd.i())
        return false;
#endif

    // set output values to point to the subset of points saved for the data source.
    dataStored_->yValues(indexStart.i(), indexEnd.i(), outputValues);

    return false;
}

AMNumber STRealtimeModelAB::axisValue(int axisNumber, int index) const
{
    if (!isValid())
        return AMNumber(AMNumber::InvalidError);

    if (axisNumber != 0)
        return AMNumber(AMNumber::DimensionError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
    if (index >= sources_.first()->size(0))
        return AMNumber(AMNumber::OutOfBoundsError);
#endif

    return sources_.first()->axisValue(0, index);
}

void STRealtimeModelAB::onInputSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end)
{
    // update the values stored for this source.
//    double *newValue = 0;
//    sources_.at(0)->values(start, end, newValue);

//    dataStored_->insertPointBack(dataCount_, *newValue);
//    dataCount_++;

//    if (dataStored_->count() > dataDisplayed_) {
//        dataStored_->removePointFront();
//    }

    emitValuesChanged(start, end);
}

void STRealtimeModelAB::onInputSourceStateChanged()
{
    reviewState();
}

void STRealtimeModelAB::reviewState()
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

    // is the data source the right size?
    if (axes_[0].size != sources_.at(0)->axisInfoAt(0).size) {
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
