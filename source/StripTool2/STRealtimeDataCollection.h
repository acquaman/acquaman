#ifndef STREALTIMEDATACOLLECTION_H
#define STREALTIMEDATACOLLECTION_H

#include "MPlot/MPlotSeriesData.h"

#include "StripTool2/STDataCollection.h"

class STRealtimeDataCollection : public STDataCollection
{
    Q_OBJECT

public:
    explicit STRealtimeDataCollection(const QString &name, QObject *parent = 0);
    virtual ~STRealtimeDataCollection();

    MPlotRealtimeModel* displayModel();

signals:

public slots:
    virtual void addData(QTime &time, double value);

protected slots:
//    void onModelDataChanged(const QModelIndex &start, const QModelIndex &end);

protected:
    MPlotRealtimeModel *displayModel_;

};

#endif // STREALTIMEDATACOLLECTION_H
