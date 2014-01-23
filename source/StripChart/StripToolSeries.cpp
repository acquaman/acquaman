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
    int count = model()->count();
    qDebug() << "Count :" << count;

    qreal min = 0;

    for (int i = 0; i < count; i++) {
        if (i == 0)
            min = model()->y(i);

        else if (min > model()->y(i))
            min = model()->y(i);

        qDebug() << "\tData value :" << model()->y(i);
    }

    qDebug() << "Minimum data value :" << min;
    return min;
}



qreal StripToolSeries::dataMax()
{
    int count = model()->count();
    qDebug() << "Count :" << count;
    qreal max = 0;

    for (int i = 0; i < count; i++) {
        if (i == 0)
            max = model()->y(i);

        else if (max < model()->y(i))
            max = model()->y(i);
    }

    qDebug() << "Maximum data value :" << max;
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
    qreal min = dataRange()->min();
    qreal max = dataRange()->max();

    MPlotAxisRange *displayRange;

    if (!customLimitsDefined()) {

        if (min == max && min == 0) {
            displayRange = new MPlotAxisRange(-1, 1);

        } else if (min == max) {
            displayRange = new MPlotAxisRange(0.95 * min, 1.05 * max);

        } else {
            displayRange = dataRange();
        }

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



void StripToolSeries::enableYNormalization(bool on, qreal normMin, qreal normMax)
{
    double pvYMin, pvYMax;

    if (customLimitsDefined()) {

        if (normMin == customMin())
            pvYMin = dataMin();

        else
            pvYMin = normMin + (dataMin() - customMin()) / (customMax() - customMin()) * (normMax - normMin);


        if (dataMin() == dataMax())
            pvYMax = pvYMin;

        else if (normMax == customMax())
            pvYMax = dataMax();

        else
            pvYMax = normMax - (customMax() - dataMax()) / (customMax() - customMin()) * (normMax - normMin);

    } else {

        pvYMin = normMin;
        pvYMax = normMax;

    }

    enableYAxisNormalization(on, pvYMin, pvYMax);

}




//void StripToolSeries::enableYAxisNormalization(bool on, qreal min, qreal max)
//{
//    qDebug() << "The series is being normalized according to StripToolSeries method.";
//    qDebug() << "newMin = " << min;
//    qDebug() << "newMax = " << max;

//    if (customLimitsDefined() && on) {
//        yAxisNormalizationOn_ = on;

//        if (min == customMin()) {
//            normYMin_ = dataMin();

//        } else {
//            normYMin_ = min + (dataMin() - customMin()) / (customMax() - customMin()) * (max - min);
//        }


//        if (dataMin() == dataMax()) {
//            normYMax_ = normYMin_;

//        } else if (max == customMax()) {
//            normYMax_ = dataMax();

//        } else {
//            normYMax_ = max - (customMax() - dataMax()) / (customMax() - customMin()) * (max - min);
//        }


//        qDebug() << "using custom limits : normYMin = " << normYMin_;
//        qDebug() << "using custom limits : normYMax = " << normYMax_;



//    } else if (!customLimitsDefined() && on) {
//        yAxisNormalizationOn_ = on;

//        normYMin_ = min;
//        normYMax_ = max;

//        qDebug() << "using data limits : normYMin = " << normYMin_;
//        qDebug() << "using data limits : normYMax = " << normYMax_;

//    } else {
//        sx_ = 1.0;
//        dx_ = 0.0;
//    }

//    MPlotSeriesBasic::onDataChanged();
//}
