#include "BioXASSSRLMonochromator.h"

BioXASSSRLMonochromator::BioXASSSRLMonochromator(const QString &name, QObject *parent) :
	BioXASMonochromator(name, parent)
{
	// Initialize local variables.

	mask_ = 0;
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
	crystal1Pitch_ = 0;
	crystal1Roll_ = 0;
	crystal2Pitch_ = 0;
	crystal2Roll_ = 0;

	m1Pitch_ = 0;

	stepBragg_ = 0;
	encoderBragg_ = 0;
	bragg_ = 0;

	stepEnergy_ = 0;
	encoderEnergy_ = 0;

	region_ = 0;

	settlingTime_ = 0;
	mode_ = Mode::None;

	// Current settings.

	setSettlingTime(0.1);
	setMode(Mode::Step);
}

BioXASSSRLMonochromator::~BioXASSSRLMonochromator()
{

}

bool BioXASSSRLMonochromator::isConnected() const
{
	bool connected = (

				BioXASMonochromator::isConnected() &&

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

				stepBragg_ && stepBragg_->isConnected() &&
				encoderBragg_ && encoderBragg_->isConnected() &&
				bragg_ && bragg_->isConnected() &&

				stepEnergy_ && stepEnergy_->isConnected() &&
				encoderEnergy_ && encoderEnergy_->isConnected() &&

				region_ && region_->isConnected() &&

				mask_ && mask_->isConnected() &&

				m1Pitch_ && m1Pitch_->isConnected()

				);

	return connected;
}

void BioXASSSRLMonochromator::setSettlingTime(double newTimeSeconds)
{
	if (settlingTime_ != newTimeSeconds) {
		settlingTime_ = newTimeSeconds;

		updateStepBragg();
		updateEncoderBragg();

		emit settlingTimeChanged(settlingTime_);
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

void BioXASSSRLMonochromator::setMode(Mode::Value newMode)
{
	if (mode_ != newMode) {
		mode_ = newMode;
		updateBragg();
		updateEnergy();

		emit modeChanged(mode_);
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
	if (vertical_ != newControl) {

		if (vertical_)
			removeChildControl(vertical_);

		vertical_ = newControl;

		if (vertical_)
			addChildControl(vertical_);

		emit verticalChanged(vertical_);
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

void BioXASSSRLMonochromator::setStepBragg(CLSMAXvMotor *newControl)
{
	if (stepBragg_ != newControl) {

		if (stepBragg_)
			removeChildControl(stepBragg_);

		stepBragg_ = newControl;

		if (stepBragg_)
			addChildControl(stepBragg_);

		updateStepBragg();
		updateBragg();
		updateStepEnergy();

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

		updateEncoderBragg();
		updateBragg();
		updateEncoderEnergy();

		emit encoderBraggChanged(encoderBragg_);
	}
}

void BioXASSSRLMonochromator::setBragg(CLSMAXvMotor *newControl)
{
	if (bragg_ != newControl) {

		if (bragg_)
			removeChildControl(bragg_);

		bragg_ = newControl;

		if (bragg_)
			addChildControl(bragg_);

		updateRegion();

		emit braggChanged(bragg_);
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

		updateStepEnergy();
		updateEnergy();

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

		updateEncoderEnergy();
		updateEnergy();

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
		updateStepEnergy();
		updateEncoderEnergy();

		emit regionChanged(region_);
	}
}

void BioXASSSRLMonochromator::setMask(BioXASSSRLMonochromatorMask *newControl)
{
	if (mask_ != newControl) {

		if (mask_)
			removeChildControl(mask_);

		mask_ = newControl;

		if (mask_)
			addChildControl(mask_);

		emit maskChanged(mask_);
	}
}

void BioXASSSRLMonochromator::updateStepBragg()
{
	if (stepBragg_)
		stepBragg_->setSettlingTime(settlingTime_);
}

void BioXASSSRLMonochromator::updateEncoderBragg()
{
	if (encoderBragg_)
		encoderBragg_->setSettlingTime(settlingTime_);
}

void BioXASSSRLMonochromator::updateBragg()
{
	CLSMAXvMotor *newBragg = 0;

	switch (int(mode_)) {
	case Mode::Encoder:
		newBragg = encoderBragg_;
		break;
	case Mode::Step:
		newBragg = stepBragg_;
		break;
	default:
		break;
	}

	setBragg(newBragg);
}

void BioXASSSRLMonochromator::updateStepEnergy()
{
	if (stepEnergy_) {
		stepEnergy_->setBraggControl(stepBragg_);
		stepEnergy_->setM1MirrorPitchControl(m1Pitch_);
		stepEnergy_->setRegionControl(region_);
	}
}

void BioXASSSRLMonochromator::updateEncoderEnergy()
{
	if (encoderEnergy_) {
		encoderEnergy_->setBraggControl(encoderBragg_);
		encoderEnergy_->setM1MirrorPitchControl(m1Pitch_);
		encoderEnergy_->setRegionControl(region_);
	}
}

void BioXASSSRLMonochromator::updateEnergy()
{
	BioXASSSRLMonochromatorEnergyControl *newEnergy = 0;

	switch (int(mode_)) {
	case Mode::Encoder:
		newEnergy = encoderEnergy_;
		break;
	case Mode::Step:
		newEnergy = stepEnergy_;
		break;
	default:
		break;
	}

	setEnergy(newEnergy);
}

void BioXASSSRLMonochromator::updateRegion()
{
	if (region_) {

		if (mask_) {
			region_->setUpperSlitBladeControl(mask_->upperBlade()); // only the mask state needs to be included here. Fix this when region control is refactored.
			region_->setLowerSlitBladeControl(mask_->lowerBlade());
			region_->setSlitsStatusControl(mask_->bladesState());
		} else {
			region_->setUpperSlitBladeControl(0);
			region_->setLowerSlitBladeControl(0);
			region_->setSlitsStatusControl(0);
		}

		region_->setPaddleControl(paddle_);
		region_->setPaddleStatusControl(paddleStatus_);
		region_->setKeyStatusControl(keyStatus_);
		region_->setBrakeStatusControl(brakeStatus_);
		region_->setBraggControl(bragg_);
		region_->setBraggAtCrystalChangePositionStatusControl(braggAtCrystalChangePositionStatus_);
		region_->setCrystalChangeControl(crystalChange_);

		if (crystalChange_) { // only the crystal change MAXvMotor needed here. fix this when region control is refactored.
			region_->setCrystalChangeCWLimitStatusControl(crystalChange_->cwLimitControl());
			region_->setCrystalChangeCCWLimitStatusControl(crystalChange_->ccwLimitControl());
		} else {
			region_->setCrystalChangeCWLimitStatusControl(0);
			region_->setCrystalChangeCCWLimitStatusControl(0);
		}

		region_->setRegionAStatusControl(regionAStatus_);
		region_->setRegionBStatusControl(regionBStatus_);
	}
}
