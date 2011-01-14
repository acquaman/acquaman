#include "AMScanControllerActionInfo.h"

#include "acquaman/AMScanConfiguration.h"
#include <QStringBuilder>

AMScanControllerActionInfo::AMScanControllerActionInfo(AMScanConfiguration* scanConfig, QObject *parent) :
	AMActionInfo(scanConfig->description(), scanConfig->description(), ":/utilities-system-monitor.png", parent) {
	scanConfig_ = scanConfig;
}

AMScanControllerActionInfo::AMScanControllerActionInfo(const AMScanControllerActionInfo &other) :
	AMActionInfo(other) {
	scanConfig_ = other.scanConfig()->createCopy();
}

AMScanControllerActionInfo::~AMScanControllerActionInfo() {
	delete scanConfig_;
}

QString AMScanControllerActionInfo::typeDescription() const
{
	return "Scan: " % scanConfig_->description();
}
