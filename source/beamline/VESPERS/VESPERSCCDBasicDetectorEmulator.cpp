#include "VESPERSCCDBasicDetectorEmulator.h"

VESPERSCCDBasicDetectorEmulator::VESPERSCCDBasicDetectorEmulator(VESPERSCCDDetector *ccd, const QString &name, const QString &description, AMControl *control, AMControl *statusControl, double statusAcquiringValue, double statusNotAcquiringValue, AMDetectorDefinitions::ReadMethod readMethod, QObject *parent)
	: AMBasicControlDetectorEmulator(name, description, control, statusControl, statusAcquiringValue, statusNotAcquiringValue, readMethod, parent)
{
	ccd_ = ccd;
}
