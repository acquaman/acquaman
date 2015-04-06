#include "CLSStandardsWheel.h"

// CLSStandardsWheelElement
///////////////////////////////////////////////

CLSStandardsWheelElement::CLSStandardsWheelElement(const QString &name, double position, QObject *parent)
	: QObject(parent)
{
	name_ = name;
	position_ = position;
}

CLSStandardsWheelElement::~CLSStandardsWheelElement()
{

}

void CLSStandardsWheelElement::setName(const QString &newName)
{
	if (newName != name_){

		name_ = newName;
		emit nameChanged(name_);
	}
}

void CLSStandardsWheelElement::setPosition(double newPosition)
{
	if (newPosition != position_){

		position_ = newPosition;
		emit positionChanged(position_);
	}
}


// CLSStandardsWheel
///////////////////////////////////////////////

CLSStandardsWheel::CLSStandardsWheel(const QString &name, const QString &basePVName, QObject *parent)
	: QObject(parent)
{
	wheel_ = new CLSMAXvMotor(name, basePVName, name, true, 0.1, 2.0, this, ":deg");

	for (int i = 0; i < 12; i++)
		wheelElements_ << new CLSStandardsWheelElement("", i*30.0, this);
}

CLSStandardsWheel::~CLSStandardsWheel()
{

}

void CLSStandardsWheel::moveToIndex(int index)
{
	if (index < 12 && index >= 0)
		wheel_->move(wheelElements_.at(index)->position());
}
