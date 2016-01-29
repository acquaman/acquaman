#include "BioXASXRFScanConfiguration.h"

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
	return 0;
}

QString BioXASXRFScanConfiguration::detailedDescription() const
{
	return QString();
}
