#include "BioXASMirror.h"

BioXASMirror::BioXASMirror(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	// Initialize member variables.

	upstreamInboardMotor_ = 0;
	upstreamOutboardMotor_ = 0;
	downstreamMotor_ = 0;
	stripeSelectMotor_ = 0;
	yawMotor_ = 0;
	upstreamBenderMotor_ = 0;
	downstreamBenderMotor_ = 0;

	pitch_ = 0;
	roll_ = 0;
	height_ = 0;
	lateral_ = 0;
	yaw_ = 0;
	bend_ = 0;

	upstreamLength_ = 0;
	downstreamLength_ = 0;
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
					stripeSelectMotor_->canStop() &&
					yawMotor_->canStop() &&
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
	bool isConnected = (
				upstreamInboardMotor_ && upstreamInboardMotor_->isConnected() &&
				upstreamOutboardMotor_ && upstreamOutboardMotor_->isConnected() &&
				downstreamMotor_ && downstreamMotor_->isConnected() &&
				stripeSelectMotor_ && stripeSelectMotor_->isConnected() &&
				yawMotor_ && yawMotor_->isConnected() &&
				upstreamBenderMotor_ && upstreamBenderMotor_->isConnected() &&
				downstreamBenderMotor_ && downstreamBenderMotor_->isConnected() &&

				pitch_ && pitch_->isConnected() &&
				roll_ && roll_->isConnected() &&
				height_ && height_->isConnected() &&
				lateral_ && lateral_->isConnected() &&
				yaw_ && yaw_->isConnected() &&
				bend_ && bend_->isConnected()
				);

	return isConnected;
}

void BioXASMirror::setUpstreamLength(double newLength)
{
	if (upstreamLength_ != newLength) {
		upstreamLength_ = newLength;

		updateYaw();
		updateLateral();

		emit upstreamLengthChanged(upstreamLength_);
	}
}

void BioXASMirror::setDownstreamLength(double newLength)
{
	if (downstreamLength_ != newLength) {
		downstreamLength_ = newLength;

		updateYaw();
		updateLateral();

		emit downstreamLengthChanged(downstreamLength_);
	}
}

void BioXASMirror::setUpstreamInboardMotor(BioXASMirrorMotor *newControl)
{
	if (upstreamInboardMotor_ != newControl) {

		if (upstreamInboardMotor_)
			removeChildControl(upstreamInboardMotor_);

		upstreamInboardMotor_ = newControl;

		if (upstreamInboardMotor_)
			addChildControl(upstreamInboardMotor_);

		updatePitch();
		updateRoll();
		updateHeight();

		emit upstreamInboardMotorChanged(upstreamInboardMotor_);
	}
}

void BioXASMirror::setUpstreamOutboardMotor(BioXASMirrorMotor *newControl)
{
	if (upstreamOutboardMotor_ != newControl) {

		if (upstreamOutboardMotor_)
			removeChildControl(upstreamOutboardMotor_);

		upstreamOutboardMotor_ = newControl;

		if (upstreamOutboardMotor_)
			addChildControl(upstreamOutboardMotor_);

		updatePitch();
		updateRoll();
		updateHeight();

		emit upstreamOutboardMotorChanged(upstreamOutboardMotor_);
	}
}

void BioXASMirror::setDownstreamMotor(BioXASMirrorMotor *newControl)
{
	if (downstreamMotor_ != newControl) {

		if (downstreamMotor_)
			removeChildControl(downstreamMotor_);

		downstreamMotor_ = newControl;

		if (downstreamMotor_)
			addChildControl(downstreamMotor_);

		updatePitch();
		updateRoll();
		updateHeight();

		emit downstreamMotorChanged(downstreamMotor_);
	}
}

void BioXASMirror::setStripeSelectMotor(CLSMAXvMotor *newControl)
{
	if (stripeSelectMotor_ != newControl) {

		if (stripeSelectMotor_)
			removeChildControl(stripeSelectMotor_);

		stripeSelectMotor_ = newControl;

		if (stripeSelectMotor_)
			addChildControl(stripeSelectMotor_);

		updateLateral();
		updateYaw();

		emit stripeSelectMotorChanged(stripeSelectMotor_);
	}
}

void BioXASMirror::setYawMotor(CLSMAXvMotor *newControl)
{
	if (yawMotor_ != newControl) {

		if (yawMotor_)
			removeChildControl(yawMotor_);

		yawMotor_ = newControl;

		if (yawMotor_)
			addChildControl(yawMotor_);

		updateLateral();
		updateYaw();

		emit yawMotorChanged(yawMotor_);
	}
}

void BioXASMirror::setUpstreamBenderMotor(CLSMAXvMotor *newControl)
{
	if (upstreamBenderMotor_ != newControl) {

		if (upstreamBenderMotor_)
			removeChildControl(upstreamBenderMotor_);

		upstreamBenderMotor_ = newControl;

		if (upstreamBenderMotor_)
			addChildControl(upstreamBenderMotor_);

		updateBend();

		emit upstreamBenderMotorChanged(upstreamBenderMotor_);
	}
}

void BioXASMirror::setDownstreamBenderMotor(CLSMAXvMotor *newControl)
{
	if (downstreamBenderMotor_ != newControl) {

		if (downstreamBenderMotor_)
			removeChildControl(downstreamBenderMotor_);

		downstreamBenderMotor_ = newControl;

		if (downstreamBenderMotor_)
			addChildControl(downstreamBenderMotor_);

		updateBend();

		emit downstreamBenderMotorChanged(downstreamBenderMotor_);
	}
}

void BioXASMirror::setPitch(BioXASMirrorPitchControl *newControl)
{
	if (pitch_ != newControl) {

		if (pitch_)
			removeChildControl(pitch_);

		pitch_ = newControl;

		if (pitch_)
			addChildControl(pitch_);

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

		if (roll_)
			addChildControl(roll_);

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

		if (height_)
			addChildControl(height_);

		updateHeight();

		emit heightChanged(height_);
	}
}

void BioXASMirror::setLateral(BioXASMirrorLateralControl *newControl)
{
	if (lateral_ != newControl) {

		if (lateral_)
			removeChildControl(lateral_);

		lateral_ = newControl;

		if (lateral_)
			addChildControl(lateral_);

		updateLateral();

		emit lateralChanged(lateral_);
	}
}

void BioXASMirror::setYaw(BioXASMirrorYawControl *newControl)
{
	if (yaw_ != newControl) {

		if (yaw_)
			removeChildControl(yaw_);

		yaw_ = newControl;

		if (yaw_)
			addChildControl(yaw_);

		updateYaw();

		emit yawChanged(yaw_);
	}
}

void BioXASMirror::setBend(BioXASMirrorBendControl *newControl)
{
	if (bend_ != newControl) {

		if (bend_)
			removeChildControl(bend_);

		bend_ = newControl;

		if (bend_)
			addChildControl(bend_);

		updateBend();

		emit bendChanged(bend_);
	}
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
		lateral_->setStripeSelectionMotor(stripeSelectMotor_);
		lateral_->setYawMotor(yawMotor_);
		lateral_->setUpstreamLength(upstreamLength_);
		lateral_->setDownstreamLength(downstreamLength_);
	}
}

void BioXASMirror::updateYaw()
{
	if (yaw_) {
		yaw_->setYawMotor(yawMotor_);
		yaw_->setStripeSelectionMotor(stripeSelectMotor_);
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
