#include "SGMSampleChamberVacuum.h"
#include "actions3/SGM/SGMSampleChamberVacuumMoveToVentedAction.h"
#include "actions3/SGM/SGMSampleChamberVacuumMoveToRoughVacuumFromVentedAction.h"
#include "actions3/SGM/SGMSampleChamberVacuumMoveToRoughVacuumFromHighVacuumAction.h"
#include "actions3/SGM/SGMSampleChamberVacuumMoveToHighVacuumFromVentedAction.h"
#include "actions3/SGM/SGMSampleChamberVacuumMoveToHighVacuumFromRoughVacuumAction.h"
#include "ui/SGM/SGMSampleChamberVacuumMoveActionView.h"

SGMSampleChamberVacuum::SGMSampleChamberVacuum(QObject *parent) :
	SGMSampleChamberVacuumControl("sampleChamberVacuum", parent)
{
	// Setup available vacuum states.

	addVacuumOption(Vented, "Vented", SGMSAMPLECHAMBERVACUUM_VENTED_MIN, SGMSAMPLECHAMBERVACUUM_VENTED_MAX);
	addVacuumOption(RoughVacuum, "Rough vacuum", SGMSAMPLECHAMBERVACUUM_ROUGHVACUUM_MIN, SGMSAMPLECHAMBERVACUUM_ROUGHVACUUM_MAX);
	addVacuumOption(HighVacuum, "High vacuum", SGMSAMPLECHAMBERVACUUM_HIGHVACUUM_MIN, SGMSAMPLECHAMBERVACUUM_HIGHVACUUM_MAX);
}

SGMSampleChamberVacuum::~SGMSampleChamberVacuum()
{

}

AMAction3* SGMSampleChamberVacuum::createMoveAction(double setpoint)
{
	AMAction3 *action = 0;

	// Create action info based on given setpoint.

	SGMSampleChamberVacuumMoveActionInfo *actionInfo = new SGMSampleChamberVacuumMoveActionInfo(AMNumber(setpoint));

	// Resolve the current value and the vacuum setpoint to the needed action.

	double currentValue = value();

	if (setpoint == SGMSampleChamberVacuum::Vented)
		action = new SGMSampleChamberVacuumMoveToVentedAction(actionInfo);
	else if (setpoint == SGMSampleChamberVacuum::RoughVacuum && currentValue == SGMSampleChamberVacuum::Vented)
		action = new SGMSampleChamberVacuumMoveToRoughVacuumFromVentedAction(actionInfo);
	else if (setpoint == SGMSampleChamberVacuum::HighVacuum && currentValue == SGMSampleChamberVacuum::Vented)
		action = new SGMSampleChamberVacuumMoveToHighVacuumFromVentedAction(actionInfo);
	else if (setpoint == SGMSampleChamberVacuum::HighVacuum && currentValue == SGMSampleChamberVacuum::RoughVacuum)
		action = new SGMSampleChamberVacuumMoveToHighVacuumFromRoughVacuumAction(actionInfo);
	else if (setpoint == SGMSampleChamberVacuum::RoughVacuum && currentValue == SGMSampleChamberVacuum::HighVacuum)
		action = new SGMSampleChamberVacuumMoveToRoughVacuumFromHighVacuumAction(actionInfo);

	return action;
}

//QWidget* SGMSampleChamberVacuum::createMoveActionView(AMAction3 *moveAction)
//{
//	QWidget *result = 0;

//	SGMSampleChamberVacuumMoveAction *vacuumMove = qobject_cast<SGMSampleChamberVacuumMoveAction*>(moveAction);

//	if (vacuumMove)
//		result = new SGMSampleChamberVacuumMoveActionDialog(vacuumMove);

//	return result;
//}
