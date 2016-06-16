#include "BioXASSSRLMonochromatorRegionControl.h"
#include "BioXASSSRLMonochromator.h"

BioXASSSRLMonochromatorRegionControl::BioXASSSRLMonochromatorRegionControl(const QString &name, QObject *parent) :
	AMEnumeratedControl(name, "", parent)
{
	// Initialize local variables.

	maskUpperBlade_ = 0;
	maskLowerBlade_ = 0;
	maskBladesStatus_ = 0;
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

	addOption(BioXASSSRLMonochromator::Region::A, "A");
	addOption(BioXASSSRLMonochromator::Region::B, "B");
	addOption(BioXASSSRLMonochromator::Region::None, "None", true);

	setAllowsMovesWhileMoving(false);
	setContextKnownDescription("Region Control");

	// Current settings.

	updateStates();
}

BioXASSSRLMonochromatorRegionControl::~BioXASSSRLMonochromatorRegionControl()
{

}

bool BioXASSSRLMonochromatorRegionControl::canMeasure() const
{
	bool result = false;

	if (regionAStatus_ && regionBStatus_) {
		result = (
				regionAStatus_->canMeasure() &&
				regionBStatus_->canMeasure()
				);
	}

	return result;
}

bool BioXASSSRLMonochromatorRegionControl::canMove() const
{
	bool result = false;

	if (isConnected() && !isMoving()) {
		result = (
			maskUpperBlade_->canMeasure() &&
			maskLowerBlade_->canMeasure() &&
			maskBladesStatus_->canMeasure() &&
			paddleStatus_->canMeasure() &&
			keyStatus_->canMeasure() &&
			brakeStatus_->canMeasure() &&
			braggAtCrystalChangePositionStatus_->canMeasure() &&
			crystalChangeCWLimitStatus_->canMeasure() &&
			crystalChangeCCWLimitStatus_->canMeasure() &&

			maskUpperBlade_->canMove() &&
			maskLowerBlade_->canMove() &&
			paddle_->canMove() &&
			bragg_->canMove() &&
			crystalChange_->canMove()
		);
	}

	return result;
}

bool BioXASSSRLMonochromatorRegionControl::validValue(double value) const
{
	bool isValid = false;

	switch ((int)value) {
	case BioXASSSRLMonochromator::Region::A:
		isValid = true;
		break;
	case BioXASSSRLMonochromator::Region::B:
		isValid = true;
		break;
	case BioXASSSRLMonochromator::Region::None:
		isValid = true;
		break;
	default:
		break;
	}

	return isValid;
}

bool BioXASSSRLMonochromatorRegionControl::validSetpoint(double value) const
{
	bool isValid = false;

	if (value == BioXASSSRLMonochromator::Region::A || value == BioXASSSRLMonochromator::Region::B)
		isValid = true;

	return isValid;
}

void BioXASSSRLMonochromatorRegionControl::setUpperSlitBladeControl(BioXASMAXvMotor *newControl)
{
	if (maskUpperBlade_ != newControl) {

		if (maskUpperBlade_)
			removeChildControl(maskUpperBlade_);

		maskUpperBlade_ = newControl;

		if (maskUpperBlade_)
			addChildControl(maskUpperBlade_);
	}
}

void BioXASSSRLMonochromatorRegionControl::setLowerSlitBladeControl(BioXASMAXvMotor *newControl)
{
	if (maskLowerBlade_ != newControl) {

		if (maskLowerBlade_)
			removeChildControl(maskLowerBlade_);

		maskLowerBlade_ = newControl;

		if (maskLowerBlade_)
			addChildControl(maskLowerBlade_);
	}
}

void BioXASSSRLMonochromatorRegionControl::setSlitsStatusControl(AMControl *newControl)
{
	if (maskBladesStatus_ != newControl) {

		if (maskBladesStatus_)
			removeChildControl(maskBladesStatus_);

		maskBladesStatus_ = newControl;

		if (maskBladesStatus_)
			addChildControl(maskBladesStatus_);
	}
}

void BioXASSSRLMonochromatorRegionControl::setPaddleControl(BioXASMAXvMotor *paddle)
{
	if (paddle_ != paddle) {

		if (paddle_)
			removeChildControl(paddle_);

		paddle_ = paddle;

		if (paddle_)
			addChildControl(paddle_);
	}
}

