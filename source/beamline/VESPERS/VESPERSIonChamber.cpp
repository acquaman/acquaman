/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "VESPERSIonChamber.h"

VESPERSIonChamber::VESPERSIonChamber(QString name, QString hvName, QString sensitivityBaseName, QString voltageName, QString countName, QObject *parent)
	: QObject(parent)
{
	name_ = name;
	atMinimumSensitivity_ = false;
	atMaximumSensitivity_ = false;

	hv_ = new AMProcessVariable(hvName, true, this);
	sensitivityValue_ = new AMProcessVariable(sensitivityBaseName+":sens_num.VAL", true, this);
	sensitivityUnits_ = new AMProcessVariable(sensitivityBaseName+":sens_unit.VAL", true, this);
	voltage_ = new AMProcessVariable(voltageName, true, this);
	counts_ = new AMProcessVariable(countName, true, this);

	connect(hv_, SIGNAL(valueChanged(int)), this, SIGNAL(highVoltageChanged(int)));
	connect(sensitivityValue_, SIGNAL(valueChanged(int)), this, SLOT(onSensitivityValueChanged(int)));
	connect(sensitivityUnits_, SIGNAL(valueChanged(QString)), this, SIGNAL(sensitivityUnitsChanged(QString)));
	connect(voltage_, SIGNAL(valueChanged(double)), this, SIGNAL(voltageChanged(double)));
	connect(counts_, SIGNAL(valueChanged(double)), this, SIGNAL(countsChanged(double)));

	connect(sensitivityValue_, SIGNAL(valueChanged()), this, SLOT(onSensitivityChanged()));
	connect(sensitivityUnits_, SIGNAL(valueChanged()), this, SLOT(onSensitivityChanged()));
}

void VESPERSIonChamber::onSensitivityValueChanged(int index)
{
	switch(index){

	case 0:
		emit sensitivityValueChanged(1);
		break;
	case 1:
		emit sensitivityValueChanged(2);
		break;
	case 2:
		emit sensitivityValueChanged(5);
		break;
	case 3:
		emit sensitivityValueChanged(10);
		break;
	case 4:
		emit sensitivityValueChanged(20);
		break;
	case 5:
		emit sensitivityValueChanged(50);
		break;
	case 6:
		emit sensitivityValueChanged(100);
		break;
	case 7:
		emit sensitivityValueChanged(200);
		break;
	case 8:
		emit sensitivityValueChanged(500);
		break;
	}
}

void VESPERSIonChamber::increaseSensitivity()
{
	// Don't do anything if we are already at the maximum sensitivity.
	if (atMaximumSensitivity_)
		return;

	int current = sensitivityValue_->getInt();

	// If possible to just move to the next lower value, do so.
	if (current != 1)
		sensitivityValue_->setValue(nextSensitivityValue(true, current));

	// Otherwise, we need to move the sensitivity unit to the next more sensitive value.
	else {

		sensitivityUnits_->setValue(nextSensitivityUnits(true, sensitivityUnits_->getString()));
		sensitivityValue_->setValue(500);
	}
}

void VESPERSIonChamber::decreaseSensitivity()
{
	// Don't do anything if we are already at the minimum sensitivity.
	if (atMinimumSensitivity_)
		return;

	int current = sensitivityValue_->getInt();

	// If possible to just move to the next higher value, do so.
	if (current != 500)
		sensitivityValue_->setValue(nextSensitivityValue(false, current));

	// Otherwise, we need to move the sensitivity unit to the next less sensitive value.
	else {

		sensitivityUnits_->setValue(nextSensitivityUnits(false, sensitivityUnits_->getString()));
		sensitivityValue_->setValue(1);
	}
}

int VESPERSIonChamber::nextSensitivityValue(bool increase, int current)
{
	int next = 0;

	switch(current){
	case 1:
		next = (increase == true) ? -1 : 2;
		break;
	case 2:
		next = (increase == true) ? 1 : 5;
		break;
	case 5:
		next = (increase == true) ? 2 : 10;
		break;
	case 10:
		next = (increase == true) ? 5 : 20;
		break;
	case 20:
		next = (increase == true) ? 10 : 50;
		break;
	case 50:
		next = (increase == true) ? 20 : 100;
		break;
	case 100:
		next = (increase == true) ? 50 : 200;
		break;
	case 200:
		next = (increase == true) ? 100 : 500;
		break;
	case 500:
		next = (increase == true) ? 200 : -1;
		break;
	}

	return next;
}

QString VESPERSIonChamber::nextSensitivityUnits(bool increase, QString current)
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

void VESPERSIonChamber::onSensitivityChanged()
{
	if (!atMaximumSensitivity_ && (sensitivityValue_->getInt() == 1 && sensitivityUnits_->getString() == "pA/V"))
		emit maximumSensitivity(atMaximumSensitivity_ = true);
	else if (atMaximumSensitivity_ && (sensitivityValue_->getInt() != 1 || sensitivityUnits_->getString() != "pA/V"))
		emit maximumSensitivity(atMaximumSensitivity_ = false);
	else if (!atMinimumSensitivity_ && (sensitivityValue_->getInt() == 500 && sensitivityUnits_->getString() == "mA/V"))
		emit minimumSensitivity(atMinimumSensitivity_ = true);
	else if (atMinimumSensitivity_ && (sensitivityValue_->getInt() != 500 || sensitivityUnits_->getString() != "mA/V"))
		emit minimumSensitivity(atMinimumSensitivity_ = false);
}
