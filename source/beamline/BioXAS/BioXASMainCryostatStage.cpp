#include "BioXASMainCryostatStage.h"

BioXASMainCryostatStage::BioXASMainCryostatStage(const QString &deviceName, QObject *parent) :
	BioXASCryostatStage(deviceName, parent)
{
	cryostatX_ = new CLSMAXvMotor(name()+"CryostatX", "SMTR1607-7-I21-14", name()+"CryostatX", false, 0.01, 2.0, this, ":mm");
	cryostatY_ = new CLSMAXvMotor(name()+"CryostatY", "SMTR1607-7-I21-15", name()+"CryostatY", false, 0.01, 2.0, this, ":mm");
	cryostatZ_ = new CLSMAXvMotor(name()+"CryostatZ", "SMTR1607-7-I21-16", name()+"CryostatZ", false, 0.01, 2.0, this, ":mm");

	AMMotorGroupObject *cryostatStageGroupObject = new AMMotorGroupObject("Cryostat Stage - X, Z, Y", this);
	cryostatStageGroupObject->setDirectionAxis(AMMotorGroupObject::HorizontalMotion, "X", cryostatX_, "", 0);
	cryostatStageGroupObject->setDirectionAxis(AMMotorGroupObject::VerticalMotion, "Z", cryostatZ_, "", 0);
	cryostatStageGroupObject->setDirectionAxis(AMMotorGroupObject::NormalMotion, "Y", cryostatY_, "", 0);

	cryostatStageMotors_->addMotorGroupObject(cryostatStageGroupObject);
}

BioXASMainCryostatStage::~BioXASMainCryostatStage()
{

}
