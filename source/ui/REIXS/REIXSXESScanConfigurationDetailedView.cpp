#include "REIXSXESScanConfigurationDetailedView.h"

#include <QBoxLayout>

REIXSXESScanConfigurationDetailedView::REIXSXESScanConfigurationDetailedView(REIXSXESMCPDetector* detector, QWidget *parent) :
	AMScanConfigurationView(parent)
{
	basicConfigurationView_ = new REIXSXESScanConfigurationView();
	detectorView_ = new REIXSXESMCPDetectorView(detector);

	QHBoxLayout* hl = new QHBoxLayout();
	QVBoxLayout* vl1 = new QVBoxLayout();
	vl1->addWidget(basicConfigurationView_);
	vl1->addStretch();
	hl->addLayout(vl1);
	hl->addWidget(detectorView_);

	setLayout(hl);
}
