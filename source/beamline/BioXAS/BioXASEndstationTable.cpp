#include "BioXASEndstationTable.h"

BioXASEndstationTable::BioXASEndstationTable(const QString &name, const QString &pvBaseName, const bool canRoll, QObject *parent) :
    QObject(parent)
{
	name_ = name;

	heightPVController_ = new AMPVwStatusControl(name+" Height", pvBaseName+":Height:mm:fbk", pvBaseName+":Height:mm", pvBaseName+":Height:status", pvBaseName+":Height:stop", this);
	pitchPVController_ = new AMPVwStatusControl(name+" Pitch", pvBaseName+":Pitch:deg:fbk", pvBaseName+":Pitch:deg", pvBaseName+":Pitch:status", pvBaseName+":Pitch:stop", this);
	lateralPVController_ = new AMPVwStatusControl(name+" Lateral", pvBaseName+":Lateral:mm:fbk", pvBaseName+":Lateral:mm", pvBaseName+":Lateral:status", pvBaseName+":Lateral:stop", this);
	yawPVController_ = new AMPVwStatusControl(name+" Yaw", pvBaseName+":Yaw:deg:fbk", pvBaseName+":Yaw:deg", pvBaseName+":Yaw:status", pvBaseName+":Yaw:stop", this);
	if (canRoll) {
		rollPVController_ = new AMPVwStatusControl(name+" Roll", pvBaseName+":Roll:deg:fbk", pvBaseName+":Roll:deg", pvBaseName+":Roll:status", pvBaseName+":Roll:stop", this);
	} else {
		rollPVController_ = 0;
	}

	connect(heightPVController_, SIGNAL(connected(bool)), this, SLOT(onPVConnected()));
	connect(heightPVController_, SIGNAL(valueChanged(double)), this, SIGNAL(heightValueChanged(double)));
	connect(pitchPVController_, SIGNAL(connected(bool)), this, SLOT(onPVConnected()));
	connect(pitchPVController_, SIGNAL(valueChanged(double)), this, SIGNAL(pitchValueChanged(double)));
	connect(lateralPVController_, SIGNAL(connected(bool)), this, SLOT(onPVConnected()));
	connect(lateralPVController_, SIGNAL(valueChanged(double)), this, SIGNAL(lateralValueChanged(double)));
	connect(yawPVController_, SIGNAL(connected(bool)), this, SLOT(onPVConnected()));
	connect(yawPVController_, SIGNAL(valueChanged(double)), this, SIGNAL(yawValueChanged(double)));
	if (rollPVController_) {
		connect(rollPVController_, SIGNAL(connected(bool)), this, SLOT(onPVConnected()));
		connect(rollPVController_, SIGNAL(valueChanged(double)), this, SIGNAL(rollValueChanged(double)));
	}

	wasConnected_ = false;
	onPVConnected();
}

bool BioXASEndstationTable::isConnected() const
{
	bool connected = heightPVController_->isConnected() && pitchPVController_->isConnected() && lateralPVController_->isConnected() && yawPVController_->isConnected();

	if (rollPVController_) {
		connected &= rollPVController_->isConnected();
	}

	return connected;
}

void BioXASEndstationTable::moveHeight(double setpoint)
{
	if (heightPVController_->isConnected())
		heightPVController_->move(setpoint);
}

void BioXASEndstationTable::movePitch(double setpoint)
{
	if (pitchPVController_->isConnected())
		pitchPVController_->move(setpoint);
}

void BioXASEndstationTable::moveLateral(double setpoint)
{
	if (lateralPVController_->isConnected())
		lateralPVController_->move(setpoint);
}

void BioXASEndstationTable::moveYaw(double setpoint)
{
	if (yawPVController_->isConnected())
		yawPVController_->move(setpoint);
}

void BioXASEndstationTable::moveRoll(double setpoint)
{
	if (rollPVController_->isConnected())
		rollPVController_->move(setpoint);
}

void BioXASEndstationTable::onPVConnected()
{
	if (wasConnected_ != isConnected()) {
		wasConnected_ = isConnected();
		emit connected(wasConnected_);
	}
}

