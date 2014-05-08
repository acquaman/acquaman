#ifndef STREALTIMEDATACOLLECTION_H
#define STREALTIMEDATACOLLECTION_H

#include "MPlot/MPlotSeriesData.h"

class STDataCollection;
class STData;

class STRealtimeDataCollection : public STDataCollection
{
    Q_OBJECT

public:
    explicit STRealtimeDataCollection(const QString &name, QObject *parent = 0);
    ~STRealtimeDataCollection();

    MPlotRealtimeModel* displayModel();

signals:

public slots:
    void addData(QTime time, double value);

protected slots:
    void onModelDataChanged(const QModelIndex &start, const QModelIndex &end);

protected:
    MPlotRealtimeModel *displayModel_;

};

#endif // STREALTIMEDATACOLLECTION_H
