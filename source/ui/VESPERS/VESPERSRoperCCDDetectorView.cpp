#include "VESPERSRoperCCDDetectorView.h"

#include "ui/AMTopFrame.h"

#include <QToolButton>
#include <QGroupBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

VESPERSRoperCCDDetectorView::VESPERSRoperCCDDetectorView(VESPERSRoperCCDDetector *detector, bool configureOnly, QWidget *parent)
	: AMDetailedDetectorView(configureOnly, parent)
{
	detector_ = 0;
	setDetector(detector, configureOnly);
}

bool VESPERSRoperCCDDetectorView::setDetector(AMDetector *detector, bool configureOnly)
{
	//I don't have a configure only view for these.  It doesn't make quite as much sense for the stand alone spectra to have configure only views.
	Q_UNUSED(configureOnly)

	// If there is no valid detector pointer, then return false.
	if (!detector)
		return false;

	AMTopFrame *topFrame = new AMTopFrame(QString("X-Ray Diffraction - Roper CCD"));
	topFrame->setIcon(QIcon(":/utilities-system-monitor.png"));

	detector_ = static_cast<VESPERSRoperCCDDetector *>(detector);
	connect(detector_, SIGNAL(connected(bool)), this, SLOT(setEnabled(bool)));

	isAcquiring_ = new QLabel;
	isAcquiring_->setPixmap(QIcon(":/OFF.png").pixmap(25));
	connect(detector_, SIGNAL(isAcquiringChanged(bool)), this, SLOT(onIsAcquiringChanged(bool)));

	state_ = new QLabel;
	connect(detector_, SIGNAL(stateChanged(VESPERSRoperCCDDetector::State)), this, SLOT(onStateChanged(VESPERSRoperCCDDetector::State)));

	timeRemaining_ = new QLabel("- s");
	connect(detector_, SIGNAL(timeRemainingChanged(double)), this, SLOT(onTimeRemainingChanged(double)));

	acquireTime_ = new QDoubleSpinBox;
	acquireTime_->setSuffix(" s");
	acquireTime_->setDecimals(2);
	acquireTime_->setRange(0, 1000);
	connect(detector_, SIGNAL(acquireTimeChanged(double)), acquireTime_, SLOT(setValue(double)));
	connect(acquireTime_, SIGNAL(editingFinished()), this, SLOT(setAcquireTime()));

	QToolButton *startButton = new QToolButton;
	startButton->setIcon(QIcon(":/play_button_green.png"));
	connect(startButton, SIGNAL(clicked()), detector_, SLOT(start()));

	QToolButton *stopButton = new QToolButton;
	stopButton->setIcon(QIcon(":/red-stop-button.png"));
	connect(stopButton, SIGNAL(clicked()), detector_, SLOT(stop()));

	triggerMode_ = new QComboBox;
	triggerMode_->addItem("Free Run");
	triggerMode_->addItem("External Sync");
	connect(detector_, SIGNAL(triggerModeChanged(VESPERSRoperCCDDetector::TriggerMode)), this, SLOT(onTriggerModeChanged(VESPERSRoperCCDDetector::TriggerMode)));
	connect(triggerMode_, SIGNAL(currentIndexChanged(int)), this, SLOT(setTriggerMode(int)));

	imageMode_ = new QComboBox;
	imageMode_->addItem("Normal");
	imageMode_->addItem("Focus");
	connect(detector_, SIGNAL(imageModeChanged(VESPERSRoperCCDDetector::ImageMode)), this, SLOT(onImageModeChanged(VESPERSRoperCCDDetector::ImageMode)));
	connect(imageMode_, SIGNAL(currentIndexChanged(int)), this, SLOT(setImageMode(int)));

	// Setup the CCD file path signals and layout.
	filePathEdit_ = new QLineEdit;
	connect(filePathEdit_, SIGNAL(editingFinished()), this, SLOT(ccdPathEdited()));
	connect(detector_, SIGNAL(ccdPathChanged(QString)), filePathEdit_, SLOT(setText(QString)));

	fileNameEdit_ = new QLineEdit;
	connect(fileNameEdit_, SIGNAL(editingFinished()), this, SLOT(ccdFileEdited()));
	connect(detector_, SIGNAL(ccdNameChanged(QString)), fileNameEdit_, SLOT(setText(QString)));

	fileNumberEdit_ = new QLineEdit;
	connect(fileNumberEdit_, SIGNAL(editingFinished()), this, SLOT(ccdNumberEdited()));
	connect(detector_, SIGNAL(ccdNumberChanged(int)), this, SLOT(ccdNumberUpdate(int)));

	QGroupBox *acquisitionBox = new QGroupBox("Acquisition");

	QHBoxLayout *statusLayout = new QHBoxLayout;
	statusLayout->addWidget(isAcquiring_);
	statusLayout->addWidget(state_);
	statusLayout->addWidget(timeRemaining_);

	QHBoxLayout *startLayout = new QHBoxLayout;
	startLayout->addWidget(acquireTime_);
	startLayout->addWidget(startButton);
	startLayout->addWidget(stopButton);

	QHBoxLayout *modeLayout = new QHBoxLayout;
	modeLayout->addWidget(triggerMode_);
	modeLayout->addWidget(imageMode_);

	QVBoxLayout *acquisitionLayout = new QVBoxLayout;
	acquisitionLayout->addLayout(statusLayout);
	acquisitionLayout->addLayout(startLayout);
	acquisitionLayout->addLayout(modeLayout);

	acquisitionBox->setLayout(acquisitionLayout);

	QGroupBox *ccdGB = new QGroupBox(tr("Image Path"));
	QFormLayout *ccdGBLayout = new QFormLayout;
	ccdGBLayout->addRow("Path:", filePathEdit_);
	ccdGBLayout->addRow("Name:", fileNameEdit_);
	ccdGBLayout->addRow("Number:", fileNumberEdit_);
	ccdGBLayout->setLabelAlignment(Qt::AlignRight);
	ccdGB->setLayout(ccdGBLayout);

	QVBoxLayout *detectorLayout = new QVBoxLayout;
	detectorLayout->addStretch();
	detectorLayout->addWidget(acquisitionBox);
	detectorLayout->addWidget(ccdGB);
	detectorLayout->addStretch();

	QHBoxLayout *horizontalSquishLayout = new QHBoxLayout;
	horizontalSquishLayout->addStretch();
	horizontalSquishLayout->addLayout(detectorLayout);
	horizontalSquishLayout->addStretch();

	QVBoxLayout *masterLayout = new QVBoxLayout;
	masterLayout->addWidget(topFrame);
	masterLayout->addStretch();
	masterLayout->addLayout(horizontalSquishLayout);
	masterLayout->addStretch();

	setLayout(masterLayout);

	return true;
}

