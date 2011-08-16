#include "CLSVMEMotor.h"



CLSVMEMotor::CLSVMEMotor(const QString &name, const QString &baseName, const QString &description, bool hasEncoder, double tolerance, double moveStartTimeoutSeconds, QObject *parent) :
		AMPVwStatusControl(name, baseName+":mm:fbk", baseName+":mm", baseName+":status", baseName+":stop", parent, tolerance, moveStartTimeoutSeconds, new AMControlStatusCheckerCLSVME(), 1, description)
		//AMPVwStatusControl(name, baseName+":mm:fbk", baseName+":mm", baseName+":status", baseName+":stop", parent, tolerance, moveStartTimeoutSeconds, new AMControlStatusCheckerStopped(0), 1, description)
{
	usingKill_ = false;
	killPV_ = new AMProcessVariable(baseName+":kill", true, this);
	positionReadbackPV_ = new AMProcessVariable(baseName+":mm:sp", true, this);
	encoderReadbackPV_ = readPV_;

	if(!hasEncoder){
		disconnect(readPV_, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged(double)));
		disconnect(readPV_, SIGNAL(readReadyChanged(bool)), this, SLOT(onPVConnected(bool)));
		disconnect(readPV_, SIGNAL(connectionTimeout()), this, SIGNAL(readConnectionTimeoutOccurred()));
		disconnect(readPV_, SIGNAL(error(int)), this, SLOT(onReadPVError(int)));
		disconnect(readPV_, SIGNAL(connectionTimeout()), this, SLOT(onConnectionTimeout()));
		disconnect(readPV_, SIGNAL(initialized()), this, SLOT(onReadPVInitialized()));
		readPV_ = positionReadbackPV_;
		connect(readPV_, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged(double)));
		connect(readPV_, SIGNAL(readReadyChanged(bool)), this, SLOT(onPVConnected(bool)));
		connect(readPV_, SIGNAL(connectionTimeout()), this, SIGNAL(readConnectionTimeoutOccurred()));
		connect(readPV_, SIGNAL(error(int)), this, SLOT(onReadPVError(int)));
		connect(readPV_, SIGNAL(connectionTimeout()), this, SLOT(onConnectionTimeout()));
		connect(readPV_, SIGNAL(initialized()), this, SLOT(onReadPVInitialized()));
	}

	step_ = new AMPVControl(name+"Step", baseName+":step:sp", baseName+":step", QString(), this, 20);
	velocity_ = new AMPVControl(name+"Velocity", baseName+":velo:sp", baseName+":velo", QString(), this, 2);
	baseVelocity_ = new AMPVControl(name+"BaseVelocity", baseName+":vBase:sp", baseName+":veloBase", QString(), this, 2);
	acceleration_ = new AMPVControl(name+"Acceleration", baseName+":accel:sp", baseName+":accel", QString(), this, 2);
	currentVelocity_ = new AMReadOnlyPVControl(name+"CurrentVelocity", baseName+":velo:fbk", this);

	connect(step_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(step_, SIGNAL(valueChanged(double)), this, SIGNAL(stepChanged(double)));
	connect(velocity_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(velocity_, SIGNAL(valueChanged(double)), this, SIGNAL(velocityChanged(double)));
	connect(baseVelocity_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(baseVelocity_, SIGNAL(valueChanged(double)), this, SIGNAL(baseVelocityChanged(double)));
	connect(acceleration_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(acceleration_, SIGNAL(valueChanged(double)), this, SIGNAL(accelerationChanged(double)));
	connect(currentVelocity_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(currentVelocity_, SIGNAL(valueChanged(double)), this, SIGNAL(currentVelocityChanged(double)));
}

bool CLSVMEMotor::isConnected() const{
	return AMPVwStatusControl::isConnected() && step_->isConnected() && velocity_->isConnected() && baseVelocity_->isConnected() && acceleration_->isConnected() && currentVelocity_->isConnected();
}

double CLSVMEMotor::step() const
{
	if (isConnected())
		return step_->value();

	return 0.5;
}

double CLSVMEMotor::velocity() const{
	if(isConnected())
		return velocity_->value();

	return 0.5;
}

double CLSVMEMotor::baseVelocity() const{
	if(isConnected())
		return baseVelocity_->value();

	return 0.5;
}

double CLSVMEMotor::acceleration() const{
	if(isConnected())
		return acceleration_->value();

	return 0.5;
}

double CLSVMEMotor::currentVelocity() const{
	if(isConnected())
		return currentVelocity_->value();

	return 0.5;
}

bool CLSVMEMotor::usingKill() const{
	return usingKill_;
}

void CLSVMEMotor::setStep(double step)
{
	if (isConnected())
		step_->move(step);
}

void CLSVMEMotor::setVelocity(double velocity){
	if(isConnected())
		velocity_->move(velocity);
}

void CLSVMEMotor::setBaseVelocity(double baseVelocity){
	if(isConnected())
		baseVelocity_->move(baseVelocity);
}

void CLSVMEMotor::setAcceleration(double acceleration){
	if(isConnected())
		acceleration_->move(acceleration);
}

void CLSVMEMotor::setUsingKill(bool usingKill){
	usingKill_ = true;
}

bool CLSVMEMotor::stop(){
	if(!canStop())
		return false;

	if(usingKill_)
		killPV_->setValue(stopValue_);
	else
		stopPV_->setValue(stopValue_);

	stopInProgress_ = true;	// flag that a stop is "in progress" -- we've issued the stop command.
	moveInProgress_ = false;	// one of "our" moves is no longer in progress.
	return true;
}
