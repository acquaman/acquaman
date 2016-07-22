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

		removeChildControl(upstreamInboardMotor_);

		upstreamInboardMotor_ = newControl;

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

		removeChildControl(upstreamOutboardMotor_);

		upstreamOutboardMotor_ = newControl;

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

		removeChildControl(downstreamMotor_);

		downstreamMotor_ = newControl;

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


		removeChildControl(stripeSelectMotor_);

		stripeSelectMotor_ = newControl;

		addChildControl(stripeSelectMotor_);

		updateLateral();
		updateYaw();

		emit stripeSelectMotorChanged(stripeSelectMotor_);
	}
}

void BioXASMirror::setYawMotor(CLSMAXvMotor *newControl)
{
	if (yawMotor_ != newControl) {

		removeChildControl(yawMotor_);

		yawMotor_ = newControl;

		addChildControl(yawMotor_);

		updateLateral();
		updateYaw();

		emit yawMotorChanged(yawMotor_);
	}
}

void BioXASMirror::setUpstreamBenderMotor(CLSMAXvMotor *newControl)
{
	if (upstreamBenderMotor_ != newControl) {

		removeChildControl(upstreamBenderMotor_);

		upstreamBenderMotor_ = newControl;

		addChildControl(upstreamBenderMotor_);

		updateBend();

		emit upstreamBenderMotorChanged(upstreamBenderMotor_);
	}
}

void BioXASMirror::setDownstreamBenderMotor(CLSMAXvMotor *newControl)
{
	if (downstreamBenderMotor_ != newControl) {

		removeChildControl(downstreamBenderMotor_);

		downstreamBenderMotor_ = newControl;

		addChildControl(downstreamBenderMotor_);

		updateBend();

		emit downstreamBenderMotorChanged(downstreamBenderMotor_);
	}
}

void BioXASMirror::setPitch(BioXASMirrorPitchControl *newControl)
{
	if (pitch_ != newControl) {

		removeChildControl(pitch_);

		pitch_ = newControl;

		addChildControl(pitch_);

		updatePitch();

		emit pitchChanged(pitch_);
	}
}

void BioXASMirror::setRoll(BioXASMirrorRollControl *newControl)
{
	if (roll_ != newControl) {

		removeChildControl(roll_);

		roll_ = newControl;

		addChildControl(roll_);

		updateRoll();

		emit rollChanged(roll_);
	}
}

void BioXASMirror::setHeight(BioXASMirrorHeightControl *newControl)
{
	if (height_ != newControl) {

		removeChildControl(height_);

		height_ = newControl;

		addChildControl(height_);

		updateHeight();

		emit heightChanged(height_);
	}
}

void BioXASMirror::setLateral(BioXASMirrorLateralControl *newControl)
{
	if (lateral_ != newControl) {

		removeChildControl(lateral_);

		lateral_ = newControl;

		addChildControl(lateral_);

		updateLateral();

		emit lateralChanged(lateral_);
	}
}

void BioXASMirror::setYaw(BioXASMirrorYawControl *newControl)
{
	if (yaw_ != newControl) {

		removeChildControl(yaw_);

		yaw_ = newControl;

		addChildControl(yaw_);

		updateYaw();

		emit yawChanged(yaw_);
	}
}

void BioXASMirror::setBend(BioXASMirrorBendControl *newControl)
{
	if (bend_ != newControl) {

		removeChildControl(bend_);

		bend_ = newControl;

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
