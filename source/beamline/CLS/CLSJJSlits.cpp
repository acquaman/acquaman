#include "CLSJJSlits.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

CLSJJSlits::CLSJJSlits(const QString &name, const QString &upperBladePVName, const QString &lowerBladePVName, const QString &inboardBladePVName, const QString &outboardBladePVName, QObject *parent, AMDetectorSet *optimizationDetectors) :
	AMPseudoMotorControl(name, QString(), parent, name, optimizationDetectors)
{
	// Initialize member variables.

	upperBlade_ = 0;
	lowerBlade_ = 0;
	inboardBlade_ = 0;
	outboardBlade_ = 0;

	verticalGap_ = new CLSJJSlitGapControl(name+"VerticalGap", 0, 0, this);
	addChildControl(verticalGap_);

	verticalCenter_ = new CLSJJSlitCenterControl(name+"VerticalCenter", 0, 0, this);
	addChildControl(verticalCenter_);

	horizontalGap_ = new CLSJJSlitGapControl(name+"HorizontalGap", 0, 0, this);
	addChildControl(horizontalGap_);

	horizontalCenter_ = new CLSJJSlitCenterControl(name+"HorizontalCenter", 0, 0, this);
	addChildControl(horizontalCenter_);

	// Current settings.

	setupBladeControls(upperBladePVName, lowerBladePVName, inboardBladePVName, outboardBladePVName);
	updateStates();
}

CLSJJSlits::~CLSJJSlits()
{

}

double CLSJJSlits::value(Direction::Option directionOption, Control::Option controlOption) const
{
	double result = 0;
	AMControl *desiredControl = control(directionOption, controlOption);

	if (desiredControl)
		result = desiredControl->value();

	return result;
}

double CLSJJSlits::setpoint(Direction::Option directionOption, Control::Option controlOption) const
{
	double result = 0;
	AMControl *desiredControl = control(directionOption, controlOption);

	if (desiredControl)
		result = desiredControl->setpoint();

	return result;
}

double CLSJJSlits::minimumValue(Direction::Option directionOption, Control::Option controlOption) const
{
	double result = 0;
	AMControl *desiredControl = control(directionOption, controlOption);

	if (desiredControl)
		result = desiredControl->minimumValue();

	return result;
}

double CLSJJSlits::maximumValue(Direction::Option directionOption, Control::Option controlOption) const
{
	double result = 0;
	AMControl *desiredControl = control(directionOption, controlOption);

	if (desiredControl)
		result = desiredControl->maximumValue();

	return result;
}

bool CLSJJSlits::shouldMeasure(Direction::Option directionOption, Control::Option controlOption) const
{
	bool result = false;
	AMControl *desiredControl = control(directionOption, controlOption);

	if (desiredControl)
		result = desiredControl->shouldMeasure();

	return result;
}

bool CLSJJSlits::shouldMove(Direction::Option directionOption, Control::Option controlOption) const
{
	bool result = false;
	AMControl *desiredControl = control(directionOption, controlOption);

	if (desiredControl)
		result = desiredControl->shouldMove();

	return result;
}

bool CLSJJSlits::shouldStop(Direction::Option directionOption, Control::Option controlOption) const
{
	bool result = false;
	AMControl *desiredControl = control(directionOption, controlOption);

	if (desiredControl)
		result = desiredControl->shouldStop();

	return result;
}

bool CLSJJSlits::shouldMeasure(Direction::Option directionOption) const
{
	bool result = true;

	foreach (AMControl *control, controls(directionOption))
		result && control->shouldMeasure();

	return result;
}

bool CLSJJSlits::shouldMove(Direction::Option directionOption) const
{
	bool result = true;

	foreach (AMControl *control, controls(directionOption))
		result && control->shouldMove();

	return result;
}

bool CLSJJSlits::shouldStop(Direction::Option directionOption) const
{
	bool result = true;

	foreach (AMControl *control, controls(directionOption))
		result && control->shouldStop();

	return result;
}

bool CLSJJSlits::shouldMeasure(Control::Option controlOption) const
{
	bool result = true;

	foreach (AMControl *control, controls(controlOption))
		result && control->shouldMeasure();

	return result;
}

bool CLSJJSlits::shouldMove(Control::Option controlOption) const
{
	bool result = true;

	foreach (AMControl *control, controls(controlOption))
		result && control->shouldMove();

	return result;
}

bool CLSJJSlits::shouldStop(Control::Option controlOption) const
{
	bool result = true;

	foreach (AMControl *control, controls(controlOption))
		result && control->shouldStop();

	return result;
}

