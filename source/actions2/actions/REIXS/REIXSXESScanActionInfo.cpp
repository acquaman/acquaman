#include "REIXSXESScanActionInfo.h"

REIXSXESScanActionInfo::REIXSXESScanActionInfo(REIXSXESScanConfiguration *scanConfig, QObject *parent) : AMScanControllerActionInfo(scanConfig, parent) {}

// This will never be called, because we will always have a valid REIXSXESScanConfiguration returned by dbReadScanConfig().
void REIXSXESScanActionInfo::dbLoadScanConfig(AMDbObject *newObject) {
	REIXSXESScanConfiguration* newScanConfig = qobject_cast<REIXSXESScanConfiguration*>(newObject);
	if(!newScanConfig) {
		delete newObject;
		return;
	}

	delete scanConfig_;
	scanConfig_ = newScanConfig;
}

AMDbObject * REIXSXESScanActionInfo::dbReadScanConfig()
{
	return scanConfig_;
}

void REIXSXESScanActionInfo::setConfigCenterEV(double eV)
{
		xesConfig()->setCenterEV(eV);
		QString description = xesConfig()->description();
		setShortDescription(description);
		setLongDescription(description);
}

void REIXSXESScanActionInfo::setConfigDefocusMm(double mm)
{
		xesConfig()->setDefocusDistanceMm(mm);
		QString description = xesConfig()->description();
		setShortDescription(description);
		setLongDescription(description);
}

void REIXSXESScanActionInfo::setConfigTiltOffset(double degrees)
{
		xesConfig()->setDetectorTiltOffset(degrees);
		QString description = xesConfig()->description();
		setShortDescription(description);
		setLongDescription(description);
}
