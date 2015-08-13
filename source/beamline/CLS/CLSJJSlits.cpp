#include "CLSJJSlits.h"
#include "acquaman/CLS/CLSJJSlitScanConfiguration.h"
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

	slitScanConfiguration_ = new CLSJJSlitScanConfiguration(this);

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

AMControl* CLSJJSlits::control(CLSJJSlits::Direction::Option direction, CLSJJSlits::Property::Option property)
{
	AMControl *result = 0;

	if (direction == Direction::Vertical && property == Property::Gap)
		result = verticalGap_;
	else if (direction == Direction::Vertical && property == Property::Center)
		result = verticalCenter_;
	else if (direction == Direction::Horizontal && property == Property::Gap)
		result = horizontalGap_;
	else if (direction == Direction::Horizontal && property == Property::Center)
		result = horizontalCenter_;

	return result;
}

QList<AMControl*> CLSJJSlits::controls(CLSJJSlits::Direction::Option direction)
{
	QList<AMControl*> result;

	if (direction == Direction::Vertical) {
		result.append(verticalGap_);
		result.append(verticalCenter_);
	} else if (direction == Direction::Horizontal) {
		result.append(horizontalGap_);
		result.append(horizontalCenter_);
	}

	return result;
}

QList<AMControl*> CLSJJSlits::controls(CLSJJSlits::Property::Option property)
{
	QList<AMControl*> result;

	if (property == Property::Gap) {
		result.append(verticalGap_);
		result.append(horizontalGap_);
	} else if (property == Property::Center) {
		result.append(verticalCenter_);
		result.append(horizontalCenter_);
	}

	return result;
}

AMAction3* CLSJJSlits::createMoveAction(CLSJJSlits::Direction::Option direction, CLSJJSlits::Property::Option property, double setpoint)
{
	AMControl *toMove = control(direction, property);
	return AMActionSupport::buildControlMoveAction(toMove, setpoint);
}

AMAction3* CLSJJSlits::createMoveAction(double verticalGapSetpoint, double verticalCenterSetpoint, double horizontalGapSetpoint, double horizontalCenterSetpoint)
{
	AMListAction3 *moveGap = new AMListAction3(new AMListActionInfo3("Moving gap", "Move gap"), AMListAction3::Parallel);
	moveGap->addSubAction(createMoveAction(CLSJJSlits::Direction::Vertical, CLSJJSlits::Property::Gap, verticalGapSetpoint));
	moveGap->addSubAction(createMoveAction(CLSJJSlits::Direction::Horizontal, CLSJJSlits::Property::Gap, horizontalGapSetpoint));

	AMListAction3 *moveCenter = new AMListAction3(new AMListActionInfo3("Move center", "Move center"), AMListAction3::Parallel);
	moveCenter->addSubAction(createMoveAction(CLSJJSlits::Direction::Vertical, CLSJJSlits::Property::Center, verticalCenterSetpoint));
	moveCenter->addSubAction(createMoveAction(CLSJJSlits::Direction::Horizontal, CLSJJSlits::Property::Center, horizontalCenterSetpoint));

	AMListAction3 *result = new AMListAction3(new AMListActionInfo3("Move JJ slits", "Move JJ slits"), AMListAction3::Sequential);
	result->addSubAction(moveGap);
	result->addSubAction(moveCenter);

	return result;
}

AMAction3* CLSJJSlits::createMoveToOriginAction()
{
	return createMoveAction(0, 0, 0, 0);
}

AMAction3* CLSJJSlits::createMoveToOriginAction(CLSJJSlits::Direction::Option direction)
{
	AMListAction3 *result = new AMListAction3(new AMListActionInfo3(QString("Moving to %1 Origin").arg(directionToString(direction)), QString("Moving to %1 Origin").arg(directionToString(direction))), AMListAction3::Sequential);
	result->addSubAction(createMoveAction(direction, CLSJJSlits::Property::Center, 0));
	result->addSubAction(createMoveAction(direction, CLSJJSlits::Property::Gap, 0));

	return result;
}

