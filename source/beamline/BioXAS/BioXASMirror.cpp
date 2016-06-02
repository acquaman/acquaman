#include "BioXASMirror.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"
#include "util/AMErrorMonitor.h"

BioXASMirror::BioXASMirror(const QString &name, QObject *parent, const QString &description) :
	BioXASMirrorControl(name, "", parent, description)
{
	// Initialize member variables.

	pitch_ = 0;
	roll_ = 0;
	height_ = 0;
	lateral_ = 0;
	yaw_ = 0;
	bend_ = 0;

	// Current settings.

//	setPitch(new BioXASMirrorPitchControl(QString("%1%2").arg(name()).arg("Pitch"), "deg", this));
//	setRoll(new BioXASMirrorRollControl(QString("%1%2").arg(name()).arg("Roll"), "deg", this));
//	setHeight(new BioXASMirrorHeightControl(QString("%1%2").arg(name()).arg("Height"), "mm", this));
//	setYaw(new BioXASMirrorYawControl(QString("%1%2").arg(name()).arg("Yaw"), "deg", this));
//	setLateral(new BioXASMirrorLateralControl(QString("%1%2").arg(name()).arg("Lateral"), "mm", this));
}

BioXASMirror::~BioXASMirror()
{

}

bool BioXASMirror::canStop() const
{
	bool result = false;

	if (isConnected()) {
		bool stoppable = (
					upstreamInboardMotor_->canStop() &&
					upstreamOutboardMotor_->canStop() &&
					downstreamMotor_->canStop() &&
					yawMotor_->canStop() &&
					lateralMotor_->canStop() &&
					upstreamBenderMotor_->canStop() &&
					downstreamBenderMotor_->canStop() &&

					pitch_->canStop() &&
					roll_->canStop() &&
					height_->canStop() &&
					lateral_->canStop() &&
					yaw_->canStop() &&
					bend_->canStop()
					);

		result = stoppable;
	}

	return result;
}

bool BioXASMirror::isConnected() const
{
	bool connected = (
				upstreamInboardMotor_ && upstreamInboardMotor_->isConnected() &&
				upstreamOutboardMotor_ && upstreamOutboardMotor_->isConnected() &&
				downstreamMotor_ && downstreamMotor_->isConnected() &&
				yawMotor_ && yawMotor_->isConnected() &&
				lateralMotor_ && lateralMotor_->isConnected() &&
				upstreamBenderMotor_ && upstreamBenderMotor_->isConnected() &&
				downstreamBenderMotor_ && downstreamBenderMotor_->isConnected() &&

				pitch_ && pitch_->isConnected() &&
				roll_ && roll_->isConnected() &&
				height_ && height_->isConnected() &&
				lateral_ && lateral_->isConnected() &&
				yaw_ && yaw_->isConnected() &&
				bend_ && bend_->isConnected()
				);

	return connected;
}

double BioXASMirror::pitchValue() const
{
	double result = 0;

	if (pitch_ && pitch_->canMeasure())
		result = pitch_->value();

	return result;
}

double BioXASMirror::rollValue() const
{
	double result = 0;

	if (roll_ && roll_->canMeasure())
		result = roll_->value();

	return result;
}

double BioXASMirror::heightValue() const
{
	double result = 0;

	if (height_ && height_->canMeasure())
		result = height_->value();

	return result;
}

double BioXASMirror::yawValue() const
{
	double result = 0;

	if (yaw_ && yaw_->canMeasure())
		result = yaw_->value();

	return result;
}

double BioXASMirror::lateralValue() const
{
	double result = 0;

	if (lateral_ && lateral_->canMeasure())
		result = lateral_->value();

	return result;
}

double BioXASMirror::bendValue() const
{
	double result = 0;

	if (bend_ && bend_->canMeasure())
		result = bend_->value();

	return result;
}

void BioXASMirror::setUpstreamLength(double newLength)
{
	BioXASMirrorControl::setUpstreamLength(newLength);

	updateYaw();
	updateLateral();
}

void BioXASMirror::setDownstreamLength(double newLength)
{
	BioXASMirrorControl::setDownstreamLength(newLength);

	updateYaw();
	updateLateral();
}

void BioXASMirror::setUpstreamInboardMotor(BioXASMirrorMotor *newControl)
{
	BioXASMirrorControl::setUpstreamInboardMotor(newControl);

	updatePitch();
	updateRoll();
	updateHeight();
}

