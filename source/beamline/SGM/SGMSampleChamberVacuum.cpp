#include "SGMSampleChamberVacuum.h"
#include "beamline/AMPVControl.h"
#include "actions3/AMActionSupport.h"

SGMSampleChamberVacuum::SGMSampleChamberVacuum(const QString &name, QObject *parent) :
	AMSingleEnumeratedControl(name, "", parent)
{
	// Setup available vacuum states.

	addValueOption(Vented, "Vented", Vented, SGMSAMPLECHAMBERVACUUM_VENTED_MIN, SGMSAMPLECHAMBERVACUUM_VENTED_MAX);
	addValueOption(RoughVacuum, "Rough vacuum", RoughVacuum, SGMSAMPLECHAMBERVACUUM_ROUGHVACUUM_MIN, SGMSAMPLECHAMBERVACUUM_ROUGHVACUUM_MAX);
	addValueOption(HighVacuum, "High vacuum", HighVacuum, SGMSAMPLECHAMBERVACUUM_HIGHVACUUM_MIN, SGMSAMPLECHAMBERVACUUM_HIGHVACUUM_MAX);
}

SGMSampleChamberVacuum::~SGMSampleChamberVacuum()
{

}

void SGMSampleChamberVacuum::setPressure(AMControl *newControl)
{
	if (setBaseControl(newControl))
		emit pressureChanged(newControl);
}
