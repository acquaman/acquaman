#include "BioXASM2Mirror.h"

BioXASM2Mirror::BioXASM2Mirror(QObject *parent) :
	BioXASMirror(parent)
{
	// Initialize member variables.

	screen_ = 0;
}

BioXASM2Mirror::~BioXASM2Mirror()
{

}

void BioXASM2Mirror::setScreenPosition(Screen::Position newPosition)
{
	if (screen_ && screen_->isConnected() && newPosition != Screen::Invalid) {
		screen_->move(newPosition);
	}
}

void BioXASM2Mirror::updateConnected()
{
	bool isConnected = (
				BioXASMirror::connected() &&
				screen_ && screen_->isConnected()
				);

	setConnected(isConnected);
}


