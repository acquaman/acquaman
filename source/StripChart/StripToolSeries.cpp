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


#include "StripToolSeries.h"

StripToolSeries::StripToolSeries()
	: MPlotSeriesBasic()
{
	defaultSettings();
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
			waterfallApplied_ = true;
		}

		setWaterfallMin(min);
		setWaterfallMax(max);

		/*
	qDebug() << "StripToolSeries :: waterfall min =" << min;
	qDebug() << "StripToolSeries :: waterfall max =" << max;
	*/

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

	if (count > 0) {

		qreal min = model()->y(0);

		for (int i = 0; i < count; i++) {
			if (min > model()->y(i))
				min = model()->y(i);
		}

		return min;

	} else {
		return 0;
	}
}



qreal StripToolSeries::dataMax()
{
	int count = model()->count();

	if (count > 0) {

		qreal max = model()->y(0);

		for (int i = 0; i < count; i++) {
			if (max < model()->y(i))
				max = model()->y(i);
		}

		return max;

	} else {
		return 0;
	}
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
	double displayedMin;
	QString source;

	if (waterfallApplied()) {
		displayedMin = waterfallMin();
		source = "waterfall";

	} else if (customMinDefined()) {
		displayedMin = customMin();
		source = "custom";

	} else {
		displayedMin = dataMin();
		source = "data";

	}

	/*
	qDebug() << "Displayed min : " << source << ":" << displayedMin;
	*/

	return displayedMin;
}



double StripToolSeries::displayedMax()
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

	/*
    qDebug() << "Displayed max : " << source << ":" << displayedMax;
    */

	return displayedMax;
}



MPlotAxisRange* StripToolSeries::displayedRange()
{
	MPlotAxisRange* displayedRange = 0;

	if (displayedMax() - displayedMin() == 0) {
		displayedRange = new MPlotAxisRange(displayedMin() - 1, displayedMax() + 1);

	} else {
		displayedRange = new MPlotAxisRange(displayedMin(), displayedMax());
	}

	return displayedRange;
}



MPlotAxisRange* StripToolSeries::dataRange()
{
	return new MPlotAxisRange(dataMin(), dataMax());
}



void StripToolSeries::enableWaterfall(bool waterfallOn, int itemPosition, int itemCount)
{
	if (waterfallOn) {
		applyWaterfall(itemPosition, itemCount);

	} else {
		eraseWaterfallLimits();
	}
}



void StripToolSeries::enableYNormalization(bool normOn, qreal ymin_axis, qreal ymax_axis)
{
	// we will be effectively mapping the current display properties of this series into the current axis coordinates (ymin_axis, ymax_axis). the current axis coordinates will depend on which pv is selected.
	// if this is working, the plot itself should look the same independent of which pv is selected--the pvs should appear to be unaffected by the change in axis scale. Only the y axis scale appears to change.

	// seriesMin_axis, seriesMax_axis are the boundaries of this series, in the axis coordinates. these are what we are trying to find.
	double seriesMin_axis, seriesMax_axis;

	if (!customLimitsDefined() && dataMin() == dataMax() && model()->count() > 2) {
		qDebug() << "StripToolSeries :: dataMin and dataMax for this series are identical, and there are no custom limits defined. We are defining custom limits automatically (+/- 5) so that this series will display properly!";
		setCustomLimits(dataMin() - 5, dataMax() + 5);
	}

	// if we have special limits defined (either custom limits, or waterfall is enabled) then the range of values that this series is mapped to will depend on them.
	if (waterfallApplied() || customLimitsDefined()) {

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
	enableYAxisNormalization(normOn, seriesMin_axis, seriesMax_axis);

}



void StripToolSeries::applyWaterfall(int itemPosition, int itemCount)
{
	// we begin by dividing the series' axis into 'itemCount' pieces of size 'sectionSize'.
	// the min and max values are either the dataMin/Max or the customMin/Max for that pv.
	double sectionSize, min, max;

	if (!customLimitsDefined() && dataMin() == dataMax()) {
		qDebug() << "StripToolSeries :: The data min and max values for this pv are identical! This will not allow the waterfall to display properly.";
		qDebug() << "StripToolSeries :: Automatically introducing a difference in the form of custom min and max values for this pv.";
		setCustomLimits(dataMin() - 5, dataMax() + 5);

	} else if (!customLimitsDefined()) {
		min = dataMin();
		max = dataMax();
	}

	if (customLimitsDefined()) {

		if (customMin() == customMax()) {
			qDebug() << "StripToolSeries :: The user defined max and min for this pv are identical! This will not allow the waterfall to display properly.";
			qDebug() << "StripToolSeries :: Automatically introducing a difference in custom min and max for this pv.";
			setCustomLimits(customMin() - 5, customMax() + 5);
		}

		min = customMin();
		max = customMax();
	}

	sectionSize = max - min;

	/*
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

	if (customLimitsDefined()) {
	    sectionSize = customMax() - customMin();
	    min = customMin();
	    max = customMax();

	} else {

	    sectionSize = dataMax() - dataMin();
	    min = dataMin();
	    max = dataMax();
	}
    }
	*/

	// if the item is the first to be displayed, it's position is 0.
	// its section should be the one at the very top of the graph (ie customMax = dataMax()).
	// the minimum should be chosen so that when normalized, dataMin() is at the lower boundary for this section.

	double waterfallMin = min + (itemPosition - itemCount + 1) * sectionSize;
	double waterfallMax = max + itemPosition * sectionSize;

	setWaterfallLimits(waterfallMin, waterfallMax);
}



void StripToolSeries::defaultSettings()
{
	customMinDefined_ = false;
	customMaxDefined_ = false;

	customMin_ = 0.0;
	customMax_ = 0.0;

	waterfallApplied_ = false;

	waterfallMin_ = 0.0;
	waterfallMax_ = 0.0;
}

