#ifndef STRIPTOOLSERIES2_H
#define STRIPTOOLSERIES2_H

#include "MPlot/MPlotSeries.h"
#include <QDebug>

class StripToolSeries2 : public QObject
{
    Q_OBJECT

public:
    StripToolSeries2(QObject *parent = 0);

signals:
    void dataRangeUpdated(MPlotAxisRange* newRange);
    void displayRangeUpdated(const MPlotAxisRange* newRange);

public:
    MPlotSeriesBasic* series() const;
    MPlotAxisRange* displayedRange();
    MPlotAxisRange* dataRange();

    qreal dataMin() const;
    qreal dataMax() const;

    bool customMinDefined() const;
    bool customMaxDefined() const;
    double customMin() const;
    double customMax() const;

    double displayedMin() const;
    double displayedMax() const;

    bool waterfallApplied() const;
    qreal waterfallMin() const;
    qreal waterfallMax() const;

public slots:
    void setData(MPlotVectorSeriesData* newData);

    void setCustomMin(qreal min);
    void setCustomMax(qreal max);
    void eraseCustomMin();
    void eraseCustomMax();

    void setWaterfallMin(qreal min);
    void setWaterfallMax(qreal max);
    void eraseWaterfallLimits();

    void enableYNormalization(bool normOn, qreal ymin_axis, qreal ymax_axis);
    void enableWaterfall(bool waterfallOn, int itemPosition, int itemCount);

protected:
    void setWaterfallLimits(qreal min, qreal max);
    void applyWaterfall(int itemPosition, int itemCount);

protected slots:
//    void onDataUpdate();

private:
    void buildComponents();
    void makeConnections();
    void defaultSettings();


private:
    bool customMinDefined_;
    bool customMaxDefined_;

    double customMin_;
    double customMax_;

    bool waterfallApplied_;

    double waterfallMin_;
    double waterfallMax_;

    MPlotSeriesBasic* series_;
};

#endif // STRIPTOOLSERIES2_H
