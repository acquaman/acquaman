#include "BioXASShutters.h"
#include "beamline/AMControl.h"
#include "beamline/CLS/CLSExclusiveStatesControl.h"
#include "beamline/BioXAS/BioXASFrontEndShutters.h"
#include "actions3/AMListAction3.h"

BioXASShutters::BioXASShutters(const QString &name, QObject *parent) :
	BioXASShuttersGroup(name, parent)
{
	// Initialize class variables.

	frontEndShutters_ = 0;
	endstationShutter_ = 0;
}

BioXASShutters::~BioXASShutters()
{

}

bool BioXASShutters::hasShutter(AMControl *control) const
{
	return hasChildControl(control);
}

void BioXASShutters::setFrontEndShutters(BioXASFrontEndShutters *newShutters)
{
	if (frontEndShutters_ != newShutters) {

		if (frontEndShutters_)
			removeShutter(frontEndShutters_);

		frontEndShutters_ = newShutters;

		if (frontEndShutters_)
			addShutter(frontEndShutters_, BioXASFrontEndShutters::Open, BioXASFrontEndShutters::Closed);

		emit frontEndShuttersChanged(frontEndShutters_);
	}
}

void BioXASShutters::setEndstationShutter(CLSExclusiveStatesControl *newShutter)
{
	if (endstationShutter_ != newShutter) {

		if (endstationShutter_)
			removeShutter(endstationShutter_);

		endstationShutter_ = newShutter;

		if (endstationShutter_)
			addShutter(endstationShutter_, CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);

		emit endstationShutterChanged(endstationShutter_);
	}
}

AMAction3* BioXASShutters::createMoveToOpenAction()
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Opening shutters", "Opening shutters"), AMListAction3::Sequential);
	action->addSubAction(createMoveChildToOpen(endstationShutter_));
	action->addSubAction(createMoveChildToOpen(frontEndShutters_));

	return action;
}

AMAction3* BioXASShutters::createMoveToClosedAction()
{
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Closing shutters", "Closing shutters"), AMListAction3::Sequential);
	action->addSubAction(createMoveChildToClosed(frontEndShutters_));
	action->addSubAction(createMoveChildToClosed(endstationShutter_));

	return action;
}
