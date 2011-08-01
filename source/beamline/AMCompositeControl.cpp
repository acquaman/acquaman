#include "AMCompositeControl.h"

AMCompositeControl::AMCompositeControl(const QString& name, const QString& units, QObject* parent, const QString description) :
	AMControl(name, units, parent, description)
{
}

void AMCompositeControl::addChildControl(AMControl *control)
{
	children_ << control;
	connect(control, SIGNAL(connected(bool)), this, SLOT(onChildControlConnected()));
	connect(control, SIGNAL(movingChanged(bool)), this, SLOT(onChildControlMovingChanged()));
}

void AMCompositeControl::onChildControlConnected()
{
	bool nowConnected = isConnected();

	// Losing the connection:
	if(wasConnected_ && !nowConnected ) {
		emit connected(wasConnected_ = nowConnected);
	}

	// gaining the connection:
	if(!wasConnected_ && nowConnected ) {
		emit connected(wasConnected_ = nowConnected);
	}
}

void AMCompositeControl::onChildControlMovingChanged()
{
	bool nowMoving = isMoving();

	if(wasMoving_ && !nowMoving)
		emit movingChanged(wasMoving_ = nowMoving);

	if(!wasMoving_ && nowMoving)
		emit movingChanged(wasMoving_ = nowMoving);
}
