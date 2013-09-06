#include "AMXRFBaseDetectorView.h"

#include "MPlot/MPlotAxisScale.h"
#include "MPlot/MPlotTools.h"
#include "dataman/datasource/AMDataSourceSeriesData.h"

#include <QVBoxLayout>

AMXRFBaseDetectorView::AMXRFBaseDetectorView(AMXRFDetector *detector, QWidget *parent)
	: QWidget(parent)
{
	detector_ = detector;

	setupPlot();

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(view_);

	setLayout(layout);
}

void AMXRFBaseDetectorView::setupPlot()
{
	// Create the plot window.
	view_ = new MPlotWidget;
	view_->enableAntiAliasing(true);

	// Create the plot and setup all the axes.
	plot_ = new MPlot;
	plot_->axisBottom()->setAxisNameFont(QFont("Helvetica", 6));
	plot_->axisBottom()->setTickLabelFont(QFont("Helvetica", 6));
	plot_->axisBottom()->setAxisName("Energy, eV");
	plot_->axisLeft()->setAxisNameFont(QFont("Helvetica", 6));
	plot_->axisLeft()->setTickLabelFont(QFont("Helvetica", 6));
	plot_->axisLeft()->setAxisName("Counts");

	// Set the margins for the plot.
	plot_->setMarginLeft(10);
	plot_->setMarginBottom(15);
	plot_->setMarginRight(2);
	plot_->setMarginTop(2);

	MPlotSeriesBasic *test = new MPlotSeriesBasic;
	test->setModel(new AMDataSourceSeriesData(detector_->dataSource()), true);
	test->setMarker(MPlotMarkerShape::None);
	test->setDescription(detector_->dataSource()->name());
	test->setLinePen(QPen(Qt::red));
	plot_->addItem(test);

	((AM1DProcessVariableDataSource *)detector_->dataSource())->setScale(detector_->axes().at(0).increment);
	// Enable autoscaling of both axes.
	plot_->axisScaleLeft()->setAutoScaleEnabled();
	plot_->axisScaleBottom()->setAutoScaleEnabled();

	// Enable some convenient zoom tools.
	plot_->addTool(new MPlotDragZoomerTool());
	plot_->addTool(new MPlotWheelZoomerTool());
	view_->setPlot(plot_);
	view_->setMinimumHeight(450);

	// Set the number of ticks.  A balance between readability and being practical.
	plot_->axisBottom()->setTicks(3);
	plot_->axisTop()->setTicks(0);
	plot_->axisLeft()->setTicks(4);
	plot_->axisRight()->setTicks(0);

	// Set the autoscale constraints.
	plot_->axisScaleLeft()->setDataRangeConstraint(MPlotAxisRange(0, MPLOT_POS_INFINITY));
}
