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


#include "StripToolSeries2.h"

StripToolSeries2::StripToolSeries2(QObject *parent) :
    QObject(parent)
{
    series_ = 0;

    buildComponents();
    makeConnections();
    defaultSettings();

    qDebug() << "StripToolSeries2 object created.";
}



MPlotSeriesBasic* StripToolSeries2::series() const
{
    return series_;
}



MPlotAxisRange* StripToolSeries2::displayedRange()
{
    MPlotAxisRange* displayedRange = 0;

    if (displayedMax() - displayedMin() == 0) {
        displayedRange = new MPlotAxisRange(-1, 1);

    } else {
        displayedRange = new MPlotAxisRange(dataMin(), dataMax());
    }

    return displayedRange;
}



MPlotAxisRange* StripToolSeries2::dataRange()
{
    return new MPlotAxisRange(dataMin(), dataMax());
}



qreal StripToolSeries2::dataMin() const
{
    int count = series_->model()->count();

    if (count > 0) {

        qreal min = series_->model()->y(0);

        for (int i = 0; i < count; i++) {
            if (min > series_->model()->y(i))
                min = series_->model()->y(i);
        }

        return min;

    } else {
        return 0;
    }
}



qreal StripToolSeries2::dataMax() const
{
    int count = series_->model()->count();

    if (count > 0) {

        qreal max = series_->model()->y(0);

        for (int i = 0; i < count; i++) {
            if (max < series_->model()->y(i))
                max = series_->model()->y(i);
        }

        return max;

    } else {
        return 0;
    }
}



bool StripToolSeries2::customMinDefined() const
{
    return customMinDefined_;
}



bool StripToolSeries2::customMaxDefined() const
{
    return customMaxDefined_;
}



double StripToolSeries2::customMin() const
{
    return customMin_;
}



double StripToolSeries2::customMax() const
{
    return customMax_;
}



double StripToolSeries2::displayedMin() const
{
    double displayedMin;

    if (waterfallApplied()) {
        displayedMin = waterfallMin();

    } else if (customMinDefined()) {
        displayedMin = customMin();

    } else {
        displayedMin = dataMin();
    }

    return displayedMin;
}



double StripToolSeries2::displayedMax() const
{
    double displayedMax;
    QString source;

    if (waterfallApplied()) {
        displayedMax = waterfallMax();
        source = "waterfall";

    } else if (customMaxDefined()) {
        displayedMax = customMax();
        source = "custom";

    } else {
        displayedMax = dataMax();
        source = "data";
    }

//    qDebug() << "Displayed max : " << source << ":" << displayedMax;

    return displayedMax;
}



bool StripToolSeries2::waterfallApplied() const
{
    return waterfallApplied_;
}



qreal StripToolSeries2::waterfallMin() const
{
    return waterfallMin_;
}



qreal StripToolSeries2::waterfallMax() const
{
    return waterfallMax_;
}



void StripToolSeries2::setData(MPlotVectorSeriesData *newData)
{
    series_->setModel(newData, true);
//    connect( newData->signalSource(), SIGNAL(dataChanged()), this, SLOT(onDataUpdate()) );

}



void StripToolSeries2::setCustomMin(qreal min)
{
    customMinDefined_ = true;
    customMin_ = min;
}



void StripToolSeries2::setCustomMax(qreal max)
{
    customMaxDefined_ = true;
    customMax_ = max;
}



void StripToolSeries2::eraseCustomMin()
{
    customMinDefined_ = false;
    customMin_ = 0;
}



void StripToolSeries2::eraseCustomMax()
{
    customMaxDefined_ = false;
    customMax_ = 0;
}



void StripToolSeries2::setWaterfallMin(double min)
{
    waterfallMin_ = min;
}



void StripToolSeries2::setWaterfallMax(double max)
{
    waterfallMax_ = max;
}



