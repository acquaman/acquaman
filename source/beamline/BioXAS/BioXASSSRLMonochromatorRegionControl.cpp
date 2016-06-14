#include "BioXASSSRLMonochromatorRegionControl.h"
#include "BioXASSSRLMonochromator.h"

BioXASSSRLMonochromatorRegionControl::BioXASSSRLMonochromatorRegionControl(const QString &name, QObject *parent) :
	AMEnumeratedControl(name, "", parent)
{
	// Initialize local variables.

	upperSlitBlade_ = 0;
	lowerSlitBlade_ = 0;
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
			upperSlitBlade_->canMeasure() &&
			lowerSlitBlade_->canMeasure() &&
			slitsStatus_->canMeasure() &&
			paddleStatus_->canMeasure() &&
			keyStatus_->canMeasure() &&
			brakeStatus_->canMeasure() &&
			braggAtCrystalChangePositionStatus_->canMeasure() &&
			crystalChangeCWLimitStatus_->canMeasure() &&
			crystalChangeCCWLimitStatus_->canMeasure() &&

			upperSlitBlade_->canMove() &&
			lowerSlitBlade_->canMove() &&
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

void BioXASSSRLMonochromatorRegionControl::setUpperSlitBladeControl(AMControl *newControl)
{
	if (upperSlitBlade_ != newControl) {

		removeChildControl(upperSlitBlade_);

		upperSlitBlade_ = newControl;

		addChildControl(upperSlitBlade_);
	}
}

void BioXASSSRLMonochromatorRegionControl::setLowerSlitBladeControl(AMControl *newControl)
{
	if (lowerSlitBlade_ != newControl) {

		removeChildControl(lowerSlitBlade_);

		lowerSlitBlade_ = newControl;

		addChildControl(lowerSlitBlade_);
	}
}

void BioXASSSRLMonochromatorRegionControl::setSlitsStatusControl(AMControl *newControl)
{
	if (slitsStatus_ != newControl) {

		removeChildControl(slitsStatus_);

		slitsStatus_ = newControl;

		addChildControl(slitsStatus_);
	}
}

void BioXASSSRLMonochromatorRegionControl::setPaddleControl(AMControl *paddle)
{
	if (paddle_ != paddle) {

		removeChildControl(paddle_);

		paddle_ = paddle;

		addChildControl(paddle_);
	}
}

void BioXASSSRLMonochromatorRegionControl::setPaddleStatusControl(AMControl *paddleStatus)
{
	if (paddleStatus_ != paddleStatus) {

		removeChildControl(paddleStatus_);

		paddleStatus_ = paddleStatus;

		addChildControl(paddleStatus_);
	}
}

void BioXASSSRLMonochromatorRegionControl::setKeyStatusControl(AMControl *keyStatus)
{
	if (keyStatus_ != keyStatus) {

		removeChildControl(keyStatus_);

		keyStatus_ = keyStatus;

		addChildControl(keyStatus_);
	}
}

void BioXASSSRLMonochromatorRegionControl::setBrakeStatusControl(AMControl *brakeStatus)
{
	if (brakeStatus_ != brakeStatus) {

		removeChildControl(brakeStatus_);

		brakeStatus_ = brakeStatus;

		addChildControl(brakeStatus_);
	}
}

void BioXASSSRLMonochromatorRegionControl::setBraggControl(AMControl *bragg)
{
	if (bragg_ != bragg) {

		removeChildControl(bragg_);

		bragg_ = bragg;

		addChildControl(bragg_);
	}
}

void BioXASSSRLMonochromatorRegionControl::setBraggAtCrystalChangePositionStatusControl(AMControl *inPosition)
{
	if (braggAtCrystalChangePositionStatus_ != inPosition) {

		removeChildControl(braggAtCrystalChangePositionStatus_);

		braggAtCrystalChangePositionStatus_ = inPosition;

		addChildControl(braggAtCrystalChangePositionStatus_);
	}
}

void BioXASSSRLMonochromatorRegionControl::setCrystalChangeControl(AMControl *crystalChange)
{
	if (crystalChange_ != crystalChange) {

		removeChildControl(crystalChange_);

		crystalChange_ = crystalChange;

		addChildControl(crystalChange_);
	}
}

void BioXASSSRLMonochromatorRegionControl::setCrystalChangeCWLimitStatusControl(AMControl *limitStatus)
{
	if (crystalChangeCWLimitStatus_ != limitStatus) {

		removeChildControl(crystalChangeCCWLimitStatus_);

		crystalChangeCWLimitStatus_ = limitStatus;

		addChildControl(crystalChangeCWLimitStatus_);
	}
}

void BioXASSSRLMonochromatorRegionControl::setCrystalChangeCCWLimitStatusControl(AMControl *limitStatus)
{
	if (crystalChangeCCWLimitStatus_ != limitStatus) {

		removeChildControl(crystalChangeCCWLimitStatus_);

		crystalChangeCCWLimitStatus_ = limitStatus;

		addChildControl(crystalChangeCCWLimitStatus_);
	}
}

void BioXASSSRLMonochromatorRegionControl::setRegionAStatusControl(AMControl *regionStatus)
{
	if (regionAStatus_ != regionStatus) {

		removeChildControl(regionAStatus_);

		regionAStatus_ = regionStatus;

		addChildControl(regionAStatus_);
	}
}

void BioXASSSRLMonochromatorRegionControl::setRegionBStatusControl(AMControl *regionStatus)
{
	if (regionBStatus_ != regionStatus) {

		removeChildControl(regionBStatus_);

		regionBStatus_ = regionStatus;

		addChildControl(regionBStatus_);
	}
}

void BioXASSSRLMonochromatorRegionControl::updateConnected()
{
	bool slitsOK = ( upperSlitBlade_ && upperSlitBlade_->isConnected() && lowerSlitBlade_ && lowerSlitBlade_->isConnected() && slitsStatus_ && slitsStatus_->isConnected() );
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
					upperSlitBlade_->isMoving() ||
					lowerSlitBlade_->isMoving() ||
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
		action->addSubAction(createCloseSlitsAction());
		action->addSubAction(createRemovePaddleAction());
		action->addSubAction(createWaitForKeyEnabledAction());
		action->addSubAction(createMoveBraggToCrystalChangePositionAction());
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

AMAction3* BioXASSSRLMonochromatorRegionControl::createCloseUpperSlitAction()
{
	AMAction3 *action = 0;

	if (upperSlitBlade_ && upperSlitBlade_->isConnected())
		action = AMActionSupport::buildControlMoveAction(upperSlitBlade_, SETPOINT_SLIT_CLOSED);

	if (!action)
		AMErrorMon::error(this, BioXAS_MONO_REGION_CLOSE_UPPER_SLIT_FAILED, "Failed to create action to close mono upper slit.");

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createCloseLowerSlitAction()
{
	AMAction3 *action = 0;

	if (lowerSlitBlade_ && lowerSlitBlade_->isConnected())
		action = AMActionSupport::buildControlMoveAction(lowerSlitBlade_, SETPOINT_SLIT_CLOSED);

	if (!action)
		AMErrorMon::error(this, BioXAS_MONO_REGION_CLOSE_LOWER_SLIT_FAILED, "Failed to create action to close mono lower slit.");

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForSlitsClosedAction()
{
	AMAction3 *action = 0;

	if (slitsStatus_ && slitsStatus_->isConnected())
		action = AMActionSupport::buildControlWaitAction(slitsStatus_, BioXASSSRLMonochromator::Slits::Closed, TIMEOUT_SLITS_CLOSED);

	if (!action)
		AMErrorMon::error(this, BioXAS_MONO_REGION_CLOSE_SLITS_FAILED, "Failed to create action to wait for both mono slits to be closed.");

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

	return closeAndConfirm;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createMovePaddleAction(double destination)
{
	AMControl *control = paddle_;
	AMAction3 *action = 0;

	if (control && control->isConnected())
		action = AMActionSupport::buildControlMoveAction(control, destination);

	if (!action)
		AMErrorMon::error(this, BioXAS_MONO_REGION_MOVE_PADDLE_FAILED, "Failed to create action to move the mono paddle motor to destination " + QString::number(destination));

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

	if (!action)
		AMErrorMon::error(this, BioXAS_MONO_REGION_PADDLE_WAIT_FAILED, "Failed to create action to wait until mono paddle motor is removed.");

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

	if (!action)
		AMErrorMon::error(this, BioXAS_MONO_REGION_KEY_ENABLED_WAIT_FAILED, "Failed to create action to wait for the mono key to be turned to 'Enabled.'");

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createSetBraggToleranceAction(double newTolerance)
{
	AMControl *control = bragg_;
	AMAction3 *action = 0;

	if (control && control->isConnected())
		action = AMActionSupport::buildChangeToleranceAction(bragg_, newTolerance);

	if (!action)
		AMErrorMon::error(this, BioXAS_MONO_REGION_CHANGE_BRAGG_TOLERANCE_FAILED, "Failed to set the bragg motor tolerance.");

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createMoveBraggAction(double destination)
{
	AMControl *control = bragg_;
	AMAction3 *action = 0;

	if (control && control->isConnected())
		action = AMActionSupport::buildControlMoveAction(control, destination);

	if (!action)
		AMErrorMon::error(this, BioXAS_MONO_REGION_MOVE_BRAGG_FAILED, "Failed to create action to move the mono bragg motor to destination " + QString::number(destination));

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
	}

	if (!action)
		AMErrorMon::error(this, BioXAS_MONO_REGION_BRAGG_WAIT_FAILED, "Failed to create action to wait for the mono bragg motor to reach the crystal change position.");

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

	if (!action)
		AMErrorMon::error(this, BioXAS_MONO_REGION_BRAKE_DISABLED_WAIT_FAILED, "Failed to create action to wait for the mono brake to be disabled.");

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createMoveBraggToCrystalChangePositionAction()
{
	AMControl *control = bragg_;
	AMAction3 *action = 0;

	if (control && control->isConnected()) {
		double oldTolerance = bragg_->tolerance();

		AMListAction3 *moveAndConfirm = new AMListAction3(new AMListActionInfo3("MoveBraggToCCPositionAndConfirm", "Move bragg motor to the crystal change position and confirm it's in position"), AMListAction3::Sequential);
		moveAndConfirm->addSubAction(createSetBraggToleranceAction(1));
		moveAndConfirm->addSubAction(createMoveBraggAction(SETPOINT_BRAGG_MOTOR_CRYSTAL_CHANGE_POSITION));
		moveAndConfirm->addSubAction(createWaitForBraggAtCrystalChangePositionAction());
		moveAndConfirm->addSubAction(createSetBraggToleranceAction(oldTolerance));

		action = moveAndConfirm;
	}

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createMoveCrystalChangeAction(double destination)
{
	AMControl *control = crystalChange_;
	AMAction3 *action = 0;

	if (control && control->isConnected())
		action = AMActionSupport::buildControlMoveAction(control, destination);

	if (!action)
		AMErrorMon::error(this, BioXAS_MONO_REGION_MOVE_CRYSTAL_CHANGE_FAILED, "Failed to create action to move the mono crystal change motor to destination " + QString::number(destination));

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForCrystalChangeAtCWLimitAction()
{
	AMControl *control = crystalChangeCWLimitStatus_;
	AMListAction3 *waitAndConfirm = new AMListAction3(new AMListActionInfo3("Confirm cw limit reached", "Confirms crystal change motor cw limit was reached"), AMListAction3::Sequential);

	if (control && control->isConnected()) {
		AMControlInfo setpoint = control->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::CrystalChange::AtLimit);

		AMControlWaitAction *limitReached = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_CRYSTAL_CHANGE_MOTOR_LIMIT_REACHED, AMControlWaitActionInfo::MatchEqual), control);
		waitAndConfirm->addSubAction(limitReached);

		AMWaitAction *wait = new AMWaitAction(new AMWaitActionInfo(TIMEOUT_CRYSTAL_CHANGE_MOVE_WAIT));
		waitAndConfirm->addSubAction(wait);

		AMControlWaitAction *doubleCheck = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_CRYSTAL_CHANGE_MOTOR_LIMIT_REACHED, AMControlWaitActionInfo::MatchEqual), control);
		waitAndConfirm->addSubAction(doubleCheck);
	}

	return waitAndConfirm;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForCrystalChangeAtCCWLimitAction()
{
	AMControl *control = crystalChangeCCWLimitStatus_;
	AMListAction3 *waitAndConfirm = new AMListAction3(new AMListActionInfo3("Confirm ccw limit reached", "Confirms crystal change motor ccw limit was reached"), AMListAction3::Sequential);

	if (control && control->isConnected()) {
		AMControlInfo setpoint = control->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::CrystalChange::AtLimit);

		AMControlWaitAction *limitReached = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_CRYSTAL_CHANGE_MOTOR_LIMIT_REACHED, AMControlWaitActionInfo::MatchEqual), control);
		waitAndConfirm->addSubAction(limitReached);

		AMWaitAction *wait = new AMWaitAction(new AMWaitActionInfo(TIMEOUT_CRYSTAL_CHANGE_MOVE_WAIT));
		waitAndConfirm->addSubAction(wait);

		AMControlWaitAction *doubleCheck = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_CRYSTAL_CHANGE_MOTOR_LIMIT_REACHED, AMControlWaitActionInfo::MatchEqual), control);
		waitAndConfirm->addSubAction(doubleCheck);
	}

	return waitAndConfirm;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForCrystalChangeAtRegionLimitAction(int region)
{
	AMAction3 *action = 0;

	if (region == BioXASSSRLMonochromator::Region::A)
		action = createWaitForCrystalChangeAtCWLimitAction();
	else if (region == BioXASSSRLMonochromator::Region::B)
		action = createWaitForCrystalChangeAtCCWLimitAction();

	if (!action)
		AMErrorMon::error(this, BioXAS_MONO_REGION_CRYSTAL_CHANGE_WAIT_FAILED, "Failed to create action to wait for the mono crystal change motor to reach region " + regionStateToString(region));

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
	AMControl *control = brakeStatus_;
	AMAction3 *action = 0;

	if (control && control->isConnected()) {
		AMControlInfo setpoint = control->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::Brake::Enabled);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_BRAKE_STATUS_CHANGE, AMControlWaitActionInfo::MatchEqual), control);
	}

	if (!action)
		AMErrorMon::error(this, BioXAS_MONO_REGION_BRAKE_ENABLED_WAIT_FAILED, "Failed to create action to wait for the mono brake to be enabled.");

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForRegionChangedToAAction()
{
	AMControl *control = regionAStatus_;
	AMAction3 *action = 0;

	if (control && control->isConnected()) {
		AMControlInfo setpoint = control->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::Region::In);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_REGION_STATE_CHANGED, AMControlWaitActionInfo::MatchEqual), control);
	}

	if (!action)
		AMErrorMon::error(this, BioXAS_MONO_REGION_REGION_A_WAIT_FAILED, "Failed to create action to wait for the mono to reach region A.");

	return action;
}

AMAction3* BioXASSSRLMonochromatorRegionControl::createWaitForRegionChangedToBAction()
{
	AMControl *control = regionBStatus_;
	AMAction3 *action = 0;

	if (control && control->isConnected()) {
		AMControlInfo setpoint = control->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::Region::In);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_REGION_STATE_CHANGED, AMControlWaitActionInfo::MatchEqual), control);
	}

	if (!action)
		AMErrorMon::error(this, BioXAS_MONO_REGION_REGION_B_WAIT_FAILED, "Failed to create action to wait for the mono to reach region B.");

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
	AMControl *control = keyStatus_;
	AMAction3 *action = 0;

	if (control && control->isConnected()) {
		AMControlInfo setpoint = control->toInfo();
		setpoint.setValue(BioXASSSRLMonochromator::Key::Disabled);

		action = new AMControlWaitAction(new AMControlWaitActionInfo(setpoint, TIMEOUT_KEY_STATUS_CHANGE, AMControlWaitActionInfo::MatchEqual), control);
	}

	if (!action)
		AMErrorMon::error(this, BioXAS_MONO_REGION_KEY_DISABLED_WAIT_FAILED, "Failed to create action to wait for the mono key to be turned to 'Disabled.'");

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
