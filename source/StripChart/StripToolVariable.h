#ifndef STRIPTOOLVARIABLE_H
#define STRIPTOOLVARIABLE_H

#include <QObject>

#include "MPlot/MPlotSeriesData.h"
#include "dataman/AMnDIndex.h"

#include "StripChart/StripToolSeries.h"
#include "StripChart/StripToolSeries2.h"
#include "StripChart/StripToolVariableInfo.h"

class MPlotAxisRange;

class StripToolVariable : public QObject
{
    Q_OBJECT

public:
    explicit StripToolVariable(StripToolVariableInfo *info = 0, QObject *parent = 0);

signals:
//    void dataRangeUpdated(MPlotAxisRange* newRange);
    void displayRangeUpdated(const MPlotAxisRange* newRange);

public:
    QModelIndex index() const;
    StripToolVariableInfo* info() const;
    MPlotVectorSeriesData* data() const;
    StripToolSeries2 *series() const;
    MPlotSeriesBasic *plotItem() const;

public slots:
    void setIndex(const QModelIndex &index);
    void setInfo(StripToolVariableInfo* newInfo);

protected slots:
    void onDataSourceValuesChanged(QTime measurementTime, double measurementValue);
    void onDisplayRangeUpdated(const MPlotAxisRange* newRange);

private:
    void totalVectorSizeCheck();
    void updateTotalValues(QTime latestTime, double latestValue);
    void updateDisplayValues();
    void buildComponents();
    void makeConnections();
    void defaultSettings();

private:
    QModelIndex index_;
    StripToolVariableInfo* info_;
    MPlotVectorSeriesData* data_;
    StripToolSeries2* series_;

    int updateCount_;
    int displayedTime_;
    QVector<QTime> totalUpdateTimes_;
    QVector<double> totalUpdateValues_;

};

#endif // STRIPTOOLVARIABLE_H
