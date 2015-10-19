#include "AMControlCalibrateActionInfo.h"

AMControlCalibrateActionInfo::AMControlCalibrateActionInfo(const AMControlInfo &controlInfo, double oldValue, double newValue, QObject *parent) :
	AMActionInfo3(QString(), QString(), QString(), parent)
{
	controlInfo_.setValuesFrom(controlInfo);
	oldValue_ = oldValue;
	newValue_ = newValue;
}

AMControlCalibrateActionInfo::AMControlCalibrateActionInfo(const AMControlCalibrateActionInfo &original) :
	AMActionInfo3(original)
{
	controlInfo_.setValuesFrom(original.controlInfo());
	oldValue_ = original.oldValue();
	newValue_ = original.newValue();
}

AMControlCalibrateActionInfo::~AMControlCalibrateActionInfo()
{

}

AMActionInfo3* AMControlCalibrateActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMControlCalibrateActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

void AMControlCalibrateActionInfo::setControlInfo(const AMControlInfo &newInfo)
{
	controlInfo_.setValuesFrom(newInfo);
	setModified(true);

	updateDescriptionText();
}

void AMControlCalibrateActionInfo::setOldValue(double newValue)
{
	oldValue_ = newValue;
	setModified(true);

	updateDescriptionText();
}

void AMControlCalibrateActionInfo::setNewValue(double newValue)
{
	newValue_ = newValue;
	setModified(true);

	updateDescriptionText();
}

void AMControlCalibrateActionInfo::updateDescriptionText()
{
	QString controlName = (controlInfo_.description().isEmpty() ? controlInfo_.name() : controlInfo_.description());
	QString oldValue = QString::number(oldValue_);
	QString newValue = QString::number(newValue_);

	QString newText = QString("Calibrating %1 such that %2 becomes %3").arg(controlName).arg(oldValue).arg(newValue);

	setShortDescription(newText);
	setLongDescription(newText);
}
