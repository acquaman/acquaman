#include "AMXspress3XRFDetectorView.h"

#include <QSpinBox>

AMXspress3XRFDetectorView::AMXspress3XRFDetectorView(AMXspress3XRFDetector *detector, QWidget *parent)
	: AMXRFDetailedDetectorView(detector, parent)
{
}

void AMXspress3XRFDetectorView::buildDetectorView()
{
	// This is going to extend the base view, so we will simply call it's build method too.
	AMXRFDetailedDetectorView::buildDetectorView();

	statusMessageLabel_ = new QLabel("");
	connect(xspress3Detector(), SIGNAL(statusMessageChanged(QString)), statusMessageLabel_, SLOT(setText(QString)));

	initializationLabel_ = new QLabel("");
	acquisitionStatus_ = new QLabel("");

	connect(detector_, SIGNAL(initializationStateChanged(AMDetector::InitializationState)), this, SLOT(initializationStatusUpdate()));
	connect(detector_, SIGNAL(acquisitionStateChanged(AMDetector::AcqusitionState)), this, SLOT(acquisitionStatusUpdate()));

	initializeButton_ = new QPushButton(QIcon(":/hammer-wrench.png"), "Initialize");
	connect(initializeButton_, SIGNAL(clicked()), detector_, SLOT(initialize()));

	thresholdSpinBox_ = new QSpinBox;
	thresholdSpinBox_->setPrefix("Threshold: ");
	thresholdSpinBox_->setRange(0, 1000000);
	thresholdSpinBox_->setValue(xspress3Detector()->threshold());
	connect(thresholdSpinBox_, SIGNAL(editingFinished()), this, SLOT(setThreshold()));
	connect(xspress3Detector(), SIGNAL(thresholdChanged()), this, SLOT(updateThresholdSpinBox()));

	framesLabel_ = new QLabel();
	connect(xspress3Detector(), SIGNAL(currentFrameCountChanged()), this, SLOT(onFramesChanged()));
	connect(xspress3Detector(), SIGNAL(framesPerAcquisitionChanged()), this, SLOT(onFramesChanged()));

	framesPerAcquisitionSpinBox_ = new QSpinBox;
	framesPerAcquisitionSpinBox_->setPrefix("# Frames:");
	framesPerAcquisitionSpinBox_->setRange(0, 16384);
	framesPerAcquisitionSpinBox_->setValue(xspress3Detector()->framesPerAcquisition());
	connect(xspress3Detector(), SIGNAL(framesPerAcquisitionChanged()), this, SLOT(updateFramesPerAcquisiton()));
	connect(framesPerAcquisitionSpinBox_, SIGNAL(editingFinished()), this, SLOT(onFramesPerAcquisitionSpinBoxChanged()));

	disarmButton_ = new QPushButton(QIcon(":/16x16/network-error.png"), "Disarm");
	connect(disarmButton_, SIGNAL(clicked()), xspress3Detector(), SLOT(disarm()));

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

void AMXspress3XRFDetectorView::initializationStatusUpdate()
{
	initializationLabel_->setText(QString("Init: %1").arg(detector_->initializationStateDescription(detector_->initializationState())));
	acquisitionStatus_->setText(QString("Acq: %1").arg(detector_->acquisitionStateDescription(detector_->acquisitionState())));
}

void AMXspress3XRFDetectorView::acquisitionStatusUpdate()
{
	initializationLabel_->setText(QString("Init: %1").arg(detector_->initializationStateDescription(detector_->initializationState())));
	acquisitionStatus_->setText(QString("Acq: %1").arg(detector_->acquisitionStateDescription(detector_->acquisitionState())));
}

void AMXspress3XRFDetectorView::onInitializationStateChanged(AMDetector::InitializationState state)
{
	Q_UNUSED(state);
	updateStatusAndAcquisitionButtons();
}

void AMXspress3XRFDetectorView::onAcquisitionStateChanged(AMDetector::AcqusitionState state)
{
	Q_UNUSED(state);
	updateStatusAndAcquisitionButtons();
}

void AMXspress3XRFDetectorView::updateStatusAndAcquisitionButtons()
{
	if (detector_->requiresInitialization() || detector_->isNotReadyForAcquisition()){

		statusLabel_->setPixmap(QIcon(":/32x32/redLEDOn.png").pixmap(22));
		acquireButton_->setEnabled(false);
		cancelButton_->setEnabled(false);
		initializeButton_->setVisible(true);
	}

	else if (detector_->isInitializing()){

		statusLabel_->setPixmap(QIcon(":/32x32/yellowLEDOn.png").pixmap(22));
		acquireButton_->setEnabled(false);
		cancelButton_->setEnabled(false);
		initializeButton_->setVisible(false);
	}

	else if (detector_->isReadyForAcquisition()){

		statusLabel_->setPixmap(QIcon(":/32x32/greenLEDOff.png").pixmap(22));
		acquireButton_->setEnabled(true);
		cancelButton_->setEnabled(false);
		initializeButton_->setVisible(false);
	}

	else if (detector_->isAcquiring()){

		statusLabel_->setPixmap(QIcon(":/32x32/greenLEDOn.png").pixmap(22));
		acquireButton_->setEnabled(false);
		cancelButton_->setEnabled(true);
		initializeButton_->setVisible(false);
	}
}

void AMXspress3XRFDetectorView::setThreshold()
{
	xspress3Detector()->setThreshold(thresholdSpinBox_->value());
}

void AMXspress3XRFDetectorView::onFramesChanged()
{
	framesLabel_->setText(QString("# Frames: %1/%2").arg(xspress3Detector()->currentFrame()).arg(xspress3Detector()->framesPerAcquisition()));
}

void AMXspress3XRFDetectorView::updateThresholdSpinBox()
{
	thresholdSpinBox_->setValue(xspress3Detector()->threshold());
}

void AMXspress3XRFDetectorView::updateFramesPerAcquisiton()
{
	framesPerAcquisitionSpinBox_->setValue(xspress3Detector()->framesPerAcquisition());
}

void AMXspress3XRFDetectorView::onFramesPerAcquisitionSpinBoxChanged()
{
	xspress3Detector()->setFramesPerAcquisition(framesPerAcquisitionSpinBox_->value());
}