void BioXASSSRLMonochromatorRegionControl::setPaddleStatusControl(AMControl *paddleStatus)
{
	if (paddleStatus_ != paddleStatus) {

		if (paddleStatus_)
			removeChildControl(paddleStatus_);

		paddleStatus_ = paddleStatus;

		if (paddleStatus_)
			addChildControl(paddleStatus_);
	}
}

void BioXASSSRLMonochromatorRegionControl::setKeyStatusControl(AMControl *keyStatus)
{
	if (keyStatus_ != keyStatus) {

		if (keyStatus_)
			removeChildControl(keyStatus_);

		keyStatus_ = keyStatus;

		if (keyStatus_)
			addChildControl(keyStatus_);
	}
}

void BioXASSSRLMonochromatorRegionControl::setBrakeStatusControl(AMControl *brakeStatus)
{
	if (brakeStatus_ != brakeStatus) {

		if (brakeStatus_)
			removeChildControl(brakeStatus_);

		brakeStatus_ = brakeStatus;

		if (brakeStatus_)
			addChildControl(brakeStatus_);
	}
}

void BioXASSSRLMonochromatorRegionControl::setBraggControl(BioXASMAXvMotor *bragg)
{
	if (bragg_ != bragg) {

		if (bragg_)
			removeChildControl(bragg_);

		bragg_ = bragg;

		if (bragg_)
			addChildControl(bragg_);
	}
}

void BioXASSSRLMonochromatorRegionControl::setBraggAtCrystalChangePositionStatusControl(AMControl *inPosition)
{
	if (braggAtCrystalChangePositionStatus_ != inPosition) {

		if (braggAtCrystalChangePositionStatus_)
			removeChildControl(braggAtCrystalChangePositionStatus_);

		braggAtCrystalChangePositionStatus_ = inPosition;

		if (braggAtCrystalChangePositionStatus_)
			addChildControl(braggAtCrystalChangePositionStatus_);
	}
}

void BioXASSSRLMonochromatorRegionControl::setCrystalChangeControl(BioXASMAXvMotor *crystalChange)
{
	if (crystalChange_ != crystalChange) {

		if (crystalChange_)
			removeChildControl(crystalChange_);

		crystalChange_ = crystalChange;

		if (crystalChange_)
			addChildControl(crystalChange_);
	}
}

void BioXASSSRLMonochromatorRegionControl::setCrystalChangeCWLimitStatusControl(AMControl *limitStatus)
{
	if (crystalChangeCWLimitStatus_ != limitStatus) {

		if (crystalChangeCWLimitStatus_)
			removeChildControl(crystalChangeCCWLimitStatus_);

		crystalChangeCWLimitStatus_ = limitStatus;

		if (crystalChangeCWLimitStatus_)
			addChildControl(crystalChangeCWLimitStatus_);
	}
}

void BioXASSSRLMonochromatorRegionControl::setCrystalChangeCCWLimitStatusControl(AMControl *limitStatus)
{
	if (crystalChangeCCWLimitStatus_ != limitStatus) {

		if (crystalChangeCCWLimitStatus_)
			removeChildControl(crystalChangeCCWLimitStatus_);

		crystalChangeCCWLimitStatus_ = limitStatus;

		if (crystalChangeCCWLimitStatus_)
			addChildControl(crystalChangeCCWLimitStatus_);
	}
}

void BioXASSSRLMonochromatorRegionControl::setRegionAStatusControl(AMControl *regionStatus)
{
	if (regionAStatus_ != regionStatus) {

		if (regionAStatus_)
			removeChildControl(regionAStatus_);

		regionAStatus_ = regionStatus;

		if (regionAStatus_)
			addChildControl(regionAStatus_);
	}
}

void BioXASSSRLMonochromatorRegionControl::setRegionBStatusControl(AMControl *regionStatus)
{
	if (regionBStatus_ != regionStatus) {

		if (regionBStatus_)
			removeChildControl(regionBStatus_);

		regionBStatus_ = regionStatus;

		if (regionBStatus_)
			addChildControl(regionBStatus_);
	}
}

