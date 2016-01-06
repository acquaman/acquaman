#include "CLSSIS3820ScalerModeControl.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

CLSSIS3820ScalerModeControl::CLSSIS3820ScalerModeControl(const QString &name, const QString &units, QObject *parent) :
	AMEnumeratedControl(name, units, parent)
{
	// Initialize inherited variables.

	setContextKnownDescription("Scaler Mode");
	setAllowsMovesWhileMoving(false);
	setAllowsDuplicateOptions(false);

	// Initialize local variables.

	scanCountControl_ = 0;
	numberOfScansPerBufferControl_ = 0;
	startScanControl_ = 0;

	singleShotScanCountValue_ = 1;
	singleShotNumberOfScansPerBufferValue_ = 1;

	// Current settings.

	addMoveOption(Mode::SingleShot, modeToString(Mode::SingleShot));
	addMoveOption(Mode::Continuous, modeToString(Mode::Continuous));

	updateStates();
}

CLSSIS3820ScalerModeControl::~CLSSIS3820ScalerModeControl()
{

}

bool CLSSIS3820ScalerModeControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = ( scanCountControl_->canMeasure() && numberOfScansPerBufferControl_->canMeasure());

	return result;
}

bool CLSSIS3820ScalerModeControl::canMove() const
{
	bool result = false;

	if (isConnected() && !isMoving())
		result = ( scanCountControl_->canMove() && numberOfScansPerBufferControl_->canMove() && startScanControl_->canMove() );

	return result;
}

bool CLSSIS3820ScalerModeControl::validValue(double value) const
{
	bool result = false;

	int valueInt = int(value);

	if (valueInt == Mode::Continuous || valueInt == Mode::SingleShot)
		result = true;

	return result;
}

QString CLSSIS3820ScalerModeControl::modeToString(double value) const
{
	QString result = "";

	switch (int(value)) {
	case Mode::SingleShot:
		result = "Single Shot";
		break;
	case Mode::Continuous:
		result = "Continuous";
		break;
	default:
		result = "Unknown";
		break;
	}

	return result;
}

bool CLSSIS3820ScalerModeControl::isContinuous() const
{
	bool result = false;

	if (isConnected()) {
		if (startScanControl_->value() == Scan::Scanning && scanCountControl_->value() == 0) {
			result = true;
		}
	}

	return result;
}

bool CLSSIS3820ScalerModeControl::isSingleShot() const
{
	bool result = false;

	if (isConnected()) {
		if (scanCountControl_->value() > 0)
			result = true;
	}

	return result;
}

void CLSSIS3820ScalerModeControl::setScanCountControl(AMControl *newControl)
{
	if (scanCountControl_ != newControl) {

		if (scanCountControl_)
			removeChildControl(scanCountControl_);

		scanCountControl_ = newControl;

		if (scanCountControl_)
			addChildControl(scanCountControl_);

		emit scanCountControlChanged(scanCountControl_);
	}
}

void CLSSIS3820ScalerModeControl::setNumberOfScansPerBufferControl(AMControl *newControl)
{
	if (numberOfScansPerBufferControl_ != newControl) {

		if (numberOfScansPerBufferControl_)
			removeChildControl(numberOfScansPerBufferControl_);

		numberOfScansPerBufferControl_ = newControl;

		if (numberOfScansPerBufferControl_)
			addChildControl(numberOfScansPerBufferControl_);

		emit numberOfScansPerBufferControlChanged(numberOfScansPerBufferControl_);
	}
}

void CLSSIS3820ScalerModeControl::setStartScanControl(AMControl *newControl)
{
	if (startScanControl_ != newControl) {

		if (startScanControl_)
			removeChildControl(startScanControl_);

		startScanControl_ = newControl;

		if (startScanControl_)
			addChildControl(startScanControl_);

		emit startScanControlChanged(startScanControl_);
	}
}

void CLSSIS3820ScalerModeControl::setSingleShotScanCountValue(double newValue)
{
	if (singleShotScanCountValue_ != newValue)
		singleShotScanCountValue_ = newValue;
}

void CLSSIS3820ScalerModeControl::setSingleShotNumberOfScansPerBufferValue(double newValue)
{
	if (singleShotNumberOfScansPerBufferValue_ != newValue)
		singleShotNumberOfScansPerBufferValue_ = newValue;
}

bool CLSSIS3820ScalerModeControl::currentConnected() const
{
	return (
				scanCountControl_ && scanCountControl_->isConnected() &&
				numberOfScansPerBufferControl_ && numberOfScansPerBufferControl_->isConnected() &&
				startScanControl_ && startScanControl_->isConnected()
				);
}

double CLSSIS3820ScalerModeControl::currentValue() const
{
	// Initialize new value to 'Unknown.'

	double newValue = Unknown;

	// Determine whether the mode is 'Continuous' or 'Single Shot'.

	if (isContinuous())
		newValue = Mode::Continuous;
	else if (isSingleShot())
		newValue = Mode::SingleShot;

	return newValue;
}

bool CLSSIS3820ScalerModeControl::currentMoving() const
{
	bool moving = false;

	if (isConnected()) {
		moving = (
					scanCountControl_->isMoving() ||
					numberOfScansPerBufferControl_->isMoving() ||
					startScanControl_->isMoving()
					);
	}

	return moving;
}

AMAction3* CLSSIS3820ScalerModeControl::createMoveAction(double setpoint)
{
	// AMPseudoMotorControl handles checking whether we are connected, whether we can move,
	// and whether the setpoint is a valid value.

	AMAction3 *result = 0;

	switch(int(setpoint)) {
	case Mode::Continuous:
		setSingleShotScanCountValue(scanCountControl_->value());
		setSingleShotNumberOfScansPerBufferValue(numberOfScansPerBufferControl_->value());
		result = createMoveToContinuousModeAction();
		break;

	case Mode::SingleShot:
		result = createMoveToSingleShotModeAction();
		break;

	default:
		break;
	}

	return result;
}

AMAction3* CLSSIS3820ScalerModeControl::createMoveToContinuousModeAction()
{
	AMListAction3 *result = new AMListAction3(new AMListActionInfo3("ModeChange", "SIS3820 Scaler Mode Change"), AMListAction3::Sequential);
	result->addSubAction(AMActionSupport::buildControlMoveAction(scanCountControl_, 0));
	result->addSubAction(AMActionSupport::buildControlMoveAction(startScanControl_, Scan::NotScanning));
	result->addSubAction(AMActionSupport::buildControlMoveAction(numberOfScansPerBufferControl_, 1));
	result->addSubAction(AMActionSupport::buildControlMoveAction(startScanControl_, Scan::Scanning));

	return result;
}

AMAction3* CLSSIS3820ScalerModeControl::createMoveToSingleShotModeAction()
{
	AMListAction3 *result = new AMListAction3(new AMListActionInfo3("ModeChange", "SIS3820 Scaler Mode Change"), AMListAction3::Sequential);
	result->addSubAction(AMActionSupport::buildControlMoveAction(scanCountControl_, singleShotScanCountValue_));
	result->addSubAction(AMActionSupport::buildControlMoveAction(startScanControl_, Scan::NotScanning));
	result->addSubAction(AMActionSupport::buildControlMoveAction(numberOfScansPerBufferControl_, singleShotNumberOfScansPerBufferValue_));

	return result;
}
