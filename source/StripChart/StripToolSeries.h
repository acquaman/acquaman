#ifndef STRIPTOOLSERIES_H
#define STRIPTOOLSERIES_H

#include "MPlot/MPlotSeries.h"
#include <QDebug>

class StripToolSeries : public MPlotSeriesBasic
{

public:
    StripToolSeries();

public:
    MPlotAxisRange* displayedRange();
    MPlotAxisRange* dataRange();

public:
    void enableYNormalization(bool normOn, qreal ymin_axis, qreal ymax_axis);
    void enableWaterfall(bool waterfallOn, int itemPosition, int itemCount);
    void setCustomLimits(qreal min, qreal max);
    void setCustomMin(qreal min);
    void setCustomMax(qreal max);
    void eraseCustomMin();
    void eraseCustomMax();

protected:
    qreal dataMin();
    qreal dataMax();

    bool customLimitsDefined();
    bool customMinDefined();
    bool customMaxDefined();
    double customMin();
    double customMax();
    void eraseCustomLimits();

    bool waterfallApplied();
    qreal waterfallMin();
    qreal waterfallMax();
    void eraseWaterfallLimits();

    double displayedMin();
    double displayedMax();


protected:
    void setWaterfallMin(qreal min);
    void setWaterfallMax(qreal max);
    void setWaterfallLimits(qreal min, qreal max);
    void applyWaterfall(int itemPosition, int itemCount);

private:
    void defaultSettings();


private:
    bool customMinDefined_;
    bool customMaxDefined_;

    double customMin_;
    double customMax_;

    bool waterfallApplied_;

    double waterfallMin_;
    double waterfallMax_;
};

#endif // STRIPTOOLSERIES_H
