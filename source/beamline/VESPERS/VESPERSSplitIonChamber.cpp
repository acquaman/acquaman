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


#include "VESPERSSplitIonChamber.h"

VESPERSSplitIonChamber::VESPERSSplitIonChamber(QString name, QString hvName, QString sensitivityBaseNameA, QString sensitivityBaseNameB, QString voltageNameA, QString voltageNameB, QString countNameA, QString countNameB, QObject *parent)
	: QObject(parent)
{
	name_ = name;
	atMinimumSensitivity_ = false;
	atMaximumSensitivity_ = false;

	hv_ = new AMProcessVariable(hvName, true, this);
	sensitivityValueA_ = new AMProcessVariable(sensitivityBaseNameA+":sens_num.VAL", true, this);
	sensitivityValueB_ = new AMProcessVariable(sensitivityBaseNameB+":sens_num.VAL", true, this);
	sensitivityUnitsA_ = new AMProcessVariable(sensitivityBaseNameA+":sens_unit.VAL", true, this);
	sensitivityUnitsB_ = new AMProcessVariable(sensitivityBaseNameB+":sens_unit.VAL", true, this);
	voltageA_ = new AMProcessVariable(voltageNameA, true, this);
	voltageB_ = new AMProcessVariable(voltageNameB, true, this);
	countsA_ = new AMProcessVariable(countNameA, true, this);
	countsB_ = new AMProcessVariable(countNameB, true, this);

	connect(hv_, SIGNAL(valueChanged(int)), this, SIGNAL(highVoltageChanged(int)));
	connect(sensitivityValueA_, SIGNAL(valueChanged(int)), this, SLOT(onSensitivityValueChanged(int)));
	connect(sensitivityValueB_, SIGNAL(valueChanged(int)), this, SLOT(onSensitivityValueChanged(int)));
	connect(sensitivityUnitsA_, SIGNAL(valueChanged(QString)), this, SIGNAL(sensitivityUnitsChanged(QString)));
	connect(sensitivityUnitsB_, SIGNAL(valueChanged(QString)), this, SIGNAL(sensitivityUnitsChanged(QString)));
	connect(voltageA_, SIGNAL(valueChanged(double)), this, SIGNAL(voltageAChanged(double)));
	connect(voltageB_, SIGNAL(valueChanged(double)), this, SIGNAL(voltageBChanged(double)));
	connect(countsA_, SIGNAL(valueChanged(double)), this, SIGNAL(countsAChanged(double)));
	connect(countsB_, SIGNAL(valueChanged(double)), this, SIGNAL(countsBChanged(double)));
	connect(countsA_, SIGNAL(valueChanged()), this, SLOT(onCountsChanged()));
	connect(countsB_, SIGNAL(valueChanged()), this, SLOT(onCountsChanged()));

	connect(sensitivityValueA_, SIGNAL(valueChanged()), this, SLOT(onSensitivityChanged()));
	connect(sensitivityUnitsA_, SIGNAL(valueChanged()), this, SLOT(onSensitivityChanged()));
	connect(sensitivityValueB_, SIGNAL(valueChanged()), this, SLOT(onSensitivityChanged()));
	connect(sensitivityUnitsB_, SIGNAL(valueChanged()), this, SLOT(onSensitivityChanged()));
}

void VESPERSSplitIonChamber::onSensitivityValueChanged(int index)
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

void VESPERSSplitIonChamber::increaseSensitivity()
{
	// Don't do anything if we are already at the maximum sensitivity.
	if (atMaximumSensitivity_)
		return;

	int current = sensitivityValue();

	// If possible to just move to the next lower value, do so.
	if (current != 1)
		setSensitivityValue(nextSensitivityValue(true, current));

	// Otherwise, we need to move the sensitivity unit to the next more sensitive value.
	else {

		setSensitivityUnits(nextSensitivityUnits(true, sensitivityUnits()));
		setSensitivityValue(500);
	}
}

void VESPERSSplitIonChamber::decreaseSensitivity()
{
	// Don't do anything if we are already at the minimum sensitivity.
	if (atMinimumSensitivity_)
		return;

	int current = sensitivityValue();

	// If possible to just move to the next higher value, do so.
	if (current != 500)
		setSensitivityValue(nextSensitivityValue(false, current));

	// Otherwise, we need to move the sensitivity unit to the next less sensitive value.
	else {

		setSensitivityUnits(nextSensitivityUnits(false, sensitivityUnits()));
		setSensitivityValue(1);
	}
}

int VESPERSSplitIonChamber::nextSensitivityValue(bool increase, int current)
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

QString VESPERSSplitIonChamber::nextSensitivityUnits(bool increase, QString current)
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

void VESPERSSplitIonChamber::onSensitivityChanged()
{
	if (!atMaximumSensitivity_ && (sensitivityValue() == 1 && sensitivityUnits() == "pA/V"))
		emit maximumSensitivity(atMaximumSensitivity_ = true);
	else if (atMaximumSensitivity_ && (sensitivityValue() != 1 || sensitivityUnits() != "pA/V"))
		emit maximumSensitivity(atMaximumSensitivity_ = false);
	else if (!atMinimumSensitivity_ && (sensitivityValue() == 500 && sensitivityUnits() == "mA/V"))
		emit minimumSensitivity(atMinimumSensitivity_ = true);
	else if (atMinimumSensitivity_ && (sensitivityValue() != 500 || sensitivityUnits() != "mA/V"))
		emit minimumSensitivity(atMinimumSensitivity_ = false);
}
