#include "BioXASSideBeamStatusControl.h"
#include "actions3/AMActionSupport.h"

BioXASSideBeamStatusControl::BioXASSideBeamStatusControl(const QString &name, QObject *parent) :
	BioXASBeamStatusControl(name, parent)
{
	// Initialize inherited variables.

	setContextKnownDescription("SideBeamStatus");

	// Initialize local variables.

	frontEndStatus_ = 0;
	poeBeamStatus_ = 0;
	soeBeamStatus_ = 0;

	// Current settings.

	updateStates();
}

BioXASSideBeamStatusControl::~BioXASSideBeamStatusControl()
{

}

bool BioXASSideBeamStatusControl::shouldStop() const
{
	bool result = false;

	if (soeBeamStatus_)
		result = soeBeamStatus_->shouldStop();

	return result;
}

bool BioXASSideBeamStatusControl::canMeasure() const
{
	bool result = false;

	if (soeBeamStatus_)
		result = soeBeamStatus_->canMeasure();

	return result;
}

bool BioXASSideBeamStatusControl::canMove() const
{
	bool result = false;

	if (soeBeamStatus_)
		result = soeBeamStatus_->canMove();

	return result;
}

bool BioXASSideBeamStatusControl::canStop() const
{
	bool result = false;

	if (soeBeamStatus_)
		result = soeBeamStatus_->canStop();

	return result;
}

bool BioXASSideBeamStatusControl::isOn() const
{
	bool result = false;

	if (soeBeamStatus_)
		result = soeBeamStatus_->isOn();

	return result;
}

bool BioXASSideBeamStatusControl::isOff() const
{
	bool result = false;

	if (soeBeamStatus_)
		result = soeBeamStatus_->isOff();

	return result;
}

void BioXASSideBeamStatusControl::setFrontEndBeamStatus(BioXASFrontEndBeamStatusControl *newControl)
{
	if (frontEndStatus_ != newControl) {

		if (poeBeamStatus_)
			poeBeamStatus_->setFrontEndBeamStatus(0);

		frontEndStatus_ = newControl;

		if (poeBeamStatus_)
			poeBeamStatus_->setFrontEndBeamStatus(frontEndStatus_);

		updateStates();

		emit frontEndBeamStatusChanged(frontEndStatus_);
	}
}

void BioXASSideBeamStatusControl::setPOEBeamStatus(BioXASSidePOEBeamStatusControl *newControl)
{
	if (poeBeamStatus_ != newControl) {

		if (poeBeamStatus_)
			disconnect( poeBeamStatus_, 0, this, 0 );

		if (soeBeamStatus_)
			soeBeamStatus_->setPOEBeamStatus(0);

		poeBeamStatus_ = newControl;

		if (poeBeamStatus_) {
			poeBeamStatus_->setFrontEndBeamStatus(frontEndStatus_);

			connect( poeBeamStatus_, SIGNAL(frontEndBeamStatusChanged(BioXASFrontEndBeamStatusControl*)), this, SLOT(setFrontEndBeamStatus(BioXASFrontEndBeamStatusControl*)) );
		}

		if (soeBeamStatus_)
			soeBeamStatus_->setPOEBeamStatus(poeBeamStatus_);

		updateStates();

		emit poeBeamStatusChanged(poeBeamStatus_);
	}
}

void BioXASSideBeamStatusControl::setSOEBeamStatus(BioXASSideSOEBeamStatusControl *newControl)
{
	if (soeBeamStatus_ != newControl) {

		if (soeBeamStatus_)
			disconnect( soeBeamStatus_, 0, this, 0 );

		soeBeamStatus_ = newControl;

		if (soeBeamStatus_) {
			soeBeamStatus_->setPOEBeamStatus(poeBeamStatus_);

			connect( soeBeamStatus_, SIGNAL(poeBeamStatusChanged(BioXASSidePOEBeamStatusControl*)), this, SLOT(setPOEBeamStatus(BioXASSidePOEBeamStatusControl*)) );
			connect( soeBeamStatus_, SIGNAL(connected(bool)), this, SIGNAL(connected(bool)) );
			connect( soeBeamStatus_, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged(double)) );
		}

		updateStates();

		emit soeBeamStatusChanged(soeBeamStatus_);
	}
}

void BioXASSideBeamStatusControl::updateConnected()
{
	bool connected = false;

	if (soeBeamStatus_)
		connected = soeBeamStatus_->isConnected();

	setConnected(connected);
}

void BioXASSideBeamStatusControl::updateMoving()
{
	bool moving = false;

	if (soeBeamStatus_)
		moving = soeBeamStatus_->isMoving();

	setIsMoving(moving);
}

AMAction3* BioXASSideBeamStatusControl::createBeamOnAction()
{
	AMAction3 *action = AMActionSupport::buildControlMoveAction(soeBeamStatus_, BioXAS::Beam::On);
	return action;
}

AMAction3* BioXASSideBeamStatusControl::createBeamOffAction()
{
	AMAction3 *action = AMActionSupport::buildControlMoveAction(soeBeamStatus_, BioXAS::Beam::Off);
	return action;
}
