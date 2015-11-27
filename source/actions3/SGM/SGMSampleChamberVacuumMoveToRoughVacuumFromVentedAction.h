#ifndef SGMSAMPLECHAMBERVACUUMMOVETOROUGHVACUUMFROMVENTEDACTION_H
#define SGMSAMPLECHAMBERVACUUMMOVETOROUGHVACUUMFROMVENTEDACTION_H

#include "actions3/SGM/SGMSampleChamberVacuumMoveAction.h"

class SGMSampleChamberVacuumMoveToRoughVacuumFromVentedAction : public SGMSampleChamberVacuumMoveAction
{
	Q_OBJECT

public:
	/// Constructor.
	SGMSampleChamberVacuumMoveToRoughVacuumFromVentedAction(SGMSampleChamberVacuumMoveActionInfo *info, QObject *parent = 0);
	/// Copy constructor.
	SGMSampleChamberVacuumMoveToRoughVacuumFromVentedAction(const SGMSampleChamberVacuumMoveToRoughVacuumFromVentedAction &original);
	/// Destructor.
	virtual ~SGMSampleChamberVacuumMoveToRoughVacuumFromVentedAction();
};

#endif // SGMSAMPLECHAMBERVACUUMMOVETOROUGHVACUUMFROMVENTEDACTION_H
