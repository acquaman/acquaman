#include "BioXASSollerSlitScanConfigurationView.h"

BioXASSollerSlitScanConfigurationView::BioXASSollerSlitScanConfigurationView(BioXASSollerSlitScanConfiguration *configuration, QWidget *parent)
	: AMScanConfigurationView(parent)
{
}

BioXASSollerSlitScanConfigurationView::~BioXASSollerSlitScanConfigurationView()
{

}

const AMScanConfiguration *BioXASSollerSlitScanConfigurationView::configuration() const
{
	return 0;
}
