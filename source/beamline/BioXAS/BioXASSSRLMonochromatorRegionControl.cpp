#include "BioXASSSRLMonochromatorRegionControl.h"
#include "BioXASSSRLMonochromator.h"
#include <QDebug>

BioXASSSRLMonochromatorRegionControl::BioXASSSRLMonochromatorRegionControl(QObject *parent) :
	AMCompositeControl("RegionControl", "", parent, "BioXAS SSRL Monochromator Region Control")
{
	// Initialize local variables.

	value_ = BioXASSSRLMonochromator::Region::None;
	setpoint_ = BioXASSSRLMonochromator::Region::None;
	moveInProgress_ = false;

	upperSlit_ = 0;
	lowerSlit_ = 0;
	slitsStatus_ = 0;
	paddle_ = 0;
	paddleStatus_ = 0;
	keyStatus_ = 0;
	brakeStatus_ = 0;
	bragg_ = 0;
	braggAtCrystalChangePositionStatus_ = 0;
	crystalChange_ = 0;
	crystalChangeCWLimitStatus_ = 0;
	crystalChangeCCWLimitStatus_ = 0;
	regionAStatus_ = 0;
	regionBStatus_ = 0;

	// Initialize inherited variables.

	setEnumStates(QStringList() << "None" << "A" << "B");
	setMoveEnumStates(QStringList() << "A" << "B");
	setAllowsMovesWhileMoving(false);
	setContextKnownDescription("Region Control");
}

BioXASSSRLMonochromatorRegionControl::~BioXASSSRLMonochromatorRegionControl()
{

}

double BioXASSSRLMonochromatorRegionControl::minimumValue() const
{
	return BioXASSSRLMonochromator::Region::None;
}

double BioXASSSRLMonochromatorRegionControl::maximumValue() const
{
	return BioXASSSRLMonochromator::Region::B;
}

bool BioXASSSRLMonochromatorRegionControl::canMove() const
{
	bool exists = false;
	bool connected = false;
	bool canMove = false;

	exists = (
		upperSlit_ &&
		lowerSlit_ &&
		slitsStatus_ &&
		paddle_ &&
		paddleStatus_ &&
		keyStatus_ &&
		brakeStatus_ &&
		bragg_ &&
		braggAtCrystalChangePositionStatus_ &&
		crystalChange_ &&
		crystalChangeCWLimitStatus_ &&
		crystalChangeCCWLimitStatus_ &&
		regionAStatus_ &&
		regionBStatus_
	);

	if (exists) {
		connected = isConnected();

		canMove = (
			upperSlit_->canMove() &&
			lowerSlit_->canMove() &&
			paddle_->canMove() &&
			bragg_->canMove() &&
			crystalChange_->canMove()
		);
	}

	return (exists && connected && canMove);
}

AMControl::FailureExplanation BioXASSSRLMonochromatorRegionControl::move(double setpoint)
{
	if (!isConnected())
		return AMControl::NotConnectedFailure;

	if (isMoving())
		return AMControl::AlreadyMovingFailure;

	if (!canMove())
		return AMControl::LimitFailure;

	// Now we can assume that the mono is not moving and is valid/connected.

	// Update the saved setpoint value.

	setSetpoint((int)setpoint);

	if (!validRegionSetpoint(setpoint_)) {
		qDebug() << "Destination" << regionStateToString(regionSetpointToState(setpoint_)) << "is not a valid control setpoint.";
		return AMControl::LimitFailure;
	}

	// Resolve the given setpoint to a region state, create the appropriate action.

	AMListAction3 *action = createChangeRegionAction(regionSetpointToState(setpoint_));

	// If a valid action was NOT generated, there must be some problem with one of the child controls.

	if (!action) {
		qDebug() << "Invalid crystal change action generated.";
		return AMControl::LimitFailure;
	}

	// If a valid action was generated, make connections and run it.

	connect( action, SIGNAL(progressChanged(double, double)), this, SIGNAL(moveProgressChanged(double, double)) );
	connect( action, SIGNAL(currentSubActionChanged(int)), this, SLOT(onCurrentMoveStepChanged(int)) );
	connect( action, SIGNAL(started()), this, SLOT(onRegionChangeStarted()) );
	connect( action, SIGNAL(cancelled()), this, SLOT(onRegionChangeCancelled()) );
	connect( action, SIGNAL(failed()), this, SLOT(onRegionChangeFailed()) );
	connect( action, SIGNAL(succeeded()), this, SLOT(onRegionChangeSucceeded()) );

	action->start();

	return AMControl::NoFailure;
}

