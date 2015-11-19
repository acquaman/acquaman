#include "SGMVATValve.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

SGMVATValve::SGMVATValve(const QString &name, QObject *parent) :
	AMSingleEnumeratedControl(name, "", parent)
{
	// Setup value options.

	addValueOption(Closed, "Closed", SGMVATVALVE_CLOSED_SETPOINT);
	addValueOption(Open, "Open", SGMVATVALVE_OPEN_SETPOINT, SGMVATVALVE_OPEN_MIN, SGMVATVALVE_OPEN_MAX);

	// Setup controls.

	setBaseControl(new AMSinglePVControl("vatValvePosition", "VVR1611-4-I10-11:ctrl:posn", this));
	speed_ = new AMSinglePVControl("vatValveSpeed", "VVR1611-4-I10-11:valveSpeed", this);
}

SGMVATValve::~SGMVATValve()
{

}

AMAction3* SGMVATValve::createMoveAtSpeedAction(double setpoint, double speed)
{
	AMListAction3 *moveAction = new AMListAction3(new AMListActionInfo3("Move VAT valve", "Move VAT valve"), AMListAction3::Sequential);
	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(speed_, speed));
	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(this, setpoint));

	return moveAction;
}
