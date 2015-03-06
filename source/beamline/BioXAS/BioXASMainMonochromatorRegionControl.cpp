#include "BioXASMainMonochromatorRegionControl.h"
#include "source/beamline/BioXAS/BioXASMainBeamline.h"
#include <QDebug>

BioXASMainMonochromatorRegionControl::BioXASMainMonochromatorRegionControl(QObject *parent) :
	BioXASSSRLMonochromatorRegionControl(parent)
{
	// Create controls.

	upperSlitMotor_ = new AMPVControl("Upper slit motor control", "SMTR1607-5-I21-09:mm:fbk", "SMTR1607-5-I21-09:mm", QString(), this);
	lowerSlitMotor_ = new AMPVControl("Lower slit motor control", "SMTR1607-5-I21-10:mm:fbk", "SMTR1607-5-I21-10:mm", QString(), this);
	slitsStatus_ = new AMReadOnlyPVControl("Slits status control", "BL1607-5-I21:Mono:SlitsClosed", this);
	paddleMotor_ = new AMPVControl("Paddle motor control", "SMTR1607-5-I21-11:mm:fbk", "SMTR1607-5-I21-11:mm", QString(), this);
	paddleStatus_ = new AMReadOnlyPVControl("PhosphorPaddleOut", "BL1607-5-I21:Mono:PaddleExtracted", this);
	keyStatus_ = new AMReadOnlyPVControl("CrystalChangeEnabled", "BL1607-5-I21:Mono:KeyStatus", this);
	braggMotor_ = new AMPVControl("Bragg motor control", "SMTR1607-5-I21-12:deg:fbk", "SMTR1607-5-I21-12:deg", QString(), this);
	braggAtCrystalChangePositionStatus_ = new AMReadOnlyPVControl("AtCrystalChangePosition", "BL1607-5-I21:Mono:XtalChangePos", this);
	brakeStatus_ = new AMReadOnlyPVControl("BrakeEnabled", "BL1607-5-I21:Mono:BrakeOff", this);
	crystalChangeMotor_ = new AMPVControl("CrystalChange", "SMTR1607-5-I21-22:mm:fbk", "SMTR1607-5-I21-22:mm", QString(), this);
	crystalChangeCWLimitStatus_ = new AMReadOnlyPVControl("CrystalChangeCWLimit", "SMTR1607-5-I21-22:cw", this);
	crystalChangeCCWLimitStatus_ = new AMReadOnlyPVControl("CrystalChangeCCWLimit", "SMTR1607-5-I21-22:ccw", this);
	regionAStatus_ = new AMReadOnlyPVControl("RegionAStatus", "BL1607-5-I21:Mono:Region:A", this);
	regionBStatus_ = new AMReadOnlyPVControl("RegionBStatus", "BL1607-5-I21:Mono:Region:B", this);

	addChildControl(upperSlitMotor_);
	addChildControl(lowerSlitMotor_);
	addChildControl(slitsStatus_);
	addChildControl(paddleMotor_);
	addChildControl(paddleStatus_);
	addChildControl(keyStatus_);
	addChildControl(braggMotor_);
	addChildControl(braggAtCrystalChangePositionStatus_);
	addChildControl(brakeStatus_);
	addChildControl(crystalChangeMotor_);
	addChildControl(crystalChangeCWLimitStatus_);
	addChildControl(crystalChangeCCWLimitStatus_);
	addChildControl(regionAStatus_);
	addChildControl(regionBStatus_);

	// Make connections.

	connect( regionAStatus_, SIGNAL(valueChanged(double)), this, SLOT(onRegionControlValueChanged()) );
	connect( regionBStatus_, SIGNAL(valueChanged(double)), this, SLOT(onRegionControlValueChanged()) );
}

BioXASMainMonochromatorRegionControl::~BioXASMainMonochromatorRegionControl()
{

}

