#include "BioXASSideXRFScanConfiguration.h"

#include "acquaman/BioXASSide/BioXASSideXRFScanController.h"

BioXASSideXRFScanConfiguration::BioXASSideXRFScanConfiguration(QObject *parent)
	: AMScanConfiguration(parent)
{
	setAutoExportEnabled(false);
}

BioXASSideXRFScanConfiguration::BioXASSideXRFScanConfiguration(const BioXASSideXRFScanConfiguration &other)
	: AMScanConfiguration(other)
{
	setAutoExportEnabled(false);
}

BioXASSideXRFScanConfiguration::~BioXASSideXRFScanConfiguration()
{

}

AMScanConfiguration *BioXASSideXRFScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new BioXASSideXRFScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

AMScanController *BioXASSideXRFScanConfiguration::createController()
{
	return new BioXASSideXRFScanController(this);
}

QString BioXASSideXRFScanConfiguration::detailedDescription() const
{
	return QString();
}
