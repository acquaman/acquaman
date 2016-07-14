#include "BioXASSSRLMonochromator.h"

BioXASSSRLMonochromator::BioXASSSRLMonochromator(const QString &name, QObject *parent) :
	BioXASMonochromator(name, parent)
{
	// Initialize local variables.

	maskUpperBlade_ = 0;
	maskLowerBlade_ = 0;
	bladesState_ = 0;
	maskState_ = 0;
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

	setMaskState(new BioXASSSRLMonochromatorMaskState(QString("%1MaskState").arg(name), this));
}

BioXASSSRLMonochromator::~BioXASSSRLMonochromator()
{

}

bool BioXASSSRLMonochromator::isConnected() const
{
	bool connected = (

				BioXASMonochromator::isConnected() &&

				maskUpperBlade_ && maskUpperBlade_->isConnected() &&
				maskLowerBlade_ && maskLowerBlade_->isConnected() &&
				bladesState_ && bladesState_->isConnected() &&
				maskState_ && maskState_->isConnected() &&
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

				m1Pitch_ && m1Pitch_->isConnected()

				);

	return connected;
}

bool BioXASSSRLMonochromator::canStop() const
{
	bool result = false;

	if (isConnected()) {
		result = (
					maskUpperBlade_->canStop() &&
					maskLowerBlade_->canStop() &&
					paddle_->canStop() &&
					crystalChange_->canStop() &&
					vertical_->canStop() &&
					lateral_->canStop() &&
					crystal1Pitch_->canStop() &&
					crystal1Roll_->canStop() &&
					crystal2Pitch_->canStop() &&
					crystal2Roll_->canStop() &&
					stepBragg_->canStop() &&
					encoderBragg_->canStop() &&
					bragg_->canStop() &&
					stepEnergy_->canStop() &&
					encoderEnergy_->canStop()
					);
	}

	return result;
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

		removeChildControl(m1Pitch_);

		m1Pitch_ = newControl;

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

void BioXASSSRLMonochromator::setUpperBlade(BioXASMAXvMotor *newControl)
{
	if (maskUpperBlade_ != newControl) {

		removeChildControl(maskUpperBlade_);

		maskUpperBlade_ = newControl;

		addChildControl(maskUpperBlade_);

		updateMaskState();

		emit maskUpperBladeChanged(maskUpperBlade_);
	}
}

void BioXASSSRLMonochromator::setLowerBlade(BioXASMAXvMotor *newControl)
{
	if (maskLowerBlade_ != newControl) {

		removeChildControl(maskLowerBlade_);

		maskLowerBlade_ = newControl;

		addChildControl(maskLowerBlade_);

		updateMaskState();

		emit maskLowerBladeChanged(maskLowerBlade_);
	}
}

void BioXASSSRLMonochromator::setBladesState(AMControl *newControl)
{
	if (bladesState_ != newControl) {

		removeChildControl(bladesState_);

		bladesState_ = newControl;

		addChildControl(bladesState_);

		updateMaskState();

		emit bladesStateChanged(bladesState_);
	}
}

void BioXASSSRLMonochromator::setMaskState(BioXASSSRLMonochromatorMaskState *newControl)
{
	if (maskState_ != newControl) {

		removeChildControl(maskState_);

		maskState_ = newControl;

		addChildControl(maskState_);

		updateMaskState();

		emit maskStateChanged(maskState_);
	}
}

void BioXASSSRLMonochromator::setPaddle(BioXASMAXvMotor *newControl)
{
	if (paddle_ != newControl) {

		removeChildControl(paddle_);

		paddle_ = newControl;

		addChildControl(paddle_);

		updateRegion();

		emit paddleChanged(paddle_);
	}
}

void BioXASSSRLMonochromator::setPaddleStatus(AMControl *newControl)
{
	if (paddleStatus_ != newControl) {

		removeChildControl(paddleStatus_);

		paddleStatus_ = newControl;

		addChildControl(paddleStatus_);

		updateRegion();

		emit paddleStatusChanged(paddleStatus_);
	}
}

void BioXASSSRLMonochromator::setKeyStatus(AMControl *newControl)
{
	if (keyStatus_ != newControl) {

		removeChildControl(keyStatus_);

		keyStatus_ = newControl;

		addChildControl(keyStatus_);

		updateRegion();

		emit keyStatusChanged(keyStatus_);
	}
}

void BioXASSSRLMonochromator::setBrakeStatus(AMControl *newControl)
{
	if (brakeStatus_ != newControl) {

		removeChildControl(brakeStatus_);

		brakeStatus_ = newControl;

		addChildControl(brakeStatus_);

		updateRegion();

		emit brakeStatusChanged(brakeStatus_);
	}
}

void BioXASSSRLMonochromator::setBraggAtCrystalChangePositionStatus(AMControl *newControl)
{
	if (braggAtCrystalChangePositionStatus_ != newControl) {

		removeChildControl(braggAtCrystalChangePositionStatus_);

		braggAtCrystalChangePositionStatus_ = newControl;

		addChildControl(braggAtCrystalChangePositionStatus_);

		updateRegion();

		emit braggAtCrystalChangePositionStatusChanged(braggAtCrystalChangePositionStatus_);
	}
}

void BioXASSSRLMonochromator::setCrystalChange(BioXASMAXvMotor *newControl)
{
	if (crystalChange_ != newControl) {

		removeChildControl(crystalChange_);

		crystalChange_ = newControl;

		addChildControl(crystalChange_);

		updateRegion();

		emit crystalChangeChanged(crystalChange_);
	}
}

void BioXASSSRLMonochromator::setRegionAStatus(AMControl *newControl)
{
	if (regionAStatus_ != newControl) {

		removeChildControl(regionAStatus_);

		regionAStatus_ = newControl;

		addChildControl(regionAStatus_);

		updateRegion();

		emit regionAStatusChanged(regionAStatus_);
	}
}

void BioXASSSRLMonochromator::setRegionBStatus(AMControl *newControl)
{
	if (regionBStatus_ != newControl) {

		removeChildControl(regionBStatus_);

		regionBStatus_ = newControl;

		addChildControl(regionBStatus_);

		updateRegion();

		emit regionBStatusChanged(regionBStatus_);
	}
}

void BioXASSSRLMonochromator::setVertical(CLSMAXvMotor *newControl)
{
	if (vertical_ != newControl) {

		removeChildControl(vertical_);

		vertical_ = newControl;

		addChildControl(vertical_);

		emit verticalChanged(vertical_);
	}
}

void BioXASSSRLMonochromator::setLateral(CLSMAXvMotor *newControl)
{
	if (lateral_ != newControl) {

		removeChildControl(lateral_);

		lateral_ = newControl;

		addChildControl(lateral_);

		emit lateralChanged(lateral_);
	}
}

void BioXASSSRLMonochromator::setCrystal1Pitch(CLSMAXvMotor *newControl)
{
	if (crystal1Pitch_ != newControl) {

		removeChildControl(crystal1Pitch_);

		crystal1Pitch_ = newControl;

		addChildControl(crystal1Pitch_);

		emit crystal1PitchChanged(crystal1Pitch_);
	}
}

void BioXASSSRLMonochromator::setCrystal1Roll(CLSMAXvMotor *newControl)
{
	if (crystal1Roll_ != newControl) {

		removeChildControl(crystal1Roll_);

		crystal1Roll_ = newControl;

		addChildControl(crystal1Roll_);

		emit crystal1RollChanged(crystal1Roll_);
	}
}

void BioXASSSRLMonochromator::setCrystal2Pitch(CLSMAXvMotor *newControl)
{
	if (crystal2Pitch_ != newControl) {

		removeChildControl(crystal2Pitch_);

		crystal2Pitch_ = newControl;

		addChildControl(crystal2Pitch_);

		emit crystal2PitchChanged(crystal2Pitch_);
	}
}

void BioXASSSRLMonochromator::setCrystal2Roll(CLSMAXvMotor *newControl)
{
	if (crystal2Roll_ != newControl) {

		removeChildControl(crystal2Roll_);

		crystal2Roll_ = newControl;

		addChildControl(crystal2Roll_);

		emit crystal2RollChanged(crystal2Roll_);
	}
}

void BioXASSSRLMonochromator::setStepBragg(BioXASMAXvMotor *newControl)
{
	if (stepBragg_ != newControl) {

		removeChildControl(stepBragg_);

		stepBragg_ = newControl;

		addChildControl(stepBragg_);

		updateStepBragg();
		updateBragg();
		updateStepEnergy();

		emit stepBraggChanged(stepBragg_);
	}
}

void BioXASSSRLMonochromator::setEncoderBragg(BioXASMAXvMotor *newControl)
{
	if (encoderBragg_ != newControl) {

		removeChildControl(encoderBragg_);

		encoderBragg_ = newControl;

		addChildControl(encoderBragg_);

		updateEncoderBragg();
		updateBragg();
		updateEncoderEnergy();

		emit encoderBraggChanged(encoderBragg_);
	}
}

void BioXASSSRLMonochromator::setBragg(BioXASMAXvMotor *newControl)
{
	if (bragg_ != newControl) {

		removeChildControl(bragg_);

		bragg_ = newControl;

		addChildControl(bragg_);

		updateRegion();

		emit braggChanged(bragg_);
	}
}

void BioXASSSRLMonochromator::setStepEnergy(BioXASSSRLMonochromatorEnergyControl *newControl)
{
	if (stepEnergy_ != newControl) {

		removeChildControl(stepEnergy_);

		stepEnergy_ = newControl;

		addChildControl(stepEnergy_);

		updateStepEnergy();
		updateEnergy();

		emit stepEnergyChanged(stepEnergy_);
	}
}

void BioXASSSRLMonochromator::setEncoderEnergy(BioXASSSRLMonochromatorEnergyControl *newControl)
{
	if (encoderEnergy_ != newControl) {

		removeChildControl(encoderEnergy_);

		encoderEnergy_ = newControl;

		addChildControl(encoderEnergy_);

		updateEncoderEnergy();
		updateEnergy();

		emit encoderEnergyChanged(encoderEnergy_);
	}
}

void BioXASSSRLMonochromator::setRegion(BioXASSSRLMonochromatorRegionControl *newControl)
{
	if (region_ != newControl) {

		removeChildControl(region_);

		region_ = newControl;

		addChildControl(region_);

		updateRegion();
		updateStepEnergy();
		updateEncoderEnergy();

		emit regionChanged(region_);
	}
}

void BioXASSSRLMonochromator::updateMaskState()
{
	if (maskState_) {
		maskState_->setUpperBlade(maskUpperBlade_);
		maskState_->setLowerBlade(maskLowerBlade_);
		maskState_->setBladesState(bladesState_);
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
	BioXASMAXvMotor *newBragg = 0;

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

		region_->setUpperSlitBladeControl(maskUpperBlade_);
		region_->setLowerSlitBladeControl(maskLowerBlade_);
		region_->setSlitsStatusControl(bladesState_);
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
