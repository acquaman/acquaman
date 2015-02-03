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

	rightLayout_->addWidget(statusMessageLabel_, 0, Qt::AlignCenter);
	rightLayout_->addWidget(initializationLabel_, 0, Qt::AlignCenter);
	rightLayout_->addWidget(acquisitionStatus_, 0, Qt::AlignCenter);
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
