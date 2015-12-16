#include "BioXASM1Mirror.h"

BioXASM1Mirror::BioXASM1Mirror(const QString &name, QObject *parent) :
	BioXASMirror(name, parent)
{
	// Initialize member variables.

	upperSlitBladeMotor_ = 0;
}

BioXASM1Mirror::~BioXASM1Mirror()
{

}

bool BioXASM1Mirror::isConnected() const
{
	bool isConnected = (
				BioXASMirror::isConnected() &&
				upperSlitBladeMotor_ && upperSlitBladeMotor_->isConnected()
				);

	return isConnected;
}

void BioXASM1Mirror::setUpperSlitBladeMotor(CLSMAXvMotor *newControl)
{
	if (upperSlitBladeMotor_ != newControl) {

		if (upperSlitBladeMotor_)
			removeChildControl(upperSlitBladeMotor_);

		upperSlitBladeMotor_ = newControl;

		if (upperSlitBladeMotor_)
			addChildControl(upperSlitBladeMotor_);

		emit upperSlitBladeMotorChanged(upperSlitBladeMotor_);
	}
}
