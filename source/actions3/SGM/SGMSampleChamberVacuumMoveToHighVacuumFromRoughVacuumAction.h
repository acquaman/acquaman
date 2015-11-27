#ifndef SGMSAMPLECHAMBERVACUUMMOVETOHIGHVACUUMFROMROUGHVACUUMACTION_H
#define SGMSAMPLECHAMBERVACUUMMOVETOHIGHVACUUMFROMROUGHVACUUMACTION_H

#include "actions3/SGM/SGMSampleChamberVacuumMoveAction.h"

class SGMSampleChamberVacuumMoveToHighVacuumFromRoughVacuumAction : public SGMSampleChamberVacuumMoveAction
{
	Q_OBJECT

public:
	/// Constructor.
	SGMSampleChamberVacuumMoveToHighVacuumFromRoughVacuumAction(SGMSampleChamberVacuumMoveActionInfo *info, QObject *parent = 0);
	/// Copy constructor.
	SGMSampleChamberVacuumMoveToHighVacuumFromRoughVacuumAction(const SGMSampleChamberVacuumMoveToHighVacuumFromRoughVacuumAction &original);
	/// Destructor.
	virtual ~SGMSampleChamberVacuumMoveToHighVacuumFromRoughVacuumAction();
};

#endif // SGMSAMPLECHAMBERVACUUMMOVETOHIGHVACUUMFROMROUGHVACUUMACTION_H
