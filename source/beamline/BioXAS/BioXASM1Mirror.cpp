#include "BioXASM1Mirror.h"

BioXASM1Mirror::BioXASM1Mirror(QObject *parent) :
	BioXASMirror(parent)
{
	// Initialize member variables.

	upperSlitBlade_ = 0;
}

BioXASM1Mirror::~BioXASM1Mirror()
{

}

void BioXASM1Mirror::updateConnected()
{
	bool isConnected = (
				BioXASMirror::connected() &&
				upperSlitBlade_ && upperSlitBlade_->isConnected()
				);

	setConnected(isConnected);
}