bool CLSJJSlits::shouldMeasure() const
{
	bool result = (
				upperBlade_ && upperBlade_->shouldMeasure() &&
				lowerBlade_ && lowerBlade_->shouldMeasure() &&
				inboardBlade_ && inboardBlade_->shouldMeasure() &&
				outboardBlade_ && outboardBlade_->shouldMeasure() &&
				verticalGap_ && verticalGap_->shouldMeasure() &&
				verticalCenter_ && verticalCenter_->shouldMeasure() &&
				horizontalGap_ && horizontalGap_->shouldMeasure() &&
				horizontalCenter_ && horizontalCenter_->shouldMeasure()
				);

	return result;
}

bool CLSJJSlits::shouldMove() const
{
	bool result = (
				upperBlade_ && upperBlade_->shouldMove() &&
				lowerBlade_ && lowerBlade_->shouldMove() &&
				inboardBlade_ && inboardBlade_->shouldMove() &&
				outboardBlade_ && outboardBlade_->shouldMove() &&
				verticalGap_ && verticalGap_->shouldMove() &&
				verticalCenter_ && verticalCenter_->shouldMove() &&
				horizontalGap_ && horizontalGap_->shouldMove() &&
				horizontalCenter_ && horizontalCenter_->shouldMove()
				);

	return result;
}

bool CLSJJSlits::shouldStop() const
{
	bool result = (
				upperBlade_ && upperBlade_->shouldStop() &&
				lowerBlade_ && lowerBlade_->shouldStop() &&
				inboardBlade_ && inboardBlade_->shouldStop() &&
				outboardBlade_ && outboardBlade_->shouldStop() &&
				verticalGap_ && verticalGap_->shouldStop() &&
				verticalCenter_ && verticalCenter_->shouldStop() &&
				horizontalGap_ && horizontalGap_->shouldStop() &&
				horizontalCenter_ && horizontalCenter_->shouldStop()
				);

	return result;
}

bool CLSJJSlits::canMeasure(Direction::Option directionOption, Control::Option controlOption) const
{
	bool result = false;
	AMControl *desiredControl = control(directionOption, controlOption);

	if (desiredControl)
		result = desiredControl->canMeasure();

	return result;
}

bool CLSJJSlits::canMove(Direction::Option directionOption, Control::Option controlOption) const
{
	bool result = false;
	AMControl *desiredControl = control(directionOption, controlOption);

	if (desiredControl)
		result = desiredControl->canMove();

	return result;
}

bool CLSJJSlits::canStop(Direction::Option directionOption, Control::Option controlOption) const
{
	bool result = false;
	AMControl *desiredControl = control(directionOption, controlOption);

	if (desiredControl)
		result = desiredControl->canStop();

	return result;
}

bool CLSJJSlits::canMeasure(Direction::Option directionOption) const
{
	bool result = true;

	foreach (AMControl *control, controls(directionOption))
		result && control->canMeasure();

	return result;
}

bool CLSJJSlits::canMove(Direction::Option directionOption) const
{
	bool result = true;

	foreach (AMControl *control, controls(directionOption))
		result && control->canMove();

	return result;
}

bool CLSJJSlits::canStop(Direction::Option directionOption) const
{
	bool result = true;

	foreach (AMControl *control, controls(directionOption))
		result && control->canStop();

	return result;
}

bool CLSJJSlits::canMeasure(Control::Option controlOption) const
{
	bool result = true;

	foreach (AMControl *control, controls(controlOption))
		result && control->canMeasure();

	return result;
}

bool CLSJJSlits::canMove(Control::Option controlOption) const
{
	bool result = true;

	foreach (AMControl *control, controls(controlOption))
		result && control->canMove();

	return result;
}

bool CLSJJSlits::canStop(Control::Option controlOption) const
{
	bool result = true;

	foreach (AMControl *control, controls(controlOption))
		result && control->canStop();

	return result;
}

bool CLSJJSlits::canMeasure() const
{
	bool result = (
				upperBlade_ && upperBlade_->canMeasure() &&
				lowerBlade_ && lowerBlade_->canMeasure() &&
				inboardBlade_ && inboardBlade_->canMeasure() &&
				outboardBlade_ && outboardBlade_->canMeasure() &&
				verticalGap_ && verticalGap_->canMeasure() &&
				verticalCenter_ && verticalCenter_->canMeasure() &&
				horizontalGap_ && horizontalGap_->canMeasure() &&
				horizontalCenter_ && horizontalCenter_->canMeasure()
				);

	return result;
}

bool CLSJJSlits::canMove() const
{
	bool result = (
				upperBlade_ && upperBlade_->canMove() &&
				lowerBlade_ && lowerBlade_->canMove() &&
				inboardBlade_ && inboardBlade_->canMove() &&
				outboardBlade_ && outboardBlade_->canMove() &&
				verticalGap_ && verticalGap_->canMove() &&
				verticalCenter_ && verticalCenter_->canMove() &&
				horizontalGap_ && horizontalGap_->canMove() &&
				horizontalCenter_ && horizontalCenter_->canMove()
				);

	return result;
}

