#include "AMBeamlineControlGroup.h"
#include "beamline/AMBeamlineControl.h"

AMBeamlineControlGroup::AMBeamlineControlGroup(const QString &name, QObject *parent) :
	AMConnectedControl(name, "", parent)
{
	status_ = new AMBeamlineControlGroupStatus(QString("%1%2").arg(name).arg("Status"), this);
}

AMBeamlineControlGroup::~AMBeamlineControlGroup()
{

}

bool AMBeamlineControlGroup::isBad() const
{
	return status_->isBad();
}

bool AMBeamlineControlGroup::isGood() const
{
	return status_->isGood();
}

bool AMBeamlineControlGroup::hasControl(AMControl *control) const
{
	return hasChildControl(control);
}

bool AMBeamlineControlGroup::addControl(AMBeamlineControl *control)
{
	bool result = false;

	if (control && !children_.contains(control)) {
		addChildControl(control);
		status_->addControl(control);
		result = true;
	}

	return result;
}

bool AMBeamlineControlGroup::removeControl(AMBeamlineControl *control)
{
	bool result = false;

	if (control && children_.contains(control)) {
		removeChildControl(control);
		status_->removeControl(control);
		result = true;
	}

	return result;
}

bool AMBeamlineControlGroup::clearControls()
{
	clearChildControls();
	status_->clearControls();
	return true;
}

