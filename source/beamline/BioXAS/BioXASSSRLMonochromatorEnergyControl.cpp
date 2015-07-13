#include "BioXASSSRLMonochromatorEnergyControl.h"
#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include <math.h>

#include <QDebug>

BioXASSSRLMonochromatorEnergyControl::BioXASSSRLMonochromatorEnergyControl(const QString &name, QObject *parent) :
	BioXASMonochromatorEnergyControl(name, parent)
{
	// Initialize inherited variables.

	value_ = 0;
	setpoint_ = 0;
	minimumValue_ = -1000000;
	maximumValue_ = 1000000;

	setAllowsMovesWhileMoving(false);
	setContextKnownDescription("Energy");
	setTolerance(0.05);

	// Initialize member variables.

	hc_ = 12398.42;
	crystal2D_ = 3.8403117;
	thetaBraggOffset_ = 180.0;
	regionOffset_ = 180;

	bragg_ = 0;
	braggSetPosition_ = 0;
	region_ = 0;
	m1MirrorPitch_ = 0;

	// Current settings.

	updateStates();
}

BioXASSSRLMonochromatorEnergyControl::~BioXASSSRLMonochromatorEnergyControl()
{

}

bool BioXASSSRLMonochromatorEnergyControl::canMeasure() const
{
	bool result = (
				bragg_ && bragg_->canMeasure() &&
				region_ && region_->canMeasure() &&
				m1MirrorPitch_ && m1MirrorPitch_->canMeasure()
				);

	return result;
}

bool BioXASSSRLMonochromatorEnergyControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = ( bragg_->canMove() );

	return result;
}

bool BioXASSSRLMonochromatorEnergyControl::canStop() const
{
	bool result = false;

	if (isConnected())
		result = ( bragg_->canStop() );

	return false;
}

AMAction3* BioXASSSRLMonochromatorEnergyControl::createSetEnergyAction(double newEnergy)
{
	AMAction3 *result = 0;

	if (bragg_ && bragg_->isConnected()) {
		result = createSetEnergyAction(bragg_->value(), newEnergy);
	}

	return result;
}

AMAction3* BioXASSSRLMonochromatorEnergyControl::createSetEnergyAction(double braggPosition, double newEnergy)
{
	AMAction3 *result = 0;

	if (bragg_ && braggSetPosition_) {
		AMListAction3 *setEnergy = new AMListAction3(new AMListActionInfo3("Set Monochromator Energy", "Set Monochromator Energy"), AMListAction3::Sequential);
		setEnergy->addSubAction(AMActionSupport::buildControlMoveAction(bragg_, braggPosition));
		setEnergy->addSubAction(AMActionSupport::buildControlMoveAction(braggSetPosition_, calculateBraggPositionFromEnergy(hc_, crystal2D_, newEnergy, region_->value(), m1MirrorPitch_->value(), thetaBraggOffset_, regionOffset_)));

		result = setEnergy;
	}

	return result;
}

void BioXASSSRLMonochromatorEnergyControl::setBraggControl(AMControl *newControl)
{
	if (bragg_ != newControl) {

		if (bragg_)
			removeChildControl(bragg_);

		bragg_ = newControl;

		if (bragg_)
			addChildControl(bragg_);

		updateStates();

		emit braggControlChanged(bragg_);
	}
}

void BioXASSSRLMonochromatorEnergyControl::setBraggSetPositionControl(AMControl *newControl)
{
	if (braggSetPosition_ != newControl) {

		if (braggSetPosition_)
			removeChildControl(braggSetPosition_);

		braggSetPosition_ = newControl;

		if (braggSetPosition_)
			addChildControl(braggSetPosition_);

		updateStates();

		emit braggSetPositionControlChanged(braggSetPosition_);
	}
}

void BioXASSSRLMonochromatorEnergyControl::setRegionControl(AMControl *newControl)
{
	if (region_ != newControl) {

		if (region_)
			removeChildControl(region_);

		region_ = newControl;

		if (region_)
			addChildControl(region_);

		updateStates();

		emit regionControlChanged(region_);
	}
}

void BioXASSSRLMonochromatorEnergyControl::setM1MirrorPitchControl(AMControl *newControl)
{
	if (m1MirrorPitch_ != newControl) {

		if (m1MirrorPitch_)
			removeChildControl(m1MirrorPitch_);

		m1MirrorPitch_ = newControl;

		if (m1MirrorPitch_)
			addChildControl(m1MirrorPitch_);

		updateStates();

		emit m1MirrorControlChanged(m1MirrorPitch_);
	}
}

void BioXASSSRLMonochromatorEnergyControl::setEnergy(double newEnergy)
{
	if (isConnected())
		setEnergy(bragg_->value(), newEnergy);
}

void BioXASSSRLMonochromatorEnergyControl::setEnergy(double braggPosition, double newEnergy)
{
	if (isConnected()) {

		// Convert the desired energy into a bragg position.
		double newPosition = calculateBraggPositionFromEnergy(hc_, crystal2D_, newEnergy, region_->value(), m1MirrorPitch_->value(), thetaBraggOffset_, regionOffset_);

		// Move the bragg motor to the given position, set the bragg position as the desired 'energy' position.
		bragg_->move(braggPosition);
		braggSetPosition_->move(newPosition);

		// Update the control value.
		updateValue();
	}
}

