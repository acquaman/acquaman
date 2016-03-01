#include "BioXASBeamlineComponent.h"

BioXASBeamlineComponent::BioXASBeamlineComponent(const QString &name, QObject *parent) :
	AMConnectedControl(name, "", parent)
{

}

BioXASBeamlineComponent::~BioXASBeamlineComponent()
{

}

bool BioXASBeamlineComponent::canStop() const
{
	bool result = false;

	// This control can stop if all controls are valid and
	// all children that can move can also be stopped.

	if (isConnected()) {

		QList<AMControl*> children = childControls();

		if (children.count() > 0) {

			bool childrenValid = true;
			bool childrenStoppable = true;

			for (int i = 0, count = children.count(); i < count && childrenValid && childrenStoppable; i++) { // We want to stop if we come across either a null child or a child that can move but can't be stopped.
				bool childValid = false;
				bool childStoppable = false;

				AMControl *child = childControlAt(i);

				if (child) {
					childValid = true;

					if (!child->canMove())
						childStoppable = true;
					else if (child->canMove() && child->canStop())
						childStoppable = true;
					else
						childStoppable = false;
				}

				childrenValid &= childValid;
				childrenStoppable &= childStoppable;
			}

			result = childrenValid && childrenStoppable;
		}
	}

	return result;
}

bool BioXASBeamlineComponent::stop()
{
	bool result = true;

	if (canStop()) {

		bool childrenStopped = true;

		// Iterate through all child controls, attempting
		// to stop them.

		foreach (AMControl *control, children_) {
			if (control)
				childrenStopped &= control->stop();
		}

		result = childrenStopped;
	}

	return result;
}

void BioXASBeamlineComponent::addChildControl(AMControl *control)
{
	if (control && !children_.contains(control)) {
		children_ << control;

		connect( control, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	}
}

void BioXASBeamlineComponent::removeChildControl(AMControl *control)
{
	if (children_.contains(control)) {
		disconnect( control, 0, this, 0 );
		children_.removeOne(control);
	}
}
