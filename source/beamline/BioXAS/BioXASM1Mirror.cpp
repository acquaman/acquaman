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
