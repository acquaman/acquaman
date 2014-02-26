#ifndef STRIPTOOLSERIES_H
#define STRIPTOOLSERIES_H

#include "MPlot/MPlotSeries.h"
#include <QDebug>

class StripToolSeries : public MPlotSeriesBasic
{

public:
    StripToolSeries();
    friend class StripToolPV;
    friend class StripTool0DVariable;


public:
    MPlotAxisRange* displayedRange();
    void enableYNormalization(bool normOn, qreal ymin_axis, qreal ymax_axis);
    void enableWaterfall(bool waterfallOn, int itemPosition, int itemCount);


protected:
    qreal dataMin();
    qreal dataMax();
    MPlotAxisRange* dataRange();

    bool customLimitsDefined();
    bool customMinDefined();
    bool customMaxDefined();
    double customMin();
    double customMax();
    void setCustomLimits(qreal min, qreal max);
    void eraseCustomLimits();

    bool waterfallApplied();
    qreal waterfallMin();
    qreal waterfallMax();
    void eraseWaterfallLimits();

    double displayedMin();
    double displayedMax();


private:
    void setWaterfallMin(qreal min);
    void setWaterfallMax(qreal max);
    void setWaterfallLimits(qreal min, qreal max);
    void applyWaterfall(int itemPosition, int itemCount);

    void setCustomMin(qreal min);
    void setCustomMax(qreal max);
    void eraseCustomMin();
    void eraseCustomMax();


private:
    bool customMinDefined_;
    bool customMaxDefined_;

    double dataMin_;
    double dataMax_;

    double customMin_;
    double customMax_;

    bool waterfallApplied_;

    double waterfallMin_;
    double waterfallMax_;
};

#endif // STRIPTOOLSERIES_H
