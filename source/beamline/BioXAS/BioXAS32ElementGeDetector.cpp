#include "BioXAS32ElementGeDetector.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

BioXAS32ElementGeDetector::BioXAS32ElementGeDetector(const QString &name, const QString &description, AMPVControl *triggerControl, BioXASZebraPulseControl *pulseControl, QObject *parent)
	: AMXspress3XRFDetector(name, description, parent)
{
	// Stuff required by AMXRFDetector.

	pulseControl_ = pulseControl;
	acquireControl_ = triggerControl;
	acquireTimeControl_ = pulseControl_->pulseWidthSecondsControl();
	elapsedTimeControl_ = 0;
}

BioXAS32ElementGeDetector::~BioXAS32ElementGeDetector()
{

}

bool BioXAS32ElementGeDetector::canDisableElement(int index) const
{
	// All valid elements can technically be disabled.
	return (AMXspress3XRFDetector::canDisableElement(index) && index >= 0 && index < 32);
}

bool BioXAS32ElementGeDetector::setAcquisitionTime(double seconds)
{
	if(!isConnected())
		return false;

	pulseControl_->setPulseWidthValueSeconds(seconds);

	return true;
}

AMAction3* BioXAS32ElementGeDetector::createEraseAction()
{
	AMAction3 *result = 0;

	// Build the erase action.

	AMListAction3* eraseAction = new AMListAction3(new AMListActionInfo3("Erase detector data", "Erase detector data"), AMListAction3::Sequential);

	// Begin by manually toggling the erase control.

	eraseAction->addSubAction(AMActionSupport::buildControlMoveAction(eraseControl_, 1.0));
	eraseAction->addSubAction(AMActionSupport::buildControlMoveAction(eraseControl_, 0.0));

	// Iterate through the SCA controls we care about, making sure they all get set to 0.

	bool controlsValid = true;

	for (int i = 0, count = elements(); i < count && controlsValid; i++) {
		if (isElementEnabled(i)) {
			AMControl *icrControl = icrControlAt(i);

			if (icrControl)
				eraseAction->addSubAction(AMActionSupport::buildControlWaitAction(icrControl, 0.0));
			else
				controlsValid = false;
		}
	}

	// If there was no problem with the controls, then the result is the generated erase action.

	if (controlsValid)
		result = eraseAction;

	return result;
}
