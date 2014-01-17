#include "StripToolSeries.h"

StripToolSeries::StripToolSeries()
{
    customMinDefined_ = false;
    customMaxDefined_ = false;

    customMin_ = 0.0;
    customMax_ = 0.0;

}



bool StripToolSeries::customMinDefined()
{
    return customMinDefined_;
}



bool StripToolSeries::customMaxDefined()
{
    return customMaxDefined_;
}



bool StripToolSeries::customLimitsDefined()
{
    return customMinDefined() || customMaxDefined();
}



void StripToolSeries::setCustomLimits(qreal min, qreal max)
{
    if (max <= min) {
        qDebug() << "StripToolSeries :: Invalid display limits entered. No change made.";
        return;
    }

    customMinDefined_ = true;
    customMaxDefined_ = false;

    customMin_ = min;
    customMax_ = max;
}



void StripToolSeries::eraseCustomLimits()
{
    customMinDefined_ = false;
    customMaxDefined_ = false;

    customMin_ = 0.0;
    customMax_ = 0.0;
}



qreal StripToolSeries::dataMin()
{
    qreal max = model()->boundingRect().top();
    qreal min = model()->boundingRect().bottom();
    qreal trueMin;

    if (max > min) {
        trueMin = min;
    } else {
        trueMin = max;
    }

//    qDebug() << "Minimum data value :" << trueMin;
    return trueMin;
}



qreal StripToolSeries::dataMax()
{
    qreal max = model()->boundingRect().top();
    qreal min = model()->boundingRect().bottom();
    qreal trueMax;

    if (max > min) {
        trueMax = max;
    } else {
        trueMax = min;
    }

//    qDebug() << "Maximum data value :" << trueMax;
    return trueMax;
}



void StripToolSeries::setCustomMin(qreal min)
{
    qDebug() << "StripToolSeries :: setting custom y axis minimum :" << min;
    customMinDefined_ = true;
    customMin_ = min;
}



void StripToolSeries::eraseCustomMin()
{
    customMinDefined_ = false;
    customMin_ = 0.0;
}



double StripToolSeries::customMin()
{
    return customMin_;
}



void StripToolSeries::setCustomMax(qreal max)
{
    qDebug() << "StripToolSeries :: setting custom y axis maximum :" << max;
    customMaxDefined_ = true;
    customMax_ = max;
}



void StripToolSeries::eraseCustomMax()
{
    customMaxDefined_ = false;
    customMax_ = 0.0;
}



double StripToolSeries::customMax()
{
    return customMax_;
}



double StripToolSeries::displayedMin()
{
    if (customMinDefined())
        return customMin();
    else
        return dataMin();
}



double StripToolSeries::displayedMax()
{
    if (customMaxDefined())
        return customMax();
    else
        return dataMax();
}



MPlotAxisRange* StripToolSeries::displayedRange()
{
    MPlotAxisRange *displayRange;

    if (!customLimitsDefined()) {
        displayRange = dataRange();

    } else {
        displayRange = new MPlotAxisRange(displayedMin(), displayedMax());
    }

//    qDebug() << "StripToolSeries :: returning the current display range :" << displayRange->min() << "to" << displayRange->max();
    return displayRange;

}



MPlotAxisRange* StripToolSeries::dataRange()
{
//    qDebug() << "StripToolSeries :: returning the current data range :" << dataMin() << "to" << dataMax();
    MPlotAxisRange *dataRange = new MPlotAxisRange(dataMin(), dataMax());
    return dataRange;
}




void StripToolSeries::enableYAxisNormalization(bool on, qreal min, qreal max)
{
    if (customLimitsDefined() && on) {
        normXMin_ = customMin();
        normXMax_ = customMax();

    } else if (!customLimitsDefined() && on) {
        normXMin_ = min;
        normXMax_ = max;

    } else {
        sx_ = 1.0;
        dx_ = 0.0;
    }

    MPlotSeriesBasic::onDataChanged();
}
