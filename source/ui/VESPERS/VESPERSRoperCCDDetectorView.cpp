#include "VESPERSRoperCCDDetectorView.h"

#include <QToolButton>
#include <QGroupBox>
#include <QFormLayout>

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

	detector_ = static_cast<VESPERSRoperCCDDetector *>(detector);
	connect(detector_, SIGNAL(connected(bool)), this, SLOT(setEnabled(bool)));

	isAcquiring_ = new QLabel;
	isAcquiring_->setPixmap(QIcon(":/OFF.png").pixmap(25));
	connect(detector_, SIGNAL(isAcquiringChanged(bool)), this, SLOT(onIsAcquiringChanged(bool)));

	timeRemaining_ = new QLabel("- s");
	connect(detector_, SIGNAL(timeRemainingChanged(double)), this, SLOT(onTimeRemainingChanged(double)));

	acquireTime_ = QDoubleSpinBox;
	acquireTime_->setSuffix(" s");
	acquireTime_->setDecimals(2);
	acquireTime_->setRange(0, 1000);
	connect(detector_, SIGNAL(acquireTimeChanged(double)), acquireTime_, SLOT(setValue(double)));
	connecct(acquireTime_, SIGNAL(editingFinished()), this, SLOT(setAcquireTime(double)));

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
	connect(endstation_, SIGNAL(ccdPathChanged(QString)), filePathEdit_, SLOT(setText(QString)));

	fileNameEdit_ = new QLineEdit;
	connect(fileNameEdit_, SIGNAL(editingFinished()), this, SLOT(ccdFileEdited()));
	connect(endstation_, SIGNAL(ccdNameChanged(QString)), fileNameEdit_, SLOT(setText(QString)));

	fileNumberEdit_ = new QLineEdit;
	connect(fileNumberEdit_, SIGNAL(editingFinished()), this, SLOT(ccdNumberEdited()));
	connect(endstation_, SIGNAL(ccdNumberChanged(int)), this, SLOT(ccdNumberUpdate(int)));

	QGroupBox *ccdGB = new QGroupBox(tr("Image Path"));
	QFormLayout *ccdGBLayout = new QFormLayout;
	ccdGBLayout->addRow("Path:", ccdPathEdit_);
	ccdGBLayout->addRow("Name:", ccdFileEdit_);
	ccdGBLayout->addRow("Number:", ccdNumberEdit_);
	ccdGBLayout->setLabelAlignment(Qt::AlignRight);
	ccdGB->setLayout(ccdGBLayout);
}

void VESPERSRoperCCDDetectorView::onTriggerModeChanged(VESPERSRoperCCDDetector::TriggerMode mode)
{
	imageMode_->setCurrentIndex((int)mode);
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
