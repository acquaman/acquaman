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

REIXSXESScanConfiguration * REIXSXESScanActionInfo::xesConfig()
{
	return qobject_cast<REIXSXESScanConfiguration*>(scanConfig_);
}
const REIXSXESScanConfiguration * REIXSXESScanActionInfo::xesConfig() const
{
	return qobject_cast<const REIXSXESScanConfiguration*>(scanConfig_);
}

void REIXSXESScanActionInfo::setConfigCenterEV(double eV)
{
	if(xesConfig()) {
		xesConfig()->setCenterEV(eV);
		QString description = xesConfig()->description();
		setShortDescription(description);
		setLongDescription(description);
	}

}

void REIXSXESScanActionInfo::setConfigDefocusMm(double mm)
{
	if(xesConfig()) {
		xesConfig()->setDefocusDistanceMm(mm);
		QString description = xesConfig()->description();
		setShortDescription(description);
		setLongDescription(description);
	}
}

void REIXSXESScanActionInfo::setConfigTiltOffset(double degrees)
{
	if(xesConfig()) {
		xesConfig()->setDetectorTiltOffset(degrees);
		QString description = xesConfig()->description();
		setShortDescription(description);
		setLongDescription(description);
	}
}

double REIXSXESScanActionInfo::configCenterEV() const
{
	return xesConfig() ? xesConfig()->centerEV() : 0;
}

double REIXSXESScanActionInfo::configDefocusMm() const
{
	return xesConfig() ? xesConfig()->defocusDistanceMm() : 0;
}

double REIXSXESScanActionInfo::configTiltOffset() const
{
	return xesConfig() ? xesConfig()->detectorTiltOffset() : 0;
}
