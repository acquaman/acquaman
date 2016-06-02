#include "PGMXASScanConfigurationView.h"

PGMXASScanConfigurationView::PGMXASScanConfigurationView(PGMXASScanConfiguration *configuration, QWidget *parent)
	: AMScanConfigurationView(parent)
{
	configuration_ = configuration;
}

