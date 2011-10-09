#include "CLSIonChamber.h"

CLSIonChamber::CLSIonChamber(const QString &name, const QString &description, const QString &countsPV, const QString &voltagePV, QObject *parent)
	: AMIonChamber(name, description, parent)
{
	counts_ = new AMReadOnlyPVControl(name+" counts", countsPV, this);
	voltage_ = new AMReadOnlyPVControl(name+" voltage", voltagePV, this);

	connect(counts_, SIGNAL(valueChanged(double)), this, SIGNAL(countsChanged(double)));
	connect(voltage_, SIGNAL(valueChanged(double)), this, SIGNAL(voltageChanged(double)));

	connect(counts_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));
	connect(voltage_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged()));
}

bool CLSIonChamber::onConnectedChanged()
{
	if ((counts_->isConnected() && voltage_->isConnected()) != connected_)
		emit connected(connected_ = (counts_->isConnected() && voltage_->isConnected()));
}
