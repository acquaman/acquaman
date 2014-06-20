#ifndef AM1DTIMEDDATAAB_H
#define AM1DTIMEDDATAAB_H

#include <QObject>
#include <QDebug>

#include "analysis/AMStandardAnalysisBlock.h"

class AM1DTimedDataAB : public AMStandardAnalysisBlock
{
    Q_OBJECT

public:
    /// Constructor.
    Q_INVOKABLE AM1DTimedDataAB(const QString &outputName, QObject *parent = 0);
    /// Destructor.
    virtual ~AM1DTimedDataAB();

    virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource *> &dataSources) const;

    virtual void setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources);

    virtual AMNumber value(const AMnDIndex &indexes) const;

    virtual bool values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const;

    virtual AMNumber axisValue(int axisNumber, int index) const;

    bool loadFromDb(AMDatabase *db, int id);

signals:

public slots:

protected slots:
    void onInputSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end);

    void onInputSourceSizeChanged();

    void onInputSourceStateChanged();

protected:
    void reviewState();

};

#endif // AM1DTIMEDDATAAB_H
