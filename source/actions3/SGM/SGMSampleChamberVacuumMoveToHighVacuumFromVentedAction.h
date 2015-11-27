#ifndef SGMSAMPLECHAMBERVACUUMMOVETOHIGHVACUUMFROMVENTEDACTION_H
#define SGMSAMPLECHAMBERVACUUMMOVETOHIGHVACUUMFROMVENTEDACTION_H

#include "actions3/SGM/SGMSampleChamberVacuumMoveAction.h"

class SGMSampleChamberVacuumMoveToHighVacuumFromVentedAction : public SGMSampleChamberVacuumMoveAction
{
	Q_OBJECT

public:
	/// Constructor.
	SGMSampleChamberVacuumMoveToHighVacuumFromVentedAction(SGMSampleChamberVacuumMoveActionInfo *info, QObject *parent = 0);
	/// Copy constructor.
	SGMSampleChamberVacuumMoveToHighVacuumFromVentedAction(const SGMSampleChamberVacuumMoveToHighVacuumFromVentedAction &original);
	/// Destructor.
	virtual ~SGMSampleChamberVacuumMoveToHighVacuumFromVentedAction();
};

#endif // SGMSAMPLECHAMBERVACUUMMOVETOHIGHVACUUMFROMVENTEDACTION_H
