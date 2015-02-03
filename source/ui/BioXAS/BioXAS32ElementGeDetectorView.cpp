#include "BioXAS32ElementGeDetectorView.h"

BioXAS32ElementGeDetectorView::BioXAS32ElementGeDetectorView(BioXAS32ElementGeDetector *detector, QWidget *parent)
	: AMXRFDetailedDetectorView(detector, parent)
{

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

	rightLayout_->addWidget(statusMessageLabel_, 0, Qt::AlignCenter);
	rightLayout_->addWidget(initializationLabel_, 0, Qt::AlignCenter);
	rightLayout_->addWidget(acquisitionStatus_, 0, Qt::AlignCenter);
	rightLayout_->addWidget(initializeButton_);
	rightLayout_->addStretch();
}

void BioXAS32ElementGeDetectorView::initializationStatusUpdate()
{
	initializationLabel_->setText(QString("Init: %1").arg(detector_->initializationStateDescription(detector_->initializationState())));
}

void BioXAS32ElementGeDetectorView::acquisitionStatusUpdate()
{
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
