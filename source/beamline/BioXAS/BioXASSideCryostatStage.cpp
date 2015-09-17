#include "BioXASSideCryostatStage.h"

BioXASSideCryostatStage::BioXASSideCryostatStage(QObject *parent) :
	BioXASCryostatStage("SideCryostatStage", parent)
{
	cryostatX_ = new CLSMAXvMotor("SideCryostatX", "SMTR1607-6-I22-14", "SideCryostatX", true, 0.01, 2.0, this, ":mm");
	cryostatY_ = new CLSMAXvMotor("SideCryostatY", "SMTR1607-6-I22-13", "SideCryostatY", true, 0.01, 2.0, this, ":mm");
	cryostatZ_ = new CLSMAXvMotor("SideCryostatZ", "SMTR1607-6-I22-15", "SideCryostatZ", true, 0.01, 2.0, this, ":mm");


	AMMotorGroupObject *cryostatStageGroupObject = new AMMotorGroupObject("Cryostat Stage - X, Z, Y", this);
	cryostatStageGroupObject->setDirectionAxis(AMMotorGroupObject::HorizontalMotion, "X", cryostatX_, "", 0);
	cryostatStageGroupObject->setDirectionAxis(AMMotorGroupObject::VerticalMotion, "Z", cryostatZ_, "", 0);
	cryostatStageGroupObject->setDirectionAxis(AMMotorGroupObject::NormalMotion, "Y", cryostatY_, "", 0);

	cryostatStageMotors_ = new AMMotorGroup(this);
	cryostatStageMotors_->addMotorGroupObject(cryostatStageGroupObject);
}

BioXASSideCryostatStage::~BioXASSideCryostatStage()
{

}

bool BioXASSideCryostatStage::isConnected() const
{
	bool connected = (
				cryostatX_->isConnected() &&
				cryostatY_->isConnected() &&
				cryostatZ_->isConnected()
				);

	return connected;
}
