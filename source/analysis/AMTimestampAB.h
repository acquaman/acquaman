#ifndef AMTIMESTAMPAB_H
#define AMTIMESTAMPAB_H

#include <QObject>
#include <QDebug>

#include "analysis/AMStandardAnalysisBlock.h"

class AMTimestampAB : public AMStandardAnalysisBlock
{
    Q_OBJECT

public:
    /// Constructor.
    Q_INVOKABLE AMTimestampAB(const QString &outputName, QObject *parent = 0);
    /// Destructor.
    virtual ~AMTimestampAB();

    /// Returns the times stored in this analysis block, for the input data source.
    QVector<QDateTime> dataStored() const;

    /// Returns the total number of elements stored.
    int dataCount() const;

    /// Check that the input sources are acceptable. The empty list is always valid. For non-empty list of sources, the list must be of size 1."
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
    /// List storing the times saved in this analysis block.
    QVector<QDateTime> dataStored_;
    QDateTime latestUpdate_;

};

#endif // AMTIMESTAMPAB_H