void BioXASSSRLMonochromatorRegionControl::setUpperSlitControl(AMControl *upperSlit)
{
	if (!upperSlit_ && upperSlit_ != upperSlit) {
		upperSlit_ = upperSlit;
		addChildControl(upperSlit_);

		connect( upperSlit_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)) );
	}
}

void BioXASSSRLMonochromatorRegionControl::setLowerSlitControl(AMControl *lowerSlit)
{
	if (!lowerSlit_ && lowerSlit_ != lowerSlit) {
		lowerSlit_ = lowerSlit;
		addChildControl(lowerSlit_);

		connect( lowerSlit_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)) );
	}
}

void BioXASSSRLMonochromatorRegionControl::setSlitsStatusControl(AMControl *slitsStatus)
{
	if (!slitsStatus_ && slitsStatus_ != slitsStatus) {
		slitsStatus_ = slitsStatus;
		addChildControl(slitsStatus_);

		connect( slitsStatus_, SIGNAL(valueChanged(double)), this, SIGNAL(slitsStatusChanged(double)) );
		connect( slitsStatus_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)) );
	}
}

void BioXASSSRLMonochromatorRegionControl::setPaddleControl(AMControl *paddle)
{
	if (!paddle_ && paddle_ != paddle) {
		paddle_ = paddle;
		addChildControl(paddle_);

		connect( paddle_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)) );
	}
}

void BioXASSSRLMonochromatorRegionControl::setPaddleStatusControl(AMControl *paddleStatus)
{
	if (!paddleStatus_ && paddleStatus_ != paddleStatus) {
		paddleStatus_ = paddleStatus;
		addChildControl(paddleStatus_);

		connect( paddleStatus_, SIGNAL(valueChanged(double)), this, SIGNAL(paddleStatusChanged(double)) );
		connect( paddleStatus_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)) );
	}
}

void BioXASSSRLMonochromatorRegionControl::setKeyStatusControl(AMControl *keyStatus)
{
	if (!keyStatus_ && keyStatus_ != keyStatus) {
		keyStatus_ = keyStatus;
		addChildControl(keyStatus_);

		connect( keyStatus_, SIGNAL(valueChanged(double)), this, SIGNAL(keyStatusChanged(double)) );
		connect( keyStatus_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)) );
	}
}

void BioXASSSRLMonochromatorRegionControl::setBrakeStatusControl(AMControl *brakeStatus)
{
	if (!brakeStatus_ && brakeStatus_ != brakeStatus) {
		brakeStatus_ = brakeStatus;
		addChildControl(brakeStatus_);

		connect( brakeStatus_, SIGNAL(valueChanged(double)), this, SIGNAL(brakeStatusChanged(double)) );
		connect( brakeStatus_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)) );
	}
}

void BioXASSSRLMonochromatorRegionControl::setBraggControl(AMControl *bragg)
{
	if (!bragg_ && bragg_ != bragg) {
		bragg_ = bragg;
		addChildControl(bragg_);

		connect( bragg_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)) );
	}
}

void BioXASSSRLMonochromatorRegionControl::setBraggAtCrystalChangePositionStatusControl(AMControl *inPosition)
{
	if (!braggAtCrystalChangePositionStatus_ && braggAtCrystalChangePositionStatus_ != inPosition) {
		braggAtCrystalChangePositionStatus_ = inPosition;
		addChildControl(braggAtCrystalChangePositionStatus_);

		connect( braggAtCrystalChangePositionStatus_, SIGNAL(valueChanged(double)), this, SIGNAL(braggAtCrystalChangePositionStatusChanged(double)) );
		connect( braggAtCrystalChangePositionStatus_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)) );
	}
}

void BioXASSSRLMonochromatorRegionControl::setCrystalChangeControl(AMControl *crystalChange)
{
	if (!crystalChange_ && crystalChange_ != crystalChange) {
		crystalChange_ = crystalChange;
		addChildControl(crystalChange_);

		connect( crystalChange_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)) );
	}
}