void BioXASMirror::setUpstreamOutboardMotor(BioXASMirrorMotor *newControl)
{
	BioXASMirrorControl::setUpstreamOutboardMotor(newControl);

	updatePitch();
	updateRoll();
	updateHeight();
}

void BioXASMirror::setDownstreamMotor(BioXASMirrorMotor *newControl)
{
	BioXASMirrorControl::setDownstreamMotor(newControl);

	updatePitch();
	updateRoll();
	updateHeight();
}

void BioXASMirror::setYawMotor(AMControl *newControl)
{
	BioXASMirrorControl::setYawMotor(newControl);

	updateLateral();
	updateYaw();
}

void BioXASMirror::setLateralMotor(AMControl *newControl)
{
	BioXASMirrorControl::setLateralMotor(newControl);

	updateLateral();
	updateYaw();
}

void BioXASMirror::setUpstreamBenderMotor(AMControl *newControl)
{
	BioXASMirrorControl::setUpstreamBenderMotor(newControl);

	updateBend();
}

void BioXASMirror::setDownstreamBenderMotor(AMControl *newControl)
{
	BioXASMirrorControl::setDownstreamBenderMotor(newControl);

	updateBend();
}

void BioXASMirror::setPitch(BioXASMirrorPitchControl *newControl)
{
	if (pitch_ != newControl) {

		if (pitch_)
			removeChildControl(pitch_);

		pitch_ = newControl;

		if (pitch_) {
			addChildControl(pitch_);
			connect( pitch_, SIGNAL(valueChanged(double)), this, SIGNAL(pitchValueChanged(double)) );

			emit pitchValueChanged(pitch_->value());
		}

		updatePitch();

		emit pitchChanged(pitch_);
	}
}

void BioXASMirror::setRoll(BioXASMirrorRollControl *newControl)
{
	if (roll_ != newControl) {

		if (roll_)
			removeChildControl(roll_);

		roll_ = newControl;

		if (roll_) {
			addChildControl(roll_);
			connect( roll_, SIGNAL(valueChanged(double)), this, SIGNAL(rollValueChanged(double)) );

			emit rollValueChanged(roll_->value());
		}

		updateRoll();

		emit rollChanged(roll_);
	}
}

void BioXASMirror::setHeight(BioXASMirrorHeightControl *newControl)
{
	if (height_ != newControl) {

		if (height_)
			removeChildControl(height_);

		height_ = newControl;

		if (height_) {
			addChildControl(height_);
			connect( height_, SIGNAL(valueChanged(double)), this, SIGNAL(heightValueChanged(double)) );

			emit heightValueChanged(height_->value());
		}

		updateHeight();

		emit heightChanged(height_);
	}
}

void BioXASMirror::setYaw(BioXASMirrorYawControl *newControl)
{
	if (yaw_ != newControl) {

		if (yaw_)
			removeChildControl(yaw_);

		yaw_ = newControl;

		if (yaw_) {
			addChildControl(yaw_);
			connect( yaw_, SIGNAL(valueChanged(double)), this, SIGNAL(yawValueChanged(double)) );

			emit yawValueChanged(yaw_->value());
		}

		updateYaw();

		emit yawChanged(yaw_);
	}
}

void BioXASMirror::setLateral(BioXASMirrorLateralControl *newControl)
{
	if (lateral_ != newControl) {

		if (lateral_)
			removeChildControl(lateral_);

		lateral_ = newControl;

		if (lateral_) {
			addChildControl(lateral_);
			connect( lateral_, SIGNAL(valueChanged(double)), this, SIGNAL(lateralValueChanged(double)) );

			emit lateralValueChanged(lateral_->value());
		}

		updateLateral();

		emit lateralChanged(lateral_);
	}
}

void BioXASMirror::setBend(BioXASMirrorBendControl *newControl)
{
	if (bend_ != newControl) {

		if (bend_)
			removeChildControl(bend_);

		bend_ = newControl;

		if (bend_) {
			addChildControl(bend_);
			connect( bend_, SIGNAL(valueChanged(double)), this, SIGNAL(bendValueChanged(double)) );

			emit bendValueChanged(bend_->value());
		}

		updateBend();

		emit bendChanged(bend_);
	}
}

