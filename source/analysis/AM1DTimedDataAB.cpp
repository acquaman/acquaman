#include "AM1DTimedDataAB.h"

AM1DTimedDataAB::AM1DTimedDataAB(const QString &outputName, QObject *parent) : AMStandardAnalysisBlock(outputName, parent)
{
    updateOffset_ = 0;

    data_ = 0;
    timestamps_ = 0;

    values_ = QVector<double>();
    times_ = QVector<double>();

    axes_ << AMAxisInfo("invalid", 0, "No input data");
    setState(AMDataSource::InvalidFlag);
}

AM1DTimedDataAB::~AM1DTimedDataAB()
{

}

QVector<double> AM1DTimedDataAB::dataValues() const
{
    return values_;
}

QVector<double> AM1DTimedDataAB::timeValues() const
{
    return times_;
}

bool AM1DTimedDataAB::areInputDataSourcesAcceptable(const QList<AMDataSource *> &dataSources) const
{
    // null input always acceptable.
    if (dataSources.isEmpty())
        return true;

    // otherwise, we expect two input sources.
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
    // disconnect from old sources, if they exist.
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

    if (dataSources.isEmpty()) {

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
    if (!isValid())
        return AMNumber(AMNumber::InvalidError);

    if (indexes.i() < 0 || indexes.i() >= values_.size())
        return AMNumber(AMNumber::OutOfBoundsError);

    if (indexes.rank() != 1)
        return AMNumber(AMNumber::DimensionError);

    return values_.at(indexes.i());
}

bool AM1DTimedDataAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
    if (!isValid())
        return false;

    if (indexStart.i() < 0 || indexStart.i() >= values_.size())
        return false;

    if (indexEnd.i() < 0 || indexEnd.i() >= values_.size())
        return false;

    if (indexStart.i() > indexEnd.i())
        return false;

    if (indexStart.rank() != 1 || indexEnd.rank() != 1)
        return false;

    int totalPoints = indexStart.totalPointsTo(indexEnd);

    for (int i = 0; i < totalPoints; i++) {
        outputValues[i] = values_.at(i);
    }

    return true;
}

AMNumber AM1DTimedDataAB::axisValue(int axisNumber, int index) const
{
    if (!isValid())
        return AMNumber(AMNumber::InvalidError);

    if (axisNumber != 0)
        return AMNumber(AMNumber::DimensionError);

    if (index < 0 || index >= times_.size())
        return AMNumber(AMNumber::OutOfBoundsError);

    return times_.at(index);
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
}

void AM1DTimedDataAB::onTimeSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end)
{
    updateOffset_--;
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

    int pointsChanged = start.totalPointsTo(end);
    qDebug() << "Points to display :" << pointsChanged;

    if (pointsChanged > 0) {

        values_.clear();
        times_.clear();

        values_.reserve(pointsChanged);
        times_.reserve(pointsChanged);

        bool dataSuccess = data_->values(start, end, values_.data());
        bool timeSuccess = timestamps_->values(start, end, times_.data());

        if (dataSuccess && timeSuccess) {

            qDebug() << "AM1DTimedDataAB // values to display : " << values_.toList();
            qDebug() << "AM1DTimedDataAB //  times to display : " << times_.toList();

            axes_[0] = AMAxisInfo(sources_.at(0)->name(), pointsChanged);

            emitValuesChanged(AMnDIndex(0), AMnDIndex(pointsChanged - 1));
            emitAxisInfoChanged(0);
            emitSizeChanged(0);

        } else {
            qDebug() << "AM1DTimedDataAB : update unsuccessful.";
        }
    }
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


