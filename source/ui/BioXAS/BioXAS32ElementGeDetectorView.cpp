#include "BioXAS32ElementGeDetectorView.h"

#include "acquaman/BioXAS/BioXASXRFScanConfiguration.h"
#include "actions3/actions/AMScanAction.h"

#include <QSpinBox>

BioXAS32ElementGeDetectorView::BioXAS32ElementGeDetectorView(BioXAS32ElementGeDetector *detector, QWidget *parent)
	: AMXRFDetailedDetectorView(detector, parent)
{
	deadTimeViewFactor_ = 8;
}

void BioXAS32ElementGeDetectorView::buildDetectorView()
{
	// This is going to extend the base view, so we will simply call it's build method too.
	AMXRFDetailedDetectorView::buildDetectorView();

	statusMessageLabel_ = new QLabel("");
	connect(bioXAS32ElementGeDetector(), SIGNAL(statusMessageChanged(QString)), statusMessageLabel_, SLOT(setText(QString)));

	initializationLabel_ = new QLabel("");
	acquisitionStatus_ = new QLabel("");

	connect(detector_, SIGNAL(initializationStateChanged(AMDetector::InitializationState)), this, SLOT(initializationStatusUpdate()));
	connect(detector_, SIGNAL(acquisitionStateChanged(AMDetector::AcqusitionState)), this, SLOT(acquisitionStatusUpdate()));

	initializeButton_ = new QPushButton(QIcon(":/hammer-wrench.png"), "Initialize");
	connect(initializeButton_, SIGNAL(clicked()), detector_, SLOT(initialize()));

	thresholdSpinBox_ = new QSpinBox;
	thresholdSpinBox_->setPrefix("Threshold: ");
	thresholdSpinBox_->setRange(0, 1000000);
	thresholdSpinBox_->setValue(bioXAS32ElementGeDetector()->threshold());
	connect(thresholdSpinBox_, SIGNAL(editingFinished()), this, SLOT(setThreshold()));
	connect(bioXAS32ElementGeDetector(), SIGNAL(thresholdChanged()), this, SLOT(updateThresholdSpinBox()));

	framesLabel_ = new QLabel();
	connect(bioXAS32ElementGeDetector(), SIGNAL(currentFrameCountChanged()), this, SLOT(onFramesChanged()));
	connect(bioXAS32ElementGeDetector(), SIGNAL(framesPerAcquisitionChanged()), this, SLOT(onFramesChanged()));

	framesPerAcquisitionSpinBox_ = new QSpinBox;
	framesPerAcquisitionSpinBox_->setPrefix("# Frames:");
	framesPerAcquisitionSpinBox_->setRange(0, 16384);
	framesPerAcquisitionSpinBox_->setValue(bioXAS32ElementGeDetector()->framesPerAcquisition());
	connect(bioXAS32ElementGeDetector(), SIGNAL(framesPerAcquisitionChanged()), this, SLOT(updateFramesPerAcquisiton()));
	connect(framesPerAcquisitionSpinBox_, SIGNAL(editingFinished()), this, SLOT(onFramesPerAcquisitionSpinBoxChanged()));

	disarmButton_ = new QPushButton(QIcon(":/16x16/network-error.png"), "Disarm");
	connect(disarmButton_, SIGNAL(clicked()), bioXAS32ElementGeDetector(), SLOT(disarm()));

	rightLayout_->addWidget(statusMessageLabel_, 0, Qt::AlignLeft);
	rightLayout_->addWidget(initializationLabel_, 0, Qt::AlignLeft);
	rightLayout_->addWidget(acquisitionStatus_, 0, Qt::AlignLeft);
	rightLayout_->addWidget(initializeButton_, 0, Qt::AlignLeft);
	rightLayout_->addWidget(disarmButton_, 0, Qt::AlignLeft);
	rightLayout_->addWidget(thresholdSpinBox_, 0, Qt::AlignLeft);
	rightLayout_->addWidget(framesLabel_, 0, Qt::AlignLeft);
	rightLayout_->addWidget(framesPerAcquisitionSpinBox_, 0, Qt::AlignLeft);
	rightLayout_->addStretch();
}

