#include "SGMSampleChamberVacuum.h"

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

AMAction3* SGMSampleChamberVacuum::createMoveAction(double indexSetpoint)
{
	Q_UNUSED(indexSetpoint)
	return 0;
}
