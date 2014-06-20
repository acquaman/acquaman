#include "AMTimestampFilterAB.h"

AMTimestampFilterAB::AMTimestampFilterAB(const QString &outputName, QObject *parent) : AMStandardAnalysisBlock(outputName, parent)
{
    timeValue_ = 10;

    axes_ << AMAxisInfo("invalid", 0, "No input data");
    setState(AMDataSource::InvalidFlag);
}

AMTimestampFilterAB::~AMTimestampFilterAB()
{

}

int AMTimestampFilterAB::timeValue() const
{
    return timeValue_;
}

bool AMTimestampFilterAB::areInputDataSourcesAcceptable(const QList<AMDataSource *> &dataSources) const
{
    // null input is acceptable.
    if (dataSources.count() == 0)
        return true;

    // we expect there to be max 1 data source input.
    if (dataSources.count() != 1)
        return false;

    // the rank of the input source should be 1.
    if (dataSources.at(0)->rank() != 1)
        return false;

    // else, input is acceptable.
    return true;
}

void AMTimestampFilterAB::setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources)
{
    // disconnect old sources, if they exist.
    if (!sources_.isEmpty()) {
        disconnect( sources_.at(0)->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)) );
        disconnect( sources_.at(0)->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()) );
    }

    // set this AB to invalid if new dataSources is empty.
    if (dataSources.isEmpty()) {
        sources_.clear();
        axes_[0] = AMAxisInfo("invalid", 0, "No input data");
        setDescription("-- No input data --");
    }

    // if data source is valid, set sources_, axis info, description and connections.
    else {
        sources_ = dataSources;
        axes_[0] = sources_.at(0)->axisInfoAt(0);
        setDescription(QString("Time filtering for %1").arg(sources_.at(0)->name()));

        connect( sources_.at(0)->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)) );
        connect( sources_.at(0)->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()) );
    }

    reviewState();

    emitSizeChanged();
    emitValuesChanged();
    emitAxisInfoChanged();
    emitInfoChanged();
}


AMNumber AMTimestampFilterAB::value(const AMnDIndex &indexes) const
{
    if(!isValid())
        return AMNumber(AMNumber::InvalidError);

    return sources_.at(0)->value(indexes);
}

bool AMTimestampFilterAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
    if (!isValid()) {
        outputValues = 0;
        return false;
    }

    return sources_.at(0)->values(indexStart, indexEnd, outputValues);
}

AMNumber AMTimestampFilterAB::axisValue(int axisNumber, int index) const
{
    if (!isValid())
        return AMNumber(AMNumber::InvalidError);

    return sources_.at(0)->axisValue(axisNumber, index);
}

bool AMTimestampFilterAB::loadFromDb(AMDatabase *db, int id)
{
    bool success = AMDbObject::loadFromDb(db, id);

    if (success) {
        AMDataSource::name_ = AMDbObject::name();
    }

    return success;
}

void AMTimestampFilterAB::setTimeValue(int newValue)
{
    if (timeValue_ != newValue) {
        timeValue_ = newValue;
        emit timeValueChanged(timeValue_);
    }
}

void AMTimestampFilterAB::onInputSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end)
{
    int totalPoints = start.totalPointsTo(end);

    qDebug() << "Filter source points changed : " << totalPoints;
    qDebug() << "Filter source point start : " << start.i();
    qDebug() << "Filter source point end : " << end.i();

    if (totalPoints <= timeValue_) {
        // update the axes info for this data source to reflect the (possibly) new number of points to plot.
        axes_[0] = sources_.at(0)->axisInfoAt(0);

        emitValuesChanged(start, end);
        emitAxisInfoChanged();
        emitSizeChanged();

    } else {
        axes_[0] = AMAxisInfo(sources_.at(0)->name(), timeValue_);

        emitValuesChanged(AMnDIndex(end.i() - timeValue_), end);
        emitAxisInfoChanged();
    }
}

void AMTimestampFilterAB::onInputSourceStateChanged()
{
    reviewState();
}

void AMTimestampFilterAB::reviewState()
{
    // are there data sources?
    if (sources_.isEmpty()) {
        setState(AMDataSource::InvalidFlag);
        return;
    }

    // is there one data source?
    if (sources_.count() != 1) {
        setState(AMDataSource::InvalidFlag);
        return;
    }

    // is the data source valid?
    if (!sources_.at(0)->isValid()) {
        setState(AMDataSource::InvalidFlag);
        return;
    }

    // is timeValue_ positive?
    if (timeValue_ <= 0) {
        setState(AMDataSource::InvalidFlag);
        return;
    }

    // else, this AB is valid.
    setState(0);
}
