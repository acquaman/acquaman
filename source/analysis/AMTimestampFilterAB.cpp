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
    Q_UNUSED(dataSources)
    return false;
}

void AMTimestampFilterAB::setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources)
{
    Q_UNUSED(dataSources)
}


AMNumber AMTimestampFilterAB::value(const AMnDIndex &indexes) const
{
    Q_UNUSED(indexes)
    return AMNumber(AMNumber::InvalidError);
}

bool AMTimestampFilterAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
    Q_UNUSED(indexStart)
    Q_UNUSED(indexEnd)
    Q_UNUSED(outputValues)

    return false;
}

AMNumber AMTimestampFilterAB::axisValue(int axisNumber, int index) const
{
    Q_UNUSED(axisNumber)
    Q_UNUSED(index)

    return AMNumber(AMNumber::InvalidError);
}

bool AMTimestampFilterAB::loadFromDb(AMDatabase *db, int id)
{
    Q_UNUSED(db)
    Q_UNUSED(id)

    return false;
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
    Q_UNUSED(start)
    Q_UNUSED(end)
}

void AMTimestampFilterAB::onInputSourceSizeChanged()
{
    return;
}

void AMTimestampFilterAB::onInputSourceStateChanged()
{
    reviewState();
}

void AMTimestampFilterAB::reviewState()
{
    return;
}
