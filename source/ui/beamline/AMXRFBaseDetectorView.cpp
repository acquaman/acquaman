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


#include "AMXRFBaseDetectorView.h"

#include "MPlot/MPlotAxisScale.h"
#include "MPlot/MPlotTools.h"
#include "dataman/datasource/AMDataSourceSeriesData.h"

 AMXRFBaseDetectorView::~AMXRFBaseDetectorView(){}
AMXRFBaseDetectorView::AMXRFBaseDetectorView(AMXRFDetector *detector, QWidget *parent)
	: AMSpectrumAndPeriodicTableView(parent)
{
	detector_ = detector;
	topFrame_ = new AMTopFrame(detector_->description(), QIcon(":/utilities-system-monitor.png"));

	topLayout_ = new QHBoxLayout;
	bottomLayout_ = new QHBoxLayout;
	leftLayout_ = new QVBoxLayout;
	rightLayout_ = new QVBoxLayout;
	plotLayout_ = new QHBoxLayout;
	masterLayout_ = new QGridLayout;

	masterLayout_->addLayout(topLayout_, 0, 1);
	masterLayout_->addLayout(leftLayout_, 1, 0);
	masterLayout_->addLayout(plotLayout_, 1, 1);
	masterLayout_->addLayout(rightLayout_, 1, 2);
	masterLayout_->addLayout(bottomLayout_, 2, 1);

	QVBoxLayout *detectorLayout = new QVBoxLayout;
	detectorLayout->addWidget(topFrame_);
	detectorLayout->addLayout(masterLayout_);

	setLayout(detectorLayout);
}

void AMXRFBaseDetectorView::buildDetectorView()
{
	setupPlot();

	plotLayout_->addWidget(plotView_);

	acquireButton_ = new QPushButton(QIcon(":/22x22/media-playback-start.png"), "Acquire");
	acquireButton_->setMaximumHeight(25);
	connect(acquireButton_, SIGNAL(clicked()), this, SLOT(startAcquisition()));

	cancelButton_ = new QPushButton(QIcon(":/22x22/media-playback-stop.png"), "Stop");
	cancelButton_->setMaximumHeight(25);
	connect(cancelButton_, SIGNAL(clicked()), this, SLOT(cancelAcquisition()));

	statusLabel_ = new QLabel;
	statusLabel_->setPixmap(QIcon(":/32x32/greenLEDOff.png").pixmap(22));
	connect(detector_, SIGNAL(initializationStateChanged(AMDetector::InitializationState)), this, SLOT(onInitializationStateChanged(AMDetector::InitializationState)));
	connect(detector_, SIGNAL(acquisitionStateChanged(AMDetector::AcqusitionState)), this, SLOT(onAcquisitionStateChanged(AMDetector::AcqusitionState)));
	connect(detector_, SIGNAL(cleanupStateChanged(AMDetector::CleanupState)), this, SLOT(onCleanupStateChanged(AMDetector::CleanupState)));

	QHBoxLayout *startLayout = new QHBoxLayout;
	startLayout->addWidget(statusLabel_);
	startLayout->addWidget(acquireButton_);
	startLayout->addWidget(cancelButton_);

	acquireTimeSpinBox_ = new QDoubleSpinBox;
	acquireTimeSpinBox_->setRange(0, 1e6);
	acquireTimeSpinBox_->setSuffix(" s");
	acquireTimeSpinBox_->setPrefix("Time: ");
	acquireTimeSpinBox_->setDecimals(2);
	acquireTimeSpinBox_->setSingleStep(1.0);
	acquireTimeSpinBox_->setAlignment(Qt::AlignCenter);
	connect(acquireTimeSpinBox_, SIGNAL(editingFinished()), this, SLOT(onNewAcquisitionTimeSetpoint()));
	connect(detector_, SIGNAL(acquisitionTimeChanged(double)), acquireTimeSpinBox_, SLOT(setValue(double)));
	if(detector_->isConnected())
		acquireTimeSpinBox_->setValue(detector_->acquisitionTime());

	elapsedTimeLabel_ = new QLabel("0.00 s");
	connect(detector_, SIGNAL(elapsedTimeChanged(double)), this, SLOT(onElapsedTimeChanged(double)));

	QHBoxLayout *timeLayout = new QHBoxLayout;
	timeLayout->addWidget(acquireTimeSpinBox_);
	timeLayout->addWidget(elapsedTimeLabel_);

	rightLayout_->addLayout(startLayout);
	rightLayout_->addLayout(timeLayout);
	rightLayout_->addStretch();

	cancelButton_->setVisible(detector_->canCancel());
	elapsedTimeLabel_->setVisible(detector_->supportsElapsedTime());
}