AMControl::FailureExplanation BioXASMirror::moveMirror(double pitch, double roll, double height, double yaw, double lateral)
{
	// Check that this control is connected and able to move before proceeding.

	if (!(upstreamInboardMotor_ && upstreamInboardMotor_->isConnected()) || !(upstreamOutboardMotor_ && upstreamOutboardMotor_->isConnected()) || !(downstreamMotor_ && downstreamMotor_->isConnected()) || !(yawMotor_ && yawMotor_->isConnected()) || !(lateralMotor_ && lateralMotor_->isConnected())) {
		AMErrorMon::alert(this, BIOXASMIRROR_NOT_CONNECTED, QString("Failed to move %1: mirror is not connected.").arg(name()));
		return AMControl::NotConnectedFailure;
	}

	if (!upstreamInboardMotor_->canMove() || !upstreamOutboardMotor_->canMove() || !downstreamMotor_->canMove() || !yawMotor_->canMove() || !lateralMotor_->canMove()) {
		AMErrorMon::alert(this, BIOXASMIRROR_CANNOT_MOVE, QString("Failed to move %1: mirror cannot move.").arg(name()));
		return AMControl::OtherFailure;
	}

	if ((upstreamInboardMotor_->isMoving() && !upstreamInboardMotor_->allowsMovesWhileMoving()) || (upstreamOutboardMotor_->isMoving() && !upstreamOutboardMotor_->allowsMovesWhileMoving()) || (downstreamMotor_->isMoving() && !downstreamMotor_->allowsMovesWhileMoving()) || (yawMotor_->isMoving() && !yawMotor_->allowsMovesWhileMoving()) || (lateralMotor_->isMoving() && !lateralMotor_->allowsMovesWhileMoving())) {
		AMErrorMon::alert(this, BIOXASMIRROR_ALREADY_MOVING, QString("Failed to move %1: mirror is already moving.").arg(name()));
		return AMControl::AlreadyMovingFailure;
	}

	// Create move action.

	AMListAction3 *moveAction = new AMListAction3(new AMListActionInfo3(name()+" move", name()+" move"), AMListAction3::Parallel);

	double upstreamInboardDestination = calculateUpstreamInboardPosition(upstreamInboardMotor_->xPosition(), upstreamInboardMotor_->yPosition(), pitch, roll, height);
	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(upstreamInboardMotor_, upstreamInboardDestination));

	double upstreamOutboardDestination = calculateUpstreamOutboardPosition(upstreamOutboardMotor_->xPosition(), upstreamOutboardMotor_->yPosition(), pitch, roll, height);
	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(upstreamOutboardMotor_, upstreamOutboardDestination));

	double downstreamDestination = calculateDownstreamPosition(downstreamMotor_->xPosition(), downstreamMotor_->yPosition(), pitch, roll, height);
	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(downstreamMotor_, downstreamDestination));

	double yawMotorDestination = calculateYawPosition(yaw, upstreamLength_, downstreamLength_);
	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(yawMotor_, yawMotorDestination));

	double lateralMotorDestination = calculateLateralPosition(lateral, upstreamLength_, downstreamLength_, yaw);
	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(lateralMotor_, lateralMotorDestination));

	// Create move action signal mappings.

	startedMapper_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(started()), startedMapper_, SLOT(map()) );

	cancelledMapper_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(cancelled()), cancelledMapper_, SLOT(map()) );

	failedMapper_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(failed()), failedMapper_, SLOT(map()) );

	succeededMapper_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(succeeded()), succeededMapper_, SLOT(map()) );

	// Run action.

	moveAction->start();

	return AMControl::NoFailure;
}

