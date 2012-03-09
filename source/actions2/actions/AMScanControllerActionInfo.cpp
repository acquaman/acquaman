#include "AMScanControllerActionInfo.h"

#include "acquaman/AMScanConfiguration.h"
#include <QStringBuilder>

AMScanControllerActionInfo::AMScanControllerActionInfo(AMScanConfiguration* scanConfig, QObject *parent) :
	AMActionInfo(scanConfig ? scanConfig->description() : "Invalid Scan",
				 scanConfig ? scanConfig->description() : "This action attempts to run a scan, but does not have a valid scan configuration.",
				 ":/utilities-system-monitor.png",
				 parent) {
	scanConfig_ = scanConfig;
}

AMScanControllerActionInfo::AMScanControllerActionInfo(const AMScanControllerActionInfo &other) :
	AMActionInfo(other) {
	scanConfig_ = other.scanConfig_ ? other.scanConfig_->createCopy() : 0;
}

AMScanControllerActionInfo::~AMScanControllerActionInfo() {
	delete scanConfig_;
}

QString AMScanControllerActionInfo::typeDescription() const
{
	if(scanConfig_)
		return "Scan: " % scanConfig_->description();
	else
		return "Scan";
}