void BioXASSSRLMonochromatorRegionControl::setCrystalChangeCWLimitStatusControl(AMControl *limitStatus)
{
	if (!crystalChangeCWLimitStatus_ && crystalChangeCWLimitStatus_ != limitStatus) {
		crystalChangeCWLimitStatus_ = limitStatus;
		addChildControl(crystalChangeCWLimitStatus_);

		connect( crystalChangeCWLimitStatus_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)) );
	}
}

void BioXASSSRLMonochromatorRegionControl::setCrystalChangeCCWLimitStatusControl(AMControl *limitStatus)
{
	if (!crystalChangeCCWLimitStatus_ && crystalChangeCCWLimitStatus_ != limitStatus) {
		crystalChangeCCWLimitStatus_ = limitStatus;
		addChildControl(crystalChangeCWLimitStatus_);

		connect( crystalChangeCCWLimitStatus_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)) );
	}

}

void BioXASSSRLMonochromatorRegionControl::setRegionAStatusControl(AMControl *regionStatus)
{
	if (!regionAStatus_ && regionAStatus_ != regionStatus) {
		regionAStatus_ = regionStatus;
		addChildControl(regionAStatus_);

		connect( regionAStatus_, SIGNAL(valueChanged(double)), this, SLOT(onRegionControlValueChanged()) );
		connect( regionAStatus_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)) );
	}
}

void BioXASSSRLMonochromatorRegionControl::setRegionBStatusControl(AMControl *regionStatus)
{
	if (!regionBStatus_ && regionBStatus_ != regionStatus) {
		regionBStatus_ = regionStatus;
		addChildControl(regionBStatus_);

		connect( regionBStatus_, SIGNAL(valueChanged(double)), this, SLOT(onRegionControlValueChanged()) );
		connect( regionBStatus_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)) );
	}
}

void BioXASSSRLMonochromatorRegionControl::setValue(int newValue)
{
	if (value_ != newValue) {
		qDebug() << "Updating region control value to " << regionStateToString(newValue);

		value_ = newValue;
		emit valueChanged(value_);
		emit enumChanged();
	}
}

void BioXASSSRLMonochromatorRegionControl::setSetpoint(int newSetpoint)
{
	if (setpoint_ != newSetpoint) {
		qDebug() << "Updating region control setpoint to " << regionStateToString(regionSetpointToState(newSetpoint));
		setpoint_ = newSetpoint;
		emit setpointChanged(setpoint_);
		emit enumChanged();
	}
}

void BioXASSSRLMonochromatorRegionControl::setMoveInProgress(bool isMoving)
{
	if (moveInProgress_ != isMoving) {
		moveInProgress_ = isMoving;
		emit movingChanged(moveInProgress_);


		if (moveInProgress_) {
			qDebug() << "Updating move in progress: a control move has started.";
			emit moveStarted();
		} else {
			qDebug() << "Updating move in progress: a control move has finished.";
		}
	}
}

void BioXASSSRLMonochromatorRegionControl::onCurrentMoveStepChanged(int stepIndex)
{
	qDebug() << "Executing crystal change step number" << stepIndex;
	emit moveStepChanged(stepDescription(stepIndex), stepInstruction(stepIndex));
}

void BioXASSSRLMonochromatorRegionControl::onRegionControlValueChanged()
{
	BioXASSSRLMonochromator::Region::State newRegion = BioXASSSRLMonochromator::Region::None;

	if (regionAStatus_ && regionAStatus_->isConnected() && regionBStatus_ && regionBStatus_->isConnected()) {

		int regionAVal = (int)regionAStatus_->value();
		int regionBVal = (int)regionBStatus_->value();

		if (regionAVal == BioXASSSRLMonochromator::Region::NotIn && regionBVal == BioXASSSRLMonochromator::Region::In)
			newRegion = BioXASSSRLMonochromator::Region::B;
		else if (regionAVal == BioXASSSRLMonochromator::Region::In && regionBVal == BioXASSSRLMonochromator::Region::NotIn)
			newRegion = BioXASSSRLMonochromator::Region::A;
	}

	setValue(newRegion);
}

void BioXASSSRLMonochromatorRegionControl::onRegionChangeStarted()
{
	setMoveInProgress(true);
}

void BioXASSSRLMonochromatorRegionControl::onRegionChangeCancelled()
{
	moveCleanup(sender());
	emit moveFailed(AMControl::WasStoppedFailure);
}

