#include "BioXASM2Mirror.h"

BioXASM2Mirror::BioXASM2Mirror(QObject *parent) :
    QObject(parent)
{
	// Initialize class variables.
	// Subclasses should create instances of each control.

	isConnected_ = false;

	screen_ = 0;
}

BioXASM2Mirror::~BioXASM2Mirror()
{

}

double BioXASM2Mirror::screenPosition() const
{
	double result = Screen::Invalid;

	if (screen_ && screen_->isConnected()) {
		result = screen_->value();
	}

	return result;
}

void BioXASM2Mirror::setScreenPosition(Screen::Position newPosition)
{
	if (screen_ && screen_->isConnected() && newPosition != Screen::Invalid) {
		screen_->move(newPosition);
	}
}

void BioXASM2Mirror::setConnected(bool isConnected)
{
	if (isConnected_ != isConnected) {
		isConnected_ = isConnected;
		emit connected(isConnected_);
	}
}

void BioXASM2Mirror::onConnectedChanged()
{
	bool screenOK = (screen_ && screen_->isConnected());

	setConnected(screenOK);
}


