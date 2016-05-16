#include "AMConnectedControl.h"

AMConnectedControl::AMConnectedControl(const QString &name, const QString &units, QObject *parent, const QString &description) :
	AMControl(name, units, parent, description)
{
	connected_ = false;
}

AMConnectedControl::~AMConnectedControl()
{

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
	setConnected( childrenConnected() );
}

bool AMConnectedControl::addChildControl(AMControl *control)
{
        if (AMControl::addChildControl(control)){
		connect( control, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
		updateConnected();
                return true;
	}
        return false;
}

bool AMConnectedControl::removeChildControl(AMControl *control)
{
        if (AMControl::removeChildControl(control)){
		updateConnected();
                return true;
	}
        return false;
}

bool AMConnectedControl::childrenConnected() const
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
