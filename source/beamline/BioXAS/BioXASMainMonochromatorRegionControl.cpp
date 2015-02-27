#include "BioXASMainMonochromatorRegionControl.h"
#include <QDebug>

BioXASMainMonochromatorRegionControl::BioXASMainMonochromatorRegionControl(QObject *parent) :
	AMCompositeControl("RegionControl", "", parent, "BioXAS SSRL Monochromator Region Control")
{
	// Initialize local variables.

	region_ = Region::None;
	regionSetpoint_ = Region::None;

	// Initialize inherited variables.

	setEnumStates(QStringList() << "None" << "A" << "B");
	setMoveEnumStates(QStringList() << "A" << "B");
	setAllowsMovesWhileMoving(false);
	setContextKnownDescription("Region Control");
}

BioXASMainMonochromatorRegionControl::~BioXASMainMonochromatorRegionControl()
{

}

bool BioXASMainMonochromatorRegionControl::canMeasure() const
{
	if (wasConnected_)
		return true;

	return false;
}

bool BioXASMainMonochromatorRegionControl::canMove() const
{
	if (wasConnected_)
		return true;

	return false;
}

bool BioXASMainMonochromatorRegionControl::canStop() const
{
	if (wasConnected_)
		return true;

	return false;
}

AMControl::FailureExplanation BioXASMainMonochromatorRegionControl::move(double setpoint)
{
	// Setup local vars.

	int newRegion = (int)setpoint;
	AMListAction3 *action = 0;

	// Resolve the given setpoint to a Region.

	if (newRegion == Region::A)
		action = createChangeRegionAction(Region::A);
	else if (newRegion == Region::B)
		action = createChangeRegionAction(Region::B);

	// If a valid action was generated, make connections and run it.

	if (action) {
		connect( action, SIGNAL(progressChanged(double, double)), this, SIGNAL(moveProgressChanged(double, double)) );
		connect( action, SIGNAL(currentSubActionChanged(int)), this, SLOT(onCurrentMoveStepChanged(int)) );
		connect( action, SIGNAL(cancelled()), this, SLOT(onRegionChangeCancelled()) );
		connect( action, SIGNAL(failed()), this, SLOT(onRegionChangeFailed()) );
		connect( action, SIGNAL(succeeded()), this, SLOT(onRegionChangeSucceeded()) );

		action->start();
	}

	return AMControl::NoFailure;
}

AMControl::FailureExplanation BioXASMainMonochromatorRegionControl::moveRelative(double distance, RelativeMoveType relativeMoveType)
{
	Q_UNUSED(distance)
	Q_UNUSED(relativeMoveType)

	qDebug() << "The region control does not support relative moves.";

	return AMControl::OtherFailure;
}

void BioXASMainMonochromatorRegionControl::setUpperSlitControl(AMControl *upperSlit)
{
	if (upperSlitControl_ != upperSlit) {

		if (upperSlitControl_) {
			children_.removeOne(upperSlitControl_);
			upperSlitControl_ = 0;
		}

		upperSlitControl_ = upperSlit;

		if (upperSlitControl_) {
			addChildControl(upperSlitControl_);
		}
	}
}

void BioXASMainMonochromatorRegionControl::setLowerSlitControl(AMControl *lowerSlit)
{
	if (lowerSlitControl_ != lowerSlit) {

		if (lowerSlitControl_) {
			children_.removeOne(lowerSlitControl_);
			lowerSlitControl_ = 0;
		}

		lowerSlitControl_ = lowerSlit;

		if (lowerSlitControl_) {
			addChildControl(lowerSlitControl_);
		}
	}
}

void BioXASMainMonochromatorRegionControl::setSlitsStatusControl(AMControl *slitsClosed)
{
	if (slitsStatus_ != slitsClosed) {

		if (slitsStatus_) {
			children_.removeOne(slitsStatus_);
			slitsStatus_ = 0;
		}

		slitsStatus_ = slitsClosed;

		if (slitsStatus_) {
			addChildControl(slitsStatus_);
		}
	}
}

void BioXASMainMonochromatorRegionControl::setPaddleControl(AMControl *paddleControl)
{
	if (paddleControl_ != paddleControl) {

		if (paddleControl_) {
			children_.removeOne(paddleControl_);
			paddleControl_ = 0;
		}

		paddleControl_ = paddleControl;

		if (paddleControl_) {
			addChildControl(paddleControl_);
		}
	}
}