void BioXASSSRLMonochromatorRegionControl::onRegionChangeFailed()
{
	moveCleanup(sender());
	emit moveFailed(AMControl::OtherFailure);
}

void BioXASSSRLMonochromatorRegionControl::onRegionChangeSucceeded()
{
	moveCleanup(sender());
	emit moveSucceeded();
}

AMListAction3* BioXASSSRLMonochromatorRegionControl::createChangeRegionAction(int newRegion)
{
	AMListAction3 *action = 0;

	qDebug() << "About to create crystal change action from:" << regionStateToString(value_) << "to:" << regionStateToString(newRegion);

	if (value_ != newRegion && validRegionSetpoint(newRegion)) {

		qDebug() << "Creating crystal change action.";

		action = new AMListAction3(new AMListActionInfo3("CrystalChange", "BioXAS SSRL Mono Crystal Change Action"), AMListAction3::Sequential);
		action->addSubAction(createCloseSlitsAction());
		action->addSubAction(createRemovePaddleAction());
		action->addSubAction(createWaitForKeyEnabledAction());
		action->addSubAction(createMoveBraggToCrystalChangePositionAction());
		action->addSubAction(createWaitForBrakeDisabledAction());
		action->addSubAction(createMoveCrystalChangeToRegionLimitAction(newRegion));
		action->addSubAction(createWaitForBrakeEnabledAction());
		action->addSubAction(createMoveBraggToRegionAction(newRegion));
		action->addSubAction(createWaitForKeyDisabledAction());
	}

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createCloseUpperSlitAction()
{
	AMControl *control = upperSlit_;
	AMAction3 *action = 0;

	if (control && control->isConnected())
		action = AMActionSupport::buildControlMoveAction(control, SETPOINT_SLIT_CLOSED);

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createCloseLowerSlitAction()
{
	AMControl *control = lowerSlit_;
	AMAction3 *action = 0;

	if (control && control->isConnected())
		action = AMActionSupport::buildControlMoveAction(control, SETPOINT_SLIT_CLOSED);

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForSlitsClosedAction()
{
	AMControl *control = slitsStatus_;
	AMAction3 *action = 0;

	if (control && control->isConnected()) {
		AMControlInfo setpoint = slitsStatusControl()->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::Slits::Closed);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_SLITS_CLOSED, AMControlWaitActionInfo::MatchEqual), control);
	}

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createCloseSlitsAction()
{
	AMListAction3 *closeSlits = new AMListAction3(new AMListActionInfo3("CloseSlits", "Close mono slits"), AMListAction3::Parallel);
	closeSlits->addSubAction(createCloseUpperSlitAction());
	closeSlits->addSubAction(createCloseLowerSlitAction());

	AMListAction3 *closeAndConfirm = new AMListAction3(new AMListActionInfo3("CloseSlitsAndConfirm", "Close mono slits and confirm they're closed"), AMListAction3::Sequential);
	closeAndConfirm->addSubAction(closeSlits);
	closeAndConfirm->addSubAction(createWaitForSlitsClosedAction());

	return closeSlits;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createMovePaddleAction(double destination)
{
	AMControl *control = paddle_;
	AMAction3 *action = 0;

	if (control && control->isConnected())
		action = AMActionSupport::buildControlMoveAction(control, destination);

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForPaddleRemovedAction()
{
	AMControl *control = paddleStatus_;
	AMAction3 *action = 0;

	if (control && control->isConnected()) {
		AMControlInfo setpoint = control->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::Paddle::Out);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_PADDLE_OUT, AMControlWaitActionInfo::MatchEqual), control);
	}

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createRemovePaddleAction()
{
	AMListAction3 *removeAndConfirm = new AMListAction3(new AMListActionInfo3("RemovePaddleAndConfirm", "Remove mono paddle and confirm it's removed"), AMListAction3::Sequential);
	removeAndConfirm->addSubAction(createMovePaddleAction(SETPOINT_PADDLE_REMOVED));
	removeAndConfirm->addSubAction(createWaitForPaddleRemovedAction());

	return removeAndConfirm;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForKeyEnabledAction()
{
	AMControl *control = keyStatus_;
	AMAction3 *action = 0;

	if (control && control->isConnected()) {
		AMControlInfo setpoint = control->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::Key::Enabled);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_KEY_STATUS_CHANGE, AMControlWaitActionInfo::MatchEqual), control);
	}

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createMoveBraggAction(double destination)
{
	AMControl *control = bragg_;
	AMAction3 *action = 0;

	if (control && control->isConnected())
		action = AMActionSupport::buildControlMoveAction(control, destination);

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForBraggAtCrystalChangePositionAction()
{
	AMControl *control = braggAtCrystalChangePositionStatus_;
	AMAction3 *action = 0;

	if (control && control->isConnected()) {
		AMControlInfo setpoint = control->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::Bragg::InPosition);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_BRAGG_MOTOR_LIMIT_REACHED, AMControlWaitActionInfo::MatchEqual), control);

	} else {
		qDebug() << "Action was not created: wait for bragg at crystal change position.";
	}

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForBrakeDisabledAction()
{
	AMControl *control = brakeStatus_;
	AMAction3 *action = 0;

	if (control && control->isConnected()) {
		AMControlInfo setpoint = control->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::Brake::Disabled);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_BRAKE_STATUS_CHANGE, AMControlWaitActionInfo::MatchEqual), control);
	}

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createMoveBraggToCrystalChangePositionAction()
{
	AMListAction3 *moveAndConfirm = new AMListAction3(new AMListActionInfo3("MoveBraggToCCPositionAndConfirm", "Move bragg motor to the crystal change position and confirm it's in position"), AMListAction3::Sequential);
	moveAndConfirm->addSubAction(createMoveBraggAction(SETPOINT_BRAGG_MOTOR_CRYSTAL_CHANGE_POSITION));
	moveAndConfirm->addSubAction(createWaitForBraggAtCrystalChangePositionAction());

	return moveAndConfirm;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createMoveCrystalChangeAction(double destination)
{
	AMControl *control = crystalChange_;
	AMAction3 *action = 0;

	if (control && control->isConnected())
		action = AMActionSupport::buildControlMoveAction(control, destination);

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForCrystalChangeAtCWLimitAction()
{
	AMControl *control = crystalChangeCWLimitStatus_;
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Confirm cw limit reached", "Confirms crystal change motor cw limit was reached"), AMListAction3::Sequential);

	if (control && control->isConnected()) {
		AMControlInfo setpoint = control->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::CrystalChange::AtLimit);

		AMControlWaitAction *limitReached = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_CRYSTAL_CHANGE_MOTOR_LIMIT_REACHED, AMControlWaitActionInfo::MatchEqual), control);
		action->addSubAction(limitReached);

		AMWaitAction *wait = new AMWaitAction(new AMWaitActionInfo(TIMEOUT_CRYSTAL_CHANGE_MOVE_WAIT));
		action->addSubAction(wait);

		AMControlWaitAction *doubleCheck = new AMControlWaitAction(*limitReached);
		action->addSubAction(doubleCheck);
	}

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForCrystalChangeAtCCWLimitAction()
{
	AMControl *control = crystalChangeCCWLimitStatus_;
	AMListAction3 *action = new AMListAction3(new AMListActionInfo3("Confirm ccw limit reached", "Confirms crystal change motor ccw limit was reached"), AMListAction3::Sequential);

	if (control && control->isConnected()) {
		AMControlInfo setpoint = control->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::CrystalChange::AtLimit);

		AMControlWaitAction *limitReached = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_CRYSTAL_CHANGE_MOTOR_LIMIT_REACHED, AMControlWaitActionInfo::MatchEqual), control);
		action->addSubAction(limitReached);

		AMWaitAction *wait = new AMWaitAction(new AMWaitActionInfo(TIMEOUT_CRYSTAL_CHANGE_MOVE_WAIT));
		action->addSubAction(wait);

		AMControlWaitAction *doubleCheck = new AMControlWaitAction(*limitReached);
		action->addSubAction(doubleCheck);
	}

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForCrystalChangeAtRegionLimitAction(int region)
{
	AMAction3 *action = 0;

	if (region == BioXASSSRLMonochromator::Region::A)
		action = createWaitForCrystalChangeAtCWLimitAction();
	else if (region == BioXASSSRLMonochromator::Region::B)
		action = createWaitForCrystalChangeAtCCWLimitAction();

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createMoveCrystalChangeToRegionLimitAction(int region)
{
	AMListAction3 *moveAndConfirm = 0;

	if (region == BioXASSSRLMonochromator::Region::A) {
		moveAndConfirm = new AMListAction3(new AMListActionInfo3("MoveCrystalChangeToRegionALimitAndConfirm", "Move crystal change motor to the region A limit and confirm it's in position"), AMListAction3::Sequential);
		moveAndConfirm->addSubAction(createMoveCrystalChangeAction(SETPOINT_CRYSTAL_CHANGE_MOTOR_REGION_A_DESTINATION));
		moveAndConfirm->addSubAction(createWaitForCrystalChangeAtRegionLimitAction(region));

	} else if (region == BioXASSSRLMonochromator::Region::B) {
		moveAndConfirm = new AMListAction3(new AMListActionInfo3("MoveCrystalChangeToRegionBLimitAndConfirm", "Move crystal change motor to the region B limit and confirm it's in position"), AMListAction3::Sequential);
		moveAndConfirm->addSubAction(createMoveCrystalChangeAction(SETPOINT_CRYSTAL_CHANGE_MOTOR_REGION_B_DESTINATION));
		moveAndConfirm->addSubAction(createWaitForCrystalChangeAtRegionLimitAction(region));
	}

	return moveAndConfirm;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForBrakeEnabledAction()
{
	AMControl *control = brakeStatusControl();
	AMAction3 *action = 0;

	if (control && control->isConnected()) {
		AMControlInfo setpoint = control->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::Brake::Enabled);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_BRAKE_STATUS_CHANGE, AMControlWaitActionInfo::MatchEqual), control);
	}

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForRegionChangedToAAction()
{
	AMControl *control = regionAStatusControl();
	AMAction3 *action = 0;

	if (control && control->isConnected()) {
		AMControlInfo setpoint = control->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::Region::In);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_REGION_STATE_CHANGED, AMControlWaitActionInfo::MatchEqual), control);
	}

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForRegionChangedToBAction()
{
	AMControl *control = regionBStatusControl();
	AMAction3 *action = 0;

	if (control && control->isConnected()) {
		AMControlInfo setpoint = control->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::Region::In);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_REGION_STATE_CHANGED, AMControlWaitActionInfo::MatchEqual), control);
	}

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForRegionChangedAction(int regionDestination)
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