AMAction3* CLSJJSlits::createMoveToMinGapAction(CLSJJSlits::Direction::Option direction)
{
	return createMoveToMinPositionAction(direction, CLSJJSlits::Property::Gap);
}

AMAction3* CLSJJSlits::createMoveToMaxGapAction(CLSJJSlits::Direction::Option direction)
{
	return createMoveToMaxPositionAction(direction, CLSJJSlits::Property::Gap);
}

AMAction3* CLSJJSlits::createCloseGapAction(CLSJJSlits::Direction::Option direction)
{
	return createMoveToOriginAction(direction);
}

AMAction3* CLSJJSlits::createOpenGapAction(CLSJJSlits::Direction::Option direction)
{
	AMListAction3 *result = new AMListAction3(new AMListActionInfo3(QString("Opening %1 Gap").arg(directionToString(direction)), QString("Opening %1 Gap").arg(directionToString(direction))), AMListAction3::Sequential);
	result->addSubAction(createMoveToOriginAction(direction));
	result->addSubAction(createMoveToMaxPositionAction(direction, CLSJJSlits::Property::Gap));

	return result;
}

AMAction3* CLSJJSlits::createMoveToMinPositionAction(CLSJJSlits::Direction::Option direction, CLSJJSlits::Property::Option property)
{
	AMControl *toMove = control(direction, property);
	return AMActionSupport::buildControlMoveAction(toMove, toMove->minimumValue());
}

AMAction3* CLSJJSlits::createMoveToMaxPositionAction(CLSJJSlits::Direction::Option direction, CLSJJSlits::Property::Option property)
{
	AMControl *toMove = control(direction, property);
	return AMActionSupport::buildControlMoveAction(toMove, toMove->maximumValue());
}

AMAction3* CLSJJSlits::createOptimizationAction(CLSJJSlits::Direction::Option direction, CLSJJSlits::Property::Option property)
{
	CLSJJSlitOptimizationActionInfo *actionInfo = new CLSJJSlitOptimizationActionInfo(direction, property, verticalGap_->value(), verticalCenter_->value(), horizontalGap_->value(), horizontalCenter_->value(), slitScanConfiguration_);
	CLSJJSlitOptimizationAction *action = new CLSJJSlitOptimizationAction(actionInfo);

	return action;
}

AMAction3* CLSJJSlits::createOptimizationAction(CLSJJSlits::Property::Option property)
{
	Q_UNUSED(property)
//	CLSJJSlitsOptimizationActionInfo *actionInfo = new CLSJJSlitsOptimizationActionInfo();
//	CLSJJSlitsOptimizationAction *action = new CLSJJSlitsOptimizationAction(actionInfo);

//	return action;

	return 0;
}

QString CLSJJSlits::directionToString(CLSJJSlits::Direction::Option direction)
{
	QString result;

	switch (direction) {
	case CLSJJSlits::Direction::Vertical:
		result = "Vertical";
		break;
	case CLSJJSlits::Direction::Horizontal:
		result = "Horizontal";
		break;
	default:
		break;
	}

	return result;
}

QString CLSJJSlits::propertyToString(CLSJJSlits::Property::Option property)
{
	QString result;

	switch (property) {
	case CLSJJSlits::Property::Gap:
		result = "Gap";
		break;
	case CLSJJSlits::Property::Center:
		result = "Center";
		break;
	default:
		break;
	}

	return result;
}

CLSJJSlits::Direction::Option CLSJJSlits::stringToDirection(const QString &string)
{
	CLSJJSlits::Direction::Option result = CLSJJSlits::Direction::Invalid;

	if (string == "Vertical")
		result = CLSJJSlits::Direction::Vertical;
	else if (string == "Horizontal")
		result = CLSJJSlits::Direction::Horizontal;

	return result;
}

CLSJJSlits::Property::Option CLSJJSlits::stringToProperty(const QString &string)
{
	CLSJJSlits::Property::Option result = CLSJJSlits::Property::Invalid;

	if (string == "Gap")
		result = CLSJJSlits::Property::Gap;
	else if (string == "Center")
		result = CLSJJSlits::Property::Center;

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
