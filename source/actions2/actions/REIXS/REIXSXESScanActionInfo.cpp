#include "REIXSXESScanActionInfo.h"

#include "acquaman/REIXS/REIXSXESScanConfiguration.h"

REIXSXESScanActionInfo::REIXSXESScanActionInfo(REIXSXESScanConfiguration *scanConfig, QObject *parent) : AMScanControllerActionInfo(scanConfig, parent) {}

void REIXSXESScanActionInfo::dbLoadScanConfig(AMDbObject *newObject) {
	REIXSXESScanConfiguration* newScanConfig = qobject_cast<REIXSXESScanConfiguration*>(newObject);
	if(!newScanConfig) {
		delete newObject;
		return;
	}

	delete scanConfig_;	// if there is one. If there isn't (ie: == 0) does nothing.
	scanConfig_ = newScanConfig;
}

AMDbObject * REIXSXESScanActionInfo::dbReadScanConfig()
{
	return scanConfig_;
}
