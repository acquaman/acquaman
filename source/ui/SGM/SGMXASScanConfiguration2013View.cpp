#include "SGMXASScanConfiguration2013View.h"

SGMXASScanConfiguration2013View::SGMXASScanConfiguration2013View(SGMXASScanConfiguration2013 *scanConfiguration, QWidget *parent) :
	AMScanConfigurationView(parent)
{

}

const AMScanConfiguration* SGMXASScanConfiguration2013View::configuration() const{
	//cfg_->setDetectorConfigurations(xasDetectorsView_->configValues());
	return configuration_;
}
