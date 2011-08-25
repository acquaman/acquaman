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
