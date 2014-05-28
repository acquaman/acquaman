#include "VESPERSTimeScanConfigurationView.h"

VESPERSTimeScanConfigurationView::VESPERSTimeScanConfigurationView(VESPERSTimeScanConfiguration *config, QWidget *parent)
	: AMScanConfigurationView(parent)
{
	config_ = config;
}
