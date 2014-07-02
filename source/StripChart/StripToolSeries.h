/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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
