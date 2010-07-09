#include "AMBeamlineControlAction.h"

AMBeamlineControlAction::AMBeamlineControlAction(AMControl *control, QObject *parent) :
	AMBeamlineActionItem(parent)
{
	control_ = control;
	connect(control_, SIGNAL(moveStarted()), this, SIGNAL(started()));
	connect(control_, SIGNAL(moveSucceeded()), this, SIGNAL(succeeded()));
	connect(control_, SIGNAL(moveFailed(int)), this, SIGNAL(failed(int)));
}