bool CLSJJSlits::canStop() const
{
	bool result = (
				upperBlade_ && upperBlade_->canStop() &&
				lowerBlade_ && lowerBlade_->canStop() &&
				inboardBlade_ && inboardBlade_->canStop() &&
				outboardBlade_ && outboardBlade_->canStop() &&
				verticalGap_ && verticalGap_->canStop() &&
				verticalCenter_ && verticalCenter_->canStop() &&
				horizontalGap_ && horizontalGap_->canStop() &&
				horizontalCenter_ && horizontalCenter_->canStop()
				);

	return result;
}

bool CLSJJSlits::validValue(Direction::Option directionOption, Control::Option controlOption, double value) const
{
	bool result = false;
	AMPseudoMotorControl *desiredControl = qobject_cast<AMPseudoMotorControl*>(control(directionOption, controlOption));


	if (desiredControl)
		result = desiredControl->validValue(value);

	return result;
}

bool CLSJJSlits::validSetpoint(Direction::Option directionOption, Control::Option controlOption, double setpoint) const
{
	bool result = false;
	AMPseudoMotorControl *desiredControl = qobject_cast<AMPseudoMotorControl*>(control(directionOption, controlOption));


	if (desiredControl)
		result = desiredControl->validSetpoint(setpoint);

	return result;
}

AMControl* CLSJJSlits::control(CLSJJSlits::Direction::Option directionOption, CLSJJSlits::Control::Option controlOption) const
{
	AMControl *result = 0;

	if (directionOption == Direction::Vertical && controlOption == Control::Gap)
		result = verticalGap_;
	else if (directionOption == Direction::Vertical && controlOption == Control::Center)
		result = verticalCenter_;
	else if (directionOption == Direction::Horizontal && controlOption == Control::Gap)
		result = horizontalGap_;
	else if (directionOption == Direction::Horizontal && controlOption == Control::Center)
		result = horizontalCenter_;
	else if (directionOption == Direction::Upper && controlOption == Control::Blade)
		result = upperBlade_;
	else if (directionOption == Direction::Lower && controlOption == Control::Blade)
		result = lowerBlade_;
	else if (directionOption == Direction::Inboard && controlOption == Control::Blade)
		result = inboardBlade_;
	else if (directionOption == Direction::Outboard && controlOption == Control::Blade)
		result = outboardBlade_;

	return result;
}

QList<AMControl*> CLSJJSlits::controls(CLSJJSlits::Direction::Option directionOption) const
{
	QList<AMControl*> result;

	switch (directionOption) {
	case Direction::Vertical:
		result << verticalGap_;
		result << verticalCenter_;
		result << upperBlade_;
		result << lowerBlade_;
		break;
	case Direction::Horizontal:
		result << horizontalGap_;
		result << horizontalCenter_;
		result << inboardBlade_;
		result << outboardBlade_;
		break;
	case Direction::Upper:
		result << upperBlade_;
		break;
	case Direction::Lower:
		result << lowerBlade_;
		break;
	case Direction::Inboard:
		result << inboardBlade_;
		break;
	case Direction::Outboard:
		result << outboardBlade_;
		break;
	default:
		break;
	}

	return result;
}

QList<AMControl*> CLSJJSlits::controls(CLSJJSlits::Control::Option control) const
{
	QList<AMControl*> result;

	switch (control) {
	case Control::Gap:
		result << verticalGap_;
		result << horizontalGap_;
		break;
	case Control::Center:
		result << verticalCenter_;
		result << horizontalCenter_;
		break;
	case Control::Blade:
		result << upperBlade_;
		result << lowerBlade_;
		result << inboardBlade_;
		result << outboardBlade_;
		break;
	default:
		break;
	}

	return result;
}

QList<AMControl*> CLSJJSlits::controls() const
{
	QList<AMControl*> controlsList;

	controlsList
		<< upperBlade_
		<< lowerBlade_
		<< inboardBlade_
		<< outboardBlade_
		<< verticalGap_
		<< verticalCenter_
		<< horizontalGap_
		<< horizontalCenter_;

	return controlsList;
}

AMControl::FailureExplanation CLSJJSlits::move(Direction::Option directionOption, Control::Option controlOption, double setpoint)
{
	FailureExplanation result = AMControl::OtherFailure;
	AMControl *desiredControl = control(directionOption, controlOption);

	if (desiredControl)
		result = desiredControl->move(setpoint);

	return result;
}

