#include "BioXASSideCryostatStage.h"

BioXASSideCryostatStage::BioXASSideCryostatStage(const QString &deviceName, QObject *parent) :
	BioXASCryostatStage(deviceName, parent)
{
	cryostatX_ = new CLSMAXvMotor(name()+"CryostatX", "SMTR1607-6-I22-14", name()+"CryostatX", false, 0.01, 2.0, this, ":mm");
	cryostatY_ = new CLSMAXvMotor(name()+"CryostatY", "SMTR1607-6-I22-13", name()+"CryostatY", false, 0.01, 2.0, this, ":mm");
	cryostatZ_ = new CLSMAXvMotor(name()+"CryostatZ", "SMTR1607-6-I22-15", name()+"CryostatZ", false, 0.01, 2.0, this, ":mm");

	AMMotorGroupObject *cryostatStageGroupObject = new AMMotorGroupObject("Cryostat Stage - X, Z, Y", this);
	cryostatStageGroupObject->setDirectionAxis(AMMotorGroupObject::HorizontalMotion, "X", cryostatX_, "", 0);
	cryostatStageGroupObject->setDirectionAxis(AMMotorGroupObject::VerticalMotion, "Z", cryostatZ_, "", 0);
	cryostatStageGroupObject->setDirectionAxis(AMMotorGroupObject::NormalMotion, "Y", cryostatY_, "", 0);

	cryostatStageMotors_->addMotorGroupObject(cryostatStageGroupObject);
}

BioXASSideCryostatStage::~BioXASSideCryostatStage()
{

}
