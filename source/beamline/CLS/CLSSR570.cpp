#include "CLSSR570.h"

CLSSR570::CLSSR570(const QString &valueName, const QString &unitsName, QObject *parent)
	: QObject(parent)
{
	atMinimumSensitivity_ = false;
	atMaximumSensitivity_ = false;
	connected_ = false;

	value_ = new AMProcessVariable(valueName, true, this);
	units_ = new AMProcessVariable(unitsName, true, this);

	connect(value_, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
	connect(units_, SIGNAL(valueChanged(QString)), this, SIGNAL(unitsChanged(QString)));
	connect(value_, SIGNAL(valueChanged()), this, SIGNAL(sensitivityChanged()));
	connect(units_, SIGNAL(valueChanged()), this, SIGNAL(sensitivityChanged()));

	connect(value_, SIGNAL(valueChanged()), this, SLOT(onSensitivityChanged()));
	connect(units_, SIGNAL(valueChanged()), this, SLOT(onSensitivityChanged()));

	connect(value_, SIGNAL(connected()), this, SLOT(onConnectedChanged()));
	connect(units_, SIGNAL(connected()), this, SLOT(onConnectedChanged()));
}

void CLSSR570::onConnectedChanged()
{
	if ((value_->isConnected() && units_->isConnected()) != connected_)
		emit connected(connected_ = (value_->isConnected() && units_->isConnected()));
}

void CLSSR570::onValueChanged(int index)
{
	switch(index){

	case 0:
		emit valueChanged(1);
		break;
	case 1:
		emit valueChanged(2);
		break;
	case 2:
		emit valueChanged(5);
		break;
	case 3:
		emit valueChanged(10);
		break;
	case 4:
		emit valueChanged(20);
		break;
	case 5:
		emit valueChanged(50);
		break;
	case 6:
		emit valueChanged(100);
		break;
	case 7:
		emit valueChanged(200);
		break;
	case 8:
		emit valueChanged(500);
		break;
	}
}

bool CLSSR570::increaseSensitivity()
{
	// Don't do anything if we are already at the maximum sensitivity.
	if (atMaximumSensitivity_)
		return false;

	int current = value_->getInt();
	int next = nextValue(true, current);
	QString units = nextUnits(true, units_->getString());

	if (next == -1 && units.isEmpty())
		return false;

	// If possible to just move to the next lower value, do so.
	if (current != 0 && next != -1)
		value_->setValue(next);

	// Otherwise, we need to move the sensitivity unit to the next more sensitive value.
	else {

		units_->setValue(units);
		value_->setValue(8);
	}

	return true;
}

bool CLSSR570::decreaseSensitivity()
{
	// Don't do anything if we are already at the minimum sensitivity.
	if (atMinimumSensitivity_)
		return false;

	int current = value_->getInt();
	int next = nextValue(false, current);
	QString units = nextUnits(false, units_->getString());

	if (next == -1 && units.isEmpty())
		return false;

	// If possible to just move to the next higher value, do so.
	if (current != 8 && next != -1)
		value_->setValue(next);

	// Otherwise, we need to move the sensitivity unit to the next less sensitive value.
	else {

		units_->setValue(units);
		value_->setValue(0);
	}

	return true;
}

int CLSSR570::nextValue(bool increase, int current)
{
	if (increase)
		return current == 0 ? -1 : current - 1;
	else if (!increase)
		return current == 8 ? -1 : current + 1;
	else
		return -1;
}

QString CLSSR570::nextUnits(bool increase, QString current)
{
	QString next;

	if (current == "pA/V")
		next = (increase == true) ? QString() : QString("nA/V");
	else if (current == "nA/V")
		next = (increase == true) ? QString("pA/V") : QString("uA/V");
	else if (current == "uA/V")
		next = (increase == true) ? QString("nA/V") : QString("mA/V");
	else if (current == "mA/V")
		next = (increase == true) ? QString("uA/V") : QString();
	else
		next = QString();

	return next;
}

void CLSSR570::onSensitivityChanged()
{
	if (!atMaximumSensitivity_ && (value_->getInt() == 0 && units_->getString() == "pA/V"))
		emit maximumSensitivity(atMaximumSensitivity_ = true);
	else if (atMaximumSensitivity_ && (value_->getInt() != 0 || units_->getString() != "pA/V"))
		emit maximumSensitivity(atMaximumSensitivity_ = false);
	else if (!atMinimumSensitivity_ && (value_->getInt() == 8 && units_->getString() == "mA/V"))
		emit minimumSensitivity(atMinimumSensitivity_ = true);
	else if (atMinimumSensitivity_ && (value_->getInt() != 8 || units_->getString() != "mA/V"))
		emit minimumSensitivity(atMinimumSensitivity_ = false);
}
