#include "CLSJJSlitOptimizationActionInfo.h"

CLSJJSlitOptimizationActionInfo::CLSJJSlitOptimizationActionInfo(AMGenericStepScanConfiguration *configuration, double initialGap, double initialCenter, QObject *parent) :
	AMListActionInfo3(QString(), QString(), QString(), parent)
{
	// Initialize member variables.

	configuration_ = 0;

	initialGap_ = initialGap;
	initialCenter_ = initialCenter;

	// Current settings.

	setShortDescription(typeDescription());
	setLongDescription(typeDescription());

	setConfiguration(configuration);
}

CLSJJSlitOptimizationActionInfo::CLSJJSlitOptimizationActionInfo(const CLSJJSlitOptimizationActionInfo &original) :
	AMListActionInfo3(original)
{
	configuration_ = original.configuration();

	initialGap_ = original.initialGap();
	initialCenter_ = original.initialCenter();
}

CLSJJSlitOptimizationActionInfo::~CLSJJSlitOptimizationActionInfo()
{

}

AMActionInfo3* CLSJJSlitOptimizationActionInfo::createCopy() const
{
	AMActionInfo3 *info = new CLSJJSlitOptimizationActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

void CLSJJSlitOptimizationActionInfo::setConfiguration(AMGenericStepScanConfiguration *newConfiguration)
{
	if (configuration_ != newConfiguration) {
		configuration_ = newConfiguration;
		emit configurationChanged(configuration_);
	}
}