AMAction3* BioXASSSRLMonochromatorRegionControl::createMoveBraggToRegionAction(int region)
{
	AMListAction3 *moveAndConfirm = 0;

	if (region == BioXASSSRLMonochromator::Region::A) {
		moveAndConfirm = new AMListAction3(new AMListActionInfo3("MoveBraggToRegionAAndConfirm", "Move bragg motor into region A and confirm that it made it"), AMListAction3::Sequential);
		moveAndConfirm->addSubAction(createMoveBraggAction(SETPOINT_BRAGG_MOTOR_REGION_A_DESTINATION));
		moveAndConfirm->addSubAction(createWaitForRegionChangedAction(region));

	} else if (region == BioXASSSRLMonochromator::Region::B) {
		moveAndConfirm = new AMListAction3(new AMListActionInfo3("MoveBraggToRegionBAndConfirm", "Move bragg motor into region B and confirm that it made it"), AMListAction3::Sequential);
		moveAndConfirm->addSubAction(createMoveBraggAction(SETPOINT_BRAGG_MOTOR_REGION_B_DESTINATION));
		moveAndConfirm->addSubAction(createWaitForRegionChangedAction(region));
	}

	return moveAndConfirm;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForKeyDisabledAction()
{
	AMControl *control = keyStatusControl();
	AMAction3 *action = 0;

	if (control && control->isConnected()) {
		AMControlInfo setpoint = control->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::Key::Disabled);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_KEY_STATUS_CHANGE, AMControlWaitActionInfo::MatchEqual), control);
	}

	return action;
}

