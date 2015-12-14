#include "BioXASFrontEndShutters.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSBiStateControl.h"

BioXASFrontEndShutters:: BioXASFrontEndShutters(QObject *parent) :
	BioXASShutters("BioXASFrontEndShutters", parent)
{
	// Initialize class variables.

	upstreamPhotonShutter_ = 0;
	downstreamPhotonShutter_ = 0;
	safetyShutter_ = 0;
}

BioXASFrontEndShutters::~BioXASFrontEndShutters()
{

}

void BioXASFrontEndShutters::setUpstreamPhotonShutter(AMReadOnlyPVControl *newControl)
{
	if (upstreamPhotonShutter_ != newControl) {

		if (upstreamPhotonShutter_)
			removeShutter(upstreamPhotonShutter_);

		upstreamPhotonShutter_ = newControl;

		if (upstreamPhotonShutter_)
			addShutter(upstreamPhotonShutter_, CLSBiStateControl::Open, CLSBiStateControl::Closed);

		emit upstreamPhotonShutterChanged(upstreamPhotonShutter_);
	}
}

void BioXASFrontEndShutters::setDownstreamPhotonShutter(CLSBiStateControl *newControl)
{
	if (downstreamPhotonShutter_ != newControl) {

		if (downstreamPhotonShutter_)
			removeShutter(downstreamPhotonShutter_);

		downstreamPhotonShutter_ = newControl;

		if (downstreamPhotonShutter_)
			addShutter(downstreamPhotonShutter_, CLSBiStateControl::Open, CLSBiStateControl::Closed);

		emit downstreamPhotonShutterChanged(downstreamPhotonShutter_);
	}
}

void BioXASFrontEndShutters::setSafetyShutter(CLSBiStateControl *newControl)
{
	if (safetyShutter_ != newControl) {

		if (safetyShutter_)
			removeShutter(safetyShutter_);

		safetyShutter_ = newControl;

		if (safetyShutter_)
			addShutter(safetyShutter_, CLSBiStateControl::Open, CLSBiStateControl::Closed);

		emit safetyShutterChanged(safetyShutter_);
	}
}

AMAction3* BioXASFrontEndShutters::createMoveToOpenAction()
{
	AMListAction3 *actionList = new AMListAction3(new AMListActionInfo3("Opening front-end shutters", "Opening front-end shutters"), AMListAction3::Sequential);
	actionList->addSubAction(createCheckChildIsOpen(upstreamPhotonShutter_, 10));
	actionList->addSubAction(createMoveChildToOpen(safetyShutter_));
	actionList->addSubAction(createMoveChildToOpen(downstreamPhotonShutter_));

	return actionList;
}

AMAction3* BioXASFrontEndShutters::createMoveToClosedAction()
{
	AMListAction3 *actionList = new AMListAction3(new AMListActionInfo3("Closing front-end shutters", "Closing front-end shutters"), AMListAction3::Sequential);
	actionList->addSubAction(createMoveChildToClosed(downstreamPhotonShutter_));
	actionList->addSubAction(createMoveChildToClosed(safetyShutter_));

	return actionList;
}
