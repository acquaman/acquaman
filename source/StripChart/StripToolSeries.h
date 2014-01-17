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

public:
    bool customLimitsDefined();
    bool customMinDefined();
    bool customMaxDefined();
    MPlotAxisRange* displayedRange();


protected:
    void setCustomLimits(qreal min, qreal max);
    void eraseCustomLimits();

    qreal dataMax();
    qreal dataMin();

    void setCustomMin(qreal min);
    void eraseCustomMin();
    double customMin();

    void setCustomMax(qreal max);
    void eraseCustomMax();
    double customMax();

    double displayedMin();
    double displayedMax();

    MPlotAxisRange* dataRange();

    void enableYAxisNormalization(bool on, qreal min, qreal max);

};

#endif // STRIPTOOLSERIES_H
