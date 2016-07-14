#include "BioXASM1Mirror.h"

BioXASM1Mirror::BioXASM1Mirror(const QString &name, QObject *parent) :
	BioXASMirror(name, parent)
{
	// Initialize member variables.

	maskUpperBlade_ = 0;
	maskState_ = 0;
	downstreamBladeCurrent_ = 0;

	// Current settings.

	setMaskState(new BioXASMirrorMaskState(QString("%1MaskState").arg(name), this));
}

BioXASM1Mirror::~BioXASM1Mirror()
{

}

bool BioXASM1Mirror::canStop() const
{
	bool stoppable = (
				BioXASMirror::canStop() &&
				maskUpperBlade_ && maskUpperBlade_->canStop()
				);

	return stoppable;
}

bool BioXASM1Mirror::isConnected() const
{
	bool isConnected = (
				BioXASMirror::isConnected() &&
				maskUpperBlade_ && maskUpperBlade_->isConnected() &&
				maskState_ && maskState_->isConnected() &&
				downstreamBladeCurrent_ && downstreamBladeCurrent_->isConnected()
				);

	return isConnected;
}

void BioXASM1Mirror::setMaskUpperBlade(AMControl *newControl)
{
	if (maskUpperBlade_ != newControl) {

		removeChildControl(maskUpperBlade_);

		maskUpperBlade_ = newControl;

		addChildControl(maskUpperBlade_);

		updateMaskState();

		emit maskUpperBladeChanged(maskUpperBlade_);
	}
}

void BioXASM1Mirror::setMaskState(BioXASMirrorMaskState *newControl)
{
	if (maskState_ != newControl) {

		removeChildControl(maskState_);

		maskState_ = newControl;

		addChildControl(maskState_);

		updateMaskState();

		emit maskStateChanged(maskState_);
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

void BioXASM1Mirror::updateMaskState()
{
	if (maskState_)
		maskState_->setUpperSlitBlade(maskUpperBlade_);
}