void BioXAS32ElementGeDetectorView::initializationStatusUpdate()
{
	initializationLabel_->setText(QString("Init: %1").arg(detector_->initializationStateDescription(detector_->initializationState())));
	acquisitionStatus_->setText(QString("Acq: %1").arg(detector_->acquisitionStateDescription(detector_->acquisitionState())));
}

void BioXAS32ElementGeDetectorView::acquisitionStatusUpdate()
{
	initializationLabel_->setText(QString("Init: %1").arg(detector_->initializationStateDescription(detector_->initializationState())));
	acquisitionStatus_->setText(QString("Acq: %1").arg(detector_->acquisitionStateDescription(detector_->acquisitionState())));
}

void BioXAS32ElementGeDetectorView::onInitializationStateChanged(AMDetector::InitializationState state)
{
	Q_UNUSED(state);
	updateStatusAndAcquisitionButtons();
}

void BioXAS32ElementGeDetectorView::onAcquisitionStateChanged(AMDetector::AcqusitionState state)
{
	Q_UNUSED(state);
	updateStatusAndAcquisitionButtons();
}

void BioXAS32ElementGeDetectorView::updateStatusAndAcquisitionButtons()
{
	if (detector_->requiresInitialization() || detector_->isNotReadyForAcquisition()){

		statusLabel_->setPixmap(QIcon(":/RED.png").pixmap(22));
		acquireButton_->setEnabled(false);
		cancelButton_->setEnabled(false);
		initializeButton_->setVisible(true);
	}

	else if (detector_->isInitializing()){

		statusLabel_->setPixmap(QIcon(":/Yellow.png").pixmap(22));
		acquireButton_->setEnabled(false);
		cancelButton_->setEnabled(false);
		initializeButton_->setVisible(false);
	}

	else if (detector_->isReadyForAcquisition()){

		statusLabel_->setPixmap(QIcon(":/OFF.png").pixmap(22));
		acquireButton_->setEnabled(true);
		cancelButton_->setEnabled(false);
		initializeButton_->setVisible(false);
	}

	else if (detector_->isAcquiring()){

		statusLabel_->setPixmap(QIcon(":/ON.png").pixmap(22));
		acquireButton_->setEnabled(false);
		cancelButton_->setEnabled(true);
		initializeButton_->setVisible(false);
	}
}

void BioXAS32ElementGeDetectorView::setThreshold()
{
	bioXAS32ElementGeDetector()->setThreshold(thresholdSpinBox_->value());
}

void BioXAS32ElementGeDetectorView::onFramesChanged()
{
	framesLabel_->setText(QString("# Frames: %1/%2").arg(bioXAS32ElementGeDetector()->currentFrame()).arg(bioXAS32ElementGeDetector()->framesPerAcquisition()));
}

void BioXAS32ElementGeDetectorView::updateThresholdSpinBox()
{
	thresholdSpinBox_->setValue(bioXAS32ElementGeDetector()->threshold());
}

void BioXAS32ElementGeDetectorView::updateFramesPerAcquisiton()
{
	framesPerAcquisitionSpinBox_->setValue(bioXAS32ElementGeDetector()->framesPerAcquisition());
}

void BioXAS32ElementGeDetectorView::onFramesPerAcquisitionSpinBoxChanged()
{
	bioXAS32ElementGeDetector()->setFramesPerAcquisition(framesPerAcquisitionSpinBox_->value());
}

void BioXAS32ElementGeDetectorView::startAcquisition()
{
	BioXASXRFScanConfiguration *configuration = new BioXASXRFScanConfiguration;
	AMDetectorInfoSet detectorSet;
	detectorSet.addDetectorInfo(detector_->toInfo());
	configuration->setDetectorConfigurations(detectorSet);
	AMScanAction *scanAction = new AMScanAction(new AMScanActionInfo(configuration));

	connect(scanAction, SIGNAL(cancelled()), scanAction, SLOT(scheduleForDeletion()));
	connect(scanAction, SIGNAL(failed()), scanAction, SLOT(scheduleForDeletion()));
	connect(scanAction, SIGNAL(succeeded()), scanAction, SLOT(scheduleForDeletion()));

	scanAction->start();
}
