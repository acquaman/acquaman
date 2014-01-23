#ifndef STRIPTOOLSERIES_H
#define STRIPTOOLSERIES_H

#include "MPlot/MPlotSeries.h"
#include <QDebug>

class StripToolSeries : public MPlotSeriesBasic
{

public:
    StripToolSeries();
    friend class StripToolPV;

protected:
    bool customMinDefined_;
    bool customMaxDefined_;

    double dataMin_;
    double dataMax_;

    double customMin_;
    double customMax_;

    bool waterfallApplied_;

    double waterfallMin_;
    double waterfallMax_;

public:
    bool customLimitsDefined();
    bool customMinDefined();
    bool customMaxDefined();
    MPlotAxisRange* displayedRange();
    bool waterfallApplied();

    void enableYNormalization(bool normOn, qreal axisMin, qreal axisMax);
    void enableWaterfall(bool waterfallOn, int itemPosition, int itemCount);


protected:
    qreal dataMin();
    qreal dataMax();
    MPlotAxisRange* dataRange();

    double customMin();
    double customMax();
    void setCustomLimits(qreal min, qreal max);
    void eraseCustomLimits();

    qreal waterfallMin();
    qreal waterfallMax();
    void setWaterfallLimits(qreal min, qreal max);
    void eraseWaterfallLimits();

    double displayedMin();
    double displayedMax();


private:
    void setWaterfallMin(qreal min);
    void setWaterfallMax(qreal max);
    void applyWaterfall(int itemPosition, int itemCount);

    void setCustomMin(qreal min);
    void setCustomMax(qreal max);
    void eraseCustomMin();
    void eraseCustomMax();


};

#endif // STRIPTOOLSERIES_H
