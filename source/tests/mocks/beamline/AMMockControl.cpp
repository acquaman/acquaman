#include "AMMockControl.h"

#include "tests/mocks/actions/AMMockMoveAction.h"

AMMockControl::AMMockControl(const QString& name,
                             const QString& units,
	                         QObject *parent)
    : AMPseudoMotorControl(name, units, parent, name)
{
	value_ = 0;
	setpoint_ = 0;
	minimumValue_ = -1000000;
	maximumValue_ = 1000000;
	currentMoveAction_ = 0;

	setAllowsMovesWhileMoving(false);
	setTolerance(0.01);
	setDisplayPrecision(2);
	setConnected(true);
}

bool AMMockControl::stop()
{
	if(currentMoveAction_) {

		currentMoveAction_->cancel();
		return true;
	}

	return false;
}

void AMMockControl::onMoveActionStateChanged(int newActionState, int /*oldActionState*/)
{

	if(newActionState == AMAction3::Running) {

		setIsMoving(true);

	} else if(newActionState == AMAction3::Cancelled ||
	          newActionState == AMAction3::Paused ||
	          newActionState == AMAction3::Succeeded ||
	          newActionState == AMAction3::Failed) {

		currentMoveAction_ = 0;
		setIsMoving(false);
	}
}

AMAction3 * AMMockControl::createMoveAction(double setpoint)
{
	AMActionInfo3* moveActionInfo = new AMActionInfo3(QString("Moving %1").arg(name()),
	                                                  QString("Moving %1").arg(description()));

	AMMockMoveAction* returnMoveAction = new AMMockMoveAction(moveActionInfo, this, setpoint);

	connect(returnMoveAction, SIGNAL(moveValueChanged(double)), this, SLOT(setValue(double)));
	connect(returnMoveAction, SIGNAL(stateChanged(int,int)), this, SLOT(onMoveActionStateChanged(int,int)));

	currentMoveAction_ = returnMoveAction;
	return returnMoveAction;
}





