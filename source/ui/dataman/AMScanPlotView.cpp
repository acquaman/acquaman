#include "AMScanPlotView.h"

AMScanPlotView::AMScanPlotView(QWidget *parent) :
    QWidget(parent)
{
	plot_ = createDefaultPlot();
}

AMScanPlotView::~AMScanPlotView()
{

}

void AMScanPlotView::setXAxisUnits(const QString &newUnits)
{
	if (xAxisUnits_ != newUnits) {
		xAxisUnits_ = newUnits;

		updateXAxisName();

		emit xAxisUnitsChanged(xAxisUnits_);
	}
}

void AMScanPlotView::setYAxisUnits(const QString &newUnits)
{
	if (yAxisUnits_ != newUnits) {
		yAxisUnits_ = newUnits;

		updateYAxisName();

		emit yAxisUnitsChanged(yAxisUnits_);
	}
}

void AMScanPlotView::setLogScaleEnabled(bool isEnabled)
{
	if (logScaleEnabled_ != isEnabled) {
		logScaleEnabled_ = isEnabled;
		emit logScaleEnabledChanged(logScaleEnabled_);
	}
}

void AMScanPlotView::setNormalizationEnabled(bool isEnabled)
{
	if (normalizationEnabled_ != isEnabled) {
		normalizationEnabled_ = isEnabled;
		emit normalizationEnabledChanged(normalizationEnabled_);
	}
}

void AMScanPlotView::setWaterfallOffsetEnabled(bool isEnabled)
{
	if (waterfallOffsetEnabled_ != isEnabled) {
		waterfallOffsetEnabled_ = isEnabled;
		emit waterfallOffsetEnabledChanged(waterfallOffsetEnabled_);
	}
}

void AMScanPlotView::setWaterfallOffset(double newOffset)
{
	if (waterfallOffset_ != newOffset) {
		waterfallOffset_ = newOffset;
		emit waterfallOffsetChanged(waterfallOffset_);
	}
}

void AMScanPlotView::updateXAxisName()
{

}

void AMScanPlotView::updateYAxisName()
{

}

void AMScanPlotView::addDataSource(AMDataSource *newSource)
{
	Q_UNUSED(newSource)
}

void AMScanPlotView::removeDataSource(AMDataSource *source)
{
	Q_UNUSED(source)
}

MPlotGW* AMScanPlotView::createDefaultPlot()
{
	MPlotGW* rv = new MPlotGW();
	rv->plot()->plotArea()->setBrush(QBrush(QColor(Qt::white)));
	rv->plot()->axisBottom()->setTicks(4);
	rv->plot()->axisTop()->setTicks(4);
	rv->plot()->axisLeft()->showGrid(false);
	rv->plot()->axisScaleLeft()->setAutoScaleEnabled();
	rv->plot()->axisScaleBottom()->setAutoScaleEnabled();
	rv->plot()->axisScaleRight()->setAutoScaleEnabled();

	rv->plot()->axisBottom()->showAxisName(true);
	rv->plot()->axisLeft()->showAxisName(false);
	rv->plot()->axisRight()->showAxisName(true);

	rv->plot()->setMarginRight(rv->plot()->marginLeft());

	return rv;
}
