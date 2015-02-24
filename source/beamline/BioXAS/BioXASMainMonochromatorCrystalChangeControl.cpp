#include "BioXASMainMonochromatorCrystalChangeControl.h"
#include <QDebug>

BioXASMainMonochromatorCrystalChangeControl::BioXASMainMonochromatorCrystalChangeControl(BioXASMainMonochromator *mono, QObject *parent) :
    QObject(parent)
{
    // Initialize variables.

    mono_ = 0;
    step_ = None;
	nextRegion_ = BioXASMainMonochromator::None;
	braggMotorCrystalChangePosition_ = SETPOINT_BRAGG_MOTOR_CRYSTAL_CHANGE_POSITION;
	braggMotorRegionADestination_ = SETPOINT_BRAGG_MOTOR_REGION_A_DESTINATION;
	braggMotorRegionBDestination_ = SETPOINT_BRAGG_MOTOR_REGION_B_DESTINATION;
	braggMotorRegionDestination_ = braggMotorRegionADestination_;
	crystalChangeMotorLimit_ = Unknown;

    // Current settings.

	setBraggMotorCrystalChangePosition(135.0);
    setMono(mono);
}

BioXASMainMonochromatorCrystalChangeControl::~BioXASMainMonochromatorCrystalChangeControl()
{

}

QString BioXASMainMonochromatorCrystalChangeControl::stepDescription(BioXASMainMonochromatorCrystalChangeControl::Step step)
{
	QString result = "";

	switch (step) {
	case None:
		result = "Waiting for valid, connected mono...";
		break;
	case SlitsNotClosed:
		result = "Closing mono slits...";
		break;
	case SlitsClosed:
		result = "Slits closed.";
		break;
	case PaddleNotOut:
		result = "Removing mono phosphor paddle...";
		break;
	case PaddleOut:
		result = "Paddle removed.";
		break;
	case KeyNotEnabled:
		result = "Waiting for crystal change key to be turned to 'Enabled'...";
		break;
	case KeyEnabled:
		result = "Key turned to 'Enabled.'";
		break;
	case BraggNotAtCrystalChangePosition:
		result = "Moving bragg motor to crystal change position...";
		break;
	case BraggAtCrystalChangePosition:
		result = "Bragg at crystal change position.";
		break;
	case BrakeNotDisabled:
		result = "Waiting for brake to be disabled...";
		break;
	case BrakeDisabled:
		result = "Brake disabled.";
		break;
	case CrystalChangeMotorNotAtCWLimit:
		result = "Moving the crystal change motor to clockwise limit...";
		break;
	case CrystalChangeMotorNotAtCCWLimit:
		result = "Moving the crystal change motor to counter-clockwise limit...";
		break;
	case CrystalChangeMotorAtCWLimit:
		result = "Crystal change motor clockwise limit reached.";
		break;
	case CrystalChangeMotorAtCCWLimit:
		result = "Crystal change motor counter-clockwise limit reached.";
		break;
	case BrakeNotEnabled:
		result = "Waiting for brake to be enabled...";
		break;
	case BrakeEnabled:
		result = "Brake enabled.";
		break;
	case BraggNotAtNewRegion:
		result = "Moving bragg motor into new region...";
		break;
	case BraggAtNewRegion:
		result = "Bragg motor has reached new region.";
		break;
	case KeyNotDisabled:
		result = "Waiting for crystal change key to be turned to 'Disabled'...";
		break;
	case KeyDisabled:
		result = "Key turned to 'Disabled.'";
		break;
	default:
		result = "";
	}

	return result;
}

QString BioXASMainMonochromatorCrystalChangeControl::stepInstruction(BioXASMainMonochromatorCrystalChangeControl::Step step)
{
	QString result = "";

	switch (step) {
	case KeyNotEnabled:
		result = "Turn the crystal change key to 'Enabled,' counter-clockwise.";
		break;
	case BrakeNotDisabled:
		result = "Flip the brake switch to 'Disabled.' There is a mandatory 30s wait after the switch is flipped before proceeding.";
		break;
	case BrakeNotEnabled:
		result = "Flip the brake switch to 'Enabled.' There is a mandatory 30s wait after the switch is flipped before proceeding.";
		break;
	case KeyNotDisabled:
		result = "Turn the crystal change key to 'Disabled,' clockwise.";
		break;
	default:
		result = "";
	}

	return result;
}

