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

QList<AMBeamlineControl*> AMBeamlineControlGroup::badMonitors() const
{
	return status_->badMonitors();
}

QList<AMBeamlineControl*> AMBeamlineControlGroup::goodMonitors() const
{
	return status_->goodMonitors();
}

bool AMBeamlineControlGroup::addMonitor(AMBeamlineControl *control)
{
	bool result = false;

	if (control && !children_.contains(control)) {
		addChildControl(control);
		status_->addMonitor(control);
		result = true;
	}

	return result;
}

bool AMBeamlineControlGroup::removeMonitor(AMBeamlineControl *control)
{
	bool result = false;

	if (control && children_.contains(control)) {
		removeChildControl(control);
		status_->removeMonitor(control);
		result = true;
	}

	return result;
}

bool AMBeamlineControlGroup::clearMonitors()
{
	clearChildControls();
	status_->clearMonitors();
	return true;
}

