#include "BioXASBiStateControl.h"

BioXASBiStateControl::BioXASBiStateControl(const QString &name, QObject *parent) :
	AMEnumeratedControl(name, "", parent)
{

}

BioXASBiStateControl::~BioXASBiStateControl()
{

}

bool BioXASBiStateControl::addChildControl(AMControl *newControl)
{
	bool result = false;

	if (newControl && !children_->contains(newControl)) {
		AMEnumeratedControl::addChildControl(newControl);

		result = true;
	}

	return result;
}

bool BioXASBiStateControl::removeChildControl(AMControl *control)
{
	bool result = false;

	if (control && children_->contains(control)) {
		AMEnumeratedControl::removeChildControl(control);

		result = true;
	}

	return result;
}