AMControl::FailureExplanation BioXASMainMonochromatorRegionControl::move(double setpoint)
{
	// Setup local vars.

	int newRegion = (int)setpoint;
	AMListAction3 *action = 0;
	AMControl::FailureExplanation failMessage;

	if (isConnected()) {

		// Resolve the given setpoint to a Region, create the appropriate action.

		if (newRegion == BioXASSSRLMonochromator::Region::A)
			action = createChangeRegionAction(BioXASSSRLMonochromator::Region::A);
		else if (newRegion == BioXASSSRLMonochromator::Region::B)
			action = createChangeRegionAction(BioXASSSRLMonochromator::Region::B);

		// If a valid action was generated, make connections and run it.

		if (action) {
			connect( action, SIGNAL(progressChanged(double, double)), this, SIGNAL(moveProgressChanged(double, double)) );
			connect( action, SIGNAL(currentSubActionChanged(int)), this, SLOT(onCurrentMoveStepChanged(int)) );
			connect( action, SIGNAL(started()), this, SLOT(onRegionChangeStarted()) );
			connect( action, SIGNAL(cancelled()), this, SLOT(onRegionChangeCancelled()) );
			connect( action, SIGNAL(failed()), this, SLOT(onRegionChangeFailed()) );
			connect( action, SIGNAL(succeeded()), this, SLOT(onRegionChangeSucceeded()) );

			action->start();

			failMessage = AMControl::NoFailure;

		} else {

			failMessage = AMControl::LimitFailure;
		}

	} else {

		failMessage = AMControl::NotConnectedFailure;
	}

	return failMessage;
}

void BioXASMainMonochromatorRegionControl::onRegionControlValueChanged()
{
	BioXASSSRLMonochromator::Region::State newRegion;

	int regionAVal = (int)regionAStatus_->value();
	int regionBVal = (int)regionBStatus_->value();

	if (regionAVal == BioXASSSRLMonochromator::Region::NotIn && regionBVal == BioXASSSRLMonochromator::Region::In)
		newRegion = BioXASSSRLMonochromator::Region::B;
	else if (regionAVal == BioXASSSRLMonochromator::Region::In && regionBVal == BioXASSSRLMonochromator::Region::NotIn)
		newRegion = BioXASSSRLMonochromator::Region::A;
	else
		newRegion = BioXASSSRLMonochromator::Region::None;

	if (value_ != newRegion) {
		value_ = newRegion;
		emit valueChanged(value_);
	}
}

