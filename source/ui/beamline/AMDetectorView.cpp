#include "AMDetectorView.h"

#include <QBoxLayout>

#include <QDebug>

#include "MPlot/MPlotAxisScale.h"
#include "MPlot/MPlotTools.h"
#include "dataman/datasource/AMDataSourceSeriesData.h"

AMDetectorGeneralBriefView::AMDetectorGeneralBriefView(AMDetector *detector, QWidget *parent) :
	QWidget(parent)
{
	detector_ = detector;

	connect(detector_, SIGNAL(acquisitionStateChanged(AMDetector::AcqusitionState)), this, SLOT(onAcquisitionStateChanged(AMDetector::AcqusitionState)));
	connect(detector_, SIGNAL(acquisitionSucceeded()), this, SLOT(onAcquisitionSucceeded()));

	statusLabel_ = new QLabel();
	statusLabel_->setPixmap(QIcon(":/OFF.png").pixmap(20));
	singleReadingLabel_ = new QLabel();

	if(detector_->isConnected())
		onAcquisitionSucceeded();

	QHBoxLayout *hl = new QHBoxLayout();
	hl->addWidget(new QLabel(detector_->name()));
	hl->addWidget(singleReadingLabel_);
	hl->addWidget(statusLabel_);
	hl->setContentsMargins(0,0,0,0);
	setLayout(hl);
}

void AMDetectorGeneralBriefView::onAcquisitionStateChanged(AMDetector::AcqusitionState acquisitionState){
	if(acquisitionState == AMDetector::ReadyForAcquisition)
		statusLabel_->setPixmap(QIcon(":/OFF.png").pixmap(20));
	else if(acquisitionState == AMDetector::Acquiring)
		statusLabel_->setPixmap(QIcon(":/ON.png").pixmap(20));
}

void AMDetectorGeneralBriefView::onAcquisitionSucceeded(){
	AMNumber singleReading = detector_->singleReading();
	if(singleReading.state() == AMNumber::Null)
		singleReadingLabel_->setText("N/A");
	else
		singleReadingLabel_->setText(singleReading.toString());
}

AMDetectorGeneralDetailedView::AMDetectorGeneralDetailedView(AMDetector *detector, QWidget *parent) :
	QWidget(parent)
{
	detector_ = detector;

	statusView_ = new AMDetectorViewStatusView(detector_);

	connect(detector_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged(bool)));
	connect(detector_, SIGNAL(acquisitionTimeChanged(double)), this, SLOT(onAcquisitionTimeChanged(double)));
	connect(detector_, SIGNAL(acquisitionStateChanged(AMDetector::AcqusitionState)), this, SLOT(onAcquisitionStateChanged(AMDetector::AcqusitionState)));

	statusLabel_ = new QLabel();
	statusLabel_->setPixmap(QIcon(":/OFF.png").pixmap(20));
	acquisitionTimeDSB_ = new QDoubleSpinBox();
	acquisitionTimeDSB_->setMinimum(0.1);
	acquisitionTimeDSB_->setMaximum(10);
	startAcquisitionButton_ = new QPushButton("Start");
	startAcquisitionButton_->setEnabled(false);

	if(detector_->isConnected()){
		acquisitionTimeDSB_->setValue(detector_->acquisitionTime());
		startAcquisitionButton_->setEnabled(true);
	}

	connect(startAcquisitionButton_, SIGNAL(clicked()), this, SLOT(onStartAcquisitionButtonClicked()));
	connect(acquisitionTimeDSB_, SIGNAL(editingFinished()), this, SLOT(onAcquisitionTimeDSBEditingFinished()));

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
	spectrumData_->setModel(new AMDataSourceSeriesData(detector_->dataSource()), true);
	spectrumData_->setMarker(MPlotMarkerShape::None);
	spectrumData_->setDescription(detector_->dataSource()->name());
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

	QVBoxLayout *tmpVL;

	mainHL->addWidget(startAcquisitionButton_);
	tmpVL = new QVBoxLayout();
	tmpVL->addWidget(new QLabel("Dwell"));
	tmpVL->addWidget(acquisitionTimeDSB_);
	mainHL->addLayout(tmpVL);

	mainHL->addWidget(statusLabel_);
	mainVL->addLayout(mainHL);
	mainVL->addWidget(view_);

	setLayout(mainVL);
}

void AMDetectorGeneralDetailedView::onConnectedChanged(bool connected){
	setEnabled(connected);
	if(connected){
		acquisitionTimeDSB_->setValue(detector_->acquisitionTime());
		startAcquisitionButton_->setEnabled(true);
	}
}

void AMDetectorGeneralDetailedView::onAcquisitionStateChanged(AMDetector::AcqusitionState acquisitionState){
	if(acquisitionState == AMDetector::ReadyForAcquisition)
		statusLabel_->setPixmap(QIcon(":/OFF.png").pixmap(20));
	else if(acquisitionState == AMDetector::Acquiring)
		statusLabel_->setPixmap(QIcon(":/ON.png").pixmap(20));

	qDebug() << "State is now " << detector_->acquisitionStateDescription(acquisitionState);
}

void AMDetectorGeneralDetailedView::onAcquisitionTimeChanged(double integrationTime){
	acquisitionTimeDSB_->setValue(integrationTime);
}

void AMDetectorGeneralDetailedView::onStartAcquisitionButtonClicked(){
	detector_->acquire();
}

void AMDetectorGeneralDetailedView::onAcquisitionTimeDSBEditingFinished(){
	detector_->setAcquisitionTime(acquisitionTimeDSB_->value());
}

AMDetectorViewStatusView::AMDetectorViewStatusView(AMDetector *detector, QWidget *parent) :
	QWidget(parent)
{
	detector_ = detector;

	connectedLabel_ = new QLabel();
	readMethodLabel_ = new QLabel();
	readModeLabel_ = new QLabel();

	if(detector_->isConnected()){
		connectedLabel_->setText("Connected");
		switch(detector_->readMethod()){
		case AMDetectorDefinitions::ImmediateRead:
			readMethodLabel_->setText("Immediate");
			break;
		case AMDetectorDefinitions::RequestRead:
			readMethodLabel_->setText("Request");
			break;
		case AMDetectorDefinitions::WaitRead:
			readMethodLabel_->setText("Wait");
			break;
		default:
			readMethodLabel_->setText("Invalid");
		}
		switch(detector_->readMode()){
		case AMDetectorDefinitions::SingleRead:
			readModeLabel_->setText("Single");
			break;
		case AMDetectorDefinitions::ContinuousRead:
			readModeLabel_->setText("Continuous");
			break;
		default:
			readModeLabel_->setText("Invalid");
		}
	}
	else{
		connectedLabel_->setText("Not Connected");
		readMethodLabel_->setText("Not Connected");
		readModeLabel_->setText("Not Connected");
	}

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(connectedLabel_);
	vl->addWidget(readMethodLabel_);
	vl->addWidget(readModeLabel_);
	setLayout(vl);
}
