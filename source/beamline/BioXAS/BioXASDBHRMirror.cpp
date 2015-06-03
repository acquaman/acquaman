#include "BioXASDBHRMirror.h"

BioXASDBHRMirror::BioXASDBHRMirror(QObject *parent) :
	QObject(parent)
{
	// Initialize class variables.

	isConnected_ = false;
	m1Vertical_ = 0;
	m2Vertical_ = 0;
	pitch_ = 0;
}

BioXASDBHRMirror::~BioXASDBHRMirror()
{

}

void BioXASDBHRMirror::setConnected(bool isConnected)
{
	if (isConnected_ != isConnected) {
		emit connectedChanged(isConnected_ = isConnected);
	}
}

void BioXASDBHRMirror::setM1MirrorControl(CLSMAXvMotor *newControl)
{
	if (m1Vertical_ != newControl) {

		if (m1Vertical_) {
			controlCleanup(m1Vertical_);
			m1Vertical_ = 0;
		}

		m1Vertical_ = newControl;

		if (m1Vertical_) {
			controlSetup(m1Vertical_);
		}

		emit m1VerticalControlChanged(m1Vertical_);
	}
}

void BioXASDBHRMirror::setM2MirrorControl(CLSMAXvMotor *newControl)
{
	if (m2Vertical_ != newControl) {

		if (m2Vertical_) {
			controlCleanup(m2Vertical_);
			m2Vertical_ = 0;
		}

		m2Vertical_ = newControl;

		if (m2Vertical_) {
			controlSetup(m2Vertical_);
		}

		emit m2VerticalControlChanged(m2Vertical_);
	}
}

void BioXASDBHRMirror::setPitchControl(CLSMAXvMotor *newControl)
{
	if (pitch_ != newControl) {

		if (pitch_) {
			controlCleanup(pitch_);
			pitch_ = 0;
		}

		pitch_ = newControl;

		if (pitch_) {
			controlSetup(pitch_);
		}

		emit pitchControlChanged(pitch_);
	}
}

void BioXASDBHRMirror::onConnectedChanged()
{
	bool m1OK = (m1Vertical_ && m1Vertical_->isConnected());
	bool m2OK = (m2Vertical_ && m2Vertical_->isConnected());
	bool pitchOK = (pitch_ && pitch_->isConnected());

	setConnected(m1OK && m2OK && pitchOK);
}

void BioXASDBHRMirror::controlSetup(AMControl *control)
{
	if (control) {
		control->setParent(this);
		connect( control, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()) );
	}
}

void BioXASDBHRMirror::controlCleanup(AMControl *control)
{
	if (control) {
		control->disconnect();
		control->deleteLater();
	}
}
