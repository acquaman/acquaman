#include "SGMSampleChamberVacuumMoveToHighVacuumFromRoughVacuumAction.h"
#include "actions3/AMActionSupport.h"
#include "beamline/SGM/SGMBeamline.h"
#include "beamline/SGM/SGMSampleChamberVacuum.h"

SGMSampleChamberVacuumMoveToHighVacuumFromRoughVacuumAction::SGMSampleChamberVacuumMoveToHighVacuumFromRoughVacuumAction(SGMSampleChamberVacuumMoveActionInfo *info, QObject *parent) :
	SGMSampleChamberVacuumMoveAction(info, parent)
{
	// First, turn on chamber turbos.

	addSubAction(turnOnChamberTurbos());

	// Finally, wait for chamber pressure to drop.

	addSubAction(waitForVacuum(SGMSampleChamberVacuum::HighVacuum));
}

SGMSampleChamberVacuumMoveToHighVacuumFromRoughVacuumAction::SGMSampleChamberVacuumMoveToHighVacuumFromRoughVacuumAction(const SGMSampleChamberVacuumMoveToHighVacuumFromRoughVacuumAction &original) :
	SGMSampleChamberVacuumMoveAction(original)
{

}

SGMSampleChamberVacuumMoveToHighVacuumFromRoughVacuumAction::~SGMSampleChamberVacuumMoveToHighVacuumFromRoughVacuumAction()
{

}

