#include "VESPERS2DScanConfiguration.h"

#include "acquaman/VESPERS/VESPERS2DDacqScanController.h"

VESPERS2DScanConfiguration::VESPERS2DScanConfiguration(QObject *parent)
	: AM2DScanConfiguration(parent)
{
}

VESPERS2DScanConfiguration::VESPERS2DScanConfiguration(const VESPERS2DScanConfiguration &original)
	: AM2DScanConfiguration(original.parent())
{

}

AMScanConfiguration *VESPERS2DScanConfiguration::createCopy() const
{
	return new VESPERS2DScanConfiguration(*this);
}

AMScanController *VESPERS2DScanConfiguration::createController()
{
	return new VESPERS2DDacqScanController(this);
}
