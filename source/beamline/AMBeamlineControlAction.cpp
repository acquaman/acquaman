#include "AMBeamlineControlAction.h"

AMBeamlineControlAction::AMBeamlineControlAction(AMControl *control, QString message, QObject *parent) :
	AMBeamlineActionItem(message, parent)
{
	needsInput_ = true;
	type_ = "controlAction";
	control_ = control;
	connect(control_, SIGNAL(moveStarted()), this, SIGNAL(started()));
	connect(control_, SIGNAL(moveSucceeded()), this, SIGNAL(succeeded()));
	connect(control_, SIGNAL(moveFailed(int)), this, SIGNAL(failed(int)));
}

QString AMBeamlineControlAction::type() const{
	return AMBeamlineActionItem::type()+"."+type_;
}
