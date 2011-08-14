#include "REIXSXESScanConfigurationDetailedView.h"

#include <QGroupBox>
#include <QBoxLayout>

REIXSXESScanConfigurationDetailedView::REIXSXESScanConfigurationDetailedView(REIXSXESMCPDetector* detector, QWidget *parent) :
	AMScanConfigurationView(parent)
{
	basicConfigurationView_ = new REIXSXESScanConfigurationView();
	detectorView_ = new REIXSXESMCPDetectorView(detector);
	QGroupBox* detectorPreviewBox = new QGroupBox("Detector Preview");
	detectorPreviewBox->setLayout(new QVBoxLayout());
	detectorPreviewBox->layout()->addWidget(detectorView_);

	QHBoxLayout* hl = new QHBoxLayout();
	QVBoxLayout* vl1 = new QVBoxLayout();
	vl1->addWidget(basicConfigurationView_);
	vl1->addStretch();
	hl->addLayout(vl1);
	hl->addWidget(detectorPreviewBox);

	setLayout(hl);
}
