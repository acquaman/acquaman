#include "StripToolSeries.h"

StripToolSeries::StripToolSeries()
    : MPlotSeriesBasic()
{
    customMinDefined_ = false;
    customMaxDefined_ = false;

    customMin_ = 0.0;
    customMax_ = 0.0;

    waterfallApplied_ = false;

    waterfallMin_ = 0.0;
    waterfallMax_ = 0.0;

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
    customMaxDefined_ = true;

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



bool StripToolSeries::waterfallApplied()
{
    return waterfallApplied_;
}



double StripToolSeries::waterfallMin()
{
    return waterfallMin_;
}



void StripToolSeries::setWaterfallMin(double min)
{
    waterfallMin_ = min;
}



double StripToolSeries::waterfallMax()
{
    return waterfallMax_;
}



void StripToolSeries::setWaterfallMax(double max)
{
    waterfallMax_ = max;
}



void StripToolSeries::setWaterfallLimits(double min, double max)
{
    if (min < max) {

        if (!waterfallApplied_) {
            qDebug() << "StripToolSeries :: waterfall added.";
        }

        waterfallApplied_ = true;
        setWaterfallMin(min);
        setWaterfallMax(max);

//        qDebug() << "StripToolSeries :: min =" << min;
//        qDebug() << "StripToolSeries :: max=" << max;

    } else {

        qDebug() << "StripToolSeries :: the minimum waterfall limit cannot be greater than or equal to the maximum! No change made.";
    }
}



void StripToolSeries::eraseWaterfallLimits()
{
    if (waterfallApplied_) {
        qDebug() << "StripToolSeries :: waterfall removed.";
    }

    waterfallApplied_ = false;
    setWaterfallMin(0.0);
    setWaterfallMax(0.0);
}



qreal StripToolSeries::dataMin()
{
    int count = model()->count();

    qreal min = 0;

    for (int i = 0; i < count; i++) {
        if (i == 0)
            min = model()->y(i);

        else if (min > model()->y(i))
            min = model()->y(i);
    }

    return min;
}



qreal StripToolSeries::dataMax()
{
    int count = model()->count();

    qreal max = 0;

    for (int i = 0; i < count; i++) {
        if (i == 0)
            max = model()->y(i);

        else if (max < model()->y(i))
            max = model()->y(i);
    }

    return max;
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
    if (waterfallApplied())
        return waterfallMin();

    else if (customMinDefined())
        return customMin();

    else
        return dataMin();
}



double StripToolSeries::displayedMax()
{
    if (waterfallApplied())
        return waterfallMax();

    else if (customMaxDefined())
        return customMax();

    else
        return dataMax();
}



MPlotAxisRange* StripToolSeries::displayedRange()
{
//    qDebug() << "StripToolSeries :: returning the current display range :" << displayRange->min() << "to" << displayRange->max();
    double max = displayedMax();
    double min = displayedMin();

//    if (min < max)
        return new MPlotAxisRange(displayedMin(), displayedMax());
//    else
//        return new MPlotAxisRange(min - 5, max + 5);

//    qreal min = dataRange()->min();
//    qreal max = dataRange()->max();

//    MPlotAxisRange *displayRange;

//    if (waterfallApplied()) {
//        return MPlotAxisRange(waterfallMin(), waterfallMax());

//    } else if

//    if (!customLimitsDefined()) {

//        if (min == max && min == 0) {
//            displayRange = new MPlotAxisRange(-1, 1);

//        } else if (min == max) {
//            displayRange = new MPlotAxisRange(0.95 * min, 1.05 * max);

//        } else {
//            displayRange = dataRange();
//        }

//    } else {
//        displayRange = new MPlotAxisRange(displayedMin(), displayedMax());
//    }

//    return displayRange;

}



MPlotAxisRange* StripToolSeries::dataRange()
{
//    qDebug() << "StripToolSeries :: returning the current data range :" << dataMin() << "to" << dataMax();
    MPlotAxisRange *dataRange = new MPlotAxisRange(dataMin(), dataMax());
    return dataRange;
}



void StripToolSeries::enableWaterfall(bool waterfallOn, int itemPosition, int itemCount)
{
    if (waterfallOn) {
        applyWaterfall(itemPosition, itemCount);

    } else {
        eraseWaterfallLimits();
    }
}



void StripToolSeries::enableYNormalization(bool normOn, qreal axisMin, qreal axisMax)
{
    double pvYMin, pvYMax;

    if (waterfallApplied() || customLimitsDefined()) {

        if (axisMin == displayedMin())
            pvYMin = dataMin();

        else
            pvYMin = axisMin + (dataMin() - displayedMin()) / (displayedMax() - displayedMin()) * (axisMax - axisMin);


        if (dataMin() == dataMax())
            pvYMax = pvYMin;

        else if (axisMax == displayedMax())
            pvYMax = dataMax();

        else
            pvYMax = axisMax - (displayedMax() - dataMax()) / (displayedMax() - customMin()) * (axisMax - axisMin);

    } else {

        pvYMin = axisMin;
        pvYMax = axisMax;
    }

    enableYAxisNormalization(normOn, pvYMin, pvYMax);

}



void StripToolSeries::applyWaterfall(int itemPosition, int itemCount)
{
    // we begin by dividing the series' axis into 'itemCount' pieces of size 'sectionSize'.
    double sectionSize, min, max;

    if (dataMax() == dataMin()) {

        if (customLimitsDefined() && customMax() != customMin()) {
            qDebug() << "StripToolSeries :: Having dataMax = dataMin will not allow the waterfall to display properly. However, there appear to be appropriate custom limits defined. Using these instead.";
            sectionSize = customMax() - customMin();
            min = customMin();
            max = customMax();

        } else {
            qDebug() << "StripToolSeries :: Having dataMax = dataMin will not allow the waterfall to display properly. Custom limits are not defined. Selecting arbitrary range (10) to display waterfall for this pv.";
            sectionSize = 10;
            min = dataMin() - 5;
            max = dataMax() + 5;
        }

    } else {

        sectionSize = dataMax() - dataMin();
        min = dataMin();
        max = dataMax();
    }

    // if the item is the first to be displayed, it's position is 0.
    // its section should be the one at the very top of the graph (ie customMax = dataMax()).
    // the minimum should be chosen so that when normalized, dataMin() is at the lower boundary for this section.

    double waterfallMin = min + (itemPosition - itemCount + 1) * sectionSize;
    double waterfallMax = max + itemPosition * sectionSize;

    setWaterfallLimits(waterfallMin, waterfallMax);

}

