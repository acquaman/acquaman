#include "AMBeamlineControlGroupStatus.h"
#include "beamline/AMBeamlineControl.h"

AMBeamlineControlGroupStatus::AMBeamlineControlGroupStatus(const QString &name, QObject *parent) :
	AMEnumeratedControl(name, "", parent)
{
	// Setup value options.

	addOption(Bad, "Bad", true);
	addOption(Good, "Good", true);
}

AMBeamlineControlGroupStatus::~AMBeamlineControlGroupStatus()
{

}

bool AMBeamlineControlGroupStatus::canMeasure() const
{
	bool result = false;

	if (isConnected()) {

		// Iterate through the list of monitors, finding out if
		// all can be measured.

		QList<AMControl*> monitors = childControls();

		if (monitors.count() > 0) {
			bool measurable = true;

			for (int i = 0, count = monitors.count(); i < count && measurable; i++) { // we want to stop if any one child is not measurable.
				AMBeamlineControl *monitor = qobject_cast<AMBeamlineControl*>(monitors.at(i));

				if ( !(monitor && monitor->canMeasure()) )
					measurable = false;
			}

			result = measurable;
		}
	}

	return result;
}

bool AMBeamlineControlGroupStatus::isBad() const
{
	bool monitorFound = false;

	// Iterate through list of monitors, finding out if
	// any are in the bad state.

	QList<AMControl*> monitors = childControls();

	if (monitors.count() > 0) {

		for (int i = 0, count = monitors.count(); i < count && !monitorFound; i++) { // we want to stop if any one child is in a bad state.
			AMBeamlineControl *monitor = qobject_cast<AMBeamlineControl*>(monitors.at(i));

			if (monitor && monitor->isBad())
				monitorFound = true;
		}
	}

	return monitorFound;
}

QList<AMBeamlineControl*> AMBeamlineControlGroupStatus::badMonitors() const
{
	QList<AMBeamlineControl*> monitorsList;

	foreach (AMControl *control, childControls()) {
		AMBeamlineControl *monitor = qobject_cast<AMBeamlineControl*>(control);

		if (monitor && monitor->isBad())
			monitorsList.append(monitor);
	}

	return monitorsList;
}

QList<AMBeamlineControl*> AMBeamlineControlGroupStatus::goodMonitors() const
{
	QList<AMBeamlineControl*> monitorsList;

	foreach (AMControl *control, childControls()) {
		AMBeamlineControl *monitor = qobject_cast<AMBeamlineControl*>(control);

		if (monitor && monitor->isGood())
			monitorsList.append(monitor);
	}

	return monitorsList;
}

bool AMBeamlineControlGroupStatus::addMonitor(AMBeamlineControl *control)
{
	bool result = false;

	if (control && !children_.contains(control)) {
		addChildControl(control);
		result = true;
	}

	return result;
}

bool AMBeamlineControlGroupStatus::removeMonitor(AMBeamlineControl *control)
{
	bool result = false;

	if (control && children_.contains(control)) {
		removeChildControl(control);
		result = true;
	}

	return result;
}

bool AMBeamlineControlGroupStatus::clearMonitors()
{
	clearChildControls();
	return true;
}

int AMBeamlineControlGroupStatus::currentIndex() const
{
	int result = enumNames().indexOf("Unknown");

	if (isBad())
		result = Bad;
	else if (isGood())
		result = Good;

	return result;
}
