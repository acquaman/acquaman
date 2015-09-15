#include "AMOptimizeControlActionInfo.h"

AMOptimizeControlActionInfo::AMOptimizeControlActionInfo(const AMControlInfo &controlInfo, QObject *parent) :
	AMListActionInfo3(QString(), QString(), QString(), parent)
{
	// Initialize member variables.

	option_ = Maximum;
	controlInfo_ = controlInfo;
	minimum_ = controlInfo_.minimum();
	maximum_ = controlInfo_.maximum();

	// Current settings.

	setShortDescription(typeDescription());
	setLongDescription(typeDescription());
}

AMOptimizeControlActionInfo::AMOptimizeControlActionInfo(const AMOptimizeControlActionInfo &original)
{
	option_ = original.option();
	controlInfo_ = original.control();
	minimum_ = original.minimum();
	maximum_ = original.maximum();
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

void AMOptimizeControlActionInfo::setControl(const AMControlInfo &newInfo)
{
	if (controlInfo_ != newInfo) {
		controlInfo_ = newInfo;
		emit controlChanged(controlInfo_);
	}
}

void AMOptimizeControlActionInfo::setMinimum(double newMin)
{
	if (minimum_ != newMin) {
		minimum_ = newMin;
		emit minimumChanged(minimum_);
	}
}

void AMOptimizeControlActionInfo::setMaximum(double newMax)
{
	if (maximum_ != newMax) {
		maximum_ = newMax;
		emit maximumChanged(maximum_);
	}
}