void StripToolSeries2::eraseWaterfallLimits()
{
    if (waterfallApplied_) {
        qDebug() << "StripToolSeries2 :: waterfall removed.";
    }

    waterfallApplied_ = false;
    setWaterfallMin(0.0);
    setWaterfallMax(0.0);
}



void StripToolSeries2::enableYNormalization(bool normOn, qreal ymin_axis, qreal ymax_axis)
{
    // we will be effectively mapping the current display properties of this series into the current axis coordinates (ymin_axis, ymax_axis). the current axis coordinates will depend on which pv is selected.
    // if this is working, the plot itself should look the same independent of which pv is selected--the pvs should appear to be unaffected by the change in axis scale. Only the y axis scale appears to change.

    // seriesMin_axis, seriesMax_axis are the boundaries of this series, in the axis coordinates. these are what we are trying to find.
    double seriesMin_axis, seriesMax_axis;

//    if (!customLimitsDefined() && dataMin() == dataMax() && model()->count() > 2) {
    if (!customMinDefined_ && !customMaxDefined_ && dataMin() == dataMax() && series_->model()->count() > 2) {
        qDebug() << "StripToolSeries :: dataMin and dataMax for this series are identical, and there are no custom limits defined. We are defining custom limits automatically (+/- 5) so that this series will display properly!";
        setCustomMin(dataMin() - 5);
        setCustomMax(dataMax() + 5);
    }

    // if we have special limits defined (either custom limits, or waterfall is enabled) then the range of values that this series is mapped to will depend on them.
    if (waterfallApplied() || (customMinDefined() || customMaxDefined())) {

        // the space that the series occupies will appear to be sandwiched between empty gaps of size seriesMin/MaxBuffer_series, and the buns should be conserved in the new coordinates. I... honestly can't think of a better way to explain this.
        // okay : we want the 'buffer' distance between the minimum data value and the minimum of the displayed range of the series to be conserved. This value is in series coordinates.
        // sames goes for max data value and max display limit.
        double seriesMinBuffer_series = dataMin() - displayedMin();
        double seriesMaxBuffer_series = displayedMax() - dataMax();

        // we want the series to appear to occupy the same amount of 'space', regardless of which pv is selected. here is the total range of values that the series occupies now (including buffers), in series coordinates.
        double seriesDisplayRange_series = displayedMax() - displayedMin();

        // this is the range of values that we will be mapping to--the current axis range. in axis coordinates.
        double axisRange_axis = ymax_axis - ymin_axis;

        // Darren's note:  We typically will never want to have a height range of zero because that will cause issues within the painter because you will accidentally create a NULL QRect, which obviously can never be painted.
        if (axisRange_axis == 0)
            axisRange_axis = 1;

        // now, we want to convert the buffer values to the axis coordinates, and then exclude them from the series range in axis coordinates.
        // remember that we will be normalizing the series to be between seriesMin/Max_axis--we are only interested in the range that the series itself occupies, without the buffers, for this (ie the range over which a line appears on the plot).
        seriesMin_axis = ymin_axis + (seriesMinBuffer_series / seriesDisplayRange_series) * (axisRange_axis);
        seriesMax_axis = ymax_axis - (seriesMaxBuffer_series / seriesDisplayRange_series) * (axisRange_axis);


    // if we do not have special limits in effect, the series is mapped directly to the full range of the axis coordinates.
    } else {

        seriesMin_axis = ymin_axis;
        seriesMax_axis = ymax_axis;
    }

    // Darren's note:  We typically will never want to have a height range of zero because that will cause issues within the painter because you will accidentally create a NULL QRect, which obviously can never be painted.
    if (seriesMax_axis == 0)
        seriesMax_axis = 1;

    // and now we normalize this series to be between the seriesMin/Max_axis values! ta da!
    series_->enableYAxisNormalization(normOn, seriesMin_axis, seriesMax_axis);
}



