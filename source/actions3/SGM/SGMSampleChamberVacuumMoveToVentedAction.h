#ifndef SGMSAMPLECHAMBERVACUUMMOVETOVENTEDACTION_H
#define SGMSAMPLECHAMBERVACUUMMOVETOVENTEDACTION_H

#include "source/actions3/SGM/SGMSampleChamberVacuumMoveAction.h"

class SGMSampleChamberVacuumMoveToVentedAction : public SGMSampleChamberVacuumMoveAction
{
	Q_OBJECT

public:
	/// Constructor.
	SGMSampleChamberVacuumMoveToVentedAction(SGMSampleChamberVacuumMoveActionInfo *info, QObject *parent = 0);
	/// Copy constructor.
	SGMSampleChamberVacuumMoveToVentedAction(const SGMSampleChamberVacuumMoveToVentedAction &original);
	/// Destructor.
	virtual ~SGMSampleChamberVacuumMoveToVentedAction();
};

#endif // SGMSAMPLECHAMBERVACUUMMOVETOVENTEDACTION_H