AMControl::FailureExplanation BioXASMirror::moveMirror(double pitch, double roll, double height)
{
	// Check that this control is connected and able to move before proceeding.

	if (!(upstreamInboardMotor_ && upstreamInboardMotor_->isConnected()) || !(upstreamOutboardMotor_ && upstreamOutboardMotor_->isConnected()) || !(downstreamMotor_ && downstreamMotor_->isConnected())) {
		AMErrorMon::alert(this, BIOXASMIRROR_NOT_CONNECTED, QString("Failed to move %1: mirror is not connected.").arg(name()));
		return AMControl::NotConnectedFailure;
	}

	if (!upstreamInboardMotor_->canMove() || !upstreamOutboardMotor_->canMove() || !downstreamMotor_->canMove()) {
		AMErrorMon::alert(this, BIOXASMIRROR_CANNOT_MOVE, QString("Failed to move %1: mirror cannot move.").arg(name()));
		return AMControl::OtherFailure;
	}

	if ((upstreamInboardMotor_->isMoving() && !upstreamInboardMotor_->allowsMovesWhileMoving()) || (upstreamOutboardMotor_->isMoving() && !upstreamOutboardMotor_->allowsMovesWhileMoving()) || (downstreamMotor_->isMoving() && !downstreamMotor_->allowsMovesWhileMoving())) {
		AMErrorMon::alert(this, BIOXASMIRROR_ALREADY_MOVING, QString("Failed to move %1: mirror is already moving.").arg(name()));
		return AMControl::AlreadyMovingFailure;
	}

	// Create move action.

	AMListAction3 *moveAction = new AMListAction3(new AMListActionInfo3(name()+" move", name()+" move"), AMListAction3::Parallel);

	double upstreamInboardDestination = calculateUpstreamInboardPosition(upstreamInboardMotor_->xPosition(), upstreamInboardMotor_->yPosition(), pitch, roll, height);
	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(upstreamInboardMotor_, upstreamInboardDestination));

	double upstreamOutboardDestination = calculateUpstreamOutboardPosition(upstreamOutboardMotor_->xPosition(), upstreamOutboardMotor_->yPosition(), pitch, roll, height);
	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(upstreamOutboardMotor_, upstreamOutboardDestination));

	double downstreamDestination = calculateDownstreamPosition(downstreamMotor_->xPosition(), downstreamMotor_->yPosition(), pitch, roll, height);
	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(downstreamMotor_, downstreamDestination));

	// Create move action signal mappings.

	startedMapper_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(started()), startedMapper_, SLOT(map()) );

	cancelledMapper_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(cancelled()), cancelledMapper_, SLOT(map()) );

	failedMapper_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(failed()), failedMapper_, SLOT(map()) );

	succeededMapper_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(succeeded()), succeededMapper_, SLOT(map()) );

	// Run action.

	moveAction->start();

	return AMControl::NoFailure;
}

AMControl::FailureExplanation BioXASMirror::moveMirrorPitch(double pitch)
{
	// Check that this control is connected and able to move before proceeding.

	if (!(roll_ && roll_->canMeasure()) || (!height_ && height_->canMeasure())) {
		AMErrorMon::alert(this, BIOXASMIRROR_NOT_CONNECTED, QString("Failed to move %1: control is not connected.").arg(name()));
		return AMControl::NotConnectedFailure;
	}

	// Move the mirror.

	return moveMirror(pitch, roll_->value(), height_->value());
}

AMControl::FailureExplanation BioXASMirror::moveMirrorRoll(double roll)
{
	// Check that this control is connected and able to move before proceeding.

	if (!(pitch_ && pitch_->canMeasure()) || !(height_ && height_->canMeasure())) {
		AMErrorMon::alert(this, BIOXASMIRROR_NOT_CONNECTED, QString("Failed to move %1: control is not connected.").arg(name()));
		return AMControl::NotConnectedFailure;
	}

	// Move the mirror.

	return moveMirror(pitch_->value(), roll, height_->value());
}

AMControl::FailureExplanation BioXASMirror::moveMirrorHeight(double height)
{
	// Check that this control is connected and able to move before proceeding.

	if (!(pitch_ && pitch_->canMeasure()) || !(roll_ && roll_->canMeasure())) {
		AMErrorMon::alert(this, BIOXASMIRROR_NOT_CONNECTED, QString("Failed to move %1: control is not connected.").arg(name()));
		return AMControl::NotConnectedFailure;
	}

	// Move the mirror.

	return moveMirror(pitch_->value(), roll_->value(), height);
}

