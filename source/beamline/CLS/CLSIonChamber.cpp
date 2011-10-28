#include "CLSIonChamber.h"

CLSIonChamber::CLSIonChamber(const QString &name, const QString &description, const QString &countsPV, const QString &voltagePV, const QString &sensitivityName, const QString &sensitivityUnits, QObject *parent)
	: AMIonChamber(name, description, parent)
{
	connected_ = false;

	counts_ = new AMReadOnlyPVControl(name+" counts", countsPV, this);
	voltage_ = new AMReadOnlyPVControl(name+" voltage", voltagePV, this);
	sensitivity_ = new CLSSR570(sensitivityName, sensitivityUnits, this);

	connect(counts_, SIGNAL(valueChanged(double)), this, SIGNAL(countsChanged()));
	connect(counts_, SIGNAL(valueChanged(double)), this, SIGNAL(countsChanged(double)));

	connect(voltage_, SIGNAL(valueChanged(double)), this, SIGNAL(voltageChanged()));
	connect(voltage_, SIGNAL(valueChanged(double)), this, SIGNAL(voltageChanged(double)));

	connect(counts_, SIGNAL(valueChanged(double)), this, SIGNAL(readingsChanged()));
	connect(voltage_, SIGNAL(valueChanged(double)), this, SIGNAL(readingsChanged()));

	connect(sensitivity_, SIGNAL(sensitivityChanged()), this, SIGNAL(sensitivityChanged()));
	connect(sensitivity_, SIGNAL(valueChanged(int)), this, SIGNAL(sensitivityValueChanged(int)));
	connect(sensitivity_, SIGNAL(unitsChanged(QString)), this, SIGNAL(sensitivityUnitsChanged(QString)));
	connect(sensitivity_, SIGNAL(minimumSensitivity(bool)), this, SIGNAL(minimumSensitivity(bool)));
	connect(sensitivity_, SIGNAL(maximumSensitivity(bool)), this, SIGNAL(maximumSensitivity(bool)));

	connect(counts_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));
	connect(voltage_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));
	connect(sensitivity_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));
}

void CLSIonChamber::onConnectedChanged()
{
	if ((counts_->isConnected() && voltage_->isConnected() && sensitivity_->isConnected()) != connected_)
		emit connected(connected_ = (counts_->isConnected() && voltage_->isConnected() && sensitivity_->isConnected()));
}
