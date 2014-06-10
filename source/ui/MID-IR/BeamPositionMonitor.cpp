/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "BeamPositionMonitor.h"
#include "dataman/datasource/AMDataSourceSeriesData.h"

#include <QVBoxLayout>

 BeamPositionMonitor::~BeamPositionMonitor(){}
BeamPositionMonitor::BeamPositionMonitor(QWidget *parent) :
	QWidget(parent)
{
	// Data source that takes in the new PVs for the x and y position.
	xy_ = new AMXYScatterPVDataSource(new AMProcessVariable("AO2023-02:Stage1:Voltage:X", true, this), new AMProcessVariable("AO2023-02:Stage1:Voltage:Y", true, this), "Look at me go!", this);

	/// These can be added back in when an intensity PV is available.
	//intensity_ = new AMProcessVariable("darrenIsAwesome:Intensity", true, this);
	//connect(intensity_, SIGNAL(valueChanged(double)), this, SLOT(onIntensityChanged(double)));

	// This has to come after xy_ is set because we make a series in here using the member variable xy_.  If it's not valid, the class would never work because we haven't
	// programmed a way to change xy_ or intensity_ after the class is instantiated.
	setupPlot();

	// Make a layout and put the plot window in it.
	QVBoxLayout *plotLayout = new QVBoxLayout;
	plotLayout->addWidget(view_);

	// Pushes all changes to the layout to be viewed.
	setLayout(plotLayout);
}

//BeamPositionMonitor::BeamPositionMonitor(QString name, AMProcessVariable *x, AMProcessVariable *y, AMProcessVariable *intensity, QWidget *parent)
BeamPositionMonitor::BeamPositionMonitor(QString name, AMProcessVariable *x, AMProcessVariable *y, QWidget *parent)
	: QWidget(parent)
{
	xy_ = new AMXYScatterPVDataSource(x, y, name, this);

	/// These can be added back in when an intensity PV is available.
	//intensity_ = intensity;
	//connect(intensity_, SIGNAL(valueChanged(double)), this, SLOT(onIntensityChanged(double)));

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
	scatter_->setMarker(MPlotMarkerShape::CrossCircle, 14);
	scatter_->setDescription("Beam Position");

	// Add the series to the plot.
	plot_->addItem(scatter_);

	// Put the plot into the plot window.  This is what lets you see the plot inside a widget.
	view_->setPlot(plot_);
	view_->setMinimumSize( 640, 480);

	// Set the range of the plot so we can actually see changes in the beam position.
	plot_->axisScaleBottom()->setDataRangeAndDisableAutoScaling(MPlotAxisRange(-0.2, 0.1));
	plot_->axisScaleLeft()->setDataRangeAndDisableAutoScaling(MPlotAxisRange(-0.1, 0.2));

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