void StripToolSeries2::enableWaterfall(bool waterfallOn, int itemPosition, int itemCount)
{
    if (waterfallOn) {
        applyWaterfall(itemPosition, itemCount);

    } else {
        eraseWaterfallLimits();
    }
}



void StripToolSeries2::applyWaterfall(int itemPosition, int itemCount)
{
    // we begin by dividing the series' axis into 'itemCount' pieces of size 'sectionSize'.
    // the min and max values are either the dataMin/Max or the customMin/Max for that pv.
    double sectionSize, min, max;

    if ((!customMinDefined() && !customMaxDefined()) && dataMin() == dataMax()) {
        qDebug() << "StripToolSeries :: The data min and max values for this pv are identical! This will not allow the waterfall to display properly.";
        qDebug() << "StripToolSeries :: Automatically introducing a difference in the form of custom min and max values for this pv.";
        setCustomMin(dataMin() - 5);
        setCustomMax(dataMax() + 5);

    } else {

        if (!customMinDefined()) {
            min = dataMin();

        } else if (!customMaxDefined()) {
            max = dataMax();
        }
    }

    if (customMinDefined() && customMaxDefined()) {

        if (customMin() == customMax()) {
            qDebug() << "StripToolSeries :: The user defined max and min for this pv are identical! This will not allow the waterfall to display properly.";
            qDebug() << "StripToolSeries :: Automatically introducing a difference in custom min and max for this pv.";
            setCustomMin(customMin() - 5);
            setCustomMax(customMax() + 5);
        }

        min = customMin();
        max = customMax();
    }

    sectionSize = max - min;



//    if (dataMax() == dataMin()) {

//        if (customLimitsDefined() && customMax() != customMin()) {
//            qDebug() << "StripToolSeries :: Having dataMax = dataMin will not allow the waterfall to display properly. However, there appear to be appropriate custom limits defined. Using these instead.";
//            sectionSize = customMax() - customMin();
//            min = customMin();
//            max = customMax();

//        } else {
//            qDebug() << "StripToolSeries :: Having dataMax = dataMin will not allow the waterfall to display properly. Custom limits are not defined. Selecting arbitrary range (10) to display waterfall for this pv.";
//            sectionSize = 10;
//            min = dataMin() - 5;
//            max = dataMax() + 5;
//        }

//    } else {

//        if (customLimitsDefined()) {
//            sectionSize = customMax() - customMin();
//            min = customMin();
//            max = customMax();

//        } else {

//            sectionSize = dataMax() - dataMin();
//            min = dataMin();
//            max = dataMax();
//        }
//    }

    // if the item is the first to be displayed, it's position is 0.
    // its section should be the one at the very top of the graph (ie customMax = dataMax()).
    // the minimum should be chosen so that when normalized, dataMin() is at the lower boundary for this section.

    double waterfallMin = min + (itemPosition - itemCount + 1) * sectionSize;
    double waterfallMax = max + itemPosition * sectionSize;

    setWaterfallMin(waterfallMin);
    setWaterfallMax(waterfallMax);

//    setWaterfallLimits(waterfallMin, waterfallMax);
}



//void StripToolSeries2::onDataUpdate()
//{
//    qDebug() << "StripToolSeries2 :: data update.";
//    emit displayRangeUpdated( new MPlotAxisRange(displayedMin(), displayedMax()) );
//    emit dataRangeUpdated( new MPlotAxisRange(dataMin(), dataMax()) );
//}



void StripToolSeries2::buildComponents()
{
    series_ = new MPlotSeriesBasic();
}



void StripToolSeries2::makeConnections()
{
    // data connections made in setData(...).
}



void StripToolSeries2::defaultSettings()
{
    customMinDefined_ = false;
    customMaxDefined_ = false;
    waterfallApplied_ = false;

    customMin_ = 0;
    customMax_ = 0;

    waterfallMin_ = 0;
    waterfallMax_ = 0;
}