AMAction3* BioXASMainMonochromatorRegionControl::createCloseUpperSlitAction()
{
	AMAction3 *action = 0;

	if (upperSlitMotor_ && upperSlitMotor_->isConnected())
		action = AMActionSupport::buildControlMoveAction(upperSlitMotor_, SETPOINT_SLIT_CLOSED);

	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createCloseLowerSlitAction()
{
	AMAction3 *action = 0;

	if (lowerSlitMotor_ && lowerSlitMotor_->isConnected())
		action = AMActionSupport::buildControlMoveAction(lowerSlitMotor_, SETPOINT_SLIT_CLOSED);

	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createCloseSlitsAction()
{
	AMListAction3 *closeSlits = new AMListAction3(new AMListActionInfo3("CloseSlits", "Close mono slits"), AMListAction3::Parallel);
	closeSlits->addSubAction(createCloseUpperSlitAction());
	closeSlits->addSubAction(createCloseLowerSlitAction());

	return closeSlits;
}

AMAction3* BioXASMainMonochromatorRegionControl::createWaitForSlitsClosedAction()
{
	AMAction3 *action = 0;

	if (slitsStatus_ && slitsStatus_->isConnected()) {
		AMControlInfo setpoint = slitsStatus_->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::Slits::Closed);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_SLITS_CLOSED, AMControlWaitActionInfo::MatchEqual), slitsStatus_);
	}

	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createRemovePaddleAction()
{
	AMAction3 *action = 0;

	if (paddleMotor_ && paddleMotor_->isConnected())
		action = AMActionSupport::buildControlMoveAction(paddleMotor_, SETPOINT_PADDLE_REMOVED);

	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createWaitForPaddleRemovedAction()
{
	AMAction3 *action = 0;

	if (paddleStatus_ && paddleStatus_->isConnected()) {
		AMControlInfo setpoint = paddleStatus_->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::Paddle::Out);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_PADDLE_OUT, AMControlWaitActionInfo::MatchEqual), paddleStatus_);
	}

	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createWaitForKeyEnabledAction()
{
	AMAction3 *action = 0;

	if (keyStatus_ && keyStatus_->isConnected()) {
		AMControlInfo setpoint = keyStatus_->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::Key::Enabled);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_KEY_STATUS_CHANGE, AMControlWaitActionInfo::MatchEqual), keyStatus_);
	}

	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createMoveBraggAction(double destination)
{
	AMAction3 *action = 0;

	if (braggMotor_ && braggMotor_->isConnected())
		action = AMActionSupport::buildControlMoveAction(braggMotor_, destination);

	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createMoveBraggToCrystalChangePositionAction()
{
	return createMoveBraggAction(SETPOINT_BRAGG_MOTOR_CRYSTAL_CHANGE_POSITION);
}

AMAction3* BioXASMainMonochromatorRegionControl::createWaitForBraggAtCrystalChangePositionAction()
{
	AMAction3 *action = 0;

	if (braggAtCrystalChangePositionStatus_ && braggAtCrystalChangePositionStatus_->isConnected()) {
		AMControlInfo setpoint = braggAtCrystalChangePositionStatus_->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::Bragg::InPosition);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_BRAGG_MOTOR_LIMIT_REACHED, AMControlWaitActionInfo::MatchEqual), braggAtCrystalChangePositionStatus_);

	} else {
		qDebug() << "Action was not created: wait for bragg at crystal change position.";
	}

	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createWaitForBrakeDisabledAction()
{
	AMAction3 *action = 0;

	if (brakeStatus_ && brakeStatus_->isConnected()) {
		AMControlInfo setpoint = brakeStatus_->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::Brake::Disabled);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_BRAKE_STATUS_CHANGE, AMControlWaitActionInfo::MatchEqual), brakeStatus_);
	}

	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createMoveCrystalChangeAction(double destination)
{
	AMAction3 *action = 0;

	if (crystalChangeMotor_ && crystalChangeMotor_->isConnected())
		action = AMActionSupport::buildControlMoveAction(crystalChangeMotor_, destination);

	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createMoveCrystalChangeToRegionLimitAction(int region)
{
	AMAction3 *action = 0;

	if (region == BioXASSSRLMonochromator::Region::A)
		action = createMoveCrystalChangeAction(SETPOINT_CRYSTAL_CHANGE_MOTOR_REGION_A_DESTINATION);
	else if (region == BioXASSSRLMonochromator::Region::B)
		action = createMoveCrystalChangeAction(SETPOINT_CRYSTAL_CHANGE_MOTOR_REGION_B_DESTINATION);

	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createWaitForCrystalChangeAtCWLimitAction()
{
	AMAction3 *action = 0;

	if (crystalChangeCWLimitStatus_ && crystalChangeCWLimitStatus_->isConnected()) {
		AMControlInfo setpoint = crystalChangeCWLimitStatus_->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::CrystalChange::AtLimit);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_CRYSTAL_CHANGE_MOTOR_LIMIT_REACHED, AMControlWaitActionInfo::MatchEqual), crystalChangeCWLimitStatus_);
	}

	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createWaitForCrystalChangeAtCCWLimitAction()
{
	AMAction3 *action = 0;

	if (crystalChangeCCWLimitStatus_ && crystalChangeCCWLimitStatus_->isConnected()) {
		AMControlInfo setpoint = crystalChangeCCWLimitStatus_->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::CrystalChange::AtLimit);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_CRYSTAL_CHANGE_MOTOR_LIMIT_REACHED, AMControlWaitActionInfo::MatchEqual), crystalChangeCCWLimitStatus_);
	}

	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createWaitForCrystalChangeAtRegionLimitAction(int region)
{
	AMAction3 *action = 0;

	if (region == BioXASSSRLMonochromator::Region::A)
		action = createWaitForCrystalChangeAtCWLimitAction();
	else if (region == BioXASSSRLMonochromator::Region::B)
		action = createWaitForCrystalChangeAtCCWLimitAction();

	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createWaitForBrakeEnabledAction()
{
	AMAction3 *action = 0;

	if (brakeStatus_ && brakeStatus_->isConnected()) {
		AMControlInfo setpoint = brakeStatus_->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::Brake::Enabled);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_BRAKE_STATUS_CHANGE, AMControlWaitActionInfo::MatchEqual), brakeStatus_);
	}

	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createMoveBraggToRegionAction(int region)
{
	AMAction3 *action = 0;

	if (region == BioXASSSRLMonochromator::Region::A)
		action = createMoveBraggAction(SETPOINT_BRAGG_MOTOR_REGION_A_DESTINATION);
	else if (region == BioXASSSRLMonochromator::Region::B)
		action = createMoveBraggAction(SETPOINT_BRAGG_MOTOR_REGION_B_DESTINATION);

	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createWaitForKeyDisabledAction()
{
	AMAction3 *action = 0;

	if (keyStatus_ && keyStatus_->isConnected()) {
		AMControlInfo setpoint = keyStatus_->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::Key::Disabled);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_KEY_STATUS_CHANGE, AMControlWaitActionInfo::MatchEqual), keyStatus_);
	}

	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createWaitForRegionChangedToAAction()
{
	AMAction3 *action = 0;

	if (regionAStatus_ && regionAStatus_->isConnected()) {
		AMControlInfo setpoint = regionAStatus_->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::Region::In);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_REGION_STATE_CHANGED, AMControlWaitActionInfo::MatchEqual), regionAStatus_);
	}

	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createWaitForRegionChangedToBAction()
{
	AMAction3 *action = 0;

	if (regionBStatus_ && regionBStatus_->isConnected()) {
		AMControlInfo setpoint = regionBStatus_->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::Region::In);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_REGION_STATE_CHANGED, AMControlWaitActionInfo::MatchEqual), regionBStatus_);
	}

	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createWaitForRegionChangedAction(int regionDestination)
{
	AMAction3 *action = 0;

	if (regionDestination == BioXASSSRLMonochromator::Region::A)
		action = createWaitForRegionChangedToAAction();
	else if (regionDestination == BioXASSSRLMonochromator::Region::B)
		action = createWaitForRegionChangedToBAction();
	else
		qDebug() << "Failed to create action: wait for region changed.";

	return action;
}

