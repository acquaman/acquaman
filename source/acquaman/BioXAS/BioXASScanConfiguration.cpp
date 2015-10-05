#include "BioXASScanConfiguration.h"

BioXASScanConfiguration::BioXASScanConfiguration()
{
	dbObject_ = new BioXASScanConfigurationDbObject;
}

BioXASScanConfiguration::BioXASScanConfiguration(const BioXASScanConfiguration &original)
{
	dbObject_ = new BioXASScanConfigurationDbObject(*original.dbObject());
}

BioXASScanConfiguration::~BioXASScanConfiguration()
{

}

QString BioXASScanConfiguration::toString() const
{
	QString text;

	text.append(QString("Edge: %1").arg(edge()));
	text.append(QString("\nEnergy: %1").arg(energy()));

	return text;
}

void BioXASScanConfiguration::dbWriteScanConfigurationDbObject(AMDbObject *object)
{
	BioXASScanConfigurationDbObject *dbo;

	if ((dbo = qobject_cast<BioXASScanConfigurationDbObject *>(object)))
		dbObject_ = dbo;
}
