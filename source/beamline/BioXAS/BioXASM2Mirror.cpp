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

void BioXASM2Mirror::setScreen(AMControl *newControl)
{
	if (screen_ != newControl) {

		if (screen_)
			removeChildControl(screen_);

		screen_ = newControl;

		if (screen_)
			addChildControl(screen_);

		emit screenChanged(screen_);
	}
}


