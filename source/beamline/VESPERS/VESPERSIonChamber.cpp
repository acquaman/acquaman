#include "VESPERSIonChamber.h"

VESPERSIonChamber::VESPERSIonChamber(QString name, QString hvName, QString sensitivityBaseName, QString voltageName, QObject *parent)
	: QObject(parent)
{
	name_ = name;

	hv_ = new AMProcessVariable(hvName, true, this);
	sensitivityValue_ = new AMProcessVariable(sensitivityBaseName+":sens_num.VAL", true, this);
	sensitivityUnits_ = new AMProcessVariable(sensitivityBaseName+":sens_unit.VAL", true, this);
	voltage_ = new AMProcessVariable(voltageName, true, this);

	connect(hv_, SIGNAL(valueChanged(int)), this, SIGNAL(highVoltageChanged(int)));
	connect(sensitivityValue_, SIGNAL(valueChanged(int)), this, SLOT(onSensitivityValueChanged(int)));
	connect(sensitivityUnits_, SIGNAL(valueChanged(QString)), this, SIGNAL(sensitivityUnitsChanged(QString)));
	connect(voltage_, SIGNAL(valueChanged(double)), this, SIGNAL(voltageChanged(double)));
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
