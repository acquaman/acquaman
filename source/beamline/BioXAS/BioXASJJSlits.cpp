#include "BioXASJJSlits.h"

BioXASJJSlits::BioXASJJSlits(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	// Initialize class variables.

	upperBlade_ = 0;
	lowerBlade_ = 0;
	inboardBlade_ = 0;
	outboardBlade_ = 0;
}

BioXASJJSlits::~BioXASJJSlits()
{

}

void BioXASJJSlits::setUpperBlade(AMControl *newControl)
{
	if (upperBlade_ != newControl) {

		if (upperBlade_)
			removeChildControl(upperBlade_);

		upperBlade_ = newControl;

		if (upperBlade_)
			addChildControl(upperBlade_);

		emit upperBladeChanged(upperBlade_);
	}
}

void BioXASJJSlits::setLowerBlade(AMControl *newControl)
{
	if (lowerBlade_ != newControl) {

		if (lowerBlade_)
			removeChildControl(lowerBlade_);

		lowerBlade_ = newControl;

		if (lowerBlade_)
			addChildControl(lowerBlade_);

		emit lowerBladeChanged(lowerBlade_);
	}
}

void BioXASJJSlits::setInboardBlade(AMControl *newControl)
{
	if (inboardBlade_ != newControl) {

		if (inboardBlade_)
			removeChildControl(inboardBlade_);

		inboardBlade_ = newControl;

		if (inboardBlade_)
			addChildControl(inboardBlade_);

		emit inboardBladeChanged(inboardBlade_);
	}
}

void BioXASJJSlits::setOutboardBlade(AMControl *newControl)
{
	if (outboardBlade_ != newControl) {

		if (outboardBlade_)
			removeChildControl(outboardBlade_);

		outboardBlade_ = newControl;

		if (outboardBlade_)
			addChildControl(outboardBlade_);

		emit outboardBladeChanged(outboardBlade_);
	}
}
