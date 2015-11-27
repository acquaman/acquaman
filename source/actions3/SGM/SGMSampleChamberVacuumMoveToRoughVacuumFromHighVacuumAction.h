#ifndef SGMSAMPLECHAMBERVACUUMMOVETOROUGHVACUUMFROMHIGHVACUUMACTION_H
#define SGMSAMPLECHAMBERVACUUMMOVETOROUGHVACUUMFROMHIGHVACUUMACTION_H

#include "actions3/SGM/SGMSampleChamberVacuumMoveAction.h"

class SGMSampleChamberVacuumMoveToRoughVacuumFromHighVacuumAction : public SGMSampleChamberVacuumMoveAction
{
	Q_OBJECT

public:
	/// Constructor.
	SGMSampleChamberVacuumMoveToRoughVacuumFromHighVacuumAction(SGMSampleChamberVacuumMoveActionInfo *info, QObject *parent = 0);
	/// Copy constructor.
	SGMSampleChamberVacuumMoveToRoughVacuumFromHighVacuumAction(const SGMSampleChamberVacuumMoveToRoughVacuumFromHighVacuumAction &original);
	/// Destructor.
	virtual ~SGMSampleChamberVacuumMoveToRoughVacuumFromHighVacuumAction();
};

#endif // SGMSAMPLECHAMBERVACUUMMOVETOROUGHVACUUMFROMHIGHVACUUMACTION_H
