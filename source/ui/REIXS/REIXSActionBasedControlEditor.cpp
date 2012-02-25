#include "REIXSActionBasedControlEditor.h"
#include "actions2/actions/REIXS/REIXSControlMoveAction.h"
#include "actions2/AMActionRunner.h"
#include "util/AMErrorMonitor.h"

#include <QApplication>

REIXSActionBasedControlEditor::REIXSActionBasedControlEditor(AMControl* control, bool okToRunInBackground, QWidget *parent) :
	AMBasicControlEditor(control, parent)
{
	okToRunInBackground_ = okToRunInBackground;
}

void REIXSActionBasedControlEditor::onNewSetpointChosen(double value)
{
	if(!control_)
		return;

	AMControlInfo setpoint = control_->toInfo();
	setpoint.setValue(value);

	if(okToRunInBackground_) {
		AMAction* action = new REIXSControlMoveAction(new REIXSControlMoveActionInfo(setpoint));
		AMActionRunner::s()->runActionImmediately(action);
	}
	else {
		if(AMActionRunner::s()->actionRunning()) {
			QApplication::beep();
			// cannot run, because other actions are running in the queue.
			AMErrorMon::alert(this, -1, QString("Cannot move '%1' right now, because this would interfere with other actions that are currently running in the workflow. You can try again when they are finished, or pause the workflow queue.").arg(control_->description().isEmpty() ? control_->name() : control_->description()));
			return;
		}
		else {
			AMAction* action = new REIXSControlMoveAction(new REIXSControlMoveActionInfo(setpoint));
			AMActionRunner::s()->runActionImmediatelyInQueue(action);
		}
	}
}
