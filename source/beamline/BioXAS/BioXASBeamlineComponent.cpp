#include "BioXASBeamlineComponent.h"
#include <QDebug>

BioXASBeamlineComponent::BioXASBeamlineComponent(const QString &name, QObject *parent) :
	AMControl(name, "", parent)
{
	connected_ = false;
}

BioXASBeamlineComponent::~BioXASBeamlineComponent()
{

}

bool BioXASBeamlineComponent::stop()
{
	// Want to return true if all the controls are valid and those
	// that can be stopped are stopped, false otherwise. Returns
	// true if there are no child controls, the expectation is that
	// this control can't be moving otherwise.

	bool childrenValid = true;
	bool childrenStopped = true;

	foreach (AMControl *control, children_) {
		if (control) {
			childrenValid &= true;

			if (control->canStop()) {
				if (control->stop())
					childrenStopped &= true;
				else
					childrenStopped &= false;
			}

		} else {
			childrenValid &= false;
		}
	}

	bool result = childrenValid && childrenStopped;

	qDebug() << "\n\nChildren stopped:" << result;

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
