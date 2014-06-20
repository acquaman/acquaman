#include "AM1DTimedDataAB.h"

AM1DTimedDataAB::AM1DTimedDataAB(const QString &outputName, QObject *parent) : AMStandardAnalysisBlock(outputName, parent)
{
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
    if (dataSources.size() != 2)
        return false;

    // both input sources must have rank 1.
    foreach (AMDataSource* source, dataSources)
        if (source->rank() != 1)
            return false;

    return true;
}

void AM1DTimedDataAB::setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources)
{
    Q_UNUSED(dataSources)
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


