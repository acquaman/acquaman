#include "BioXASSSRLMonochromator.h"

BioXASSSRLMonochromator::BioXASSSRLMonochromator(const QString &name, QObject *parent) :
	BioXASMonochromator(name, parent)
{
	// Initialize local variables.

	encoderEnergy_ = 0;
	stepEnergy_ = 0;
	region_ = 0;

	upperSlit_ = 0;
	lowerSlit_ = 0;
	slitsStatus_ = 0;
	paddle_ = 0;
	paddleStatus_ = 0;
	keyStatus_ = 0;
	brakeStatus_ = 0;
	encoderBragg_ = 0;
	braggAtCrystalChangePositionStatus_ = 0;
	crystalChange_ = 0;
	crystalChangeCWLimitStatus_ = 0;
	crystalChangeCCWLimitStatus_ = 0;
	regionAStatus_ = 0;
	regionBStatus_ = 0;
	m1Pitch_ = 0;

	braggSetPosition_ = 0;

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

bool BioXASSSRLMonochromator::isConnected() const
{
	bool connected = (
		encoderEnergy_ && encoderEnergy_->isConnected() &&
		stepEnergy_ && stepEnergy_->isConnected() &&
		region_ && region_->isConnected() &&

		upperSlit_ && upperSlit_->isConnected() &&
		lowerSlit_ && lowerSlit_->isConnected() &&
		slitsStatus_ && slitsStatus_->isConnected() &&
		paddle_ && paddle_->isConnected() &&
		paddleStatus_ && paddleStatus_->isConnected() &&
		keyStatus_ && keyStatus_->isConnected() &&
		brakeStatus_ && brakeStatus_->isConnected() &&
		encoderBragg_ && encoderBragg_->isConnected() &&
		braggAtCrystalChangePositionStatus_ && braggAtCrystalChangePositionStatus_->isConnected() &&
		crystalChange_ && crystalChange_->isConnected() &&
		crystalChangeCWLimitStatus_ && crystalChangeCWLimitStatus_->isConnected() &&
		crystalChangeCCWLimitStatus_ && crystalChangeCCWLimitStatus_->isConnected() &&
		regionAStatus_ && regionAStatus_->isConnected() &&
		regionBStatus_ && regionBStatus_->isConnected() &&

		upperSlitMotor_ && upperSlitMotor_->isConnected() &&
		lowerSlitMotor_ && lowerSlitMotor_->isConnected() &&
		paddleMotor_ && paddleMotor_->isConnected() &&
		braggMotor_ && braggMotor_->isConnected() &&
		verticalMotor_ && verticalMotor_->isConnected() &&
		lateralMotor_ && lateralMotor_->isConnected() &&
		crystalChangeMotor_ && crystalChangeMotor_->isConnected() &&
		crystal1PitchMotor_ && crystal1PitchMotor_->isConnected() &&
		crystal1RollMotor_ && crystal1RollMotor_->isConnected() &&
		crystal2PitchMotor_ && crystal2PitchMotor_->isConnected() &&
		crystal2RollMotor_ && crystal2RollMotor_->isConnected()
	);

	return connected;
}

AMAction3* BioXASSSRLMonochromator::createEnergyCalibrationAction(double monoEnergy, double desiredEnergy)
{
	AMListAction3 *result = 0;

	if (energyControl() && energyControl()->isConnected() && braggMotor_ && braggMotor_->isConnected()) {
		result = new AMListAction3(new AMListActionInfo3("SSRL Monochromator Calibration Action", "SSRL Monochromator Calibration Action"), AMListAction3::Sequential);
		result->addSubAction(AMActionSupport::buildControlMoveAction(energyControl(), monoEnergy));
		result->addSubAction(createEnergyCalibrationAction(desiredEnergy));
	}

	return result;
}

AMAction3* BioXASSSRLMonochromator::createEnergyCalibrationAction(double desiredEnergy)
{
	AMAction3* result = 0;

	if (braggMotor_ && braggMotor_->isConnected()) {
		result = braggMotor_->createEGUSetPositionAction(desiredEnergy);
	}

	return result;
}

void BioXASSSRLMonochromator::setM1MirrorPitchControl(AMControl *newControl)
{
	if (m1Pitch_ != newControl) {
		m1Pitch_ = newControl;
		encoderEnergy_->setM1MirrorPitchControl(m1Pitch_);
		stepEnergy_->setM1MirrorPitchControl(m1Pitch_);
		emit m1MirrorPitchControlChanged(m1Pitch_);
	}
}

void BioXASSSRLMonochromator::calibrateBraggPosition(double newBraggPosition)
{
	if (braggMotor_ && braggMotor_->isConnected()) {
		braggMotor_->setEGUSetPosition(newBraggPosition);
	}
}
