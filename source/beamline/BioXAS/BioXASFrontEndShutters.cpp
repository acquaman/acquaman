#include "BioXASFrontEndShutters.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include <QDebug>
BioXASFrontEndShutters:: BioXASFrontEndShutters(QObject *parent) :
	BioXASShutters("BioXASFrontEndShutters", parent)
{
	// Initialize class variables.

	upstreamPhotonShutter_ = new AMReadOnlyPVControl("IPSH1407-I00-01", "IPSH1407-I00-01:state", this);
	downstreamPhotonShutter_ = new CLSBiStateControl("IPSH1407-I00-02", "IPSH1407-I00-02", "IPSH1407-I00-02:state", "IPSH1407-I00-02:opr:open", "IPSH1407-I00-02:opr:close", new AMControlStatusCheckerDefault(2), this);
	safetyShutter_ = new CLSBiStateControl("SSH1407-I00-01", "SSH1407-I00-01", "SSH1407-I00-01:state", "SSH1407-I00-01:opr:open", "SSH1407-I00-01:opr:close", new AMControlStatusCheckerDefault(2), this);

	// Add shutters.

	addShutter(upstreamPhotonShutter_, CLSBiStateControl::Open, CLSBiStateControl::Closed);
	addShutter(downstreamPhotonShutter_, 1, 0);
	addShutter(safetyShutter_, 1, 0);
}

BioXASFrontEndShutters::~BioXASFrontEndShutters()
{

}

void BioXASFrontEndShutters::updateConnected()
{
	BioXASShutters::updateConnected();

	qDebug() << "\n\n" << toString();
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
