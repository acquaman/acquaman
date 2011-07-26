#include "BeamPositionMonitor.h"
#include "dataman/AMDataSourceSeriesData.h"

#include <QVBoxLayout>

BeamPositionMonitor::BeamPositionMonitor(QWidget *parent) :
	QWidget(parent)
{
	xy_ = new AMXYScatterPVDataSource(new AMProcessVariable("darrenIsAwesome:x", true, this), new AMProcessVariable("darrenIsAwesome:y", true, this), "Look at me go!", this);
	intensity_ = new AMProcessVariable("darrenIsAwesome:Intensity", true, this);
	connect(intensity_, SIGNAL(valueChanged(double)), this, SLOT(onIntensityChanged(double)));

	setupPlot();

	QVBoxLayout *plotLayout = new QVBoxLayout;
	plotLayout->addWidget(view_);
	setLayout(plotLayout);
}

void BeamPositionMonitor::setupPlot()
{
	// Create the plot window.
	view_ = new MPlotWidget;
	view_->enableAntiAliasing(true);

	// Create the plot and setup all the axes.
	plot_ = new MPlot;
	plot_->axisBottom()->setAxisNameFont(QFont("Helvetica", 6));
	plot_->axisBottom()->setTickLabelFont(QFont("Helvetica", 6));
	plot_->axisBottom()->setAxisName("X");
	plot_->axisLeft()->setAxisNameFont(QFont("Helvetica", 6));
	plot_->axisLeft()->setTickLabelFont(QFont("Helvetica", 6));
	plot_->axisLeft()->setAxisName("Y");

	// Set the margins for the plot.
	plot_->setMarginLeft(10);
	plot_->setMarginBottom(15);
	plot_->setMarginRight(2);
	plot_->setMarginTop(2);

	scatter_ = new MPlotSeriesBasic;
	scatter_->setModel(new AMDataSourceSeriesData(xy_), true);
	scatter_->setMarker(MPlotMarkerShape::Circle);
	scatter_->setDescription("Beam Position");

	plot_->addItem(scatter_);

	view_->setPlot(plot_);
	view_->setMinimumSize( 640, 480);

	// Set the number of ticks.  A balance between readability and being practical.
	plot_->axisBottom()->setTicks(4, MPlotAxis::Middle);
	plot_->axisTop()->setTicks(4, MPlotAxis::Middle);
	plot_->axisLeft()->setTicks(4, MPlotAxis::Middle);
	plot_->axisRight()->setTicks(4, MPlotAxis::Middle);
}