AMListAction3* BioXASMainMonochromatorRegionControl::createChangeRegionAction(int newRegion)
{
	AMListAction3 *action = 0;

	if (value_ != newRegion && validRegion(newRegion)) {
		action = new AMListAction3(new AMListActionInfo3("CrystalChange", "BioXAS Main Mono Crystal Change Action"), AMListAction3::Sequential);
		action->addSubAction(createCloseSlitsAction());
		action->addSubAction(createWaitForSlitsClosedAction());
		action->addSubAction(createRemovePaddleAction());
		action->addSubAction(createWaitForPaddleRemovedAction());
		action->addSubAction(createWaitForKeyEnabledAction());
		action->addSubAction(createMoveBraggToCrystalChangePositionAction());
		action->addSubAction(createWaitForBraggAtCrystalChangePositionAction());
		action->addSubAction(createWaitForBrakeDisabledAction());
		action->addSubAction(createMoveCrystalChangeToRegionLimitAction(newRegion));
		action->addSubAction(createWaitForCrystalChangeAtRegionLimitAction(newRegion));
		action->addSubAction(createWaitForBrakeEnabledAction());
		action->addSubAction(createMoveBraggToRegionAction(newRegion));
		action->addSubAction(createWaitForRegionChangedAction(newRegion));
		action->addSubAction(createWaitForKeyDisabledAction());
	}

	return action;
}


