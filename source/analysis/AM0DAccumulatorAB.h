#ifndef AM0DACCUMULATORAB_H
#define AM0DACCUMULATORAB_H

#include <QObject>
#include <QDebug>

#include "analysis/AMStandardAnalysisBlock.h"

class AM0DAccumulatorAB : public AMStandardAnalysisBlock
{
    Q_OBJECT

    Q_CLASSINFO("AMDbObject_Attributes", "description=0D Accumulator Block")

public:
    /// Constructor.
    Q_INVOKABLE AM0DAccumulatorAB(const QString &outputName, QObject *parent = 0);
    /// Destructor.
    virtual ~AM0DAccumulatorAB();

    /// Returns the data stored in this analysis block, for the input data source.
    QVector<double> dataStored() const;
    /// Returns the number of data elements stored in this analysis block.
    int dataCount() const;

    /// Check that the input sources are acceptable. The empty list is always valid. For non-empty list of sources, the list must be of size 1 and the source must have rank 1."
    virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource *> &dataSources) const;
    /// Set the data source inputs.
    virtual void setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources);
    /// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are out of range, are incomplete, or the data is not ready."
    virtual AMNumber value(const AMnDIndex &indexes) const;
    /// Returns the block of values from indexStart to indexEnd, copied into outputValues. Returns false if the provided AMnDIndex values have the wrong dimension or if they are out of range.
    virtual bool values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const;
    /// Returns the independent value at the given axis and index.
    virtual AMNumber axisValue(int axisNumber, int index) const;
    /// Reimplemented from AMDbObject to set the AMDataSource name once we have an AMDbObject::name().
    virtual bool loadFromDb(AMDatabase *db, int id);

signals:

public slots:

protected slots:
    /// Handles adding the new value to the dataStored_ for this particular data source.
    void onInputSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end);
    /// Handles checking whether this AB is valid if the state of any sources changes.
    void onInputSourceStateChanged();

protected:
    /// Helper function that reviews this AB's current state.
    void reviewState();

protected:
    /// Data stored in this analysis block.
    QVector<double> dataStored_;

};

#endif // AM0DACCUMULATORAB_H
