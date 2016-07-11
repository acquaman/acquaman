#include "BioXASM1Mirror.h"
#include "beamline/BioXAS/BioXASM1MirrorMask.h"

BioXASM1Mirror::BioXASM1Mirror(const QString &name, QObject *parent) :
	BioXASMirror(name, parent)
{
	// Initialize member variables.

	mask_ = 0;

	downstreamBladeCurrent_ = 0;
}

BioXASM1Mirror::~BioXASM1Mirror()
{

}

bool BioXASM1Mirror::canStop() const
{
	bool result = false;

	if (isConnected())
		result = ( BioXASMirror::canStop() && mask_->canStop() );

	return result;
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

		removeChildControl(mask_);

		mask_ = newControl;

		addChildControl(mask_);

		emit maskChanged(mask_);
	}
}

void BioXASM1Mirror::setDownstreamBladeCurrent(AMControl *newControl)
{
	if (downstreamBladeCurrent_ != newControl) {

		removeChildControl(downstreamBladeCurrent_);

		downstreamBladeCurrent_ = newControl;

		addChildControl(downstreamBladeCurrent_);

		emit downstreamBladeCurrentChanged(downstreamBladeCurrent_);
	}
}
