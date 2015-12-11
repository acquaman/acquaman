#include "BioXASSSRLMonochromator.h"
#include <QDebug>

BioXASSSRLMonochromator::BioXASSSRLMonochromator(const QString &name, QObject *parent) :
	BioXASMonochromator(name, parent)
{
	// Initialize local variables.

	upperSlit_ = 0;
	lowerSlit_ = 0;
	slitsStatus_ = 0;
	paddle_ = 0;
	paddleStatus_ = 0;
	keyStatus_ = 0;
	brakeStatus_ = 0;
	braggAtCrystalChangePositionStatus_ = 0;
	crystalChange_ = 0;
	regionAStatus_ = 0;
	regionBStatus_ = 0;
	vertical_ = 0;
	lateral_ = 0;
	crystalChange_ = 0;
	crystal1Pitch_ = 0;
	crystal1Roll_ = 0;
	crystal2Pitch_ = 0;
	crystal2Roll_ = 0;

	m1Pitch_ = 0;

	encoderBragg_ = 0;
	stepBragg_ = 0;

	stepEnergy_ = 0;
	encoderEnergy_ = 0;

	region_ = 0;

	settlingTime_ = 0.01;
}

BioXASSSRLMonochromator::~BioXASSSRLMonochromator()
{

}

bool BioXASSSRLMonochromator::isConnected() const
{
	bool connected = (

				upperSlit_ && upperSlit_->isConnected() &&
				lowerSlit_ && lowerSlit_->isConnected() &&
				slitsStatus_ && slitsStatus_->isConnected() &&
				paddle_ && paddle_->isConnected() &&
				paddleStatus_ && paddleStatus_->isConnected() &&
				keyStatus_ && keyStatus_->isConnected() &&
				brakeStatus_ && brakeStatus_->isConnected() &&
				braggAtCrystalChangePositionStatus_ && braggAtCrystalChangePositionStatus_->isConnected() &&
				crystalChange_ && crystalChange_->isConnected() &&
				regionAStatus_ && regionAStatus_->isConnected() &&
				regionBStatus_ && regionBStatus_->isConnected() &&
				vertical_ && vertical_->isConnected() &&
				lateral_ && lateral_->isConnected() &&
				crystalChange_ && crystalChange_->isConnected() &&
				crystal1Pitch_ && crystal1Pitch_->isConnected() &&
				crystal1Roll_ && crystal1Roll_->isConnected() &&
				crystal2Pitch_ && crystal2Pitch_->isConnected() &&
				crystal2Roll_ && crystal2Roll_->isConnected() &&

				m1Pitch_ && m1Pitch_->isConnected() &&

				stepBragg_ && stepBragg_->isConnected() &&
				encoderBragg_ && encoderBragg_->isConnected() &&

				stepEnergy_ && stepEnergy_->isConnected() &&
				encoderEnergy_ && encoderEnergy_->isConnected() &&

				region_ && region_->isConnected()

				);

	return connected;
}

void BioXASSSRLMonochromator::setUpperSlit(CLSMAXvMotor *newControl)
{
	if (upperSlit_ != newControl) {

		if (upperSlit_)
			removeChildControl(upperSlit_);

		upperSlit_ = newControl;

		if (upperSlit_)
			addChildControl(upperSlit_);

		updateRegion();

		emit upperSlitChanged(upperSlit_);
	}
}

void BioXASSSRLMonochromator::setLowerSlit(CLSMAXvMotor *newControl)
{
	if (lowerSlit_ != newControl) {

		if (lowerSlit_)
			removeChildControl(lowerSlit_);

		lowerSlit_ = newControl;

		if (lowerSlit_)
			addChildControl(lowerSlit_);

		updateRegion();

		emit lowerSlitChanged(lowerSlit_);
	}
}

void BioXASSSRLMonochromator::setSlitsStatus(AMControl *newControl)
{
	if (slitsStatus_ != newControl) {

		if (slitsStatus_)
			removeChildControl(slitsStatus_);

		slitsStatus_ = newControl;

		if (slitsStatus_)
			addChildControl(slitsStatus_);

		updateRegion();

		emit slitsStatusChanged(slitsStatus_);
	}
}

void BioXASSSRLMonochromator::setPaddle(CLSMAXvMotor *newControl)
{
	if (paddle_ != newControl) {

		if (paddle_)
			removeChildControl(paddle_);

		paddle_ = newControl;

		if (paddle_)
			addChildControl(paddle_);

		updateRegion();

		emit paddleChanged(paddle_);
	}
}

void BioXASSSRLMonochromator::setPaddleStatus(AMControl *newControl)
{
	if (paddleStatus_ != newControl) {

		if (paddleStatus_)
			removeChildControl(paddleStatus_);

		paddleStatus_ = newControl;

		if (paddleStatus_)
			addChildControl(paddleStatus_);

		updateRegion();

		emit paddleStatusChanged(paddleStatus_);
	}
}

