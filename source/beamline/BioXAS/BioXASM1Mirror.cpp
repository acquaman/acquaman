#include "BioXASM1Mirror.h"

BioXASM1Mirror::BioXASM1Mirror(const QString &name, QObject *parent) :
	BioXASMirror(name, parent)
{
	// Initialize member variables.

	upperSlitBlade_ = 0;
}

BioXASM1Mirror::~BioXASM1Mirror()
{

}

bool BioXASM1Mirror::isConnected() const
{
	bool isConnected = (
				BioXASMirror::isConnected() &&
				upperSlitBlade_ && upperSlitBlade_->isConnected()
				);

	return isConnected;
}
