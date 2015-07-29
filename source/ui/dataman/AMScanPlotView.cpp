#include "AMScanPlotView.h"

AMScanPlotView::AMScanPlotView(QWidget *parent) :
    QWidget(parent)
{
	plot_ = createDefaultPlot();

	properties_ = new AMScanPlotViewProperties(this);
}

AMScanPlotView::~AMScanPlotView()
{

}

void AMScanPlotView::setProperties(AMScanPlotViewProperties *newProperties)
{
	if (properties_ != newProperties) {

		if (properties_) {

		}

		properties_ = newProperties;

		if (properties_) {
			connect( properties_, SIGNAL(xAxisUnitsChanged(QString)), this, SLOT(updateXAxisName()) );
			connect( properties_, SIGNAL(yAxisUnitsChanged(QString)), this, SLOT(updateYAxisName()) );
			connect( properties_, SIGNAL(logScaleEnabledChanged(bool)), this, SLOT(updateLogScale()) );
			connect( properties_, SIGNAL(normalizationEnabledChanged(bool)), this, SLOT(updateNormalization()) );
			connect( properties_, SIGNAL(waterfallOffsetChanged(double)), this, SLOT(updateWaterfallOffset()) );
		}
	}
}

void AMScanPlotView::updateXAxisName()
{

}

void AMScanPlotView::updateYAxisName()
{

}

void AMScanPlotView::updateLogScale()
{

}

void AMScanPlotView::updateNormalization()
{

}

void AMScanPlotView::updateWaterfallOffset()
{
	if (properties_) {
		plot_->plot()->setAxisScaleWaterfall(MPlot::Left, properties_->waterfallOffset());
	}
}

void AMScanPlotView::updateDataRangeConstraints()
{
	if (properties_) {
		plot_->plot()->axisScale(MPlot::Left)->setDataRangeConstraint(properties_->dataRangeConstraint(MPlot::Left));
		plot_->plot()->axisScale(MPlot::Right)->setDataRangeConstraint(properties_->dataRangeConstraint(MPlot::Right));
		plot_->plot()->axisScale(MPlot::Bottom)->setDataRangeConstraint(properties_->dataRangeConstraint(MPlot::Bottom));
		plot_->plot()->axisScale(MPlot::Top)->setDataRangeConstraint(properties_->dataRangeConstraint(MPlot::Top));
	}
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
