#include "BeamPositionMonitor.h"
#include "dataman/AMDataSourceSeriesData.h"

#include <QVBoxLayout>

BeamPositionMonitor::BeamPositionMonitor(QWidget *parent) :
	QWidget(parent)
{
	// Change these PVs to be something real rather than me stroking my own ego.
	xy_ = new AMXYScatterPVDataSource(new AMProcessVariable("darrenIsAwesome:x", true, this), new AMProcessVariable("darrenIsAwesome:y", true, this), "Look at me go!", this);
	intensity_ = new AMProcessVariable("darrenIsAwesome:Intensity", true, this);
	connect(intensity_, SIGNAL(valueChanged(double)), this, SLOT(onIntensityChanged(double)));

	// This has to come after xy_ is set because we make a series in here using the member variable xy_.  If it's not valid, the class would never work because we haven't
	// programmed a way to change xy_ or intensity_ after the class is instantiated.
	setupPlot();

	// Make a layout and put the plot window in it.
	QVBoxLayout *plotLayout = new QVBoxLayout;
	plotLayout->addWidget(view_);

	// Pushes all changes to the layout to be viewed.
	setLayout(plotLayout);
}

BeamPositionMonitor::BeamPositionMonitor(QString name, AMProcessVariable *x, AMProcessVariable *y, AMProcessVariable *intensity, QWidget *parent)
	: QWidget(parent)
{
	xy_ = new AMXYScatterPVDataSource(x, y, name, this);
	intensity_ = intensity;
	connect(intensity_, SIGNAL(valueChanged(double)), this, SLOT(onIntensityChanged(double)));

	// This has to come after xy_ is set because we make a series in here using the member variable xy_.  If it's not valid, the class would never work because we haven't
	// programmed a way to change xy_ or intensity_ after the class is instantiated.
	setupPlot();

	// Make a layout and put the plot window in it.
	QVBoxLayout *plotLayout = new QVBoxLayout;
	plotLayout->addWidget(view_);

	// Pushes all changes to the layout to be viewed.
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

	// Makes a series to be viewed using our XY scatter data source object.  The series is created, the data source is set as the model to be viewed,
	// and a name and marker style are assigned to the point.
	scatter_ = new MPlotSeriesBasic;
	scatter_->setModel(new AMDataSourceSeriesData(xy_), true);
	scatter_->setMarker(MPlotMarkerShape::CrossCircle);
	scatter_->setDescription("Beam Position");

	// Add the series to the plot.
	plot_->addItem(scatter_);

	// Put the plot into the plot window.  This is what lets you see the plot inside a widget.
	view_->setPlot(plot_);
	view_->setMinimumSize( 640, 480);

	// Set the number of ticks.  A balance between readability and being practical.
	plot_->axisBottom()->setTicks(4, MPlotAxis::Middle);
	plot_->axisTop()->setTicks(4, MPlotAxis::Middle);
	plot_->axisLeft()->setTicks(4, MPlotAxis::Middle);
	plot_->axisRight()->setTicks(4, MPlotAxis::Middle);
}

void BeamPositionMonitor::onIntensityChanged(double val)
{
	/// \todo This needs to be changed to something real!  You will likely need to map whatever the value is coming from the intensity PV to a small value (1-30, think font sizes for a decent size reference).
	// Currently taking whatever the value is and using it directly.  Likely needs changing.
	scatter_->setMarker(MPlotMarkerShape::CrossCircle, val);
}
