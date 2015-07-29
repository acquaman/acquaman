#include "AMScanPlotViewProperties.h"

AMScanPlotViewProperties::AMScanPlotViewProperties(QObject *parent) :
    QObject(parent)
{
	// Initialize class variables.

	xAxisUnits_ = "";
	yAxisUnits_ = "";
	logScaleEnabled_ = false;
	normalizationEnabled_ = false;
	waterfallOffsetEnabled_ = false;
	waterfallOffset_ = 0.0;
}

AMScanPlotViewProperties::~AMScanPlotViewProperties()
{

}

void AMScanPlotViewProperties::setXAxisUnits(const QString &newUnits)
{
	if (xAxisUnits_ != newUnits) {
		xAxisUnits_ = newUnits;
		emit xAxisUnitsChanged(xAxisUnits_);
	}
}

void AMScanPlotViewProperties::setYAxisUnits(const QString &newUnits)
{
	if (yAxisUnits_ != newUnits) {
		yAxisUnits_ = newUnits;
		emit yAxisUnitsChanged(yAxisUnits_);
	}
}

void AMScanPlotViewProperties::setLogScaleEnabled(bool isEnabled)
{
	if (logScaleEnabled_ != isEnabled) {
		logScaleEnabled_ = isEnabled;
		emit logScaleEnabledChanged(logScaleEnabled_);
	}
}

void AMScanPlotViewProperties::setNormalizationEnabled(bool isEnabled)
{
	if (normalizationEnabled_ != isEnabled) {
		normalizationEnabled_ = isEnabled;
		emit normalizationEnabledChanged(normalizationEnabled_);
	}
}

void AMScanPlotViewProperties::setWaterfallOffsetEnabled(bool isEnabled)
{
	if (waterfallOffsetEnabled_ != isEnabled) {
		waterfallOffsetEnabled_ = isEnabled;
		emit waterfallOffsetEnabledChanged(waterfallOffsetEnabled_);
	}
}

void AMScanPlotViewProperties::setWaterfallOffset(double newOffset)
{
	if (waterfallOffset_ != newOffset) {
		waterfallOffset_ = newOffset;
		emit waterfallOffsetChanged(waterfallOffset_);
	}
}

void AMScanPlotViewProperties::setAxisDataRangeConstraint(MPlot::StandardAxis axis, const MPlotAxisRange &range)
{
//	bool newAxis = false;
//	bool newRange = false;

//	if (!axisDataRangeConstraintMap_.contains(axis))
//		newAxis = true;

//	else if (axisDataRangeConstraintMap_.value(axis) != range)
//		newRange = true;

//	if (newAxis || newRange) {
//		axisDataRangeConstraintMap_.insert(axis, range);
//		emit axisDataRangeConstraintChanged();
//	}
}
