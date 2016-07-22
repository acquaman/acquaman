#include "CLSSIS3820ScalerAcquisitionMode.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"

CLSSIS3820ScalerAcquisitionMode::CLSSIS3820ScalerAcquisitionMode(const QString &name, QObject *parent) :
	AMEnumeratedControl(name, "", parent)
{
	// Initialize inherited variables.

	setAllowsMovesWhileMoving(false);
	setContextKnownDescription("Mode Control");

	// Initialize local variables.

	scanCountControl_ = 0;
	numberOfScansPerBufferControl_ = 0;
	startScanControl_ = 0;

	// Setup value options.

	addOption(CLSSIS3820Scaler::SingleShot, "Single shot");
	addOption(CLSSIS3820Scaler::Continuous, "Continuous");
}

CLSSIS3820ScalerAcquisitionMode::~CLSSIS3820ScalerAcquisitionMode()
{

}

bool CLSSIS3820ScalerAcquisitionMode::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = scanCountControl_->canMeasure() && numberOfScansPerBufferControl_->canMeasure() && startScanControl_->canMeasure();

	return result;
}

bool CLSSIS3820ScalerAcquisitionMode::canMove() const
{
	bool result = false;

	if (isConnected())
		result = scanCountControl_->canMove() && numberOfScansPerBufferControl_->canMove() && startScanControl_->canMove();

	return result;
}

bool CLSSIS3820ScalerAcquisitionMode::isContinuous() const
{
	bool result = false;

	if (isConnected()) {
		if (startScanControl_->value() == CLSSIS3820Scaler::Scanning && scanCountControl_->value() == 0) {
			result = true;
		}
	}

	return result;
}

bool CLSSIS3820ScalerAcquisitionMode::isSingleShot() const
{
	bool result = false;

	if (isConnected()) {
		if (scanCountControl_->value() > 0)
			result = true;
	}

	return result;
}

void CLSSIS3820ScalerAcquisitionMode::setScanCountControl(AMControl *newControl)
{
	if (scanCountControl_ != newControl) {

		removeChildControl(scanCountControl_);
		scanCountControl_ = newControl;
		addChildControl(scanCountControl_);

		emit scanCountControlChanged(scanCountControl_);
	}
}

void CLSSIS3820ScalerAcquisitionMode::setNumberOfScansPerBufferControl(AMControl *newControl)
{
	if (numberOfScansPerBufferControl_ != newControl) {

		removeChildControl(numberOfScansPerBufferControl_);
		numberOfScansPerBufferControl_ = newControl;
		addChildControl(numberOfScansPerBufferControl_);

		emit numberOfScansPerBufferControlChanged(numberOfScansPerBufferControl_);
	}
}

void CLSSIS3820ScalerAcquisitionMode::setStartScanControl(AMControl *newControl)
{
	if (startScanControl_ != newControl) {

		removeChildControl(startScanControl_);
		startScanControl_ = newControl;
		addChildControl(startScanControl_);

		emit startScanControlChanged(startScanControl_);
	}
}

void CLSSIS3820ScalerAcquisitionMode::updateValue()
{
	if (!moveInProgress())
		AMEnumeratedControl::updateValue();
}

int CLSSIS3820ScalerAcquisitionMode::currentIndex() const
{
	int result = enumNames().indexOf("Unknown");

	if (isContinuous())
		result = CLSSIS3820Scaler::Continuous;
	else if (isSingleShot())
		result = CLSSIS3820Scaler::SingleShot;

	return result;
}

AMAction3* CLSSIS3820ScalerAcquisitionMode::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	switch(int(setpoint)) {
	case CLSSIS3820Scaler::Continuous:
		result = createMoveToContinuousModeAction();
		break;

	case CLSSIS3820Scaler::SingleShot:
		result = createMoveToSingleShotModeAction();
		break;

	default:
		break;
	}

	return result;
}

AMAction3* CLSSIS3820ScalerAcquisitionMode::createMoveToContinuousModeAction()
{
	AMListAction3 *result = new AMListAction3(new AMListActionInfo3("SIS3820 Scaler Continuous Mode Change", "SIS3820 Scaler Continuous Mode Change"), AMListAction3::Sequential);
	result->addSubAction(AMActionSupport::buildControlMoveAction(startScanControl_, CLSSIS3820Scaler::NotScanning));
	result->addSubAction(AMActionSupport::buildControlMoveAction(scanCountControl_, 0));
	result->addSubAction(AMActionSupport::buildControlMoveAction(numberOfScansPerBufferControl_, 1));
	result->addSubAction(AMActionSupport::buildControlMoveAction(startScanControl_, CLSSIS3820Scaler::Scanning));

	return result;
}

AMAction3* CLSSIS3820ScalerAcquisitionMode::createMoveToSingleShotModeAction()
{
	AMListAction3 *result = new AMListAction3(new AMListActionInfo3("ModeChange", "SIS3820 Scaler Mode Change"), AMListAction3::Sequential);
	result->addSubAction(AMActionSupport::buildControlMoveAction(startScanControl_, CLSSIS3820Scaler::NotScanning));
	result->addSubAction(AMActionSupport::buildControlMoveAction(scanCountControl_, 1));
	result->addSubAction(AMActionSupport::buildControlMoveAction(numberOfScansPerBufferControl_, 1));

	return result;
}