void CLSJJSlits::setUpperBladeControl(AMControl *upperBlade)
{
	if (upperBlade_ != upperBlade) {

		if (upperBlade_)
			removeChildControl(upperBlade_);

		upperBlade_ = upperBlade;

		if (upperBlade_)
			addChildControl(upperBlade_);

		updateVerticalGapControl();
		updateVerticalCenterControl();

		emit upperBladeControlChanged(upperBlade_);
	}
}

void CLSJJSlits::setLowerBladeControl(AMControl *lowerBlade)
{
	if (lowerBlade_ != lowerBlade) {

		if (lowerBlade_)
			removeChildControl(lowerBlade_);

		lowerBlade_ = lowerBlade;

		if (lowerBlade_)
			addChildControl(lowerBlade_);

		updateVerticalGapControl();
		updateVerticalCenterControl();

		emit lowerBladeControlChanged(lowerBlade_);
	}
}

void CLSJJSlits::setInboardBladeControl(AMControl *inboardBlade)
{
	if (inboardBlade_ != inboardBlade) {

		if (inboardBlade_)
			removeChildControl(inboardBlade_);

		inboardBlade_ = inboardBlade;

		if (inboardBlade_)
			addChildControl(inboardBlade_);

		updateHorizontalGapControl();
		updateHorizontalCenterControl();

		emit inboardBladeControlChanged(inboardBlade_);
	}
}

void CLSJJSlits::setOutboardBladeControl(AMControl *outboardBlade)
{
	if (outboardBlade_ != outboardBlade) {

		if (outboardBlade_)
			removeChildControl(outboardBlade_);

		outboardBlade_ = outboardBlade;

		if (outboardBlade_)
			addChildControl(outboardBlade_);

		updateHorizontalGapControl();
		updateHorizontalCenterControl();

		emit outboardBladeControlChanged(outboardBlade_);
	}
}

void CLSJJSlits::updateConnected()
{
	bool isConnected = false;

	if (!controls().isEmpty()) {
		isConnected = true;

		foreach (AMControl *control, controls())
			isConnected && control && control->isConnected();
	}

	setConnected(isConnected);
}

void CLSJJSlits::updateMoving()
{
	bool isMoving = false;

	if (!controls().isEmpty()) {
		isMoving = true;

		foreach (AMControl *control, controls())
			isMoving && control && control->isMoving();
	}

	setIsMoving(isMoving);
}

void CLSJJSlits::updateVerticalGapControl()
{
	if (verticalGap_) {
		verticalGap_->setUpperBladeControl(upperBlade_);
		verticalGap_->setLowerBladeControl(lowerBlade_);
	}
}

void CLSJJSlits::updateVerticalCenterControl()
{
	if (verticalCenter_) {
		verticalCenter_->setUpperBladeControl(upperBlade_);
		verticalCenter_->setLowerBladeControl(lowerBlade_);
	}
}

void CLSJJSlits::updateHorizontalGapControl()
{
	if (horizontalGap_) {
		horizontalGap_->setUpperBladeControl(inboardBlade_);
		horizontalCenter_->setLowerBladeControl(outboardBlade_);
	}
}

void CLSJJSlits::updateHorizontalCenterControl()
{
	if (horizontalCenter_) {
		horizontalCenter_->setUpperBladeControl(inboardBlade_);
		horizontalCenter_->setLowerBladeControl(outboardBlade_);
	}
}

void CLSJJSlits::updatePseudoMotorControls()
{
	updateVerticalGapControl();
	updateVerticalCenterControl();
	updateHorizontalGapControl();
	updateHorizontalCenterControl();
}

AMPVwStatusControl* CLSJJSlits::createBladeControl(const QString &name, const QString &baseName)
{
	AMPVwStatusControl *result = 0;

	if (!name.isEmpty() && !baseName.isEmpty())
		result = new AMPVwStatusControl(name, baseName+":mm:sp", baseName+":mm", baseName+":status", baseName+":stop", this, 0.005, 5.0);

	return result;
}

void CLSJJSlits::setupBladeControls(const QString &upperBladeBaseName, const QString &lowerBladeBaseName, const QString &inboardBladeBaseName, const QString &outboardBladeBaseName)
{
	if (!upperBladeBaseName.isEmpty())
		setUpperBladeControl( createBladeControl(name()+"UpperBlade", upperBladeBaseName) );

	if (!lowerBladeBaseName.isEmpty())
		setLowerBladeControl( createBladeControl(name()+"LowerBlade", lowerBladeBaseName) );

	if (!inboardBladeBaseName.isEmpty())
		setInboardBladeControl( createBladeControl(name()+"InboardBlade", inboardBladeBaseName) );

	if (!outboardBladeBaseName.isEmpty())
		setOutboardBladeControl( createBladeControl(name()+"OutboardBlade", outboardBladeBaseName) );

	updatePseudoMotorControls();
	updateStates();
}
