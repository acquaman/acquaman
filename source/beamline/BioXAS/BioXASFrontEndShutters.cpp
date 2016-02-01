#include "BioXASFrontEndShutters.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSBiStateControl.h"

BioXASFrontEndShutters:: BioXASFrontEndShutters(const QString &name, QObject *parent) :
	BioXASShuttersGroup(name, parent)
{
	// Initialize class variables.

	upstreamPhotonShutter_ = 0;
	downstreamPhotonShutter_ = 0;
	safetyShutter_ = 0;
}

BioXASFrontEndShutters::~BioXASFrontEndShutters()
{

}

bool BioXASFrontEndShutters::canMove() const
{
	bool result = false;

	if (isConnected())
		result = ( downstreamPhotonShutter_->canMove() && safetyShutter_->canMove() );

	return result;
}

void BioXASFrontEndShutters::setUpstreamPhotonShutter(AMReadOnlyPVControl *newControl)
{
	if (upstreamPhotonShutter_ != newControl) {

		if (upstreamPhotonShutter_)
			removeShutter(upstreamPhotonShutter_);

		upstreamPhotonShutter_ = newControl;

		if (upstreamPhotonShutter_)
			addShutter(upstreamPhotonShutter_, 1, 4);

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
			addShutter(downstreamPhotonShutter_, 1, 4);

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
			addShutter(safetyShutter_, 1, 4);

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
