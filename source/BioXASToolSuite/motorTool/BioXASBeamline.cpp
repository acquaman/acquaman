#include "BioXASBeamline.h"

BioXASBeamline::BioXASBeamline(QList<BioXASBeamlineDef::BioXASMotor> motorDefinitions, QObject *parent)
	: QObject(parent)
{
	for (int i = 0; i < motorDefinitions.size(); i++) {
		BioXASMAXvMotor * motor = new BioXASMAXvMotor(motorDefinitions[i].motorType_, motorDefinitions[i].motorTypeName_,
													  motorDefinitions[i].pvBaseName_, motorDefinitions[i].description_,
													  motorDefinitions[i].pvUnitField_, true, 0.05, 2.0, this);

		motors_.append(motor);
	}
}

BioXASBeamline::~BioXASBeamline()
{
	motors_.clear();
}

QList<BioXASMAXvMotor *> BioXASBeamline::getMotorsByType(BioXASBeamlineDef::BioXASMotorType motorType)
{
	QList<BioXASMAXvMotor *> matchMotors;

	for (int i = 0; i < motors_.size(); i++) {
		if (motors_[i]->type() == motorType) {
			matchMotors.append(motors_[i]);
		}
	}

	return matchMotors;
}
