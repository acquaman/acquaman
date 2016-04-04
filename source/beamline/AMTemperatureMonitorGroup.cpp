#include "AMTemperatureMonitorGroup.h"
#include "beamline/AMTemperatureMonitor.h"

AMTemperatureMonitorGroup::AMTemperatureMonitorGroup(const QString &name, QObject *parent) :
	AMConnectedControl(name, "", parent)
{
	status_ = new AMTemperatureMonitorGroupStatus(QString("%1%2").arg(name).arg("Status"), this);
}

AMTemperatureMonitorGroup::~AMTemperatureMonitorGroup()
{

}

bool AMTemperatureMonitorGroup::isBad() const
{
	return status_->isBad();
}

bool AMTemperatureMonitorGroup::isGood() const
{
	return status_->isGood();
}

QList<AMTemperatureMonitor*> AMTemperatureMonitorGroup::badMonitors() const
{
	return status_->badMonitors();
}

QList<AMTemperatureMonitor*> AMTemperatureMonitorGroup::goodMonitors() const
{
	return status_->goodMonitors();
}

bool AMTemperatureMonitorGroup::addMonitor(AMTemperatureMonitor *control)
{
	bool result = false;

	if (control && !children_.contains(control)) {
		addChildControl(control);
		status_->addMonitor(control);
		result = true;
	}

	return result;
}

bool AMTemperatureMonitorGroup::removeMonitor(AMTemperatureMonitor *control)
{
	bool result = false;

	if (control && children_.contains(control)) {
		removeChildControl(control);
		status_->removeMonitor(control);
		result = true;
	}

	return result;
}

bool AMTemperatureMonitorGroup::clearMonitors()
{
	clearChildControls();
	status_->clearMonitors();
	return true;
}