void BioXASMainMonochromatorRegionControl::setPaddleStatusControl(AMControl *paddleStatus)
{
	if (paddleStatus_ != paddleStatus) {

		if (paddleStatus_) {
			children_.removeOne(paddleStatus_);
			paddleStatus_ = 0;
		}

		paddleStatus_ = paddleStatus;

		if (paddleStatus_) {
			addChildControl(paddleStatus_);
		}
	}
}

void BioXASMainMonochromatorRegionControl::setKeyStatusControl(AMControl *keyStatusControl)
{
	if (keyStatus_ != keyStatusControl) {

		if (keyStatus_) {
			children_.removeOne(keyStatus_);
			keyStatus_ = 0;
		}

		keyStatus_ = keyStatusControl;

		if (keyStatus_) {
			addChildControl(keyStatus_);
		}
	}
}

void BioXASMainMonochromatorRegionControl::setBraggControl(AMControl *braggControl)
{
	if (braggControl_ != braggControl) {

		if (braggControl_) {
			children_.removeOne(braggControl_);
			braggControl_ = 0;
		}

		braggControl_ = braggControl;

		if (braggControl_) {
			addChildControl(braggControl_);
		}
	}
}

void BioXASMainMonochromatorRegionControl::setBraggAtCrystalChangePositionControl(AMControl *braggInPositionControl)
{
	if (braggAtCrystalChangePositionStatus_ != braggInPositionControl) {

		if (braggAtCrystalChangePositionStatus_) {
			children_.removeOne(braggAtCrystalChangePositionStatus_);
			braggAtCrystalChangePositionStatus_ = 0;
		}

		braggAtCrystalChangePositionStatus_ = braggInPositionControl;

		if (braggAtCrystalChangePositionStatus_) {
			addChildControl(braggAtCrystalChangePositionStatus_);
		}
	}
}

void BioXASMainMonochromatorRegionControl::setBrakeStatusControl(AMControl *brakeStatusControl)
{
	if (brakeStatus_ != brakeStatusControl) {

		if (brakeStatus_) {
			children_.removeOne(brakeStatus_);
			brakeStatus_ = 0;
		}

		brakeStatus_ = brakeStatusControl;

		if (brakeStatus_) {
			addChildControl(brakeStatus_);
		}
	}
}

void BioXASMainMonochromatorRegionControl::setCrystalChangeControl(AMControl *crystalChangeControl)
{
	if (crystalChangeControl_ != crystalChangeControl) {

		if (crystalChangeControl_) {
			children_.removeOne(crystalChangeControl_);
			crystalChangeControl_ = 0;
		}

		crystalChangeControl_ = crystalChangeControl;

		if (crystalChangeControl_) {
			addChildControl(crystalChangeControl_);
		}
	}
}

void BioXASMainMonochromatorRegionControl::setCrystalChangeCWLimitControl(AMControl *cwLimitControl)
{
	if (crystalChangeCWLimitStatus_ != cwLimitControl) {

		if (crystalChangeCWLimitStatus_) {
			children_.removeOne(crystalChangeCWLimitStatus_);
			crystalChangeCWLimitStatus_ = 0;
		}

		crystalChangeCWLimitStatus_ = cwLimitControl;

		if (crystalChangeCWLimitStatus_) {
			addChildControl(crystalChangeCWLimitStatus_);
		}
	}
}

void BioXASMainMonochromatorRegionControl::setCrystalChangeCCWLimitControl(AMControl *ccwLimitControl)
{
	if (crystalChangeCCWLimitStatus_ != ccwLimitControl) {

		if (crystalChangeCCWLimitStatus_) {
			children_.removeOne(crystalChangeCCWLimitStatus_);
			crystalChangeCCWLimitStatus_ = 0;
		}

		crystalChangeCCWLimitStatus_ = ccwLimitControl;

		if (crystalChangeCCWLimitStatus_) {
			addChildControl(crystalChangeCCWLimitStatus_);
		}
	}
}

