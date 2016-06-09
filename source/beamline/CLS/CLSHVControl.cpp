#include "CLSHVControl.h"

CLSHVControl::~CLSHVControl()
{

}

CLSHVControl::CLSHVControl( const QString& name, const QString& basePVName, const QString& readPVName, const QString& writePVName, const QString& powerPVName,
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

	if (measuredCurrentPVName.length() > 0) {
		measuredCurrentPV_ = new AMProcessVariable(basePVName + measuredCurrentPVName, true, this);
		connect(measuredCurrentPV_, SIGNAL(writeReadyChanged(bool)), this, SLOT(onPVConnected(bool)));
		connect(measuredCurrentPV_, SIGNAL(connectionTimeout()), this, SLOT(onConnectionTimeout()));
		connect(measuredCurrentPV_, SIGNAL(valueChanged(double)), this, SIGNAL(currentValueChanged(double)));
	} else {
		measuredCurrentPV_ = 0;
	}

	connect(this, SIGNAL(valueChanged(double)), this, SIGNAL(voltageValueChanged(double)));
}

int CLSHVControl::powerStatus() const
{
	return powerPV_->getInt();
}

double CLSHVControl::voltage() const
{
	return value();
}

double CLSHVControl::measuredCurrent() const
{
	if (measuredCurrentPV_)
		return measuredCurrentPV_->getDouble();
	else
		return 0;
}

AMControl *CLSHVControl::powerOnOffControl() const
{
	return powerOnOffControl_;
}

bool CLSHVControl::isConnected() const {
	bool connectedNow = AMPVControl::isConnected() && powerPV_->writeReady() && statusPV_->readReady();
	if (measuredCurrentPV_)
		connectedNow = connectedNow && measuredCurrentPV_->readReady();

	return connectedNow;
}

void CLSHVControl::onPowerOn()
{
	if (powerPV_->isConnected()) {
		powerPV_->setValue(1);
	}
}

void CLSHVControl::onPowerOff()
{
	if (powerPV_->isConnected()) {
		powerPV_->setValue(0);
	}
}
