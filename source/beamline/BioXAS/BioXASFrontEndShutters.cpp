#include "BioXASFrontEndShutters.h"

#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSExclusiveStatesControl.h"

BioXASFrontEndShutters:: BioXASFrontEndShutters(const QString &name, QObject *parent) :
	CLSShutters(name, parent)
{
	// Initialize class variables.

	upstreamPhotonShutter_ = 0;
	downstreamPhotonShutter_ = 0;

	// Current settings.

	setUpstreamPhotonShutter(new AMReadOnlyPVControl("IPSH1407-I00-01", "IPSH1407-I00-01:state", this));
	setDownstreamPhotonShutter(new CLSExclusiveStatesControl("IPSH1407-I00-02", "IPSH1407-I00-02:state", "IPSH1407-I00-02:opr:open", "IPSH1407-I00-02:opr:close", this));
	setSafetyShutter(new CLSExclusiveStatesControl("SSH1407-I00-01", "SSH1407-I00-01:state", "SSH1407-I00-01:opr:open", "SSH1407-I00-01:opr:close", this));
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
			addShutter(upstreamPhotonShutter_);

		emit upstreamPhotonShutterChanged(upstreamPhotonShutter_);
	}
}

void BioXASFrontEndShutters::setDownstreamPhotonShutter(CLSExclusiveStatesControl *newControl)
{
	if (downstreamPhotonShutter_ != newControl) {

		if (downstreamPhotonShutter_)
			removeShutter(downstreamPhotonShutter_);

		downstreamPhotonShutter_ = newControl;

		if (downstreamPhotonShutter_)
			addShutter(downstreamPhotonShutter_, -1, CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);

		emit downstreamPhotonShutterChanged(downstreamPhotonShutter_);
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
