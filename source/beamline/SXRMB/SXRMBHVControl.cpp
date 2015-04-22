#include "SXRMBHVControl.h"

#include <QDebug>
SXRMBHVControl::~SXRMBHVControl()
{

}

SXRMBHVControl::SXRMBHVControl( const QString& name, const QString& basePVName, const QString& readPVName, const QString& writePVName, const QString& powerPVName,
								const QString& statusPVName, const QString& measuredCurrentPVName,
								QObject* parent, double tolerance, double moveStartTimeoutSeconds, int stopValue, const QString &description)
	:AMPVControl(name, basePVName + readPVName, basePVName + writePVName, QString(), parent, tolerance, moveStartTimeoutSeconds, stopValue, description)
{
	powerOnOffControl_ = new AMPVControl("HV Power OnOff control", basePVName + powerPVName, basePVName + powerPVName, QString(), this);

	powerPV_ = new AMProcessVariable(basePVName + powerPVName, true, this);
	connect(powerPV_, SIGNAL(writeReadyChanged(bool)), this, SLOT(onPVConnected(bool)));
//	connect(powerPV_, SIGNAL(error(int)), this, SLOT(onPowerPVError(int)));
	connect(powerPV_, SIGNAL(connectionTimeout()), this, SIGNAL(powerPVConnectionTimeout()));
	connect(powerPV_, SIGNAL(connectionTimeout()), this, SLOT(onConnectionTimeout()));

	statusPV_ = new AMProcessVariable(basePVName + statusPVName, true, this);
	connect(statusPV_, SIGNAL(writeReadyChanged(bool)), this, SLOT(onPVConnected(bool)));
	connect(statusPV_, SIGNAL(connectionTimeout()), this, SLOT(onConnectionTimeout()));
	connect(statusPV_, SIGNAL(valueChanged(int)), this, SIGNAL(powerStatusChanged(int)));

	measuredCurrentPV_ = new AMProcessVariable(basePVName + measuredCurrentPVName, true, this);
	connect(measuredCurrentPV_, SIGNAL(writeReadyChanged(bool)), this, SLOT(onPVConnected(bool)));
	connect(measuredCurrentPV_, SIGNAL(connectionTimeout()), this, SLOT(onConnectionTimeout()));
	connect(measuredCurrentPV_, SIGNAL(valueChanged(double)), this, SIGNAL(currentValueChanged(double)));

	connect(this, SIGNAL(valueChanged(double)), this, SIGNAL(voltageValueChanged(double)));
}

int SXRMBHVControl::powerStatus() const
{
	return powerPV_->getInt();
}

double SXRMBHVControl::voltage() const
{
	return value();
}

double SXRMBHVControl::measuredCurrent() const
{
	return measuredCurrentPV_->getDouble();
}

AMControl *SXRMBHVControl::powerOnOffControl() const
{
	return powerOnOffControl_;
}

bool SXRMBHVControl::isConnected() const {
	return AMPVControl::isConnected() && powerPV_->writeReady() && statusPV_->readReady() && measuredCurrentPV_->readReady();
}

void SXRMBHVControl::onPowerOn()
{
	if (powerPV_->isConnected()) {
		powerPV_->setValue(1);
	}
}

void SXRMBHVControl::onPowerOff()
{
	if (powerPV_->isConnected()) {
		powerPV_->setValue(0);
	}
}
