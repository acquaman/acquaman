#include "BioXASM2Mirror.h"

BioXASM2Mirror::BioXASM2Mirror(QObject *parent) :
    QObject(parent)
{
	// Initialize member variables.
	// Subclasses should create instances of each control.

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
	if (screen_ && screen_->isConnected()) {
		screen_->move(newPosition);
	}
}


