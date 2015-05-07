#include "BioXASSSRLMonochromator.h"

BioXASSSRLMonochromator::BioXASSSRLMonochromator(QObject *parent) :
	BioXASMonochromator(parent)
{
	// Initialize local variables.

	connected_ = false;

	energy_ = 0;
	region_ = 0;

	upperSlit_ = 0;
	lowerSlit_ = 0;
	slitsStatus_ = 0;
	paddle_ = 0;
	paddleStatus_ = 0;
	keyStatus_ = 0;
	brakeStatus_ = 0;
	bragg_ = 0;
	braggAtCrystalChangePositionStatus_ = 0;
	crystalChange_ = 0;
	crystalChangeCWLimitStatus_ = 0;
	crystalChangeCCWLimitStatus_ = 0;
	regionAStatus_ = 0;
	regionBStatus_ = 0;

	upperSlitMotor_ = 0;
	lowerSlitMotor_ = 0;
	paddleMotor_ = 0;
	braggMotor_ = 0;
	verticalMotor_ = 0;
	lateralMotor_ = 0;
	crystalChangeMotor_ = 0;
	crystal1PitchMotor_ = 0;
	crystal1RollMotor_ = 0;
	crystal2PitchMotor_ = 0;
	crystal2RollMotor_ = 0;
}

BioXASSSRLMonochromator::~BioXASSSRLMonochromator()
{

}

void BioXASSSRLMonochromator::calibrateBraggPosition(double newBraggPosition)
{
	if (braggMotor_ && braggMotor_->isConnected()) {
		braggMotor_->setEGUSetPosition(newBraggPosition);
	}
}

void BioXASSSRLMonochromator::setConnected(bool isConnected)
{
	if (connected_ != isConnected) {
		connected_ = isConnected;
		emit connectedChanged(connected_);
	}
}

void BioXASSSRLMonochromator::updateConnected()
{
	bool exists = false;
	bool connected = false;

	exists = (
				energy_ &&
				region_ &&

				upperSlit_ &&
				lowerSlit_ &&
				slitsStatus_ &&
				paddle_ &&
				paddleStatus_ &&
				keyStatus_ &&
				brakeStatus_ &&
				bragg_ &&
				braggAtCrystalChangePositionStatus_ &&
				crystalChange_ &&
				crystalChangeCWLimitStatus_ &&
				crystalChangeCCWLimitStatus_ &&
				regionAStatus_ &&
				regionBStatus_ &&

				upperSlitMotor_ &&
				lowerSlitMotor_ &&
				paddleMotor_ &&
				braggMotor_ &&
				verticalMotor_ &&
				lateralMotor_ &&
				crystalChangeMotor_ &&
				crystal1PitchMotor_ &&
				crystal1RollMotor_ &&
				crystal2PitchMotor_ &&
				crystal2RollMotor_
				);

	if (exists) {
		connected = (
				energy_->isConnected() &&
				region_->isConnected() &&

				upperSlit_->isConnected() &&
				lowerSlit_->isConnected() &&
				slitsStatus_->isConnected() &&
				paddle_->isConnected() &&
				paddleStatus_->isConnected() &&
				keyStatus_->isConnected() &&
				brakeStatus_->isConnected() &&
				bragg_->isConnected() &&
				braggAtCrystalChangePositionStatus_->isConnected() &&
				crystalChange_->isConnected() &&
				crystalChangeCWLimitStatus_->isConnected() &&
				crystalChangeCCWLimitStatus_->isConnected() &&
				regionAStatus_->isConnected() &&
				regionBStatus_->isConnected() &&

				upperSlitMotor_->isConnected() &&
				lowerSlitMotor_->isConnected() &&
				paddleMotor_->isConnected() &&
				braggMotor_->isConnected() &&
				verticalMotor_->isConnected() &&
				lateralMotor_->isConnected() &&
				crystalChangeMotor_->isConnected() &&
				crystal1PitchMotor_->isConnected() &&
				crystal1RollMotor_->isConnected() &&
				crystal2PitchMotor_->isConnected() &&
				crystal2RollMotor_ ->isConnected()
				);
	}

	setConnected(exists && connected);
}
