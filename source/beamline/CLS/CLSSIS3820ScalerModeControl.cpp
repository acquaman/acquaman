#include "CLSSIS3820ScalerModeControl.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

#include <QDebug>

CLSSIS3820ScalerModeControl::CLSSIS3820ScalerModeControl(const QString &name, const QString &units, QObject *parent, const QString &description) :
	AMPseudoMotorControl(name, units, parent, description)
{
	// Initialize inherited variables.

	value_ = None;
	setpoint_ = None;
	minimumValue_ = Continuous;
	maximumValue_ = None;

	setEnumStates(QStringList() << "Continuous" << "SingleShot" << "None");
	setMoveEnumStates(QStringList() << "Continuous" << "SingleShot");

	setAllowsMovesWhileMoving(false);
	setContextKnownDescription("Mode Control");

	// Initialize local variables.

	scanCountControl_ = 0;
	numberOfScansPerBufferControl_ = 0;
	startScanControl_ = 0;

	singleShotScanCountValue_ = 1;
	singleShotNumberOfScansPerBufferValue_ = 1;

	// Make connections.

	connect( this, SIGNAL(connected(bool)), this, SIGNAL(enumChanged()) );
	connect( this, SIGNAL(valueChanged(double)), this, SIGNAL(enumChanged()) );
	connect( this, SIGNAL(setpointChanged(double)), this, SIGNAL(enumChanged()) );

	// Current settings.

	updateStates();
}

CLSSIS3820ScalerModeControl::~CLSSIS3820ScalerModeControl()
{

}

bool CLSSIS3820ScalerModeControl::canMove() const
{
	bool result = false;

	if (isConnected() && !isMoving()) {
		result = (
					scanCountControl_->canMove() &&
					numberOfScansPerBufferControl_->canMove() &&
					startScanControl_->canMove()
					);
	}

	return result;
}

bool CLSSIS3820ScalerModeControl::validValue(double value) const
{
	bool result = false;

	int valueInt = int(value);

	if (valueInt == Continuous || valueInt == SingleShot || valueInt == None)
		result = true;

	return result;
}

bool CLSSIS3820ScalerModeControl::validSetpoint(double value) const
{
	bool result = false;

	int valueInt = int(value);

	if (valueInt == Continuous || valueInt == SingleShot)
		result = true;

	return result;
}

QString CLSSIS3820ScalerModeControl::toString() const
{
	QString basicInfo = AMPseudoMotorControl::toString();

	// Note this control's current value.

	QString controlValue = "Value: " + toString(value());

	// Note this control's tolerance.

	QString controlTolerance = "Tolerance: " + QString::number(tolerance());

	// Create and return complete info string.

	QString result = basicInfo + "\n" + controlValue + "\n" + controlTolerance;

	return result;
}

QString CLSSIS3820ScalerModeControl::toString(double value) const
{
	QString result = "";

	switch (int(value)) {
	case SingleShot:
		result = "Single Shot";
		break;
	case Continuous:
		result = "Continuous";
		break;
	case None:
		result = "None";
		break;
	default:
		result = "Unknown";
		break;
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

void CLSSIS3820ScalerModeControl::updateConnected()
{
	bool wasConnected = connected_;

	bool isConnected = (
				scanCountControl_ && scanCountControl_->isConnected() &&
				numberOfScansPerBufferControl_ && numberOfScansPerBufferControl_->isConnected() &&
				startScanControl_ && startScanControl_->isConnected()
				);

	setConnected(isConnected);

	// If we weren't connected before but we are now, set mode to Continuous.

	if (!wasConnected && isConnected)
		move(Continuous);
}

void CLSSIS3820ScalerModeControl::updateValue()
{
	emit valueChanged(value_);
}

void CLSSIS3820ScalerModeControl::updateMoving()
{
	if (isConnected()) {
		bool isMoving = (
					scanCountControl_->isMoving() ||
					numberOfScansPerBufferControl_->isMoving() ||
					startScanControl_->isMoving()
					);

		setIsMoving(isMoving);
	}
}

void CLSSIS3820ScalerModeControl::onMoveSucceeded(QObject *action)
{
	setValue(setpoint_);
	AMPseudoMotorControl::onMoveSucceeded(action);
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

AMAction3* CLSSIS3820ScalerModeControl::createMoveAction(double setpoint)
{
	// AMPseudoMotorControl handles checking whether we are connected, whether we can move,
	// and whether the setpoint is a valid value.

	AMAction3 *result = 0;

	int setpointInt = int(setpoint);

	if (setpointInt == Continuous) {
		setSingleShotScanCountValue(scanCountControl_->value());
		setSingleShotNumberOfScansPerBufferValue(numberOfScansPerBufferControl_->value());

		result = createMoveToContinuousModeAction();

	} else if (setpointInt == SingleShot) {
		result = createMoveToSingleShotModeAction();
	}

	return result;
}

AMAction3* CLSSIS3820ScalerModeControl::createMoveToContinuousModeAction()
{
	AMListAction3 *result = new AMListAction3(new AMListActionInfo3("ModeChange", "SIS3820 Scaler Mode Change"), AMListAction3::Sequential);
	result->addSubAction(AMActionSupport::buildControlMoveAction(scanCountControl_, 0));
	result->addSubAction(AMActionSupport::buildControlMoveAction(numberOfScansPerBufferControl_, 1));
	result->addSubAction(AMActionSupport::buildControlMoveAction(startScanControl_, 1));

	return result;
}

AMAction3* CLSSIS3820ScalerModeControl::createMoveToSingleShotModeAction()
{
	AMListAction3 *result = new AMListAction3(new AMListActionInfo3("ModeChange", "SIS3820 Scaler Mode Change"), AMListAction3::Sequential);
	result->addSubAction(AMActionSupport::buildControlMoveAction(scanCountControl_, singleShotScanCountValue_));
	result->addSubAction(AMActionSupport::buildControlMoveAction(numberOfScansPerBufferControl_, singleShotNumberOfScansPerBufferValue_));
	result->addSubAction(AMActionSupport::buildControlMoveAction(startScanControl_, 0));

	return result;
}
