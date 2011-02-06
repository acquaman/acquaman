#include "REIXSXESScanConfigurationView.h"

REIXSXESScanConfigurationView::REIXSXESScanConfigurationView(QWidget *parent) :
	AMScanConfigurationView(parent)
{
	configuration_ = new REIXSXESScanConfiguration(this);
}
