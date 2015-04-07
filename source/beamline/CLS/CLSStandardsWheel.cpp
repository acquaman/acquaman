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
	elementMapper_ = new QSignalMapper;

	for (int i = 0; i < 12; i++){

		CLSStandardsWheelElement *element = new CLSStandardsWheelElement(QString("%1").arg(i+1), i*30.0, this);
		wheelElements_ << element;
		elementMapper_->setMapping(element, i);
		connect(element, SIGNAL(nameChanged(QString)), elementMapper_, SLOT(map()));
	}

	connect(elementMapper_, SIGNAL(mapped(int)), this, SLOT(onMappedElementChanged(int)));
}

CLSStandardsWheel::~CLSStandardsWheel()
{

}

void CLSStandardsWheel::moveToIndex(int index)
{
	if (index < 12 && index >= 0)
		wheel_->move(wheelElements_.at(index)->position());
}

void CLSStandardsWheel::setName(int index, const QString &newName)
{
	wheelElements_.at(index)->setName(newName);
}

void CLSStandardsWheel::onMappedElementChanged(int id)
{
	emit nameChanged(id, wheelElements_.at(id)->name());
}