AMControl::FailureExplanation BioXASMirror::moveMirror(double yaw, double lateral)
{
	// Check that this control is connected and able to move before proceeding.

	if (!(yawMotor_ && yawMotor_->isConnected()) || !(lateralMotor_ && lateralMotor_->isConnected())) {
		AMErrorMon::alert(this, BIOXASMIRROR_NOT_CONNECTED, QString("Failed to move %1: mirror is not connected.").arg(name()));
		return AMControl::NotConnectedFailure;
	}

	if (!yawMotor_->canMove() || !lateralMotor_->canMove()) {
		AMErrorMon::alert(this, BIOXASMIRROR_CANNOT_MOVE, QString("Failed to move %1: mirror cannot move.").arg(name()));
		return AMControl::OtherFailure;
	}

	if ((yawMotor_->isMoving() && !yawMotor_->allowsMovesWhileMoving()) || (lateralMotor_->isMoving() && !lateralMotor_->allowsMovesWhileMoving())) {
		AMErrorMon::alert(this, BIOXASMIRROR_ALREADY_MOVING, QString("Failed to move %1: mirror is already moving.").arg(name()));
		return AMControl::AlreadyMovingFailure;
	}

	// Create move action.

	AMListAction3 *moveAction = new AMListAction3(new AMListActionInfo3(name()+" move", name()+" move"), AMListAction3::Parallel);

	double yawMotorDestination = calculateYawPosition(yaw, upstreamLength_, downstreamLength_);
	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(yawMotor_, yawMotorDestination));

	double lateralMotorDestination = calculateLateralPosition(lateral, upstreamLength_, downstreamLength_, yaw);
	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(lateralMotor_, lateralMotorDestination));

	// Create move action signal mappings.

	startedMapper_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(started()), startedMapper_, SLOT(map()) );

	cancelledMapper_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(cancelled()), cancelledMapper_, SLOT(map()) );

	failedMapper_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(failed()), failedMapper_, SLOT(map()) );

	succeededMapper_->setMapping(moveAction, moveAction);
	connect( moveAction, SIGNAL(succeeded()), succeededMapper_, SLOT(map()) );

	// Run action.

	moveAction->start();

	return AMControl::NoFailure;
}

AMControl::FailureExplanation BioXASMirror::moveMirrorYaw(double yaw)
{
	// Check that this control is connected and able to move before proceeding.

	if (!(lateral_ && lateral_->canMeasure())) {
		AMErrorMon::alert(this, BIOXASMIRROR_NOT_CONNECTED, QString("Failed to move %1: control is not connected.").arg(name()));
		return AMControl::NotConnectedFailure;
	}

	// Move the mirror.

	return moveMirror(yaw, lateral_->value());
}

AMControl::FailureExplanation BioXASMirror::moveMirrorLateral(double lateral)
{
	// Check that this control is connected and able to move before proceeding.

	if (!(yaw_ && yaw_->canMeasure())) {
		AMErrorMon::alert(this, BIOXASMIRROR_NOT_CONNECTED, QString("Failed to move %1: control is not connected.").arg(name()));
		return AMControl::NotConnectedFailure;
	}

	// Move the mirror.

	return moveMirror(yaw_->value(), lateral);
}

void BioXASMirror::updatePitch()
{
	if (pitch_) {
		pitch_->setUpstreamInboardMotor(upstreamInboardMotor_);
		pitch_->setUpstreamOutboardMotor(upstreamOutboardMotor_);
		pitch_->setDownstreamMotor(downstreamMotor_);
	}
}

void BioXASMirror::updateRoll()
{
	if (roll_) {
		roll_->setUpstreamInboardMotor(upstreamInboardMotor_);
		roll_->setUpstreamOutboardMotor(upstreamOutboardMotor_);
		roll_->setDownstreamMotor(downstreamMotor_);
	}
}

void BioXASMirror::updateHeight()
{
	if (height_) {
		height_->setUpstreamInboardMotor(upstreamInboardMotor_);
		height_->setUpstreamOutboardMotor(upstreamOutboardMotor_);
		height_->setDownstreamMotor(downstreamMotor_);
	}
}

void BioXASMirror::updateLateral()
{
	if (lateral_) {
		lateral_->setLateralMotor(lateralMotor_);
		lateral_->setYawMotor(yawMotor_);
		lateral_->setUpstreamLength(upstreamLength_);
		lateral_->setDownstreamLength(downstreamLength_);
	}
}

void BioXASMirror::updateYaw()
{
	if (yaw_) {
		yaw_->setYawMotor(yawMotor_);
		yaw_->setLateralMotor(lateralMotor_);
		yaw_->setUpstreamLength(upstreamLength_);
		yaw_->setDownstreamLength(downstreamLength_);
	}
}

void BioXASMirror::updateBend()
{
	if (bend_) {
		bend_->setUpstreamBenderMotor(upstreamBenderMotor_);
		bend_->setDownstreamBenderMotor(downstreamBenderMotor_);
	}
}