void AMXRFBaseDetectorView::setupPlot()
{
	AMSpectrumAndPeriodicTableView::setupPlot();

	// Create the plot window.
//	plotView_ = new MPlotWidget;
	plotView_->enableAntiAliasing(true);

	// Create the plot and setup all the axes.
//	plot_ = new MPlot;
//	plot_->axisBottom()->setAxisNameFont(QFont("Helvetica", 6));
//	plot_->axisBottom()->setTickLabelFont(QFont("Helvetica", 6));
	plot_->axisBottom()->setAxisName("Energy, eV");
//	plot_->axisLeft()->setAxisNameFont(QFont("Helvetica", 6));
//	plot_->axisLeft()->setTickLabelFont(QFont("Helvetica", 6));
	plot_->axisLeft()->setAxisName("Counts");

//	// Set the margins for the plot.
//	plot_->setMarginLeft(10);
//	plot_->setMarginBottom(15);
//	plot_->setMarginRight(2);
//	plot_->setMarginTop(2);

	// Assumes that the dataSource() contains the spectrum most desired to view.
	MPlotSeriesBasic *defaultSpectrum = new MPlotSeriesBasic;
	defaultSpectrum->setModel(new AMDataSourceSeriesData(detector_->dataSource()), true);
	defaultSpectrum->setMarker(MPlotMarkerShape::None);
	defaultSpectrum->setDescription(detector_->dataSource()->name());
	defaultSpectrum->setLinePen(QPen(Qt::red));
	spectraPlotItems_ << defaultSpectrum;
	plot_->addItem(defaultSpectrum);

	// Enable autoscaling of both axes.
	plot_->axisScaleLeft()->setAutoScaleEnabled();
	plot_->axisScaleBottom()->setAutoScaleEnabled();

	// Enable some convenient zoom tools.
//	plot_->addTool(new MPlotDragZoomerTool());
//	plot_->addTool(new MPlotWheelZoomerTool());
//	plotView_->setPlot(plot_);
	plotView_->setMinimumSize(600, 450);

	// Set the number of ticks.  A balance between readability and being practical.
	plot_->axisBottom()->setTicks(3);
	plot_->axisTop()->setTicks(0);
	plot_->axisLeft()->setTicks(4);
	plot_->axisRight()->setTicks(0);

//	// Set the autoscale constraints.
//	plot_->axisScaleLeft()->setDataRangeConstraint(MPlotAxisRange(0, MPLOT_POS_INFINITY));
}

void AMXRFBaseDetectorView::startAcquisition()
{
	detector_->acquire();
}

void AMXRFBaseDetectorView::cancelAcquisition()
{
	detector_->cancelAcquisition();
}

void AMXRFBaseDetectorView::onAcquisitionStateChanged(AMDetector::AcqusitionState state)
{
	bool isAcquiring = state == AMDetector::Acquiring;
	acquireButton_->setDisabled(isAcquiring);
	statusLabel_->setPixmap(QIcon(isAcquiring ? ":/32x32/greenLEDOn.png" : ":/32x32/greenLEDOff.png").pixmap(22));
}

void AMXRFBaseDetectorView::onNewAcquisitionTimeSetpoint()
{
	detector_->setAcquisitionTime(acquireTimeSpinBox_->value());
}

void AMXRFBaseDetectorView::onElapsedTimeChanged(double time)
{
	elapsedTimeLabel_->setText(QString("%1 s").arg(time, 0, 'f', 2));
}

void AMXRFBaseDetectorView::setTitleBar(const QString &title, const QIcon &icon)
{
	topFrame_->setTitle(title);
	topFrame_->setIcon(icon);
}

void AMXRFBaseDetectorView::setTitleBarText(const QString &title)
{
	topFrame_->setTitle(title);
}

void AMXRFBaseDetectorView::setTitleBarIcon(const QIcon &icon)
{
	topFrame_->setIcon(icon);
}

void AMXRFBaseDetectorView::setTitleBarVisibility(bool isVisible)
{
	topFrame_->setVisible(isVisible);
}
