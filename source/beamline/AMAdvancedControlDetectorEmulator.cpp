#include "AMAdvancedControlDetectorEmulator.h"

AMAdvancedControlDetectorEmulator::AMAdvancedControlDetectorEmulator(const QString &name, const QString &description, AMControl *control, AMControl *statusControl, double statusAcquiringValue, double statusNotAcquiringValue, AMDetectorDefinitions::ReadMethod readMethod, const QString &synchronizedDwellKey, QObject *parent) :
	AMBasicControlDetectorEmulator(name, description, control, statusControl, statusAcquiringValue, statusNotAcquiringValue, readMethod, parent)
{
	setSynchronizedDwellKey(synchronizedDwellKey);
}