void BioXASMainMonochromatorRegionControl::setRegionAStatusControl(AMControl *regionAStatusControl)
{
	if (regionAStatus_ != regionAStatusControl) {

		if (regionAStatus_) {
			disconnect( regionAStatus_, 0, this, 0 );
			children_.removeOne(regionAStatus_);
			regionAStatus_ = 0;
		}

		regionAStatus_ = regionAStatusControl;

		if (regionAStatus_) {
			addChildControl(regionAStatus_);
			connect( regionAStatus_, SIGNAL(valueChanged(double)), this, SLOT(onRegionControlValueChanged()) );
		}
	}
}

void BioXASMainMonochromatorRegionControl::setRegionBStatusControl(AMControl *regionBStatusControl)
{
	if (regionBStatus_ != regionBStatusControl) {

		if (regionBStatus_) {
			disconnect( regionBStatus_, 0, this, 0 );
			children_.removeOne(regionBStatus_);
			regionBStatus_ = 0;
		}

		regionBStatus_ = regionBStatusControl;

		if (regionBStatus_) {
			addChildControl(regionBStatus_);
			connect( regionBStatus_, SIGNAL(valueChanged(double)), this, SLOT(onRegionControlValueChanged()) );
		}
	}
}

AMAction3* BioXASMainMonochromatorRegionControl::createCloseUpperSlitAction()
{
	if (!upperSlitControl_ || !upperSlitControl_->isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(upperSlitControl_, SETPOINT_SLITS_CLOSED);
}

AMAction3* BioXASMainMonochromatorRegionControl::createCloseLowerSlitAction()
{
	if (!lowerSlitControl_ || lowerSlitControl_->isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(lowerSlitControl_, SETPOINT_SLITS_CLOSED);
}

AMAction3* BioXASMainMonochromatorRegionControl::createCloseSlitsAction()
{
	AMListAction3 *closeSlits = new AMListAction3(new AMListActionInfo3("CloseSlits", "Closing mono slits in preparation for a crystal change"), AMListAction3::Parallel);
	closeSlits->addSubAction(createCloseUpperSlitAction());
	closeSlits->addSubAction(createCloseLowerSlitAction());

	return closeSlits;
}

AMAction3* BioXASMainMonochromatorRegionControl::createWaitForSlitsClosedAction()
{
	if (!slitsStatus_ || !slitsStatus_->isConnected())
		return 0;

	AMControlInfo setpoint = slitsStatus_->toInfo();
	setpoint.setValue(STATUS_SLITS_CLOSED);

	AMControlWaitAction *action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_SLITS_CLOSED, AMControlWaitActionInfo::MatchEqual), slitsStatus_);
	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createRemovePaddleAction()
{
	if (!paddleControl_ || !paddleStatus_->isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(paddleControl_, SETPOINT_PADDLE_REMOVED);
}

AMAction3* BioXASMainMonochromatorRegionControl::createWaitForPaddleRemovedAction()
{
	if (!paddleStatus_ || !paddleStatus_->isConnected())
		return 0;

	AMControlInfo setpoint = paddleStatus_->toInfo();
	setpoint.setValue(STATUS_PADDLE_OUT);

	AMControlWaitAction *action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_PADDLE_OUT, AMControlWaitActionInfo::MatchEqual), paddleStatus_);
	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createWaitForKeyEnabledAction()
{
	if (!keyStatus_ || !keyStatus_->isConnected())
		return 0;

	AMControlInfo setpoint = keyStatus_->toInfo();
	setpoint.setValue(STATUS_KEY_ENABLED);

	AMControlWaitAction *action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_KEY_STATUS_CHANGE, AMControlWaitActionInfo::MatchEqual), keyStatus_);
	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createMoveBraggAction(double destination)
{
	if (!braggControl_ || !braggControl_->isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(braggControl_, destination);
}

AMAction3* BioXASMainMonochromatorRegionControl::createMoveBraggToCrystalChangePositionAction()
{
	return createMoveBraggAction(SETPOINT_BRAGG_MOTOR_CRYSTAL_CHANGE_POSITION);
}

AMAction3* BioXASMainMonochromatorRegionControl::createWaitForBraggAtCrystalChangePositionAction()
{
	if (!braggAtCrystalChangePositionStatus_ || !braggAtCrystalChangePositionStatus_->isConnected())
		return 0;

	AMControlInfo setpoint = braggAtCrystalChangePositionStatus_->toInfo();
	setpoint.setValue(STATUS_BRAGG_AT_CRYSTAL_CHANGE_POSITION);

	AMControlWaitAction *action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_CRYSTAL_CHANGE_POSITION_REACHED, AMControlWaitActionInfo::MatchEqual), braggAtCrystalChangePositionStatus_);
	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createWaitForBrakeDisabledAction()
{
	if (!brakeStatus_ || !brakeStatus_->isConnected())
		return 0;

	AMControlInfo setpoint = brakeStatus_->toInfo();
	setpoint.setValue(STATUS_BRAKE_DISABLED);

	AMControlWaitAction *action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_BRAKE_STATUS_CHANGE, AMControlWaitActionInfo::MatchEqual), brakeStatus_);
	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createMoveCrystalChangeAction(double destination)
{
	if (!crystalChangeControl_ || !crystalChangeControl_->isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(crystalChangeControl_, destination);
}

AMAction3* BioXASMainMonochromatorRegionControl::createWaitForCrystalChangeAtCWLimitAction()
{
	if (!crystalChangeCWLimitStatus_ || !crystalChangeCWLimitStatus_->isConnected())
		return 0;

	AMControlInfo setpoint = crystalChangeCWLimitStatus_->toInfo();
	setpoint.setValue(STATUS_CRYSTAL_CHANGE_AT_LIMIT);

	AMControlWaitAction *action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_CRYSTAL_CHANGE_MOTOR_LIMIT_REACHED, AMControlWaitActionInfo::MatchEqual), crystalChangeCWLimitStatus_);
	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createWaitForCrystalChangeAtCCWLimitAction()
{
	if (!crystalChangeCCWLimitStatus_ || !crystalChangeCCWLimitStatus_->isConnected())
		return 0;

	AMControlInfo setpoint = crystalChangeCCWLimitStatus_->toInfo();
	setpoint.setValue(STATUS_CRYSTAL_CHANGE_AT_LIMIT);

	AMControlWaitAction *action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_CRYSTAL_CHANGE_MOTOR_LIMIT_REACHED, AMControlWaitActionInfo::MatchEqual), crystalChangeCCWLimitStatus_);
	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createMoveCrystalChangeToRegionLimitAction(Region::State destination)
{
	AMAction3 *action = 0;

	if (destination == Region::A)
		action = createMoveCrystalChangeAction(SETPOINT_CRYSTAL_CHANGE_MOTOR_REGION_A_DESTINATION);
	else if (destination == Region::B)
		action = createMoveCrystalChangeAction(SETPOINT_CRYSTAL_CHANGE_MOTOR_REGION_B_DESTINATION);

	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createWaitForCrystalChangeAtLimitAction(Region::State destination)
{
	AMAction3 *action = 0;

	if (destination == Region::A)
		action = createWaitForCrystalChangeAtCWLimitAction();
	else if (destination == Region::B)
		action = createWaitForCrystalChangeAtCCWLimitAction();

	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createWaitForBrakeEnabledAction()
{
	if (!brakeStatus_ || !brakeStatus_->isConnected())
		return 0;

	AMControlInfo setpoint = brakeStatus_->toInfo();
	setpoint.setValue(STATUS_BRAKE_ENABLED);

	AMControlWaitAction *action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_BRAKE_STATUS_CHANGE, AMControlWaitActionInfo::MatchEqual), brakeStatus_);
	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createMoveBraggToRegionAAction()
{
	if (!braggControl_ || !braggControl_->isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(braggControl_, SETPOINT_BRAGG_MOTOR_REGION_A_DESTINATION);
}

AMAction3* BioXASMainMonochromatorRegionControl::createMoveBraggToRegionBAction()
{
	if (!braggControl_ || !braggControl_->isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(braggControl_, SETPOINT_BRAGG_MOTOR_REGION_B_DESTINATION);
}

AMAction3* BioXASMainMonochromatorRegionControl::createMoveBraggToRegionAction(Region::State destination)
{
	AMAction3 *action = 0;

	if (destination == Region::A)
		action = createMoveBraggAction(SETPOINT_BRAGG_MOTOR_REGION_A_DESTINATION);
	else if (destination == Region::B)
		action = createMoveBraggAction(SETPOINT_BRAGG_MOTOR_REGION_B_DESTINATION);

	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createWaitForKeyDisabledAction()
{
	if (!keyStatus_ || !keyStatus_->isConnected())
		return 0;

	AMControlInfo setpoint = keyStatus_->toInfo();
	setpoint.setValue(STATUS_KEY_DISABLED);

	AMControlWaitAction *action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_KEY_STATUS_CHANGE, AMControlWaitActionInfo::MatchEqual), keyStatus_);
	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createWaitForRegionChangedToAAction()
{
	if (!regionAStatus_ || !regionAStatus_->isConnected())
		return 0;

	AMControlInfo setpoint = regionAStatus_->toInfo();
	setpoint.setValue(STATUS_REGION_IN);

	AMControlWaitAction *action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_REGION_STATE_CHANGED, AMControlWaitActionInfo::MatchEqual), regionAStatus_);
	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createWaitForRegionChangedToBAction()
{
	if (!regionBStatus_ || !regionBStatus_->isConnected())
		return 0;

	AMControlInfo setpoint = regionBStatus_->toInfo();
	setpoint.setValue(STATUS_REGION_IN);

	AMControlWaitAction *action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_REGION_STATE_CHANGED, AMControlWaitActionInfo::MatchEqual), regionBStatus_);
	return action;
}

