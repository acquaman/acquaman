#ifndef STREALTIMEDATASOURCE_H
#define STREALTIMEDATASOURCE_H

#include <QObject>

#include "analysis/AMStandardAnalysisBlock.h"

#include "MPlot/MPlotSeriesData.h"

class STRealtimeModelAB : public AMStandardAnalysisBlock
{
    Q_OBJECT
public:
    explicit STRealtimeModelAB(const QString &outputName = "InvalidInput", QObject *parent = 0);
    virtual ~STRealtimeModelAB();

    virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource *> &dataSources) const;

    virtual void setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources);

    virtual AMNumber value(const AMnDIndex &indexes) const;

    virtual bool values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const;

    virtual AMNumber axisValue(int axisNumber, int index) const;

protected slots:
    void onInputSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end);
    void onInputSourceStateChanged();

protected:
    void reviewState();

protected:
    int dataCount_;
    int dataDisplayed_;
    MPlotRealtimeModel *dataStored_;

};

#endif // STREALTIMEDATASOURCE_H