void BioXASMainMonochromatorCrystalChangeControl::setMono(BioXASMainMonochromator *newMono)
{
    if (mono_ != newMono) {

        if (mono_)
            disconnect( mono_, 0, this, 0 );

        mono_ = newMono;

        if (mono_) {
			connect( mono_, SIGNAL(connected(bool)), this, SLOT(onMonoConditionChanged()) );
			connect( mono_, SIGNAL(slitsClosedStatusChanged(bool)), this, SLOT(onMonoConditionChanged()) );
			connect( mono_, SIGNAL(paddleOutStatusChanged(bool)), this, SLOT(onMonoConditionChanged()) );
			connect( mono_, SIGNAL(keyEnabledStatusChanged(bool)), this, SLOT(onMonoConditionChanged()) );
			connect( mono_, SIGNAL(braggMotorAtCrystalChangePositionStatusChanged(bool)), this, SLOT(onMonoConditionChanged()) );
			connect( mono_, SIGNAL(crystalChangeBrakeEnabledChanged(bool)), this, SLOT(onMonoConditionChanged()) );
			connect( mono_, SIGNAL(crystalChangeMotorCCWLimitStatusChanged(bool)), this, SLOT(onMonoConditionChanged()) );
			connect( mono_, SIGNAL(crystalChangeMotorCWLimitStatusChanged(bool)), this, SLOT(onMonoConditionChanged()) );
			connect( mono_, SIGNAL(regionChanged(BioXASMainMonochromator::Region)), this, SLOT(onMonoConditionChanged()) );
        }

        emit monoChanged(mono_);

		onMonoConditionChanged();
    }
}

void BioXASMainMonochromatorCrystalChangeControl::setBraggMotorCrystalChangePosition(double newPosition)
{
	if (braggMotorCrystalChangePosition_ != newPosition) {
		braggMotorCrystalChangePosition_ = newPosition;
		braggMotorRegionADestination_ = SETPOINT_BRAGG_MOTOR_REGION_A_DESTINATION + (braggMotorCrystalChangePosition_ - SETPOINT_BRAGG_MOTOR_CRYSTAL_CHANGE_POSITION);
		braggMotorRegionBDestination_ = SETPOINT_BRAGG_MOTOR_REGION_B_DESTINATION + (braggMotorCrystalChangePosition_ - SETPOINT_BRAGG_MOTOR_CRYSTAL_CHANGE_POSITION);

		if (nextRegion_ == BioXASMainMonochromator::B)
			setBraggMotorRegionDestination(braggMotorRegionBDestination_);
		else
			setBraggMotorRegionDestination(braggMotorRegionADestination_);

		emit braggMotorCrystalChangePositionChanged(braggMotorCrystalChangePosition_);
	}
}

void BioXASMainMonochromatorCrystalChangeControl::setNextRegion(BioXASMainMonochromator::Region newRegion)
{
	if (nextRegion_ != newRegion) {
		nextRegion_ = newRegion;

		if (nextRegion_ == BioXASMainMonochromator::A)
			setBraggMotorRegionDestination(braggMotorRegionADestination_);
		else if (nextRegion_ == BioXASMainMonochromator::B)
			setBraggMotorRegionDestination(braggMotorRegionBDestination_);

		emit nextRegionChanged(nextRegion_);
	}
}

void BioXASMainMonochromatorCrystalChangeControl::startCrystalChange()
{
	AMListAction3 *crystalChangeAction = qobject_cast<AMListAction3*>(createCrystalChangeAction());

	if (crystalChangeAction) {

		// Connect to action signals.
		connect( crystalChangeAction, SIGNAL(started()), this, SLOT(onCrystalChangeStarted()) );
		connect( crystalChangeAction, SIGNAL(succeeded()), this, SLOT(onCrystalChangeSucceeded()) );
		connect( crystalChangeAction, SIGNAL(cancelled()), this, SLOT(onCrystalChangeFailed()) );
		connect( crystalChangeAction, SIGNAL(failed()), this, SLOT(onCrystalChangeFailed()) );

		// Begin crystal change.
		crystalChangeAction->start();
	}
}