AMAction3* BioXASMainMonochromatorRegionControl::createWaitForRegionChangedAction(Region::State destination)
{
	AMAction3 *action = 0;

	if (destination == Region::A)
		action = createWaitForRegionChangedToAAction();
	else if (destination == Region::B)
		action = createWaitForRegionChangedToBAction();

	return action;
}

AMListAction3* BioXASMainMonochromatorRegionControl::createChangeRegionAction(Region::State newRegion)
{
	AMListAction3 *changeRegion = 0;

	if (region_ != newRegion && newRegion != Region::None) {

		changeRegion = new AMListAction3(new AMListActionInfo3("CrystalChange", "BioXAS SSRL Monochromator crystal change action"), AMListAction3::Sequential);
		changeRegion->addSubAction(createCloseSlitsAction());
		changeRegion->addSubAction(createWaitForSlitsClosedAction());
		changeRegion->addSubAction(createRemovePaddleAction());
		changeRegion->addSubAction(createWaitForPaddleRemovedAction());

		changeRegion->addSubAction(createWaitForKeyEnabledAction());

		changeRegion->addSubAction(createMoveBraggToCrystalChangePositionAction());
		changeRegion->addSubAction(createWaitForBraggAtCrystalChangePositionAction());

		changeRegion->addSubAction(createWaitForBrakeDisabledAction());

		changeRegion->addSubAction(createMoveCrystalChangeToRegionLimitAction(newRegion));
		changeRegion->addSubAction(createWaitForCrystalChangeAtLimitAction(newRegion));

		changeRegion->addSubAction(createWaitForBrakeEnabledAction());

		changeRegion->addSubAction(createMoveBraggToRegionAction(newRegion));
		changeRegion->addSubAction(createWaitForRegionChangedAction(newRegion));
	}

	return changeRegion;
}

