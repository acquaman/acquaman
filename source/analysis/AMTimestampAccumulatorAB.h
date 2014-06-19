#ifndef AMTIMESTAMPACCUMULATORAB_H
#define AMTIMESTAMPACCUMULATORAB_H

#include <QObject>
#include <QDebug>

#include "source/analysis/AMStandardAnalysisBlock.h"
#include "source/analysis/AM0DAccumulatorAB.h"
#include "source/analysis/AMTimestampAB.h"

class AMTimestampAccumulatorAB : public AMStandardAnalysisBlock
{
    Q_OBJECT

    Q_CLASSINFO("AMDbObject_Attributes", "description=0D Timestamped Accumulator Block")

public:
    /// Constructor.
    Q_INVOKABLE AMTimestampAccumulatorAB(const QString &outputName, QObject *parent = 0);
    /// Destructor.
    virtual ~AMTimestampAccumulatorAB();

    /// Returns the time amount.
    int timeValue() const;
    /// Returns the time units.
    AMTimestampAB::TimeUnits timeUnits() const;

    /// Check that the input sources are acceptable. The empty list is always valid. For non-empty list of sources, the list must be of size 1 and the sources must have rank 0.
    virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource *> &dataSources) const;
    /// Set the data source inputs.
    virtual void setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources);
    /// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are out of range, are incomplete, or the data is not ready.
    virtual AMNumber value(const AMnDIndex &indexes) const;
    /// Returns the block of values from indexStart to indexEnd, copied into outputValues. Returns false if the provided AMnDIndex values have the wrong dimension or if they are out of range.
    virtual bool values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const;
    /// Returns the independent value at the given axis and index.
    virtual AMNumber axisValue(int axisNumber, int index) const;
    /// Reimplemented from AMDbObject to set the AMDataSource name once we have an AMDbObject::name().
    virtual bool loadFromDb(AMDatabase *db, int id);

signals:
    void timeValueChanged(int newValue);
    void timeUnitsChanged(AMTimestampAB::TimeUnits newUnits);

public slots:
    /// Sets the time window for which values() can be retrieved.
    void setTimeWindow(int newValue, AMTimestampAB::TimeUnits newUnits);
    /// Sets the time value.
    void setTimeValue(int newValue);
    /// Sets the time units.
    void setTimeUnits(AMTimestampAB::TimeUnits newUnits);

protected slots:
    /// Handles setting the maximum number of data points and times to store.
    void setDataStoredCountMax(int newMax);
    /// Handles adding the new value to the dataStored_ for this particular data source.
    void onInputSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end);
    /// Handles checking whether this AB is valid if the state of any sources changes.
    void onInputSourceStateChanged();

protected:
    void reviewState();

protected:
    int timeValue_;

    AM0DAccumulatorAB *accumulator_;
    AMTimestampAB *timestamp_;

};

#endif // AMTIMESTAMPACCUMULATORAB_H
