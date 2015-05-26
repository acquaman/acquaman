#include "BioXASM2Mirror.h"

BioXASM2Mirror::BioXASM2Mirror(const QString &name, QObject *parent) :
	BioXASMirror(name, parent)
{
	// Initialize member variables.

	screen_ = 0;
}

BioXASM2Mirror::~BioXASM2Mirror()
{

}

bool BioXASM2Mirror::isConnected() const
{
	bool isConnected = (
				BioXASMirror::isConnected() &&
				screen_ && screen_->isConnected()
				);

	return isConnected;
}