void VESPERSRoperCCDDetectorView::onTriggerModeChanged(VESPERSRoperCCDDetector::TriggerMode mode)
{
	triggerMode_->setCurrentIndex((int)mode);
}

void VESPERSRoperCCDDetectorView::setTriggerMode(int newMode)
{
	if (newMode == (int)detector_->triggerMode())
		return;

	switch(newMode){

	case 0:
		detector_->setTriggerMode(VESPERSRoperCCDDetector::FreeRun);
		break;

	case 1:
		detector_->setTriggerMode(VESPERSRoperCCDDetector::ExtSync);
		break;
	}
}

void VESPERSRoperCCDDetectorView::onImageModeChanged(VESPERSRoperCCDDetector::ImageMode mode)
{
	if (mode == VESPERSRoperCCDDetector::Focus)
		imageMode_->setCurrentIndex(1);
	else
		imageMode_->setCurrentIndex(0);
}

void VESPERSRoperCCDDetectorView::setImageMode(int newMode)
{
	switch(newMode){

	case 0:
		if (detector_->imageMode() != VESPERSRoperCCDDetector::Normal)
			detector_->setImageMode(VESPERSRoperCCDDetector::Normal);

		break;

	case 1:
		if (detector_->imageMode() != VESPERSRoperCCDDetector::Focus )
			detector_->setImageMode(VESPERSRoperCCDDetector::Focus);

		break;
	}
}

void VESPERSRoperCCDDetectorView::onStateChanged(VESPERSRoperCCDDetector::State newState)
{
	switch(newState){

	case VESPERSRoperCCDDetector::Idle:
		state_->setText("Idle");
		break;

	case VESPERSRoperCCDDetector::Acquire:
		state_->setText("Acquire");
		break;

	case VESPERSRoperCCDDetector::Readout:
		state_->setText("Readout");
		break;

	case VESPERSRoperCCDDetector::Correct:
		state_->setText("Correct");
		break;

	case VESPERSRoperCCDDetector::Saving:
		state_->setText("Saving");
		break;

	case VESPERSRoperCCDDetector::Aborting:
		state_->setText("Aborting");
		break;

	case VESPERSRoperCCDDetector::Error:
		state_->setText("Error");
		break;

	case VESPERSRoperCCDDetector::Waiting:
		state_->setText("Waiting");
		break;
	}
}
