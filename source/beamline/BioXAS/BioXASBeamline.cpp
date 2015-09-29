#include "BioXASBeamline.h"

#include "actions3/AMActionSupport.h"
#include "beamline/CLS/CLSStorageRing.h"
#include "util/AMErrorMonitor.h"

BioXASBeamline::~BioXASBeamline()
{

}

bool BioXASBeamline::beamOff() const
{
	bool result = false;

	if (isConnected())
		result = (photonShutterFEUpstream_->isClosed() && photonShutterFEDownstream_->isClosed() && safetyShutterFE_->isClosed());

	return result;
}

bool BioXASBeamline::beamOn() const
{
	bool result = false;

	if (isConnected())
		result = (photonShutterFEUpstream_->isOpen() && photonShutterFEDownstream_->isOpen() && safetyShutterFE_->isOpen());

	return result;
}

AMBasicControlDetectorEmulator* BioXASBeamline::detectorForControl(AMControl *control) const
{
	return controlDetectorMap_.value(control, 0);
}

void BioXASBeamline::setConnected(bool isConnected)
{
	if (connected_ != isConnected) {
		connected_ = isConnected;
		emit connectedChanged(connected_);
	}
}

void BioXASBeamline::updateConnected()
{
	setConnected( isConnected() );
}

void BioXASBeamline::setupComponents()
{

}

AMBasicControlDetectorEmulator* BioXASBeamline::createDetectorEmulator(const QString &name, const QString &description, AMControl *control, bool hiddenFromUsers, bool isVisible)
{
	AMBasicControlDetectorEmulator *detector = 0;

	if (control) {
		detector = new AMBasicControlDetectorEmulator(name, description, control, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
		detector->setHiddenFromUsers(hiddenFromUsers);
		detector->setIsVisible(isVisible);
	}

	return detector;
}

void BioXASBeamline::addControlAsDetector(const QString &name, const QString &description, AMControl *control, bool hiddenFromUsers, bool isVisible)
{
	if (control && !controlDetectorMap_.contains(control)) {
		AMBasicControlDetectorEmulator *detector = createDetectorEmulator(name, description, control, hiddenFromUsers, isVisible);
		controlDetectorMap_.insert(control, detector);
	}
}

BioXASBeamline::BioXASBeamline(const QString &controlName) :
	CLSBeamline(controlName)
{
	// Initialize member variables.

	connected_ = false;

	photonShutterFEUpstream_ = 0;
	photonShutterFEDownstream_ = 0;
	safetyShutterFE_ = 0;

	// Setup procedures.

	setupComponents();
}
