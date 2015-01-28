#include "AMChangeToleranceActionInfo.h"

AMChangeToleranceActionInfo::AMChangeToleranceActionInfo(const AMControlInfo &control, double newTolerance, QObject *parent)
	: AMActionInfo3("Change Tolerance", "Change Tolerance", QString(), parent)
{
	controlInfo_ = control;
	tolerance_ = newTolerance;
}

AMChangeToleranceActionInfo::AMChangeToleranceActionInfo(const AMChangeToleranceActionInfo &other)
	: AMActionInfo3(other)
{
	controlInfo_.setValuesFrom(other.controlInfo());
	tolerance_ = other.tolerance();
}

AMChangeToleranceActionInfo::~AMChangeToleranceActionInfo()
{

}

AMActionInfo3 *AMChangeToleranceActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMChangeToleranceActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

void AMChangeToleranceActionInfo::setControlInfo(const AMControlInfo &controlInfo)
{
	controlInfo_.setValuesFrom(controlInfo);
	setModified(true);
}

void AMChangeToleranceActionInfo::setTolerance(double newTolerance)
{
	if (newTolerance <= 0)
		return;

	if (newTolerance == tolerance_){

		tolerance_ = newTolerance;
		setModified(true);
	}
}