bool BioXASSSRLMonochromatorRegionControl::validRegionState(int regionState)
{
	bool isValid = false;

	if (regionState == BioXASSSRLMonochromator::Region::A ||
			regionState == BioXASSSRLMonochromator::Region::B ||
			regionState == BioXASSSRLMonochromator::Region::None)
		isValid = true;

	return isValid;
}

bool BioXASSSRLMonochromatorRegionControl::validRegionSetpoint(int regionSetpoint)
{
	bool isValid = false;

	if (regionSetpoint == BioXASSSRLMonochromatorRegionControl::Region::A || regionSetpoint == BioXASSSRLMonochromatorRegionControl::Region::B)
		isValid = true;

	return isValid;
}

QString BioXASSSRLMonochromatorRegionControl::regionStateToString(int region)
{
	QString result;

	switch (region) {
	case BioXASSSRLMonochromator::Region::A:
		result = "A";
		break;
	case BioXASSSRLMonochromator::Region::B:
		result = "B";
		break;
	case BioXASSSRLMonochromator::Region::None:
		result = "None";
		break;
	default:
		result = "ERROR";
		break;
	}

	return result;
}

int BioXASSSRLMonochromatorRegionControl::stringToRegionState(const QString &string)
{
	int result = -1;

	if (string == "A")
		result = BioXASSSRLMonochromator::Region::A;
	else if (string == "B")
		result = BioXASSSRLMonochromator::Region::B;
	else if (string == "None")
		result = BioXASSSRLMonochromator::Region::None;

	return result;
}

