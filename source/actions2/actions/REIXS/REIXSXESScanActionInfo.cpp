#include "REIXSXESScanActionInfo.h"

REIXSXESScanActionInfo::REIXSXESScanActionInfo(REIXSXESScanConfiguration *scanConfig, QObject *parent) : AMScanControllerActionInfo(scanConfig, parent)
{
	connect(scanConfig, SIGNAL(configurationChanged()), this, SLOT(onConfigurationChanged()));
}

REIXSXESScanActionInfo::REIXSXESScanActionInfo(const REIXSXESScanActionInfo &other) : AMScanControllerActionInfo(other)
{
	connect(xesConfig(), SIGNAL(configurationChanged()), this, SLOT(onConfigurationChanged()));
}

// This will never be called, because we will always have a valid REIXSXESScanConfiguration returned by dbReadScanConfig().
void REIXSXESScanActionInfo::dbLoadScanConfig(AMDbObject *newObject) {
	REIXSXESScanConfiguration* newScanConfig = qobject_cast<REIXSXESScanConfiguration*>(newObject);
	if(!newScanConfig) {
		delete newObject;
		return;
	}

	delete scanConfig_;
	scanConfig_ = newScanConfig;
	connect(newScanConfig, SIGNAL(configurationChanged()), this, SLOT(onConfigurationChanged()));
}

AMDbObject * REIXSXESScanActionInfo::dbReadScanConfig()
{
	return scanConfig_;
}


#include <QDebug>
void REIXSXESScanActionInfo::onConfigurationChanged()
{
	QString description = xesConfig()->description();
	qDebug() << "Calling onConfigurationChanged:" << description;
	setShortDescription(description);
	setLongDescription(description);
}
