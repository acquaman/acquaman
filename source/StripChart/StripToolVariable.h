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
    void dataMaxUpdated(double newMax);
    void dataMinUpdated(double newMin);
    void displayRangeUpdated(const MPlotAxisRange* newRange);

public:
    QModelIndex index() const;
    StripToolVariableInfo* info() const;
    MPlotVectorSeriesData* data() const;
    StripToolSeries *series() const;

public slots:
    void setIndex(const QModelIndex &index);
    void setInfo(StripToolVariableInfo* newInfo);
    void setCustomAxisMax(double newMax);
    void setCustomAxisMin(double newMin);
    void eraseCustomAxisMax();
    void eraseCustomAxisMin();

protected slots:
    virtual void onDataSourceValuesChanged(QTime measurementTime, double measurementValue);
    void toUpdateSeriesColor(const QString &colorName);

protected:
    void updateTotalValues(QTime latestTime, double latestValue);
    void updateDisplayValues(QTime latestTime, QVector<QTime> totalTimes, QVector<double> totalValues);

private:
    void totalVectorSizeCheck();

    QVector<double> applyAverage(QVector<double> toAverage);
    QVector<double> applyGranularity(QVector<double> toGranulize);

    void buildComponents();
    void makeConnections();
    void defaultSettings();

private:
    QModelIndex index_;
    StripToolVariableInfo* info_;
    MPlotVectorSeriesData* data_;
    StripToolSeries* series_;

    int updateCount_;
    QVector<QTime> totalUpdateTimes_;
    QVector<double> totalUpdateValues_;

};

#endif // STRIPTOOLVARIABLE_H
