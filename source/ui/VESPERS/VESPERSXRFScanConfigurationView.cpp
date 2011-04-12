#include "VESPERSXRFScanConfigurationView.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include <QHBoxLayout>

VESPERSXRFScanConfigurationView::VESPERSXRFScanConfigurationView(VESPERSXRFScanConfiguration *scanConfig, QWidget *parent)
	: AMScanConfigurationView(parent)
{
	configuration_ = scanConfig;
	view_ = new XRFDetailedDetectorView(scanConfig->detector());
	connect(view_, SIGNAL(startScan()), this, SIGNAL(startScan()));
	connect(view_, SIGNAL(stopScan()), this, SIGNAL(stopScan()));

	QHBoxLayout *configViewLayout = new QHBoxLayout;
	configViewLayout->addWidget(view_);
	setLayout(configViewLayout);
}
