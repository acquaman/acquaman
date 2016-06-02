#include "BioXAS32ElementGeDetector.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "util/AMErrorMonitor.h"

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
	AMListAction3* eraseAndCheckAction = new AMListAction3(new AMListActionInfo3("Erase detector data and check SCAs", "Erase detector data and check SCAs"), AMListAction3::Sequential);

	// Erase detector data.

	eraseAndCheckAction->addSubAction(AMXspress3XRFDetector::createEraseAction());

	// Iterate through the SCA controls we care about, making sure they all get set to 0.

	AMListAction3 *checkSCAsAction = new AMListAction3(new AMListActionInfo3("Check detector SCAs", "Check detector SCAs"), AMListAction3::Parallel);

	for (int i = 0, count = elements(); i < count; i++) {
		if (isElementEnabled(i))
			checkSCAsAction->addSubAction(AMActionSupport::buildControlWaitAction(icrControlAt(i), 0.0));
	}

	eraseAndCheckAction->addSubAction(checkSCAsAction);

	return eraseAndCheckAction;
}
