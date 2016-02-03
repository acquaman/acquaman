#include "BioXASSIS3820Scaler.h"
#include "beamline/AMPVControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/actions/AMWaitAction.h"
#include "actions3/AMActionSupport.h"
#include "beamline/AMDetectorTriggerSource.h"

BioXASSIS3820Scaler::BioXASSIS3820Scaler(const QString &baseName, BioXASZebraSoftInputControl *softInput, QObject *parent) :
	CLSSIS3820Scaler(baseName, parent)
{
	inputsMode_ = new AMSinglePVControl("InputMode", baseName+":inputMode", this);
	triggerSourceMode_ = new AMSinglePVControl("TriggerSource", baseName+":triggerSource", this);
	clockSourceMode_ = new AMSinglePVControl("ClockSource", baseName+":source", this);
	softInput_ = softInput;

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
			  clockSourceMode_ && clockSourceMode_->isConnected() &&
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

bool BioXASSIS3820Scaler::isScanning() const
{
	bool result = false;

	if (isConnected())
		result = softInput_->withinTolerance(Scanning);

	return result;
}

void BioXASSIS3820Scaler::arm()
{
	if(!isArming_){

		isArming_ = true;
		disconnect(startToggle_, SIGNAL(valueChanged(double)), this, SLOT(onScanningToggleChanged()));
		connect(startToggle_, SIGNAL(valueChanged(double)), this, SLOT(onStartToggleArmed()));

		startToggle_->move(1);
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

void BioXASSIS3820Scaler::onStartToggleArmed()
{
	if(isArming_ && startToggle_->withinTolerance(Armed)){

		isArming_ = false;
		disconnect(startToggle_, SIGNAL(valueChanged(double)), this, SLOT(onStartToggleArmed()));
		connect(startToggle_, SIGNAL(valueChanged(double)), this, SLOT(onScanningToggleChanged()));

		emit armed();
	}
}

void BioXASSIS3820Scaler::onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode readMode)
{
	Q_UNUSED(readMode)
	initializeTriggerSource();
}

void BioXASSIS3820Scaler::triggerSourceSucceeded()
{
	AMZebraDetectorTriggerSource *trigger = qobject_cast<AMZebraDetectorTriggerSource *>(triggerSource_);

	if (trigger)
		trigger->setSucceeded(this);
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
