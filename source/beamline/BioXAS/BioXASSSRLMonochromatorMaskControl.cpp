#include "BioXASSSRLMonochromatorMaskControl.h"

BioXASSSRLMonochromatorMaskControl::BioXASSSRLMonochromatorMaskControl(const QString &name, QObject *parent) :
	AMPseudoMotorControl(name, "", parent)
{
	// Initialize inherited variables.

	value_ = None;
	setpoint_ = None;
	minimumValue_ = Open;
	maximumValue_ = None;

	setEnumStates(QStringList() << valueToString(Open) << valueToString(Closed) << valueToString(None));
	setAllowsMovesWhileMoving(false);
	setContextKnownDescription("BeamStatus");

	// Initialize local variables.

	upperBlade_ = 0;
	lowerBlade_ = 0;
	status_ = 0;
}

BioXASSSRLMonochromatorMaskControl::~BioXASSSRLMonochromatorMaskControl()
{

}

bool BioXASSSRLMonochromatorMaskControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = (upperBlade_->canMeasure() && lowerBlade_->canMeasure() && status_->canMeasure());

	return result;
}

//bool BioXASSSRLMonochromatorMaskControl::canMove() const
//{
//	bool result = false;

//	if (isConnected())
//		result = (upperBlade_->canMove() && lowerBlade_->canMove());

//	return result;
//}

bool BioXASSSRLMonochromatorMaskControl::validValue(double value) const
{
	bool result = false;

	switch ((int)value) {
	case Open:
		result = true;
		break;
	case Closed:
		result = true;
		break;
	case None:
		result = true;
		break;
	default:
		break;
	}

	return result;
}

bool BioXASSSRLMonochromatorMaskControl::isOpen() const
{
	bool result = false;

	if (isConnected())
		result = (status_->value() == 0);

	return result;
}

bool BioXASSSRLMonochromatorMaskControl::isClosed() const
{
	bool result = false;

	if (isConnected())
		result = (status_->value() == 1);

	return result;
}

QString BioXASSSRLMonochromatorMaskControl::valueToString(BioXASSSRLMonochromatorMaskControl::Value value)
{
	QString result;

	switch (value) {
	case Open:
		result = "Open";
		break;
	case Closed:
		result = "Closed";
		break;
	case None:
		result = "None";
		break;
	default:
		break;
	}

	return result;
}

void BioXASSSRLMonochromatorMaskControl::setUpperBlade(AMPVwStatusControl *newControl)
{
	if (upperBlade_ != newControl) {

		if (upperBlade_)
			removeChildControl(upperBlade_);

		upperBlade_ = newControl;

		if (upperBlade_)
			addChildControl(upperBlade_);

		updateStates();

		emit upperBladeChanged(upperBlade_);
	}
}

void BioXASSSRLMonochromatorMaskControl::setLowerBlade(AMPVwStatusControl *newControl)
{
	if (lowerBlade_ != newControl) {

		if (lowerBlade_)
			removeChildControl(lowerBlade_);

		lowerBlade_ = newControl;

		if (lowerBlade_)
			addChildControl(lowerBlade_);

		updateStates();

		emit upperBladeChanged(lowerBlade_);
	}
}

void BioXASSSRLMonochromatorMaskControl::setStatus(AMReadOnlyPVControl *newControl)
{
	if (status_ != newControl) {

		if (status_)
			removeChildControl(status_);

		status_ = newControl;

		if (status_)
			addChildControl(status_);

		updateStates();

		emit statusChanged(status_);
	}
}
void BioXASSSRLMonochromatorMaskControl::updateConnected()
{
	bool connected = (
				upperBlade_ && upperBlade_->isConnected() &&
				lowerBlade_ && lowerBlade_->isConnected() &&
				status_ && status_->isConnected()
				);

	setConnected(connected);
}

void BioXASSSRLMonochromatorMaskControl::updateValue()
{
	double value = None;

	if (isConnected()) {
		if (isOpen())
			value = Open;
		else
			value = Closed;
	}

	setValue(value);
}

void BioXASSSRLMonochromatorMaskControl::updateMoving()
{
	bool moving = false;

	if (isConnected())
		moving = ( upperBlade_->isMoving() || lowerBlade_->isMoving() );

	setIsMoving(moving);
}