void BioXASMainMonochromatorCrystalChangeControl::setStep(Step newStep)
{
	if (step_ != newStep) {
		step_ = newStep;
		emit stepChanged(step_);
	}
}

void BioXASMainMonochromatorCrystalChangeControl::setBraggMotorRegionDestination(double newPosition)
{
	if (braggMotorRegionDestination_ != newPosition) {
		braggMotorRegionDestination_ = newPosition;
		emit braggMotorRegionDestinationChanged(braggMotorRegionDestination_);
	}
}

void BioXASMainMonochromatorCrystalChangeControl::onMonoConditionChanged()
{
	setStep(findCurrentStep());
}

AMAction3* BioXASMainMonochromatorCrystalChangeControl::createWaitForSlitsClosedAction()
{
	AMAction3 *action = 0;

	if (mono_ && mono_->slitsClosedControl()->isConnected()) {
		AMControlInfo setpoint = mono_->slitsClosedControl()->toInfo();
		setpoint.setValue(1);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_SLITS_CLOSED, AMControlWaitActionInfo::MatchEqual), mono_->slitsClosedControl());
	}

	return action;
}

AMAction3* BioXASMainMonochromatorCrystalChangeControl::createWaitForPaddleRemovedAction()
{
	AMAction3 *action = 0;

	if (mono_ && mono_->phosphorPaddleOutControl() && mono_->phosphorPaddleOutControl()->isConnected()) {
		AMControlInfo setpoint = mono_->phosphorPaddleOutControl()->toInfo();
		setpoint.setValue(1);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_PADDLE_OUT, AMControlWaitActionInfo::MatchEqual), mono_->phosphorPaddleOutControl());
	}

	return action;
}

AMAction3* BioXASMainMonochromatorCrystalChangeControl::createWaitForKeyEnabledAction()
{
	AMAction3 *action = 0;

	if (mono_ && mono_->keyStatusControl() && mono_->keyStatusControl()->isConnected()) {
		AMControlInfo setpoint = mono_->keyStatusControl()->toInfo();
		setpoint.setValue(1);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_KEY_STATUS_CHANGE, AMControlWaitActionInfo::MatchEqual), mono_->keyStatusControl());
	}

	return action;
}

AMAction3* BioXASMainMonochromatorCrystalChangeControl::createMoveToCrystalChangePositionAction()
{
	AMAction3 *action = 0;

	if (mono_) {
		action = mono_->createMoveBraggMotorAction(SETPOINT_BRAGG_MOTOR_CRYSTAL_CHANGE_POSITION);
	}

	return action;
}

AMAction3* BioXASMainMonochromatorCrystalChangeControl::createWaitForAtCrystalChangePositionAction()
{
	AMAction3 *action = 0;

	if (mono_ && mono_->braggMotorAtCrystalChangePositionStatusControl() && mono_->braggMotorAtCrystalChangePositionStatusControl()->isConnected()) {
		AMControlInfo setpoint = mono_->braggMotorAtCrystalChangePositionStatusControl()->toInfo();
		setpoint.setValue(1);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_CRYSTAL_CHANGE_POSITION_REACHED, AMControlWaitActionInfo::MatchEqual), mono_->braggMotorAtCrystalChangePositionStatusControl());
	}

	return action;
}

AMAction3* BioXASMainMonochromatorCrystalChangeControl::createWaitForBrakeDisabledAction()
{
	AMAction3 *action = 0;

	if (mono_ && mono_->brakeStatusControl() && mono_->brakeStatusControl()->isConnected()) {
		AMControlInfo setpoint = mono_->brakeStatusControl()->toInfo();
		setpoint.setValue(1);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_BRAKE_STATUS_CHANGE, AMControlWaitActionInfo::MatchEqual), mono_->brakeStatusControl());
	}

	return action;
}

