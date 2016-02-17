#include "SXRMBBrukerDetectorView.h"

SXRMBBrukerDetectorView::SXRMBBrukerDetectorView(SXRMBBrukerDetector *detector, QWidget *parent)
	: AMXRFDetailedDetectorView(detector, parent)
{

}

SXRMBBrukerDetectorView::~SXRMBBrukerDetectorView()
{

}

void SXRMBBrukerDetectorView::enableDeadTimeDisplay()
{
	if (deadTimeLabel_) {
		deadTimeLabel_->setVisible(true);
		deadTimeLabel_->setText(QString("Dead time: [UNCONNECTED]"));
	}
}
