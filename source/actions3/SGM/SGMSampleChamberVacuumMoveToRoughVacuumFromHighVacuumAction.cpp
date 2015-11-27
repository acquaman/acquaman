#include "SGMSampleChamberVacuumMoveToRoughVacuumFromHighVacuumAction.h"
#include "actions3/AMActionSupport.h"
#include "beamline/SGM/SGMBeamline.h"
#include "beamline/SGM/SGMSampleChamberVacuum.h"

SGMSampleChamberVacuumMoveToRoughVacuumFromHighVacuumAction::SGMSampleChamberVacuumMoveToRoughVacuumFromHighVacuumAction(SGMSampleChamberVacuumMoveActionInfo *info, QObject *parent) :
	SGMSampleChamberVacuumMoveAction(info, parent)
{
	// First, turn off turbo pumps.

	addSubAction(turnOffChamberTurbos());

	// Finally, wait for the chamber pressure to drop to rough vacuum.

	addSubAction(waitForVacuum(SGMSampleChamberVacuum::RoughVacuum));
}

SGMSampleChamberVacuumMoveToRoughVacuumFromHighVacuumAction::SGMSampleChamberVacuumMoveToRoughVacuumFromHighVacuumAction(const SGMSampleChamberVacuumMoveToRoughVacuumFromHighVacuumAction &original) :
	SGMSampleChamberVacuumMoveAction(original)
{

}

SGMSampleChamberVacuumMoveToRoughVacuumFromHighVacuumAction::~SGMSampleChamberVacuumMoveToRoughVacuumFromHighVacuumAction()
{

}

