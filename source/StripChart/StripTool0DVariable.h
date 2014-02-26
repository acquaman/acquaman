#ifndef STRIPTOOL0DVARIABLE_H
#define STRIPTOOL0DVARIABLE_H

#include <QObject>

#include "MPlot/MPlotSeriesData.h"

#include "StripChart/StripToolBasicVariable.h"
#include "StripChart/StripTool0DVariableInfo.h"
#include "StripChart/StripToolSeries.h"

class StripTool0DVariable : public StripToolBasicVariable
{
    Q_OBJECT

public:
    explicit StripTool0DVariable(StripTool0DVariableInfo *info, AMDataSource *dataSource, QObject *parent = 0);

signals:
    void infoChanged();
    void totalVectorsUpdated();
    void dataRangeUpdated(MPlotAxisRange*);
    void displayRangeUpdated(MPlotAxisRange*);

protected:
    StripTool0DVariableInfo* info() const;
    MPlotVectorSeriesData* data() const;
    StripToolSeries* series() const;
    MPlotItem* plotItem() const;

    QString colorName() const;
    void setColorName(const QString &newColor);

    int granularity() const;
    void setGranularity(int newGranularity);

    MPlotAxisRange* displayRange() const;
    MPlotAxisRange* dataRange() const;

private slots:
    void onDataSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end);
    void onTotalVectorsUpdated();

private:
    void totalVectorSizeCheck();
    void updateValues(QTime latestTime, double latestValue);
    void updateDisplayValues();

private:
    StripTool0DVariableInfo *info_;
    MPlotVectorSeriesData *data_;
    StripToolSeries *series_;

    int updateCount_;
    int displayedTime_; // the amount of time we want to see plotted. in milliseconds.
    QVector<QTime> totalUpdateTimes_;
    QVector<double> totalUpdateValues_;



};

#endif // STRIPTOOL0DVARIABLE_H
