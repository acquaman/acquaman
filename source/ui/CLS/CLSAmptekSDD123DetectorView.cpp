#include "CLSAmptekSDD123DetectorView.h"

#include "util/VESPERS/GeneralUtilities.h"
#include "MPlot/MPlotAxisScale.h"
#include "MPlot/MPlotTools.h"
#include "dataman/datasource/AMDataSourceSeriesData.h"

CLSAmptekSDD123BriefDetectorView::CLSAmptekSDD123BriefDetectorView(CLSAmptekSDD123Detector *detector, bool configureOnly, QWidget *parent) :
	AMBriefDetectorView(configureOnly, parent)
{
	detector_ = 0;
	setDetector(detector, configureOnly);
}

AMDetector* CLSAmptekSDD123BriefDetectorView::detector(){
	return detector_;
}

bool CLSAmptekSDD123BriefDetectorView::setDetector(AMDetector *detector, bool configureOnly){
	if(!detector)
		return false;

	detector_ = static_cast<CLSAmptekSDD123Detector*>(detector);

	QLabel *name = new QLabel(detector_->name());

	connect(detector_, SIGNAL(connected(bool)), this, SLOT(setEnabled(bool)));
	connect(detector_, SIGNAL(nameChanged(QString)), name, SLOT(setText(QString)));

	QHBoxLayout *briefLayout = new QHBoxLayout;
	briefLayout->addWidget(name);
	setLayout(briefLayout);

	return true;
}

CLSAmptekSDD123DetailedDetectorView::CLSAmptekSDD123DetailedDetectorView(CLSAmptekSDD123Detector *detector, bool configureOnly, QWidget *parent) :
	AMDetailedDetectorView(configureOnly, parent)
{
	detector_ = 0;
	setDetector(detector, configureOnly);
}

AMDetector* CLSAmptekSDD123DetailedDetectorView::detector(){
	return detector_;
}

MPlot* CLSAmptekSDD123DetailedDetectorView::plot() const{
	return plot_;
}

bool CLSAmptekSDD123DetailedDetectorView::setDetector(AMDetector *detector, bool configureOnly){
	if(!detector)
		return false;

	detector_ = static_cast<CLSAmptekSDD123Detector*>(detector);
	connect(detector_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged(bool)));
	connect(detector_, SIGNAL(statusChanged(bool)), this, SLOT(onStatusChanged(bool)));
	connect(detector_, SIGNAL(integrationTimeChanged(double)), this, SLOT(onIntegrationTimeChanged(double)));
	connect(detector_, SIGNAL(detectorTemperatureChanged(double)), this, SLOT(onDetectorTemperatureChanged(double)));
	connect(detector_, SIGNAL(mcaChannelsChanged(double)), this, SLOT(onMCAChannelsChanged(double)));

	statusLabel_ = new QLabel();
	statusLabel_->setPixmap(QIcon(":/OFF.png").pixmap(20));

	integrationTimeDSB_ = new QDoubleSpinBox();
	integrationTimeDSB_->setMinimum(0.1);
	integrationTimeDSB_->setMaximum(10);
	integrationTimeDSB_->setEnabled(false);
	detectorTemperatureLabel_ = new QLabel();
	mcaChannelLabel_ = new QLabel();
	startAcquisitionButton_ = new QPushButton("Start");
	startAcquisitionButton_->setEnabled(false);
	connect(startAcquisitionButton_, SIGNAL(clicked()), this, SLOT(onStartAcquisitionButtonClicked()));

	if(detector_->isConnected()){
		integrationTimeDSB_->setValue(detector_->integrationTime());
		detectorTemperatureLabel_->setText(QString("%1").arg(detector_->detectorTemperature()));
		mcaChannelLabel_->setText(QString("%1").arg(detector_->channels()));
		startAcquisitionButton_->setEnabled(true);
	}
	else{
		detectorTemperatureLabel_->setText("?");
		mcaChannelLabel_->setText("?");
	}

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

	MPlotSeriesBasic *spectrumData_ = new MPlotSeriesBasic;
	spectrumData_->setModel(new AMDataSourceSeriesData(detector_->spectrumDataSource()), true);
	spectrumData_->setMarker(MPlotMarkerShape::None);
	spectrumData_->setDescription(detector_->spectrumDataSource()->name());
	spectrumData_->setLinePen(QPen(Qt::blue));
	plot_->addItem(spectrumData_);

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

	QVBoxLayout *mainVL = new QVBoxLayout();
	QHBoxLayout *mainHL = new QHBoxLayout();

	mainHL->addWidget(startAcquisitionButton_);
	mainHL->addWidget(integrationTimeDSB_);
	mainHL->addWidget(detectorTemperatureLabel_);
	mainHL->addWidget(mcaChannelLabel_);
	mainHL->addWidget(statusLabel_);

	mainVL->addLayout(mainHL);
	mainVL->addWidget(view_);

	setLayout(mainVL);

	return true;
}

void CLSAmptekSDD123DetailedDetectorView::onConnectedChanged(bool connected){
	setEnabled(connected);
	if(connected){
		integrationTimeDSB_->setValue(detector_->integrationTime());
		detectorTemperatureLabel_->setText(QString("%1").arg(detector_->detectorTemperature()));
		mcaChannelLabel_->setText(QString("%1").arg(detector_->channels()));
		startAcquisitionButton_->setEnabled(true);
	}
}

void CLSAmptekSDD123DetailedDetectorView::onStatusChanged(bool status){
	if(status)
		statusLabel_->setPixmap(QIcon(":/ON.png").pixmap(20));
	else
		statusLabel_->setPixmap(QIcon(":/OFF.png").pixmap(20));
}

void CLSAmptekSDD123DetailedDetectorView::onIntegrationTimeChanged(double integrationTime){
	integrationTimeDSB_->setValue(integrationTime);
}

void CLSAmptekSDD123DetailedDetectorView::onDetectorTemperatureChanged(double detectorTemperature){
	detectorTemperatureLabel_->setText(QString("%1").arg(detectorTemperature));
}

void CLSAmptekSDD123DetailedDetectorView::onMCAChannelsChanged(double mcaChannels){
	mcaChannelLabel_->setText(QString("%1").arg(mcaChannels));
}

void CLSAmptekSDD123DetailedDetectorView::onStartAcquisitionButtonClicked(){
	detector_->start();
}