AMAction3* BioXASMainMonochromatorCrystalChangeControl::createWaitForCrystalChangeMotorLimitReached(bool cwLimit)
{
	AMAction3 *action = 0;

	if (mono_) {
		if (cwLimit && mono_->crystalChangeMotorCWLimitStatusControl() && mono_->crystalChangeMotorCCWLimitStatusControl()->isConnected()) {
			AMControlInfo setpoint = mono_->crystalChangeMotorCWLimitStatusControl()->toInfo();
			setpoint.setValue(1);

			action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_CRYSTAL_CHANGE_MOTOR_LIMIT_REACHED, AMControlWaitActionInfo::MatchEqual), mono_->crystalChangeMotorCWLimitStatusControl());

		} else if (!cwLimit && mono_->crystalChangeMotorCCWLimitStatusControl() && mono_->crystalChangeMotorCCWLimitStatusControl()->isConnected()){
			AMControlInfo setpoint = mono_->crystalChangeMotorCCWLimitStatusControl()->toInfo();
			setpoint.setValue(1);

			action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_CRYSTAL_CHANGE_MOTOR_LIMIT_REACHED, AMControlWaitActionInfo::MatchEqual), mono_->crystalChangeMotorCCWLimitStatusControl());
		}
	}

	return action;
}

AMAction3* BioXASMainMonochromatorCrystalChangeControl::createWaitForBrakeEnabledAction()
{
	AMAction3 *action = 0;

	if (mono_ && mono_->brakeStatusControl() && mono_->brakeStatusControl()->isConnected()) {
		AMControlInfo setpoint = mono_->brakeStatusControl()->toInfo();
		setpoint.setValue(0);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_BRAKE_STATUS_CHANGE, AMControlWaitActionInfo::MatchEqual), mono_->brakeStatusControl());
	}

	return action;
}

AMAction3* BioXASMainMonochromatorCrystalChangeControl::createWaitForBraggMotorLimitReachedAction(bool cwLimit)
{
	AMAction3 *action = 0;

	if (mono_) {
		if (cwLimit && mono_->braggMotorAtCWLimitStatusControl() && mono_->braggMotorAtCWLimitStatusControl()->isConnected()) {
			AMControlInfo setpoint = mono_->braggMotorAtCWLimitStatusControl()->toInfo();
			setpoint.setValue(0);

			action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_BRAGG_MOTOR_LIMIT_REACHED, AMControlWaitActionInfo::MatchEqual), mono_->braggMotorAtCWLimitStatusControl());

		} else if (!cwLimit && mono_->braggMotorAtCCWLimitStatusControl() && mono_->braggMotorAtCCWLimitStatusControl()->isConnected()) {
			AMControlInfo setpoint = mono_->braggMotorAtCCWLimitStatusControl()->toInfo();
			setpoint.setValue(0);

			action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_BRAGG_MOTOR_LIMIT_REACHED, AMControlWaitActionInfo::MatchEqual), mono_->braggMotorAtCCWLimitStatusControl());
		}
	}

	return action;
}

AMAction3* BioXASMainMonochromatorCrystalChangeControl::createWaitForMoveToNewRegion(BioXASMainMonochromator::Region destinationRegion)
{
	AMAction3 *action = 0;

	if (mono_) {
		if (destinationRegion == BioXASMainMonochromator::A && mono_->regionAStatusControl() && mono_->regionAStatusControl()->isConnected()) {
			AMControlInfo setpoint = mono_->regionAStatusControl()->toInfo();
			setpoint.setValue(1);

			action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_REGION_STATE_CHANGED, AMControlWaitActionInfo::MatchEqual), mono_->regionAStatusControl());

		} else if (destinationRegion == BioXASMainMonochromator::B && mono_->regionBStatusControl() && mono_->regionBStatusControl()->isConnected()) {
			AMControlInfo setpoint = mono_->regionBStatusControl()->toInfo();
			setpoint.setValue(1);

			action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_REGION_STATE_CHANGED, AMControlWaitActionInfo::MatchEqual), mono_->regionBStatusControl());
		}
	}

	return action;
}

AMAction3* BioXASMainMonochromatorCrystalChangeControl::createWaitForKeyDisabledAction()
{
	AMAction3 *action = 0;

	if (mono_) {
		AMControlInfo setpoint = mono_->keyStatusControl()->toInfo();
		setpoint.setValue(0);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_KEY_STATUS_CHANGE, AMControlWaitActionInfo::MatchEqual), mono_->keyStatusControl());
	}

	return action;
}

