#include "REIXSXASScanConfiguration.h"

REIXSXASScanConfiguration::REIXSXASScanConfiguration(QObject *parent) :
	AMXASScanConfiguration(parent)
{
}

REIXSXASScanConfiguration::~REIXSXASScanConfiguration() {

}

AMScanConfiguration * REIXSXASScanConfiguration::createCopy() const
{
	// using default copy constructor:
	return new REIXSXASScanConfiguration(*this);
}

#include "acquaman/REIXS/REIXSXASScanController.h"
AMScanController * REIXSXASScanConfiguration::createController()
{
	return new REIXSXASScanController(this);
}