void BioXASSSRLMonochromatorRegionControl::updateConnected()
{
	bool slitsOK = ( maskUpperBlade_ && maskUpperBlade_->isConnected() && maskLowerBlade_ && maskLowerBlade_->isConnected() && maskBladesStatus_ && maskBladesStatus_->isConnected() );
	bool paddleOK = (paddle_ && paddle_->isConnected());
	bool paddleStatusOK = (paddleStatus_ && paddleStatus_->isConnected());
	bool keyStatusOK = (keyStatus_ && keyStatus_->isConnected());
	bool brakeStatusOK = (brakeStatus_ && brakeStatus_->isConnected());
	bool braggOK = (bragg_ && bragg_->isConnected());
	bool braggAtCrystalChangePositionStatusOK = (braggAtCrystalChangePositionStatus_ && braggAtCrystalChangePositionStatus_->isConnected());
	bool crystalChangeOK = (crystalChange_ && crystalChange_->isConnected());
	bool crystalChangeCWLimitStatusOK = (crystalChangeCWLimitStatus_ && crystalChangeCWLimitStatus_->isConnected());
	bool crystalChangeCCWLimitStatusOK = (crystalChangeCCWLimitStatus_ && crystalChangeCCWLimitStatus_->isConnected());
	bool regionAStatusOK = (regionAStatus_ && regionAStatus_->isConnected());
	bool regionBStatusOK = (regionBStatus_ && regionBStatus_->isConnected());

	bool isConnected = (
				slitsOK &&
				paddleOK &&
				paddleStatusOK &&
				keyStatusOK &&
				brakeStatusOK &&
				braggOK &&
				braggAtCrystalChangePositionStatusOK &&
				crystalChangeOK &&
				crystalChangeCWLimitStatusOK &&
				crystalChangeCCWLimitStatusOK &&
				regionAStatusOK &&
				regionBStatusOK
				);

	setConnected(isConnected);
}

void BioXASSSRLMonochromatorRegionControl::updateMoving()
{
	if (isConnected()) {
		setIsMoving(
					maskUpperBlade_->isMoving() ||
					maskLowerBlade_->isMoving() ||
					paddle_->isMoving() ||
					bragg_->isMoving() ||
					crystalChange_->isMoving()
					);
	}
}

