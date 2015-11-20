#include "AMConnectedControl.h"

AMConnectedControl::AMConnectedControl(const QString &name, const QString &units, QObject *parent) :
	AMControl(name, units, parent)
{
	connected_ = false;
}

AMConnectedControl::~AMConnectedControl()
{

}

bool AMConnectedControl::isConnected() const
{
	bool result = false;

	if (!children_.isEmpty()) {
		bool connected = true;

		for (int childIndex = 0, childCount = children_.count(); childIndex < childCount && connected; childIndex++) {
			AMControl *child = children_.at(childIndex);

			if (child && child->isConnected())
				connected = true;
			else
				connected = false;
		}

		result = connected;
	}

	return result;
}

void AMConnectedControl::setConnected(bool isConnected)
{
	if (connected_ != isConnected) {
		connected_ = isConnected;
		emit connected(connected_);
	}
}

void AMConnectedControl::updateConnected()
{
	setConnected( isConnected() );
}

void AMConnectedControl::addChildControl(AMControl *control)
{
	if (control) {
		children_ << control;

		connect( control, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

		updateConnected();
	}
}

void AMConnectedControl::removeChildControl(AMControl *control)
{
	if (control) {
		disconnect( control, 0, this, 0 );
		children_.removeOne(control);

		updateConnected();
	}
}
