#include "BioXASM1Mirror.h"
#include "beamline/BioXAS/BioXASM1MirrorMask.h"

BioXASM1Mirror::BioXASM1Mirror(const QString &name, QObject *parent) :
	BioXASMirror(name, parent)
{
	// Initialize member variables.

	mask_ = 0;
}

BioXASM1Mirror::~BioXASM1Mirror()
{

}

bool BioXASM1Mirror::isConnected() const
{
	bool isConnected = (
				BioXASMirror::isConnected() &&
				mask_ && mask_->isConnected()
				);

	return isConnected;
}

void BioXASM1Mirror::setMask(BioXASM1MirrorMask *newControl)
{
	if (mask_ != newControl) {

		if (mask_)
			removeChildControl(mask_);

		mask_ = newControl;

		if (mask_)
			addChildControl(mask_);

		emit maskChanged(mask_);
	}
}
