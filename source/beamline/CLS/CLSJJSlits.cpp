#include "CLSJJSlits.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "actions3/CLS/CLSJJSlitOptimizationAction.h"
#include "actions3/CLS/CLSJJSlitsOptimizationAction.h"

CLSJJSlits::CLSJJSlits(const QString &name, const QString &upperBladePVName, const QString &lowerBladePVName, const QString &inboardBladePVName, const QString &outboardBladePVName, QObject *parent) :
	QObject(parent)
{
	// Initialize member variables.

	name_ = name;
	connected_ = false;

	upperBlade_ = new AMPVwStatusControl(name_+"UpperBlade", upperBladePVName+":mm:sp", upperBladePVName+":mm", upperBladePVName+":status", upperBladePVName+":stop", this, 0.005, 5.0);
	lowerBlade_ = new AMPVwStatusControl(name_+"LowerBlade", lowerBladePVName+":mm:sp", lowerBladePVName+":mm", lowerBladePVName+":status", lowerBladePVName+":stop", this, 0.005, 5.0);
	inboardBlade_ = new AMPVwStatusControl(name_+"InboardBlade", inboardBladePVName+":mm:sp", inboardBladePVName+":mm", inboardBladePVName+":status", inboardBladePVName+":stop", this, 0.005, 5.0);
	outboardBlade_ = new AMPVwStatusControl(name_+"OutboardBlade", outboardBladePVName+":mm:sp", outboardBladePVName+":mm", outboardBladePVName+":status", outboardBladePVName+":stop", this, 0.005, 5.0);

	verticalGap_ = new CLSJJSlitGapControl(name_+"VerticalGap", upperBlade_, lowerBlade_, this);
	verticalCenter_ = new CLSJJSlitCenterControl(name_+"VerticalCenter", upperBlade_, lowerBlade_, this);
	horizontalGap_ = new CLSJJSlitGapControl(name_+"HorizontalGap", outboardBlade_, inboardBlade_, this);
	horizontalCenter_ = new CLSJJSlitCenterControl(name_+"HorizontalCenter", outboardBlade_, inboardBlade_, this);

	// Listen for connected states.

	connect( verticalGap_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( verticalCenter_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( horizontalGap_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( horizontalCenter_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Current settings.

	updateConnected();
}

CLSJJSlits::~CLSJJSlits()
{

}

AMControl* CLSJJSlits::control(CLSJJSlits::Blades::Direction direction, CLSJJSlits::Blades::Value value)
{
	AMControl *result = 0;

	if (direction == Blades::Vertical && value == Blades::Gap)
		result = verticalGap_;
	else if (direction == Blades::Vertical && value == Blades::Center)
		result = verticalCenter_;
	else if (direction == Blades::Horizontal && value == Blades::Gap)
		result = horizontalGap_;
	else if (direction == Blades::Horizontal && value == Blades::Center)
		result = horizontalCenter_;

	return result;
}

QList<AMControl*> CLSJJSlits::controls(CLSJJSlits::Blades::Direction direction)
{
	QList<AMControl*> result;

	if (direction == Blades::Vertical) {
		result.append(verticalGap_);
		result.append(verticalCenter_);
	} else if (direction == Blades::Horizontal) {
		result.append(horizontalGap_);
		result.append(horizontalCenter_);
	}

	return result;
}

QList<AMControl*> CLSJJSlits::controls(CLSJJSlits::Blades::Value value)
{
	QList<AMControl*> result;

	if (value == Blades::Gap) {
		result.append(verticalGap_);
		result.append(horizontalGap_);
	} else if (value == Blades::Center) {
		result.append(verticalCenter_);
		result.append(horizontalCenter_);
	}

	return result;
}

AMAction3* CLSJJSlits::createMoveAction(CLSJJSlits::Blades::Direction direction, CLSJJSlits::Blades::Value value, double setpoint)
{
	AMControl *toMove = control(direction, value);
	return AMActionSupport::buildControlMoveAction(toMove, setpoint);
}

AMAction3* CLSJJSlits::createMoveAction(double verticalGapSetpoint, double verticalCenterSetpoint, double horizontalGapSetpoint, double horizontalCenterSetpoint)
{
	AMListAction3 *moveGap = new AMListAction3(new AMListActionInfo3("Moving gap", "Move gap"), AMListAction3::Parallel);
	moveGap->addSubAction(createMoveAction(CLSJJSlits::Blades::Vertical, CLSJJSlits::Blades::Gap, verticalGapSetpoint));
	moveGap->addSubAction(createMoveAction(CLSJJSlits::Blades::Horizontal, CLSJJSlits::Blades::Gap, horizontalGapSetpoint));

	AMListAction3 *moveCenter = new AMListAction3(new AMListActionInfo3("Move center", "Move center"), AMListAction3::Parallel);
	moveCenter->addSubAction(createMoveAction(CLSJJSlits::Blades::Vertical, CLSJJSlits::Blades::Center, verticalCenterSetpoint));
	moveCenter->addSubAction(createMoveAction(CLSJJSlits::Blades::Horizontal, CLSJJSlits::Blades::Center, horizontalCenterSetpoint));

	AMListAction3 *result = new AMListAction3(new AMListActionInfo3("Move JJ slits", "Move JJ slits"), AMListAction3::Sequential);
	result->addSubAction(moveGap);
	result->addSubAction(moveCenter);

	return result;
}

AMAction3* CLSJJSlits::createMoveToOriginAction()
{
	return createMoveAction(0, 0, 0, 0);
}

AMAction3* CLSJJSlits::createMoveToOriginAction(CLSJJSlits::Blades::Direction direction)
{
	AMListAction3 *result = new AMListAction3(new AMListActionInfo3(QString("Moving to %1 Origin").arg(directionToString(direction)), QString("Moving to %1 Origin").arg(directionToString(direction))), AMListAction3::Sequential);
	result->addSubAction(createMoveAction(direction, CLSJJSlits::Blades::Center, 0));
	result->addSubAction(createMoveAction(direction, CLSJJSlits::Blades::Gap, 0));

	return result;
}

AMAction3* CLSJJSlits::createMoveToMinGapAction(CLSJJSlits::Blades::Direction direction)
{
	return createMoveToMinPositionAction(direction, CLSJJSlits::Blades::Gap);
}

AMAction3* CLSJJSlits::createMoveToMaxGapAction(CLSJJSlits::Blades::Direction direction)
{
	return createMoveToMaxPosition(direction, CLSJJSlits::Blades::Gap);
}

AMAction3* CLSJJSlits::createCloseGapAction(CLSJJSlits::Blades::Direction direction)
{
	return createMoveToOriginAction(direction);
}

AMAction3* CLSJJSlits::createOpenGapAction(CLSJJSlits::Blades::Direction direction)
{
	AMListAction3 *result = new AMListAction3(new AMListActionInfo3(QString("Opening %1 Gap").arg(directionToString(direction)), QString("Opening %1 Gap").arg(directionToString(direction))), AMListAction3::Sequential);
	result->addSubAction(createMoveToOriginAction(direction));
	result->addSubAction(createMoveToMaxPositionAction(direction, CLSJJSlits::Blades::Gap));

	return result;
}

AMAction3* CLSJJSlits::createMoveToMinPositionAction(CLSJJSlits::Blades::Direction direction, CLSJJSlits::Blades::Value value)
{
	AMControl *toMove = control(direction, value);
	return AMActionSupport::buildControlMoveAction(toMove, toMove->minimumValue());
}

AMAction3* CLSJJSlits::createMoveToMaxPositionAction(CLSJJSlits::Blades::Direction direction, CLSJJSlits::Blades::Value value)
{
	AMControl *toMove = control(direction, value);
	return AMActionSupport::buildControlMoveAction(toMove, toMove->maximumValue());
}

AMAction3* CLSJJSlits::createOptimizationAction(CLSJJSlits::Blades::Direction direction, CLSJJSlits::Blades::Value value)
{
	CLSJJSlitOptimizationActionInfo *actionInfo = new CLSJJSlitOptimizationActionInfo(direction, value);
	CLSJJSlitOptimizationAction *action = new CLSJJSlitOptimizationAction(actionInfo);

	return action;
}

AMAction3* CLSJJSlits::createOptimizationAction(CLSJJSlits::Blades::Value value)
{
	CLSJJSlitsOptimizationActionInfo *actionInfo = new CLSJJSlitsOptimizationActionInfo();
	CLSJJSlitsOptimizationAction *action = new CLSJJSlitsOptimizationAction(actionInfo);

	return action;
}

QString CLSJJSlits::directionToString(CLSJJSlits::Blades::Direction direction) const
{
	QString result;

	switch (direction) {
	case CLSJJSlits::Blades::Vertical:
		result = "Vertical";
		break;
	case CLSJJSlits::Blades::Horizontal:
		result = "Horizontal";
		break;
	default:
		break;
	}

	return result;
}

QString CLSJJSlits::valueToString(CLSJJSlits::Blades::Value value) const
{
	QString result;

	switch (value) {
	case CLSJJSlits::Blades::Gap:
		result = "Gap";
		break;
	case CLSJJSlits::Blades::Center:
		result = "Center";
		break;
	default:
		break;
	}

	return result;
}

void CLSJJSlits::setConnected(bool isConnected)
{
	if (connected_ != isConnected) {
		connected_ = isConnected;
		emit connectedChanged(connected_);
	}
}

void CLSJJSlits::updateConnected()
{
	bool newState = verticalGap_->isConnected() && verticalCenter_->isConnected() && horizontalGap_->isConnected() && horizontalCenter_->isConnected();
	setConnected(newState);
}