void BioXASSSRLMonochromator::setKeyStatus(AMControl *newControl)
{
	if (keyStatus_ != newControl) {

		if (keyStatus_)
			removeChildControl(keyStatus_);

		keyStatus_ = newControl;

		if (keyStatus_)
			addChildControl(keyStatus_);

		updateRegion();

		emit keyStatusChanged(keyStatus_);
	}
}

void BioXASSSRLMonochromator::setBrakeStatus(AMControl *newControl)
{
	if (brakeStatus_ != newControl) {

		if (brakeStatus_)
			removeChildControl(brakeStatus_);

		brakeStatus_ = newControl;

		if (brakeStatus_)
			addChildControl(brakeStatus_);

		updateRegion();

		emit brakeStatusChanged(brakeStatus_);
	}
}

void BioXASSSRLMonochromator::setBraggAtCrystalChangePositionStatus(AMControl *newControl)
{
	if (braggAtCrystalChangePositionStatus_ != newControl) {

		if (braggAtCrystalChangePositionStatus_)
			removeChildControl(braggAtCrystalChangePositionStatus_);

		braggAtCrystalChangePositionStatus_ = newControl;

		if (braggAtCrystalChangePositionStatus_)
			addChildControl(braggAtCrystalChangePositionStatus_);

		updateRegion();

		emit braggAtCrystalChangePositionStatusChanged(braggAtCrystalChangePositionStatus_);
	}
}

void BioXASSSRLMonochromator::setCrystalChange(CLSMAXvMotor *newControl)
{
	if (crystalChange_ != newControl) {

		if (crystalChange_)
			removeChildControl(crystalChange_);

		crystalChange_ = newControl;

		if (crystalChange_)
			addChildControl(crystalChange_);

		updateRegion();

		emit crystalChangeChanged(crystalChange_);
	}
}

void BioXASSSRLMonochromator::setRegionAStatus(AMControl *newControl)
{
	if (regionAStatus_ != newControl) {

		if (regionAStatus_)
			removeChildControl(regionAStatus_);

		regionAStatus_ = newControl;

		if (regionAStatus_)
			addChildControl(regionAStatus_);

		updateRegion();

		emit regionAStatusChanged(regionAStatus_);
	}
}

void BioXASSSRLMonochromator::setRegionBStatus(AMControl *newControl)
{
	if (regionBStatus_ != newControl) {

		if (regionBStatus_)
			removeChildControl(regionBStatus_);

		regionBStatus_ = newControl;

		if (regionBStatus_)
			addChildControl(regionBStatus_);

		updateRegion();

		emit regionBStatusChanged(regionBStatus_);
	}
}

void BioXASSSRLMonochromator::setVertical(CLSMAXvMotor *newControl)
{
	if (regionBStatus_ != newControl) {

		if (regionBStatus_)
			removeChildControl(regionBStatus_);

		regionBStatus_ = newControl;

		if (regionBStatus_)
			addChildControl(regionBStatus_);

		emit regionBStatusChanged(regionBStatus_);
	}
}

void BioXASSSRLMonochromator::setLateral(CLSMAXvMotor *newControl)
{
	if (lateral_ != newControl) {

		if (lateral_)
			removeChildControl(lateral_);

		lateral_ = newControl;

		if (lateral_)
			addChildControl(lateral_);

		emit lateralChanged(lateral_);
	}
}

void BioXASSSRLMonochromator::setCrystal1Pitch(CLSMAXvMotor *newControl)
{
	if (crystal1Pitch_ != newControl) {

		if (crystal1Pitch_)
			removeChildControl(crystal1Pitch_);

		crystal1Pitch_ = newControl;

		if (crystal1Pitch_)
			addChildControl(crystal1Pitch_);

		emit crystal1PitchChanged(crystal1Pitch_);
	}
}

void BioXASSSRLMonochromator::setCrystal1Roll(CLSMAXvMotor *newControl)
{
	if (crystal1Roll_ != newControl) {

		if (crystal1Roll_)
			removeChildControl(crystal1Roll_);

		crystal1Roll_ = newControl;

		if (crystal1Roll_)
			addChildControl(crystal1Roll_);

		emit crystal1RollChanged(crystal1Roll_);
	}
}

void BioXASSSRLMonochromator::setCrystal2Pitch(CLSMAXvMotor *newControl)
{
	if (crystal2Pitch_ != newControl) {

		if (crystal2Pitch_)
			removeChildControl(crystal2Pitch_);

		crystal2Pitch_ = newControl;

		if (crystal2Pitch_)
			addChildControl(crystal2Pitch_);

		emit crystal2PitchChanged(crystal2Pitch_);
	}
}

