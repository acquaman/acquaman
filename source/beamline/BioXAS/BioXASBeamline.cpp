#include "BioXASBeamline.h"

#include "actions3/AMActionSupport.h"
#include "beamline/CLS/CLSStorageRing.h"
#include "util/AMErrorMonitor.h"

BioXASBeamline::~BioXASBeamline()
{

}

bool BioXASBeamline::isConnected() const
{
	bool connected = (
				frontEndShutters_ && frontEndShutters_->isConnected() &&
				frontEndValves_ && frontEndValves_->isConnected() &&
				sideValves_ && sideValves_->isConnected() &&
				mainValves_ && mainValves_->isConnected() &&
				imagingValves_ && imagingValves_->isConnected()
				);

	return connected;
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
	// Front end shutters.

	frontEndShutters_ = new BioXASFrontEndShutters(this);
	connect( frontEndShutters_, SIGNAL(connectedChanged(bool)), this, SLOT(updateConnected()) );

	// Valves.

	frontEndValves_ = new BioXASFrontEndValves(this);
	connect( frontEndValves_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	sideValves_ = new BioXASSideValves(this);
	connect( sideValves_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	mainValves_ = new BioXASMainValves(this);
	connect( mainValves_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	imagingValves_ = new BioXASImagingValves(this);
	connect( imagingValves_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Beam status.

	frontEndBeamStatus_ = new BioXASFrontEndBeamStatusControl("BioXASFrontEndBeamStatus", this);
	frontEndBeamStatus_->setShutters(frontEndShutters_);
//	frontEndBeamStatus_->setValves(valves_);
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

	frontEndShutters_ = 0;

	frontEndValves_ = 0;
	sideValves_ = 0;
	mainValves_ = 0;
	imagingValves_ = 0;

	// Setup procedures.

	setupComponents();
}
