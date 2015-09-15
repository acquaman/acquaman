#include "AMOptimizeControlActionInfo.h"

AMOptimizeControlActionInfo::AMOptimizeControlActionInfo(AMGenericStepScanConfiguration *configuration, QObject *parent) :
	AMListActionInfo3(QString(), QString(), QString(), parent)
{
	// Initialize member variables.

	option_ = Maximum;
	configuration_ = 0;

	// Current settings.

	setShortDescription(typeDescription());
	setLongDescription(typeDescription());
	setConfiguration(configuration);
}

AMOptimizeControlActionInfo::AMOptimizeControlActionInfo(const AMOptimizeControlActionInfo &original) :
	AMListActionInfo3(original)
{
	option_ = original.option();
	configuration_ = original.configuration();
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

void AMOptimizeControlActionInfo::setOption(AMOptimizeControlActionInfo::Option newOption)
{
	if (option_ != newOption) {
		option_ = newOption;
		emit optionChanged(option_);
	}
}

void AMOptimizeControlActionInfo::setConfiguration(AMGenericStepScanConfiguration *newConfiguration)
{
	if (configuration_ != newConfiguration) {
		configuration_ = newConfiguration;
		emit configurationChanged(configuration_);
	}
}
