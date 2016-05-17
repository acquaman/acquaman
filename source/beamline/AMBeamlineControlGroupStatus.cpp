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

	// Iterate through list of beamline controls, finding out if
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

QList<AMBeamlineControl*> AMBeamlineControlGroupStatus::badControls() const
{
	QList<AMBeamlineControl*> controlsList;

	// Iterate through list of beamline controls, identifying those
	// in the 'bad' state.

	foreach (AMControl *control, childControls()) {
		AMBeamlineControl *beamlineControl = qobject_cast<AMBeamlineControl*>(control);

		if (beamlineControl && beamlineControl->isBad())
			controlsList.append(beamlineControl);
	}

	return controlsList;
}

QList<AMBeamlineControl*> AMBeamlineControlGroupStatus::goodControls() const
{
	QList<AMBeamlineControl*> controlsList;

	// Iterate through list of beamline controls, identifying those
	// in the 'good' state.

	foreach (AMControl *control, childControls()) {
		AMBeamlineControl *beamlineControl = qobject_cast<AMBeamlineControl*>(control);

		if (beamlineControl && beamlineControl->isGood())
			controlsList.append(beamlineControl);
	}

	return controlsList;
}

bool AMBeamlineControlGroupStatus::addControl(AMBeamlineControl *control)
{
        return addChildControl(control);
}

bool AMBeamlineControlGroupStatus::removeControl(AMBeamlineControl *control)
{
        return removeChildControl(control);
}

bool AMBeamlineControlGroupStatus::clearControls()
{
        return clearChildControls();
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