void BioXASSSRLMonochromator::setCrystal2Roll(CLSMAXvMotor *newControl)
{
	if (crystal2Roll_ != newControl) {

		if (crystal2Roll_)
			removeChildControl(crystal2Roll_);

		crystal2Roll_ = newControl;

		if (crystal2Roll_)
			addChildControl(crystal2Roll_);

		emit crystal2RollChanged(crystal2Roll_);
	}
}

void BioXASSSRLMonochromator::setM1MirrorPitchControl(AMControl *newControl)
{
	if (m1Pitch_ != newControl) {

		if (m1Pitch_)
			removeChildControl(m1Pitch_);

		m1Pitch_ = newControl;

		if (m1Pitch_)
			addChildControl(m1Pitch_);

		updateStepEnergy();
		updateEncoderEnergy();

		emit m1MirrorPitchControlChanged(m1Pitch_);
	}
}

void BioXASSSRLMonochromator::setStepBragg(CLSMAXvMotor *newControl)
{
	if (stepBragg_ != newControl) {

		if (stepBragg_)
			removeChildControl(stepBragg_);

		stepBragg_ = newControl;

		if (stepBragg_)
			addChildControl(stepBragg_);

		updateRegion();

		emit stepBraggChanged(stepBragg_);
	}
}

void BioXASSSRLMonochromator::setEncoderBragg(CLSMAXvMotor *newControl)
{
	if (encoderBragg_ != newControl) {

		if (encoderBragg_)
			removeChildControl(encoderBragg_);

		encoderBragg_ = newControl;

		if (encoderBragg_)
			addChildControl(encoderBragg_);

		updateRegion();

		emit encoderBraggChanged(encoderBragg_);
	}
}

void BioXASSSRLMonochromator::setStepEnergy(BioXASSSRLMonochromatorEnergyControl *newControl)
{
	if (stepEnergy_ != newControl) {

		if (stepEnergy_)
			removeChildControl(stepEnergy_);

		stepEnergy_ = newControl;

		if (stepEnergy_)
			addChildControl(stepEnergy_);

		emit stepEnergyChanged(stepEnergy_);
	}
}

void BioXASSSRLMonochromator::setEncoderEnergy(BioXASSSRLMonochromatorEnergyControl *newControl)
{
	if (encoderEnergy_ != newControl) {

		if (encoderEnergy_)
			removeChildControl(encoderEnergy_);

		encoderEnergy_ = newControl;

		if (encoderEnergy_)
			addChildControl(encoderEnergy_);

		emit encoderEnergyChanged(encoderEnergy_);
	}
}

void BioXASSSRLMonochromator::setRegion(BioXASSSRLMonochromatorRegionControl *newControl)
{
	if (region_ != newControl) {

		if (region_)
			removeChildControl(region_);

		region_ = newControl;

		if (region_)
			addChildControl(region_);

		updateRegion();

		emit regionChanged(region_);
	}
}

void BioXASSSRLMonochromator::setSettlingTime(double newTimeSeconds)
{
	if (settlingTime_ != newTimeSeconds) {
		settlingTime_ = newTimeSeconds;

		updateMotorSettlingTime();

		emit settlingTimeChanged(settlingTime_);
	}
}

void BioXASSSRLMonochromator::updateStepEnergy()
{
	if (stepEnergy_)
		stepEnergy_->setM1MirrorPitchControl(m1Pitch_);
}

void BioXASSSRLMonochromator::updateEncoderEnergy()
{
	if (encoderEnergy_)
		encoderEnergy_->setM1MirrorPitchControl(m1Pitch_);
}

void BioXASSSRLMonochromator::updateRegion()
{
	if (region_) {
		region_->setUpperSlitControl(upperSlit_);
		region_->setLowerSlitControl(lowerSlit_);
		region_->setSlitsStatusControl(slitsStatus_);
		region_->setPaddleControl(paddle_);
		region_->setPaddleStatusControl(paddleStatus_);
		region_->setKeyStatusControl(keyStatus_);
		region_->setBrakeStatusControl(brakeStatus_);
		region_->setBraggControl(bragg());
		region_->setBraggAtCrystalChangePositionStatusControl(braggAtCrystalChangePositionStatus_);
		region_->setCrystalChangeControl(crystalChange_);
		region_->setRegionAStatusControl(regionAStatus_);
		region_->setRegionBStatusControl(regionBStatus_);
	}
}

void BioXASSSRLMonochromator::updateMotorSettlingTime()
{
	if (stepBragg_)
		stepBragg_->setSettlingTime(settlingTime_);

	if (encoderBragg_)
		encoderBragg_->setSettlingTime(settlingTime_);
}