int BioXASSSRLMonochromatorRegionControl::regionSetpointToState(int regionSetpoint)
{
	int result;

	switch (regionSetpoint) {
	case BioXASSSRLMonochromatorRegionControl::Region::A:
		result = BioXASSSRLMonochromator::Region::A;
		break;
	case BioXASSSRLMonochromatorRegionControl::Region::B:
		result = BioXASSSRLMonochromator::Region::B;
		break;
	default:
		qDebug() << "Region setpoint" << regionSetpoint << "does not correspond to a valid region state.";
		result = -1;
		break;
	}

	return result;
}

int BioXASSSRLMonochromatorRegionControl::regionStateToSetpoint(int regionState)
{
	int result;

	switch (regionState) {
	case BioXASSSRLMonochromator::Region::A:
		result = Region::A;
		break;
	case BioXASSSRLMonochromator::Region::B:
		result = Region::B;
		break;
	default:
		qDebug() << "Region state" << regionState << "does not correspond to a valid region setpoint.";
		result = -1;
		break;
	}

	return result;
}

QString BioXASSSRLMonochromatorRegionControl::stepDescription(int stepIndex)
{
	QString result;

	switch (stepIndex) {
	case CloseSlits:
		result = "Closing mono slits.";
		break;
	case RemovePaddle:
		result = "Removing mono phosphor paddle.";
		break;
	case WaitForKeyEnabled:
		result = "Waiting for crystal change key to be turned to 'Enabled'...";
		break;
	case MoveBraggIntoPosition:
		result = "Moving bragg motor to crystal change position.";
		break;
	case WaitForBrakeDisabled:
		result = "Waiting for brake switch to be flipped to 'Disabled'...";
		break;
	case MoveCrystalChangeIntoPosition:
		result = "Moving the crystal change motor to its limit.";
		break;
	case WaitForBrakeEnabled:
		result = "Waiting for brake switch to be flipped to 'Enabled'...";
		break;
	case MoveBraggIntoRegion:
		result = "Moving bragg motor into new region.";
		break;
	case WaitForKeyDisabled:
		result = "Waiting for crystal change key to be turned to 'Disabled'...";
		break;
	default:
		result = "";
	}

	return result;
}

QString BioXASSSRLMonochromatorRegionControl::stepInstruction(int stepIndex)
{
	QString result;

	switch (stepIndex) {
	case WaitForKeyEnabled:
		result = "Turn the crystal change key to 'Enabled,' counter-clockwise.";
		break;
	case WaitForBrakeDisabled:
		result = "Flip the brake switch to 'Disabled.' There is a mandatory 30s wait after the switch is flipped before proceeding.";
		break;
	case WaitForBrakeEnabled:
		result = "Flip the brake switch to 'Enabled.' There is a mandatory 30s wait after the switch is flipped before proceeding.";
		break;
	case WaitForKeyDisabled:
		result = "Turn the crystal change key to 'Disabled,' clockwise.";
		break;
	default:
		result = "";
	}

	return result;
}

void BioXASSSRLMonochromatorRegionControl::moveCleanup(QObject *action)
{
	setMoveInProgress(false);

	disconnect( action, 0, this, 0 );
	action->deleteLater();
}
