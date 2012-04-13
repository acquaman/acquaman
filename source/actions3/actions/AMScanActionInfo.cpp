#include "AMScanActionInfo.h"

AMScanActionInfo::AMScanActionInfo(AMScanConfiguration *config, const QString &iconFileName, QObject *parent)
	: AMActionInfo3(config->name(), config->description(), iconFileName, parent)
{
	config_ = config;
}

AMScanActionInfo::AMScanActionInfo(const AMScanActionInfo &other)
	: AMActionInfo3(other)
{
	config_ = other.config_ ? other.config_->createCopy() : 0;
}

AMScanActionInfo::~AMScanActionInfo()
{
	if (config_)
		delete config_;
}
