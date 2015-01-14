#include "SXRMBXRFDetailedDetectorView.h"

SXRMBXRFDetailedDetectorView::SXRMBXRFDetailedDetectorView(SXRMBBrukerDetector *detector, QWidget *parent)
	:AMXRFDetailedDetectorView(detector, parent)
{
}

SXRMBXRFDetailedDetectorView::~SXRMBXRFDetailedDetectorView()
{

}

void SXRMBXRFDetailedDetectorView::enableDeadTimeDisplay()
{
	if (deadTimeLabel_)
		deadTimeLabel_->setVisible(true);
}

void SXRMBXRFDetailedDetectorView::onDeadTimeChanged()
{
	deadTimeLabel_->setText(QString("Dead time: %1").arg(detector_->deadTime()));
}
