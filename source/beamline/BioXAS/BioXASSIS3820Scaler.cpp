#include "BioXASSIS3820Scaler.h"
#include "beamline/AMPVControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/actions/AMWaitAction.h"
#include "actions3/AMActionSupport.h"
#include "actions3/BioXAS/BioXASSIS3820ScalerDarkCurrentMeasurementAction.h"
#include "beamline/AMDetectorTriggerSource.h"

BioXASSIS3820Scaler::BioXASSIS3820Scaler(const QString &baseName, BioXASZebraSoftInputControl *softInput, QObject *parent) :
	CLSSIS3820Scaler(baseName, parent)
{
	scanning_ = false;

	connect( startToggle_, SIGNAL(valueChanged(double)), this, SLOT(onStartToggleValueChanged()) );

	inputsMode_ = new AMSinglePVControl("InputMode", baseName+":inputMode", this);
	allControls_->addControl(inputsMode_);

	connect( inputsMode_, SIGNAL(connected(bool)), this, SLOT(updateInputsModeControl()) );

	triggerSourceMode_ = new AMSinglePVControl("TriggerSource", baseName+":triggerSource", this);
	allControls_->addControl(triggerSourceMode_);

	connect( triggerSourceMode_, SIGNAL(connected(bool)), this, SLOT(updateTriggerSourceModeControl()) );

	softInput_ = softInput;
	allControls_->addControl(softInput_);

	connect( softInput_, SIGNAL(valueChanged(double)), this, SLOT(onSoftInputValueChanged()) );

	isArming_ = false;
}

BioXASSIS3820Scaler::~BioXASSIS3820Scaler()
{

}

bool BioXASSIS3820Scaler::isConnected() const
{
	bool connected = (CLSSIS3820Scaler::isConnected() &&
			  inputsMode_ && inputsMode_->isConnected() &&
			  triggerSourceMode_ && triggerSourceMode_->isConnected() &&
			  softInput_ && softInput_->isConnected());

	return connected;
}

bool BioXASSIS3820Scaler::isArmed() const
{
	bool result = false;

	if (isConnected())
		result = startToggle_->withinTolerance(Armed);

	return result;
}

void BioXASSIS3820Scaler::arm()
{
	if(!isArming_){

		isArming_ = true;
		disconnect(startToggle_, SIGNAL(valueChanged(double)), this, SLOT(onScanningToggleChanged()));
		connect(startToggle_, SIGNAL(valueChanged(double)), this, SLOT(onStartToggleArmed()));

		startToggle_->move(Armed);
	}
}

void BioXASSIS3820Scaler::setTriggerSource(AMDetectorTriggerSource *triggerSource)
{
	if(triggerSource_)
		disconnect(triggerSource_, SIGNAL(triggered(AMDetectorDefinitions::ReadMode)), this, SLOT(onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode)));

	triggerSource_ = 0;

	if(triggerSource){
		triggerSource_ = triggerSource;
		connect(triggerSource_, SIGNAL(triggered(AMDetectorDefinitions::ReadMode)), this, SLOT(onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode)));
	}
}

void BioXASSIS3820Scaler::setInputsModeValuePreference(double newValue)
{
	if (inputsModeValuePreference_ != newValue || !inputsModeValuePreferenceSet_) {
		inputsModeValuePreferenceSet_ = true;
		inputsModeValuePreference_ = newValue;
		updateInputsModeControl();

		emit inputsModeValuePreferenceChanged(inputsModeValuePreference_);
	}
}

void BioXASSIS3820Scaler::setTriggerSourceModeValuePreference(double newValue)
{
	if (triggerSourceModeValuePreference_ != newValue || !triggerSourceModeValuePreferenceSet_) {
		triggerSourceModeValuePreferenceSet_ = true;
		triggerSourceModeValuePreference_ = newValue;
		updateTriggerSourceModeControl();

		emit triggerSourceModeValuePreferenceChanged(triggerSourceModeValuePreference_);
	}
}

void BioXASSIS3820Scaler::setScanningState(bool isScanning)
{
	if (scanning_ != isScanning) {
		scanning_ = isScanning;
		emit scanningChanged(scanning_);
	}
}

void BioXASSIS3820Scaler::onStartToggleArmed()
{
	if(isArming_ && startToggle_->withinTolerance(Armed)){

		isArming_ = false;
		disconnect(startToggle_, SIGNAL(valueChanged(double)), this, SLOT(onStartToggleArmed()));
		connect(startToggle_, SIGNAL(valueChanged(double)), this, SLOT(onScanningToggleChanged()));

		emit armed();
	}
}

void BioXASSIS3820Scaler::onSoftInputValueChanged()
{
	if (isConnected()) {
		setScanningState(isArmed());
	}
}

void BioXASSIS3820Scaler::onStartToggleValueChanged()
{
	if (isConnected()) {
		if (!isArmed()) {
			setScanningState(false);
		}
	}
}

void BioXASSIS3820Scaler::onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode readMode)
{
	Q_UNUSED(readMode)
	initializeTriggerSource();
	setScanningState(true);
}

