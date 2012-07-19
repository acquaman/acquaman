#include "REIXSXASScanConfiguration.h"
#include <QDebug>

REIXSXASScanConfiguration::REIXSXASScanConfiguration(QObject *parent) :
	AMXASScanConfiguration(parent)
{
	regions()->appendRegion(380, 0.1, 420, 0.5);
}

REIXSXASScanConfiguration::~REIXSXASScanConfiguration() {

}

REIXSXASScanConfiguration::REIXSXASScanConfiguration(const REIXSXASScanConfiguration &other)
	: AMXASScanConfiguration(other)
{
	/// \todo This belongs in AMRegionScanConfiguration: copy the regions.
	regions_->setSensibleStart(other.regions()->sensibleStart());
	regions_->setSensibleEnd(other.regions()->sensibleEnd());
	regions_->setDefaultUnits(other.regions()->defaultUnits());
	regions_->setDefaultTimeUnits(other.regions()->defaultTimeUnits());
	for(int x = 0; x < other.regionCount(); x++)
		regions_->addRegion(x, other.regionStart(x), other.regionDelta(x), other.regionEnd(x));
	////////////////////////


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
