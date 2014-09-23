#include "BioXASBeamline.h"

#include <QDebug>

BioXASBeamline::BioXASBeamline(QList<BioXASBeamlineDef::BioXASMotor> motorDefinitions, QObject *parent)
	: QObject(parent)
{
	for (int i = 0; i < motorDefinitions.size(); i++) {
		BioXASMAXvMotor * motor = new BioXASMAXvMotor(motorDefinitions[i].motorCategory_, motorDefinitions[i].motorCategoryName_,
													  motorDefinitions[i].pvBaseName_, motorDefinitions[i].description_,
													  true, 0.05, 2.0, this);

		motors_.append(motor);
	}
}

QList<BioXASMAXvMotor *> BioXASBeamline::getMotorsByCategory(BioXASBeamlineDef::BioXASMotorType category)
{
	QList<BioXASMAXvMotor *> matchMotors;

	for (int i = 0; i < motors_.size(); i++) {
		if (motors_[i]->category() == category) {
			matchMotors.append(motors_[i]);
		}
	}

	return matchMotors;
}