void BioXASSIS3820Scaler::updateInputsModeControl()
{
	if (inputsMode_ && inputsMode_->canMove() && inputsModeValuePreferenceSet_)
		inputsMode_->move(inputsModeValuePreference_);
}

void BioXASSIS3820Scaler::updateTriggerSourceModeControl()
{
	if (triggerSourceMode_ && triggerSourceMode_->canMove() && triggerSourceModeValuePreference_)
		triggerSourceMode_->move(triggerSourceModeValuePreference_);
}

void BioXASSIS3820Scaler::triggerSourceSucceeded()
{
	AMZebraDetectorTriggerSource *trigger = qobject_cast<AMZebraDetectorTriggerSource *>(triggerSource_);

	if (trigger)
		trigger->setSucceeded(this);
}

AMAction3* BioXASSIS3820Scaler::createArmAction(bool setArmed)
{
	AMAction3 *result = 0;

	if (isConnected()) {

		if (setArmed)
			result = createMoveToArmedAction();
		else
			result = createMoveToNotArmedAction();
	}

	return result;
}

AMAction3* BioXASSIS3820Scaler::createMoveToArmedAction()
{
	AMAction3 *result = 0;

	if (isConnected()) {
		AMListAction3 *armingAction = new AMListAction3(new AMListActionInfo3("Arming BioXAS scaler.", "Arming BioXAS scaler."), AMListAction3::Sequential);
		armingAction->addSubAction(AMActionSupport::buildControlMoveAction(startToggle_, Armed));
		armingAction->addSubAction(new AMWaitAction(new AMWaitActionInfo(BIOXASSIS3820SCALER_WAIT_SECONDS)));

		result = armingAction;
	}

	return result;
}

AMAction3* BioXASSIS3820Scaler::createMoveToNotArmedAction()
{
	AMAction3 *result = 0;

	if (isConnected()) {
		AMListAction3 *disarmingAction = new AMListAction3(new AMListActionInfo3("Disarming BioXAS scaler.", "Disarming BioXAS scaler."), AMListAction3::Sequential);
		disarmingAction->addSubAction(AMActionSupport::buildControlMoveAction(startToggle_, NotArmed));
		disarmingAction->addSubAction(new AMWaitAction(new AMWaitActionInfo(BIOXASSIS3820SCALER_WAIT_SECONDS)));

		result = disarmingAction;
	}

	return result;
}

AMAction3* BioXASSIS3820Scaler::createStartAction3(bool setScanning)
{
	AMAction3 *result = 0;

	if (isConnected()) {

		if (setScanning) {
			AMListAction3 *scanningAction = new AMListAction3(new AMListActionInfo3("Moving BioXAS scaler to scanning.", "Moving BioXAS scaler to scanning."), AMListAction3::Sequential);
			scanningAction->addSubAction(createMoveToArmedAction());
			scanningAction->addSubAction(createMoveToScanningAction());

			result = scanningAction;

		} else {

			result = createMoveToNotScanningAction();
		}
	}

	return result;
}

AMAction3* BioXASSIS3820Scaler::createMoveToScanningAction()
{
	AMAction3 *result = 0;

	if (isConnected())
		result = AMActionSupport::buildControlMoveAction(softInput_, Scanning);

	return result;
}

AMAction3* BioXASSIS3820Scaler::createMoveToNotScanningAction()
{
	AMAction3 *result = 0;

	if (isConnected())
		result = AMActionSupport::buildControlMoveAction(softInput_, NotScanning);

	return result;
}

AMAction3* BioXASSIS3820Scaler::createMoveToSingleShotAction()
{
	AMAction3 *result = 0;

	if (isConnected()) {
		AMListAction3 *toSingleShot = new AMListAction3(new AMListActionInfo3("Moving BioXAS scaler to 'Single shot' mode.", "Moving BioXAS scaler to 'Single shot' mode."), AMListAction3::Sequential);
		toSingleShot->addSubAction(CLSSIS3820Scaler::createMoveToSingleShotAction());
		toSingleShot->addSubAction(new AMWaitAction(new AMWaitActionInfo(BIOXASSIS3820SCALER_WAIT_SECONDS)));

		result = toSingleShot;
	}

	return result;
}

AMAction3* BioXASSIS3820Scaler::createMoveToContinuousAction()
{
	AMAction3 *result = 0;

	if (isConnected()) {
		AMListAction3 *toContinuous = new AMListAction3(new AMListActionInfo3("Moving BioXAS scaler to 'Continuous' mode.", "Moving BioXAS scaler to 'Continuous' mode."), AMListAction3::Sequential);
		toContinuous->addSubAction(CLSSIS3820Scaler::createMoveToContinuousAction());
		toContinuous->addSubAction(new AMWaitAction(new AMWaitActionInfo(BIOXASSIS3820SCALER_WAIT_SECONDS)));
		toContinuous->addSubAction(AMActionSupport::buildControlMoveAction(softInput_, Scanning));

		result = toContinuous;
	}

	return result;
}

AMAction3* BioXASSIS3820Scaler::createMeasureDarkCurrentAction(double secondsDwell)
{
	return new BioXASSIS3820ScalerDarkCurrentMeasurementAction(new CLSSIS3820ScalerDarkCurrentMeasurementActionInfo(secondsDwell));
}