AMAction3* BioXASMainMonochromatorCrystalChangeControl::createCrystalChangeAction(BioXASMainMonochromator::Region newRegion)
{
	AMListAction3 *action = 0;

	if (mono_ && mono_->isConnected() && mono_->region() != newRegion && newRegion != BioXASMainMonochromator::None) {

		double crystalChangeMotorDestination;
		double braggMotorDestination;

		if (newRegion == BioXASMainMonochromator::A) {
			crystalChangeMotorDestination = SETPOINT_CRYSTAL_CHANGE_MOTOR_REGION_A_DESTINATION;
			braggMotorDestination = SETPOINT_BRAGG_MOTOR_REGION_A_DESTINATION;

		} else {
			crystalChangeMotorDestination = SETPOINT_CRYSTAL_CHANGE_MOTOR_REGION_B_DESTINATION;
			braggMotorDestination = SETPOINT_BRAGG_MOTOR_REGION_B_DESTINATION;
		}

		action = new AMListAction3(new AMListActionInfo3("Crystal Change Action", "Crystal Change Action"));
		action->addSubAction(mono_->createCloseSlitsAction());
		action->addSubAction(createWaitForSlitsClosedAction());
		action->addSubAction(mono_->createRemovePaddleAction());
		action->addSubAction(createWaitForPaddleRemovedAction());
		action->addSubAction(createWaitForKeyEnabledAction());
		action->addSubAction(createMoveToCrystalChangePositionAction());
		action->addSubAction(createWaitForAtCrystalChangePositionAction());
		action->addSubAction(createWaitForBrakeDisabledAction());
		action->addSubAction(mono_->createMoveCrystalChangeMotorAction(crystalChangeMotorDestination));
		action->addSubAction(createWaitForCrystalChangeMotorLimitReached(newRegion == BioXASMainMonochromator::A));
		action->addSubAction(createWaitForBrakeEnabledAction());
		action->addSubAction(mono_->createMoveBraggMotorAction(braggMotorDestination));
		action->addSubAction(createWaitForMoveToNewRegion(newRegion));
		action->addSubAction(createWaitForKeyDisabledAction());
	}

	return action;
}

AMAction3* BioXASMainMonochromatorCrystalChangeControl::createCrystalChangeAction()
{
	AMAction3 *action = 0;

	action = createCrystalChangeAction(nextRegion_);

	return action;
}