void BioXASMainMonochromatorRegionControl::onRegionControlValueChanged()
{
	Region::State newRegion;

	int regionAVal = (int)regionAStatus_->value();
	int regionBVal = (int)regionBStatus_->value();

	if (regionAVal == STATUS_REGION_NOT_IN && regionBVal == STATUS_REGION_IN)
		newRegion = Region::B;
	else if (regionAVal == STATUS_REGION_IN && regionBVal == STATUS_REGION_NOT_IN)
		newRegion = Region::A;
	else
		newRegion = Region::None;

	if (region_ != newRegion) {
		region_ = newRegion;
		emit valueChanged(region_);
	}
}

void BioXASMainMonochromatorRegionControl::onCurrentMoveStepChanged(int stepIndex)
{
	Q_UNUSED(stepIndex)
}

void BioXASMainMonochromatorRegionControl::onRegionChangeCancelled()
{
	disconnect( sender(), 0, this, 0 );
	sender()->deleteLater();

	emit moveFailed(AMControl::WasStoppedFailure);
}

void BioXASMainMonochromatorRegionControl::onRegionChangeFailed()
{
	disconnect( sender(), 0, this, 0 );
	sender()->deleteLater();

	emit moveFailed(AMControl::OtherFailure);
}

void BioXASMainMonochromatorRegionControl::onRegionChangeSucceeded()
{
	disconnect( sender(), 0, this, 0 );
	sender()->deleteLater();

	emit moveSucceeded();
}
