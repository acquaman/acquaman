#include "BioXASBeamlineComponent.h"

BioXASBeamlineComponent::BioXASBeamlineComponent(const QString &name, QObject *parent) :
	AMControl(name, "", parent)
{
	connected_ = false;
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
				AMControl *child = childControlAt(i);

				if (child) {

					if (child->canMove() && !child->canStop())
						childrenStoppable = false;

				} else {
					childrenValid = false;
				}
			}

			result = childrenValid && childrenStoppable;
		}
	}

	return result;
}

bool BioXASBeamlineComponent::stop()
{
	// Want to return true if all the controls are valid and those
	// that can be stopped are stopped, false otherwise. Returns
	// true if there are no child controls, the expectation is that
	// this control can't be moving otherwise.

	bool childrenValid = true;
	bool childrenStopped = true;

	foreach (AMControl *control, children_) { // We want to iterate through all children and try to stop them.

		if (control) {
			if (control->canStop())
				childrenStopped &= control->stop();

		} else {
			childrenValid &= false;
		}
	}

	bool result = childrenValid && childrenStopped;

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

void BioXASBeamlineComponent::setConnected(bool isConnected)
{
	if (connected_ != isConnected) {
		connected_ = isConnected;
		emit connected(connected_);
	}
}

void BioXASBeamlineComponent::updateConnected()
{
	setConnected( isConnected() );
}
