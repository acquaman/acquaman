#include "AMTemperatureMonitorsStatus.h"
#include "beamline/AMTemperatureMonitor.h"

AMTemperatureMonitorsStatus::AMTemperatureMonitorsStatus(const QString &name, QObject *parent) :
	AMEnumeratedControl(name, "", parent)
{
	// Setup value options.

	addOption(Bad, "Bad", true);
	addOption(Good, "Good", true);
}

AMTemperatureMonitorsStatus::~AMTemperatureMonitorsStatus()
{

}

bool AMTemperatureMonitorsStatus::canMeasure() const
{
	bool result = false;

	if (isConnected()) {

		// Iterate through the list of monitors, finding out if
		// all can be measured.

		QList<AMControl*> monitors = childControls();

		if (monitors.count() > 0) {
			bool measurable = true;

			for (int i = 0, count = monitors.count(); i < count && measurable; i++) { // we want to stop if any one child is not measurable.
				AMTemperatureMonitor *monitor = qobject_cast<AMTemperatureMonitor*>(monitors.at(i));

				if ( !(monitor && monitor->canMeasure()) )
					measurable = false;
			}

			result = measurable;
		}
	}

	return result;
}

bool AMTemperatureMonitorsStatus::isBad() const
{
	bool monitorFound = false;

	// Iterate through list of monitors, finding out if
	// any are in the bad state.

	QList<AMControl*> monitors = childControls();

	if (monitors.count() > 0) {

		for (int i = 0, count = monitors.count(); i < count && !monitorFound; i++) { // we want to stop if any one child is in a bad state.
			AMTemperatureMonitor *monitor = qobject_cast<AMTemperatureMonitor*>(monitors.at(i));

			if (monitor && monitor->isBad())
				monitorFound = true;
		}
	}

	return monitorFound;
}

bool AMTemperatureMonitorsStatus::isGood() const
{
	bool badMonitorFound = false;

	// Iterate through list of monitors, finding out if
	// all are in a good state.

	QList<AMControl*> monitors = childControls();

	if (monitors.count() > 0) {

		for (int i = 0, count = monitors.count(); i < count && !badMonitorFound; i++) { // we want to stop if any one child isn't in state 1.
			AMTemperatureMonitor *monitor = qobject_cast<AMTemperatureMonitor*>(monitors.at(i));

			if (monitor && !monitor->isGood())
				badMonitorFound = true;
		}
	}

	return !badMonitorFound;
}

QList<AMTemperatureMonitor*> AMTemperatureMonitorsStatus::badMonitors() const
{
	QList<AMTemperatureMonitor*> monitorsList;

	foreach (AMControl *control, childControls()) {
		AMTemperatureMonitor *monitor = qobject_cast<AMTemperatureMonitor*>(control);

		if (monitor && monitor->isBad())
			monitorsList.append(monitor);
	}

	return monitorsList;
}

QList<AMTemperatureMonitor*> AMTemperatureMonitorsStatus::goodMonitors() const
{
	QList<AMTemperatureMonitor*> monitorsList;

	foreach (AMControl *control, childControls()) {
		AMTemperatureMonitor *monitor = qobject_cast<AMTemperatureMonitor*>(control);

		if (monitor && monitor->isGood())
			monitorsList.append(monitor);
	}

	return monitorsList;
}

bool AMTemperatureMonitorsStatus::addMonitor(AMTemperatureMonitor *control)
{
	bool result = false;

	if (control && !children_.contains(control)) {
		addChildControl(control);
		result = true;
	}

	return result;
}

bool AMTemperatureMonitorsStatus::removeMonitor(AMTemperatureMonitor *control)
{
	bool result = false;

	if (control && children_.contains(control)) {
		removeChildControl(control);
		result = true;
	}

	return result;
}

bool AMTemperatureMonitorsStatus::clearMonitors()
{
	clearChildControls();
	return true;
}

int AMTemperatureMonitorsStatus::currentIndex() const
{
	int result = enumNames().indexOf("Unknown");

	if (isBad())
		result = Bad;
	else if (isGood())
		result = Good;

	return result;
}
