#include "SGMVATValve.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

SGMVATValve::SGMVATValve(const QString &name, QObject *parent) :
	SGMControl(name, "", parent)
{
	position_ = new AMSinglePVControl("vatValvePosition", "VVR1611-4-I10-11:ctrl:posn", this);
	addChildControl(position_);

	speed_ = new AMSinglePVControl("vatValveSpeed", "VVR1611-4-I10-11:valveSpeed", this);
	addChildControl(speed_);
}

SGMVATValve::~SGMVATValve()
{

}

bool SGMVATValve::isOpen() const
{
	double currentPosition = position_->value();
	bool result = (currentPosition == SGMVATVALVE_OPEN_POSITION);

	return result;
}

bool SGMVATValve::isClosed() const
{
	double currentPosition = position_->value();
	bool result = (currentPosition == SGMVATVALVE_CLOSED_POSITION);

	return result;
}

AMAction3* SGMVATValve::createMoveAtSpeedAction(double speedSetpoint, double positionSetpoint)
{
	AMListAction3 *moveAction = new AMListAction3(new AMListActionInfo3("Move VAT valve", "Move VAT valve"), AMListAction3::Sequential);
	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(speed_, speedSetpoint));
	moveAction->addSubAction(AMActionSupport::buildControlMoveAction(position_, positionSetpoint));

	return moveAction;
}


