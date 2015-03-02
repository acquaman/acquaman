#include "BioXASXRFScanConfiguration.h"

#include "acquaman/BioXAS/BioXASXRFScanController.h"

BioXASXRFScanConfiguration::BioXASXRFScanConfiguration(QObject *parent)
	: AMScanConfiguration(parent)
{
	setAutoExportEnabled(false);
}

BioXASXRFScanConfiguration::BioXASXRFScanConfiguration(const BioXASXRFScanConfiguration &other)
	: AMScanConfiguration(other)
{
	setAutoExportEnabled(false);
}

BioXASXRFScanConfiguration::~BioXASXRFScanConfiguration()
{

}

AMScanConfiguration *BioXASXRFScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new BioXASXRFScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

AMScanController *BioXASXRFScanConfiguration::createController()
{
	return new BioXASXRFScanController(this);
}

QString BioXASXRFScanConfiguration::detailedDescription() const
{
	return QString();
}
