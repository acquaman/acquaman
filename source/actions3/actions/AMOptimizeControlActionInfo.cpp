#include "AMOptimizeControlActionInfo.h"

AMOptimizeControlActionInfo::AMOptimizeControlActionInfo(AMGenericStepScanConfiguration *configuration, AMAnalysisBlock *optimizationAB, QObject *parent) :
	AMActionInfo3(QString(), QString(), QString(), parent)
{
	// Initialize member variables.

	configuration_ = 0;
	optimizationAB_ = 0;

	// Current settings.

	setShortDescription(typeDescription());
	setLongDescription(typeDescription());
	setConfiguration(configuration);
	setOptimizationAB(optimizationAB);
}

AMOptimizeControlActionInfo::AMOptimizeControlActionInfo(const AMOptimizeControlActionInfo &original) :
	AMActionInfo3(original)
{
	configuration_ = original.configuration();
	optimizationAB_ = original.optimizationAB();
}

AMOptimizeControlActionInfo::~AMOptimizeControlActionInfo()
{

}

AMActionInfo3* AMOptimizeControlActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMOptimizeControlActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

void AMOptimizeControlActionInfo::setConfiguration(AMGenericStepScanConfiguration *newConfiguration)
{
	if (configuration_ != newConfiguration) {
		configuration_ = newConfiguration;
		emit configurationChanged(configuration_);
	}
}

void AMOptimizeControlActionInfo::setOptimizationAB(AMAnalysisBlock *newOptimizationAB)
{
	if (optimizationAB_ != newOptimizationAB) {
		optimizationAB_ = newOptimizationAB;
		emit optimizationABChanged(optimizationAB_);
	}
}
