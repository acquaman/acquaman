/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "CLSSR570.h"
#include <QDebug>

CLSSR570::CLSSR570(const QString &name, const QString &valueName, const QString &unitsName, QObject *parent)
	: AMCurrentAmplifier(name, parent)
{
	supportsSensitivityMode_ = true;

	atMinimumSensitivity_ = false;
	atMaximumSensitivity_ = false;
	valueIndex_ = -1;

	value_ = new AMProcessVariable(valueName, true, this);
	units_ = new AMProcessVariable(unitsName, true, this);

	// emit the sensitivityChanged() and unitsChanged signals.
	connect(value_, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
	connect(units_, SIGNAL(valueChanged(QString)), this, SLOT(onUnitsChanged(QString)) );

	// check the max/min sensitivity conditions with every update.
	connect(value_, SIGNAL(valueChanged()), this, SLOT(onSensitivityChanged()));
	connect(units_, SIGNAL(valueChanged()), this, SLOT(onSensitivityChanged()));

	// report connection changes.
	connect(value_, SIGNAL(connected()), this, SLOT(onConnectedChanged()));
	connect(units_, SIGNAL(connected()), this, SLOT(onConnectedChanged()));

	// piggyback interface signals on class-specific signals.
	connect( this, SIGNAL(sensitivityChanged(int)), this, SIGNAL(valueChanged()) );
	connect( this, SIGNAL(unitsChanged(QString)), this, SIGNAL(valueChanged()) );
	connect( this, SIGNAL(maximumSensitivity(bool)), this, SIGNAL(maximumValue(bool)) );
	connect( this, SIGNAL(minimumSensitivity(bool)), this, SIGNAL(minimumValue(bool)) );

	setAmplifierMode(AMCurrentAmplifier::Sensitivity);
}

CLSSR570::~CLSSR570(){}

QList<double> CLSSR570::values() const
{
	return QList<double>() << 1 << 2 << 5 << 10 << 20 << 50 << 100 << 200 << 500;
}

QStringList CLSSR570::unitsList() const
{
	return QStringList() << "pA/V" << "nA/V" << "uA/V" << "mA/V";
}

double CLSSR570::minimumValueForUnits(const QString &units) const
{
    Q_UNUSED(units)

    return 1;
}

double CLSSR570::maximumValueForUnits(const QString &units) const
{
    int value;

    if (units == "mA/V")
        value = 1;
    else
        value = 500;

    return value;
}

void CLSSR570::setValueControl(int value)
{
	setValueIndex(valueToIndex(value));
}

void CLSSR570::setValueIndex(int index)
{
	if (valueOkay(index) && index != value_->getInt())
		value_->setValue(index);
}

void CLSSR570::setUnits(QString units)
{
	if (unitsOkay(units) && units != units_->getString())
		units_->setValue(units);
}

void CLSSR570::onValueChanged(int index)
{
	emit sensitivityChanged(index);
}

void CLSSR570::onSensitivityChanged()
{
	if (!atMaximumSensitivity_ && (value_->getInt() == 0 && units_->getString() == "pA/V")) {
		emit maximumSensitivity(atMaximumSensitivity_ = true);

	} else if (atMaximumSensitivity_ && (value_->getInt() != 0 || units_->getString() != "pA/V")) {
		emit maximumSensitivity(atMaximumSensitivity_ = false);

	} else if (!atMinimumSensitivity_ && (value_->getInt() == 0 && units_->getString() == "mA/V")) {
		emit minimumSensitivity(atMinimumSensitivity_ = true);

	} else if (atMinimumSensitivity_ && (value_->getInt() != 0 || units_->getString() != "mA/V")) {
		emit minimumSensitivity(atMinimumSensitivity_ = false);
	}
}

void CLSSR570::onUnitsChanged(const QString &newUnits)
{
	emit unitsChanged(newUnits);
}

void CLSSR570::onConnectedChanged()
{
	if ((value_->isConnected() && units_->isConnected()) != connected_){
		emit isConnected(connected_ = (value_->isConnected() && units_->isConnected()));
		onSensitivityChanged();
	}
}

void CLSSR570::valueTo8(){
	value_->setValue(8);
}

void CLSSR570::valueTo0(){
	value_->setValue(0);
}

bool CLSSR570::valueOkay(int value) const
{
	return (value >= 0 && value <= 8);
}

bool CLSSR570::unitsOkay(QString units) const
{
	return units.contains(QRegExp("^(p|n|u|m)A/V$"));
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
		valueIndex_ = 8;
		QTimer::singleShot(0, this, SLOT(doDelayedValueIndex()));
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
		valueIndex_ = 0;
		QTimer::singleShot(50, this, SLOT(doDelayedValueIndex()));
	}

	return true;
}

void CLSSR570::setValueImplementation(const QString &valueArg)
{
	QString units = valueArg.split(" ").at(1);
	if ( unitsOkay(units) )
		setUnits(units);

	int value = valueArg.split(" ").at(0).toInt();
	if ( valueOkay(valueToIndex(value)) ){

		valueIndex_ = valueToIndex(value);
		QTimer::singleShot(0, this, SLOT(doDelayedValueIndex()));
	}
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
		next = (increase) ? QString() : QString("nA/V");
	else if (current == "nA/V")
		next = (increase) ? QString("pA/V") : QString("uA/V");
	else if (current == "uA/V")
		next = (increase) ? QString("nA/V") : QString("mA/V");
	else if (current == "mA/V")
		next = (increase) ? QString("uA/V") : QString();
	else
		next = QString();

	return next;
}

int CLSSR570::valueToIndex(int value) const
{
	int val = -1;

	switch(value){

	case 1:
		val = 0;
		break;
	case 2:
		val = 1;
		break;
	case 5:
		val = 2;
		break;
	case 10:
		val = 3;
		break;
	case 20:
		val = 4;
		break;
	case 50:
		val = 5;
		break;
	case 100:
		val = 6;
		break;
	case 200:
		val = 7;
		break;
	case 500:
		val = 8;
		break;
	}

	return val;
}

int CLSSR570::indexToValue(int index) const
{
	int val = -1;

	switch(index){

	case 0:
		val = 1;
		break;
	case 1:
		val = 2;
		break;
	case 2:
		val = 5;
		break;
	case 3:
		val = 10;
		break;
	case 4:
		val = 20;
		break;
	case 5:
		val = 50;
		break;
	case 6:
		val = 100;
		break;
	case 7:
		val = 200;
		break;
	case 8:
		val = 500;
		break;
	}

	return val;
}

void CLSSR570::doDelayedValueIndex()
{
	setValueIndex(valueIndex_);
}