void BioXASSSRLMonochromatorRegionControl::onMoveStepChanged(int stepIndex)
{
	emit moveStepChanged(stepDescription(stepIndex), stepInstruction(stepIndex), stepNotes(stepIndex));
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createMoveAction(double newRegion)
{
	AMListAction3 *action = 0;

	if (value_ != newRegion && validValue(newRegion)) {

		action = new AMListAction3(new AMListActionInfo3("CrystalChange", "BioXAS SSRL Mono Crystal Change Action"), AMListAction3::Sequential);
		action->addSubAction(createCloseMaskBladesAction());
		action->addSubAction(createRemovePaddleAction());
		action->addSubAction(createWaitForKeyEnabledAction());
		action->addSubAction(createMoveBraggToCrystalChangePositionAction(int(value_)));
		action->addSubAction(createWaitForBrakeDisabledAction());
		action->addSubAction(createMoveCrystalChangeToRegionLimitAction(int(newRegion)));
		action->addSubAction(createWaitForBrakeEnabledAction());
		action->addSubAction(createMoveBraggToRegionAction(int(newRegion)));
		action->addSubAction(createWaitForKeyDisabledAction());	

		// Make additional action connections.

		connect( action, SIGNAL(progressChanged(double, double)), this, SIGNAL(moveProgressChanged(double,double)) );
		connect( action, SIGNAL(currentSubActionChanged(int)), this, SLOT(onMoveStepChanged(int)) );
	}

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createMoveMaskUpperBladeToLimitAction(CLSMAXvMotor::Limit limit)
{
	AMAction3 *action = 0;

	if (maskUpperBlade_ && maskUpperBlade_->isConnected())
		action = maskUpperBlade_->createMoveToLimitAction(limit);

	if (!action)
		AMErrorMon::error(this, BIOXASSSRLMONOCHROMATORREGIONCONTROL_CLOSE_MASK_UPPER_FAILED, "Failed to create action to close mono upper slit.");

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createMoveMaskLowerBladeToLimitAction(CLSMAXvMotor::Limit limit)
{
	AMAction3 *action = 0;

	if (maskLowerBlade_ && maskLowerBlade_->isConnected())
		action = maskLowerBlade_->createMoveToLimitAction(limit);

	if (!action)
		AMErrorMon::error(this, BIOXASSSRLMONOCHROMATORREGIONCONTROL_CLOSE_MASK_LOWER_FAILED, "Failed to create action to close mono lower slit.");

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForMaskBladesClosedAction()
{
	AMAction3 *action = 0;

	if (maskBladesStatus_ && maskBladesStatus_->isConnected())
		action = AMActionSupport::buildControlWaitAction(maskBladesStatus_, BioXASSSRLMonochromator::Slits::Closed, BIOXASSSRLMONOCHROMATORREGIONCONTROL_SLITS_CLOSED_TIMEOUT);

	if (!action)
		AMErrorMon::error(this, BIOXASSSRLMONOCHROMATORREGIONCONTROL_CLOSE_MASK_FAILED, "Failed to create action to wait for both mono slits to be closed.");

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createCloseMaskBladesAction()
{
	AMListAction3 *closeSlits = new AMListAction3(new AMListActionInfo3("CloseSlits", "Close mono slits"), AMListAction3::Parallel);
	closeSlits->addSubAction(createMoveMaskUpperBladeToLimitAction(BIOXASSSRLMONOCHROMATORREGIONCONTROL_MASK_CLOSED_LIMIT));
	closeSlits->addSubAction(createMoveMaskLowerBladeToLimitAction(BIOXASSSRLMONOCHROMATORREGIONCONTROL_MASK_CLOSED_LIMIT));

	AMListAction3 *closeAndConfirm = new AMListAction3(new AMListActionInfo3("CloseSlitsAndConfirm", "Close mono slits and confirm they're closed"), AMListAction3::Sequential);
	closeAndConfirm->addSubAction(closeSlits);
	closeAndConfirm->addSubAction(createWaitForMaskBladesClosedAction());

	return closeAndConfirm;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createMovePaddleToLimitAction(CLSMAXvMotor::Limit limit)
{
	AMAction3 *action = 0;

	if (paddle_ && paddle_->isConnected())
		action = paddle_->createMoveToLimitAction(limit);

	if (!action)
		AMErrorMon::error(this, BIOXASSSRLMONOCHROMATORREGIONCONTROL_MOVE_PADDLE_FAILED, "Failed to create action to move the mono paddle motor to limit.");

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForPaddleRemovedAction()
{
	AMAction3 *action = 0;

	if (paddleStatus_ && paddleStatus_->isConnected())
		action = AMActionSupport::buildControlWaitAction(paddleStatus_, BioXASSSRLMonochromator::Paddle::Out, BIOXASSSRLMONOCHROMATORREGIONCONTROL_PADDLE_REMOVED_TIMEOUT);

	if (!action)
		AMErrorMon::error(this, BIOXASSSRLMONOCHROMATORREGIONCONTROL_PADDLE_WAIT_FAILED, "Failed to create action to wait until mono paddle motor is removed.");

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createRemovePaddleAction()
{
	AMListAction3 *removeAndConfirm = new AMListAction3(new AMListActionInfo3("RemovePaddleAndConfirm", "Remove mono paddle and confirm it's removed"), AMListAction3::Sequential);
	removeAndConfirm->addSubAction(createMovePaddleToLimitAction(BIOXASSSRLMONOCHROMATORREGIONCONTROL_PADDLE_REMOVED_LIMIT));
	removeAndConfirm->addSubAction(createWaitForPaddleRemovedAction());

	return removeAndConfirm;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForKeyEnabledAction()
{
	AMAction3 *action = 0;

	if (keyStatus_ && keyStatus_->isConnected())
		action = AMActionSupport::buildControlWaitAction(keyStatus_, BioXASSSRLMonochromator::Key::Enabled, BIOXASSSRLMONOCHROMATORREGIONCONTROL_KEY_STATUS_CHANGE_TIMEOUT);

	if (!action)
		AMErrorMon::error(this, BIOXASSSRLMONOCHROMATORREGIONCONTROL_KEY_ENABLED_WAIT_FAILED, "Failed to create action to wait for the mono key to be turned to 'Enabled.'");

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createMoveBraggToLimitAction(CLSMAXvMotor::Limit limit)
{
	AMAction3* action = 0;

	if (bragg_ && bragg_->isConnected())
		action = bragg_->createMoveToLimitAction(limit);

	if (!action)
		AMErrorMon::error(this, BIOXASSSRLMONOCHROMATORREGIONCONTROL_MOVE_BRAGG_FAILED, "Failed to create action to move the mono bragg motor to limit.");

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createMoveBraggToCrystalChangePositionRegionAAction()
{
	return createMoveBraggToLimitAction(BIOXASSSRLMONOCHROMATORREGIONCONTROL_BRAGG_CRYSTAL_CHANGE_POSITION_REGION_A_LIMIT);
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createMoveBraggToCrystalChangePositionRegionBAction()
{
	return createMoveBraggToLimitAction(BIOXASSSRLMONOCHROMATORREGIONCONTROL_BRAGG_CRYSTAL_CHANGE_POSITION_REGION_B_LIMIT);
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForBraggAtCrystalChangePositionAction()
{
	AMAction3 *action = 0;

	if (braggAtCrystalChangePositionStatus_ && braggAtCrystalChangePositionStatus_->isConnected())
		action = AMActionSupport::buildControlWaitAction(braggAtCrystalChangePositionStatus_, BioXASSSRLMonochromator::Bragg::InPosition);

	if (!action)
		AMErrorMon::error(this, BIOXASSSRLMONOCHROMATORREGIONCONTROL_BRAGG_WAIT_FAILED, "Failed to create action to wait for the mono bragg motor to reach the crystal change position.");

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForBrakeDisabledAction()
{
	AMAction3 *action = 0;

	if (brakeStatus_ && brakeStatus_->isConnected())
		action = AMActionSupport::buildControlWaitAction(brakeStatus_, BioXASSSRLMonochromator::Brake::Disabled, BIOXASSSRLMONOCHROMATORREGIONCONTROL_BRAKE_STATUS_CHANGE_TIMEOUT);

	if (!action)
		AMErrorMon::error(this, BIOXASSSRLMONOCHROMATORREGIONCONTROL_BRAKE_DISABLED_WAIT_FAILED, "Failed to create action to wait for the mono brake to be disabled.");

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createMoveBraggToCrystalChangePositionAction(int region)
{
	AMAction3 *action = 0;
	AMAction3 *moveAction = 0;

	if (region == BioXASSSRLMonochromator::Region::A)
		moveAction = createMoveBraggToCrystalChangePositionRegionAAction();
	else if (region == BioXASSSRLMonochromator::Region::B)
		moveAction = createMoveBraggToCrystalChangePositionRegionBAction();

	if (moveAction) {
		AMListAction3 *moveAndConfirm = new AMListAction3(new AMListActionInfo3("MoveBraggToCCPositionAndConfirm", "Move bragg motor to the crystal change position and confirm it's in position"), AMListAction3::Sequential);
		moveAndConfirm->addSubAction(moveAction);
		moveAndConfirm->addSubAction(createWaitForBraggAtCrystalChangePositionAction());

		action = moveAndConfirm;
	}

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createMoveCrystalChangeMotorToLimit(CLSMAXvMotor::Limit limit)
{
	AMAction3* action = 0;

	if (crystalChange_ && crystalChange_->isConnected())
		action = crystalChange_->createMoveToLimitAction(limit);

	if (!action)
		AMErrorMon::error(this, BIOXASSSRLMONOCHROMATORREGIONCONTROL_MOVE_CRYSTAL_CHANGE_FAILED, "Failed to create action to move the mono crystal change motor to limit.");

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForCrystalChangeAtCWLimitAction()
{
	AMAction3 *action = 0;

	// We do a double-check here because it's possible for the crystal change motor might
	// 'bounce' off the limit switch. We want to know when that happens.

	if (crystalChange_ && crystalChange_->isConnected()) {
		AMListAction3 *waitAndConfirm = new AMListAction3(new AMListActionInfo3("Wait for the crystal change motor to reach CW limit and confirm that limit reached.", "Wait for crystal change motor to reach CW limit and confirm that limit reached."), AMListAction3::Sequential);
		waitAndConfirm->addSubAction(AMActionSupport::buildControlWaitAction(crystalChange_->cwLimitControl(), BioXASSSRLMonochromator::CrystalChange::AtLimit, BIOXASSSRLMONOCHROMATORREGIONCONTROL_CRYSTAL_CHANGE_MOTOR_LIMIT_REACHED_TIMEOUT));
		waitAndConfirm->addSubAction(AMActionSupport::buildWaitAction(BIOXASSSRLMONOCHROMATORREGIONCONTROL_CRYSTAL_CHANGE_MOTOR_LIMIT_DOUBLECHECK_WAIT));
		waitAndConfirm->addSubAction(AMActionSupport::buildControlWaitAction(crystalChange_->cwLimitControl(), BioXASSSRLMonochromator::CrystalChange::AtLimit, BIOXASSSRLMONOCHROMATORREGIONCONTROL_CRYSTAL_CHANGE_MOTOR_LIMIT_REACHED_TIMEOUT));

		action = waitAndConfirm;
	}

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForCrystalChangeAtCCWLimitAction()
{
	AMAction3 *action = 0;

	// We do a double-check here because it's possible for the crystal change motor might
	// 'bounce' off the limit switch. We want to know when that happens.

	if (crystalChange_ && crystalChange_->isConnected()) {
		AMListAction3 *waitAndConfirm = new AMListAction3(new AMListActionInfo3("Wait for the crystal change motor to reach CCW limit and confirm that limit reached.", "Wait for crystal change motor to reach CCW limit and confirm that limit reached."), AMListAction3::Sequential);
		waitAndConfirm->addSubAction(AMActionSupport::buildControlWaitAction(crystalChange_->ccwLimitControl(), BioXASSSRLMonochromator::CrystalChange::AtLimit, BIOXASSSRLMONOCHROMATORREGIONCONTROL_CRYSTAL_CHANGE_MOTOR_LIMIT_REACHED_TIMEOUT));
		waitAndConfirm->addSubAction(AMActionSupport::buildWaitAction(BIOXASSSRLMONOCHROMATORREGIONCONTROL_CRYSTAL_CHANGE_MOTOR_LIMIT_DOUBLECHECK_WAIT));
		waitAndConfirm->addSubAction(AMActionSupport::buildControlWaitAction(crystalChange_->ccwLimitControl(), BioXASSSRLMonochromator::CrystalChange::AtLimit, BIOXASSSRLMONOCHROMATORREGIONCONTROL_CRYSTAL_CHANGE_MOTOR_LIMIT_REACHED_TIMEOUT));

		action = waitAndConfirm;
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

	if (!action)
		AMErrorMon::error(this, BIOXASSSRLMONOCHROMATORREGIONCONTROL_CRYSTAL_CHANGE_WAIT_FAILED, "Failed to create action to wait for the mono crystal change motor to reach region " + regionStateToString(region));

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createMoveCrystalChangeToRegionLimitAction(int region)
{
	AMListAction3 *moveAndConfirm = 0;

	if (region == BioXASSSRLMonochromator::Region::A) {
		moveAndConfirm = new AMListAction3(new AMListActionInfo3("MoveCrystalChangeToRegionALimitAndConfirm", "Move crystal change motor to the region A limit and confirm it's in position"), AMListAction3::Sequential);
		moveAndConfirm->addSubAction(createMoveCrystalChangeMotorToLimit(BIOXASSSRLMONOCHROMATORREGIONCONTROL_CRYSTAL_CHANGE_REGION_A_LIMIT));
		moveAndConfirm->addSubAction(createWaitForCrystalChangeAtRegionLimitAction(region));

	} else if (region == BioXASSSRLMonochromator::Region::B) {
		moveAndConfirm = new AMListAction3(new AMListActionInfo3("MoveCrystalChangeToRegionBLimitAndConfirm", "Move crystal change motor to the region B limit and confirm it's in position"), AMListAction3::Sequential);
		moveAndConfirm->addSubAction(createMoveCrystalChangeMotorToLimit(BIOXASSSRLMONOCHROMATORREGIONCONTROL_CRYSTAL_CHANGE_REGION_B_LIMIT));
		moveAndConfirm->addSubAction(createWaitForCrystalChangeAtRegionLimitAction(region));
	}

	return moveAndConfirm;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForBrakeEnabledAction()
{
	AMAction3 *action = 0;

	if (brakeStatus_ && brakeStatus_->isConnected())
		action = AMActionSupport::buildControlWaitAction(brakeStatus_, BioXASSSRLMonochromator::Brake::Enabled, BIOXASSSRLMONOCHROMATORREGIONCONTROL_BRAKE_STATUS_CHANGE_TIMEOUT);

	if (!action)
		AMErrorMon::error(this, BIOXASSSRLMONOCHROMATORREGIONCONTROL_BRAKE_ENABLED_WAIT_FAILED, "Failed to create action to wait for the mono brake to be enabled.");

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForRegionChangedToAAction()
{
	AMAction3 *action = 0;

	if (regionAStatus_ && regionAStatus_->isConnected())
		action = AMActionSupport::buildControlWaitAction(regionAStatus_, BioXASSSRLMonochromator::Region::In, BIOXASSSRLMONOCHROMATORREGIONCONTROL_REGION_STATE_CHANGE_TIMEOUT);

	if (!action)
		AMErrorMon::error(this, BIOXASSSRLMONOCHROMATORREGIONCONTROL_REGION_A_WAIT_FAILED, "Failed to create action to wait for the mono to reach region A.");

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForRegionChangedToBAction()
{
	AMAction3 *action = 0;

	if (regionBStatus_ && regionBStatus_->isConnected())
		action = AMActionSupport::buildControlWaitAction(regionBStatus_, BioXASSSRLMonochromator::Region::In, BIOXASSSRLMONOCHROMATORREGIONCONTROL_REGION_STATE_CHANGE_TIMEOUT);

	if (!action)
		AMErrorMon::error(this, BIOXASSSRLMONOCHROMATORREGIONCONTROL_REGION_B_WAIT_FAILED, "Failed to create action to wait for the mono to reach region B.");

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForRegionChangedAction(int regionDestination)
{
	AMAction3 *action = 0;

	if (regionDestination == BioXASSSRLMonochromator::Region::A)
		action = createWaitForRegionChangedToAAction();
	else if (regionDestination == BioXASSSRLMonochromator::Region::B)
		action = createWaitForRegionChangedToBAction();

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createMoveBraggToRegionAction(int region)
{
	AMListAction3 *moveAndConfirm = 0;

	if (region == BioXASSSRLMonochromator::Region::A) {
		moveAndConfirm = new AMListAction3(new AMListActionInfo3("MoveBraggToRegionAAndConfirm", "Move bragg motor into region A and confirm that it made it"), AMListAction3::Sequential);
		moveAndConfirm->addSubAction(createMoveBraggToLimitAction(CLSMAXvMotor::LimitCW));
		moveAndConfirm->addSubAction(createWaitForRegionChangedAction(region));

	} else if (region == BioXASSSRLMonochromator::Region::B) {
		moveAndConfirm = new AMListAction3(new AMListActionInfo3("MoveBraggToRegionBAndConfirm", "Move bragg motor into region B and confirm that it made it"), AMListAction3::Sequential);
		moveAndConfirm->addSubAction(createMoveBraggToLimitAction(CLSMAXvMotor::LimitCCW));
		moveAndConfirm->addSubAction(createWaitForRegionChangedAction(region));
	}

	return moveAndConfirm;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForKeyDisabledAction()
{
	AMAction3 *action = 0;

	if (keyStatus_ && keyStatus_->isConnected())
		action = AMActionSupport::buildControlWaitAction(keyStatus_, BioXASSSRLMonochromator::Key::Disabled, BIOXASSSRLMONOCHROMATORREGIONCONTROL_KEY_STATUS_CHANGE_TIMEOUT);

	if (!action)
		AMErrorMon::error(this, BIOXASSSRLMONOCHROMATORREGIONCONTROL_KEY_DISABLED_WAIT_FAILED, "Failed to create action to wait for the mono key to be turned to 'Disabled.'");

	return action;
}

int BioXASSSRLMonochromatorRegionControl::currentIndex() const
{
	int result = BioXASSSRLMonochromator::Region::None;

	int regionAVal = (int)regionAStatus_->value();
	int regionBVal = (int)regionBStatus_->value();

	if (regionAVal == BioXASSSRLMonochromator::Region::NotIn && regionBVal == BioXASSSRLMonochromator::Region::In)
		result = BioXASSSRLMonochromator::Region::B;
	else if (regionAVal == BioXASSSRLMonochromator::Region::In && regionBVal == BioXASSSRLMonochromator::Region::NotIn)
		result = BioXASSSRLMonochromator::Region::A;

	return result;
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
		result = "";
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
		result = "Flip the brake switch to 'Disabled.'";
		break;
	case WaitForBrakeEnabled:
		result = "Flip the brake switch to 'Enabled.'";
		break;
	case WaitForKeyDisabled:
		result = "Turn the crystal change key to 'Disabled,' clockwise.";
		break;
	default:
		result = "";
	}

	return result;
}

QString BioXASSSRLMonochromatorRegionControl::stepNotes(int stepIndex)
{
	QString result = "";

	if (stepIndex == WaitForBrakeDisabled || stepIndex == WaitForBrakeEnabled) {
		result = "There is a mandatory 1m30s wait after the switch is flipped before proceeding.";
	}

	return result;
}
