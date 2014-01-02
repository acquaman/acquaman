#include "SGMScanConfiguration2013.h"

 SGMScanConfiguration2013::~SGMScanConfiguration2013(){}
SGMScanConfiguration2013::SGMScanConfiguration2013()
{
	dbObject_ = new SGMSScanConfigurationDbObject();
}

SGMScanConfiguration2013::SGMScanConfiguration2013(const SGMScanConfiguration2013 &original)
{
	dbObject_ = new SGMSScanConfigurationDbObject(*original.dbObject());
}

void SGMScanConfiguration2013::dbWriteScanConfigurationDbObject(AMDbObject *object){
	SGMSScanConfigurationDbObject *dbo;

	if ((dbo = qobject_cast<SGMSScanConfigurationDbObject *>(object)))
		dbObject_ = dbo;
}