BioXASMainMonochromatorCrystalChangeControl::Step BioXASMainMonochromatorCrystalChangeControl::nextStep(BioXASMainMonochromatorCrystalChangeControl::Step step)
{
	BioXASMainMonochromatorCrystalChangeControl::Step nextStep = None;

	if (mono_) {

		switch (step) {
		case None:
			if (mono_->isConnected())
				nextStep = SlitsNotClosed;
			else
				nextStep = None;
			break;

		case SlitsNotClosed:
			if (mono_->slitsClosed())
				nextStep = SlitsClosed;
			else
				nextStep = SlitsNotClosed;
			break;

		case SlitsClosed:
			if (mono_->paddleOut())
				nextStep = PaddleOut;
			else
				nextStep = PaddleNotOut;
			break;

		case PaddleNotOut:
			if (mono_->paddleOut())
				nextStep = PaddleOut;
			else
				nextStep = PaddleNotOut;
			break;

		case PaddleOut:
			if (mono_->keyStatusEnabled())
				nextStep = KeyEnabled;
			else
				nextStep = KeyNotEnabled;
			break;

		case KeyNotEnabled:
			if (mono_->keyStatusEnabled())
				nextStep = KeyEnabled;
			else
				nextStep = KeyNotEnabled;
			break;

		case KeyEnabled:
			if (mono_->atCrystalChangePosition())
				nextStep = BraggAtCrystalChangePosition;
			else
				nextStep = BraggNotAtCrystalChangePosition;
			break;

		case BraggNotAtCrystalChangePosition:
			if (mono_->atCrystalChangePosition())
				nextStep = BraggAtCrystalChangePosition;
			else
				nextStep = BraggNotAtCrystalChangePosition;
			break;

		case BraggAtCrystalChangePosition:
			if (!mono_->brakeStatusEnabled())
				nextStep = BrakeDisabled;
			else
				nextStep = BrakeNotDisabled;
			break;

		case BrakeNotDisabled:
			if (!mono_->brakeStatusEnabled())
				nextStep = BrakeDisabled;
			else
				nextStep = BrakeNotDisabled;
			break;

		case BrakeDisabled:
			if ((crystalChangeMotorLimit_ == CW) && mono_->crystalChangeMotorAtCWLimit())
				nextStep = CrystalChangeMotorAtCWLimit;
			else if ((crystalChangeMotorLimit_ == CW) && !mono_->crystalChangeMotorAtCWLimit())
				nextStep = CrystalChangeMotorNotAtCWLimit;
			else if ((crystalChangeMotorLimit_ == CCW) && mono_->crystalChangeMotorAtCCWLimit())
				nextStep = CrystalChangeMotorAtCCWLimit;
			else if ((crystalChangeMotorLimit_ == CCW) && !mono_->crystalChangeMotorAtCCWLimit())
				nextStep = CrystalChangeMotorNotAtCCWLimit;
			else
				nextStep = BrakeDisabled;
			break;

		case CrystalChangeMotorNotAtCWLimit:
			if (mono_->crystalChangeMotorAtCWLimit())
				nextStep = CrystalChangeMotorAtCWLimit;
			else
				nextStep = CrystalChangeMotorNotAtCWLimit;
			break;

		case CrystalChangeMotorNotAtCCWLimit:
			if (mono_->crystalChangeMotorAtCCWLimit())
				nextStep = CrystalChangeMotorAtCCWLimit;
			else
				nextStep = CrystalChangeMotorNotAtCCWLimit;
			break;

		case CrystalChangeMotorAtCWLimit:
			if (mono_->brakeStatusEnabled())
				nextStep = BrakeEnabled;
			else
				nextStep = BrakeNotEnabled;
			break;

		case CrystalChangeMotorAtCCWLimit:
			if (mono_->brakeStatusEnabled())
				nextStep = BrakeEnabled;
			else
				nextStep = BrakeNotEnabled;
			break;

		case BrakeNotEnabled:
			if (mono_->brakeStatusEnabled())
				nextStep = BrakeEnabled;
			else
				nextStep = BrakeNotEnabled;
			break;

		case BrakeEnabled:
			if (mono_->region() == nextRegion_)
				nextStep = BraggAtNewRegion;
			else
				nextStep = BraggNotAtNewRegion;
			break;

		case BraggNotAtNewRegion:
			if (mono_->region() == nextRegion_)
				nextStep = BraggAtNewRegion;
			else
				nextStep = BraggNotAtNewRegion;
			break;

		case BraggAtNewRegion:
			if (mono_->keyStatusEnabled())
				nextStep = KeyNotDisabled;
			else
				nextStep = KeyDisabled;
			break;

		case KeyNotDisabled:
			if (!mono_->keyStatusEnabled())
				nextStep = KeyDisabled;
			else
				nextStep = KeyNotDisabled;
			break;

		default:
			nextStep = None;

		}
	}

	return nextStep;
}

BioXASMainMonochromatorCrystalChangeControl::Step BioXASMainMonochromatorCrystalChangeControl::findCurrentStep()
{
	// Initialize local variables.

	BioXASMainMonochromatorCrystalChangeControl::Step step = None;
	bool stepFound = false;
	int tries = 0;
	int maxTries = 20;

	// Starting with the base step of 'None', keep trying to find the next step until the next step
	// and the initial step are equal--the signal that we are as far as we can go in the crystal
	// change. This step is the global current step.

	while (!stepFound && tries < maxTries) {
		BioXASMainMonochromatorCrystalChangeControl::Step next = nextStep(step);
		if (next == step)
			stepFound = true;
		else
			step = next;

		tries++;
	}

	if (!stepFound)
		qDebug() << "Ran out of tries. The current crystal change step could not be found properly in" << maxTries << "tries.";

	return step;
}

void BioXASMainMonochromatorCrystalChangeControl::deleteAction(QObject *crystalChangeAction)
{
    disconnect( crystalChangeAction, 0, this, 0 );
    crystalChangeAction->deleteLater();
}