void BioXASSSRLMonochromatorEnergyControl::updateConnected()
{
	bool isConnected = (
				bragg_ && bragg_->isConnected() &&
				braggSetPosition_ && braggSetPosition_->isConnected() &&
				region_ && region_->isConnected() &&
				m1MirrorPitch_ && m1MirrorPitch_->isConnected()
				);

	if (isConnected)
		qDebug() << "\nEnergy control is connected.\n";
	else
		qDebug() << "\nEnergy control is NOT connected.\n";

	setConnected(isConnected);
}

void BioXASSSRLMonochromatorEnergyControl::updateValue()
{
	if (isConnected()) {
		setValue( calculateEnergyFromBraggPosition(hc_, crystal2D_, bragg_->value(), region_->value(), m1MirrorPitch_->value(), thetaBraggOffset_, regionOffset_) );
	}
}

void BioXASSSRLMonochromatorEnergyControl::updateMoving()
{
	if (isConnected()) {
		setIsMoving( bragg_->isMoving() );
	}
}

AMAction3* BioXASSSRLMonochromatorEnergyControl::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if (isConnected()) {

		// Calculate the bragg motor position corresponding to the given energy setpoint.
		double newPosition = calculateBraggPositionFromEnergy(hc_, crystal2D_, setpoint, region_->value(), m1MirrorPitch_->value(), thetaBraggOffset_, regionOffset_);

		// Create move action.
		result = AMActionSupport::buildControlMoveAction(bragg_, newPosition);
	}

	return result;
}

double BioXASSSRLMonochromatorEnergyControl::calculateBraggAngleFromPositionRegionA(double braggPosition, double m1Pitch, double thetaBraggOffset, double regionOffset)
{
	double braggAngle = calculateBraggAngleFromPositionRegionB(braggPosition, m1Pitch, thetaBraggOffset) - regionOffset;
	return braggAngle;
}

double BioXASSSRLMonochromatorEnergyControl::calculateBraggAngleFromPositionRegionB(double braggPosition, double m1Pitch, double thetaBraggOffset)
{
	double braggAngle = thetaBraggOffset - braggPosition - (2*m1Pitch);
	return braggAngle;
}

double BioXASSSRLMonochromatorEnergyControl::calculateBraggPositionFromAngleRegionA(double braggAngle, double m1Pitch, double thetaBraggOffset, double regionOffset)
{
	double braggPosition = calculateBraggPositionFromAngleRegionB(braggAngle, m1Pitch, thetaBraggOffset) - regionOffset;
	return braggPosition;
}

double BioXASSSRLMonochromatorEnergyControl::calculateBraggPositionFromAngleRegionB(double braggAngle, double m1Pitch, double thetaBraggOffset)
{
	double braggPosition = thetaBraggOffset - (2*m1Pitch) - braggAngle;
	return braggPosition;
}

double BioXASSSRLMonochromatorEnergyControl::calculateBraggAngleFromEnergy(double hc, double crystal2D, double energy)
{
	double braggAngle = asin( hc/(energy * crystal2D) ) * 180/M_PI;
	return braggAngle;
}

double BioXASSSRLMonochromatorEnergyControl::calculateEnergyFromBraggAngle(double hc, double crystal2D, double braggAngle)
{
	double energy = hc / ( crystal2D * sin(braggAngle * M_PI/180) );
	return energy;
}

double BioXASSSRLMonochromatorEnergyControl::calculateBraggPositionFromEnergy(double hc, double crystal2D, double energy, double region, double m1Pitch, double thetaBraggOffset, double regionOffset)
{
	double result = 0;
	double braggPosition = 0;
	double braggAngle = 0;
	bool validRegion = false;

	// Find the bragg angle corresponding to the given energy and region.

	braggAngle = calculateBraggAngleFromEnergy(hc, crystal2D, energy);

	// If the given region is valid, find the bragg position.

	switch ((int)region) {
	case BioXASSSRLMonochromator::Region::A:
		braggPosition = calculateBraggPositionFromAngleRegionA(braggAngle, m1Pitch, thetaBraggOffset, regionOffset);
		validRegion = true;
		break;
	case BioXASSSRLMonochromator::Region::B:
		braggPosition = calculateBraggPositionFromAngleRegionB(braggAngle, m1Pitch, thetaBraggOffset);
		validRegion = true;
		break;
	default:
		break;
	}

	if (validRegion)
		result = braggPosition;

	return result;
}

double BioXASSSRLMonochromatorEnergyControl::calculateEnergyFromBraggPosition(double hc, double crystal2D, double braggPosition, double region, double m1Pitch, double thetaBraggOffset, double regionOffset)
{
	double result = 0;
	double braggAngle = 0;
	bool validRegion = false;

	// Find the bragg angle corresponding to the given bragg position and region.

	switch ((int)region) {
	case BioXASSSRLMonochromator::Region::A:
		braggAngle = calculateBraggAngleFromPositionRegionA(braggPosition, m1Pitch, thetaBraggOffset, regionOffset);
		validRegion = true;
		break;
	case BioXASSSRLMonochromator::Region::B:
		braggAngle = calculateBraggAngleFromPositionRegionB(braggPosition, m1Pitch, thetaBraggOffset);
		validRegion = true;
		break;
	default:
		break;
	}

	// If the given region is valid, find the energy.

	if (validRegion)
		result = calculateEnergyFromBraggAngle(hc, crystal2D, braggAngle);

	return result;
}
