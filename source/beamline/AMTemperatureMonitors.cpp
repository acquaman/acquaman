#include "AMTemperatureMonitors.h"
#include "beamline/AMTemperatureMonitor.h"

AMTemperatureMonitors::AMTemperatureMonitors(const QString &name, QObject *parent) :
	AMConnectedControl(name, "", parent)
{
	status_ = new AMTemperatureMonitorsStatus(QString("%1%2").arg(name).arg("Status"), this);
}

AMTemperatureMonitors::~AMTemperatureMonitors()
{

}

bool AMTemperatureMonitors::isBad() const
{
	return status_->isBad();
}

bool AMTemperatureMonitors::isGood() const
{
	return status_->isGood();
}

QList<AMTemperatureMonitor*> AMTemperatureMonitors::badMonitors() const
{
	return status_->badMonitors();
}

QList<AMTemperatureMonitor*> AMTemperatureMonitors::goodMonitors() const
{
	return status_->goodMonitors();
}

bool AMTemperatureMonitors::addMonitor(AMTemperatureMonitor *control)
{
	bool result = false;

	if (control && !children_.contains(control)) {
		addChildControl(control);
		status_->addMonitor(control);
		result = true;
	}

	return result;
}

bool AMTemperatureMonitors::removeMonitor(AMTemperatureMonitor *control)
{
	bool result = false;

	if (control && children_.contains(control)) {
		removeChildControl(control);
		status_->removeMonitor(control);
		result = true;
	}

	return result;
}

bool AMTemperatureMonitors::clearMonitors()
{
	clearChildControls();
	status_->clearMonitors();
	return true;
}

